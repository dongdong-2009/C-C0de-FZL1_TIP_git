 /*
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/PlanDAI.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This implements the IPlan interface and allows the user read-only access to a single plan
  */


// Disable "identifier truncated" warning
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // _MSC_VER



#include <sstream>

#include "core/data_access_interface/plans/src/PlanDAI.h"
#include "core/data_access_interface/plans/src/PlanAccessFactory.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{
    static const std::string NODE_TYPE_PLAN = "Plan";    // The identified of plan nodes in the database
    static const char CATEGORY_SEPARATOR = '/';          // Separator of categories in a plan path
    static const unsigned long ROOT_NODE_ID = 1;         // Node ID of the root of the plan tree
    

    PlanDAI::PlanDAI(const std::string& planPath) :
    m_path(planPath),
    m_nodeId(DatabaseKey::getInvalidKey()),
    m_isValidData(false)
    {
        if (!m_path.empty())
        {
            // Fix up the path if necessary - add a leading slash and remove duplicate slashes
            if (m_path[0] != CATEGORY_SEPARATOR)
            {
                m_path = CATEGORY_SEPARATOR + m_path;
            }

            for (std::string::size_type i = 1; i < m_path.length();)
            {
                if (m_path[i] == CATEGORY_SEPARATOR && m_path[i-1] == CATEGORY_SEPARATOR)
                {
                    m_path.erase(i, 1);
                    continue;
                }

                i++;
            }
        }
    }

    PlanDAI::PlanDAI( const unsigned long ulNodeId ) :
    m_path(""),
    m_nodeId(ulNodeId),
    m_isValidData(false)
    {
    }

    PlanDAI::~PlanDAI()
    {
    }


    std::string PlanDAI::getName()
    {
        if (!m_isValidData)
        {
            reload();
        }

        return PlanAccessFactory::getInstance().getPlanName(m_path);
    }


    std::string PlanDAI::getPath()
    {
        if (!m_isValidData)
        {
            reload();
        }

        return m_path;
    }


    void PlanDAI::invalidate()
    {
        m_nodeId = DatabaseKey::getInvalidKey();
        m_isValidData = false;
    }


    void PlanDAI::reload()
    {
        FUNCTION_ENTRY("reload()");

        //
        // Determine the plan key based on the path. 
        // Make sure the path denotes a plan and not a category.
        //
        if ( DatabaseKey::isInvalidKey( m_nodeId ))
        {
            refreshNodeId();
        }
        if ( m_path.empty() )
        {
            refreshPath();
        }

        validateNodeType();
       
        //
        // Indicate that we now have valid data.
        //
        m_isValidData = true;

        FUNCTION_EXIT;
    }


    void PlanDAI::refreshNodeId()
    {
        FUNCTION_ENTRY( "refreshNodeId" );

        if (m_path.empty())
        {
            TA_THROW(DataException("The specified plan path is empty", DataException::INVALID_VALUE, ""));
        }

        // 
        // Connect to the database
        //
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Plans_Cd, Read);

        // 
        // Iterate over all the names in the path, starting at the root.
        //
        unsigned long nodeId = ROOT_NODE_ID;
        std::string remainingPath = m_path;
        std::string::size_type sepPos = remainingPath.find(CATEGORY_SEPARATOR);

        remainingPath.erase(0, sepPos + 1);

        while (!remainingPath.empty())
        {
            //
            // Parse out the next name in the path
            //
            std::string::size_type sepPos = remainingPath.find(CATEGORY_SEPARATOR);
            std::string nodeName = remainingPath.substr(0, sepPos);
            TA_ASSERT(!nodeName.empty(), "Blank node name");
            
            //
            // Set up the columnNames vector to be passed to executeQuery()
            //
            static const std::string NODE_PKEY_COLUMN("P_KEY");

            std::vector<std::string> columnNames;
            columnNames.push_back(NODE_PKEY_COLUMN);

            //
            // Set up the SQL query.
            //
            /*std::ostringstream sql;
            sql << " select P_KEY " 
                   " from NODE_TREE "
                   " where NAME = '" << databaseConnection->escapeQueryString(nodeName) << "' "
                     " and PARENT_NODE = " << nodeId;*/
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), NODE_TREE_STD_SELECT_33501, 
//			std::string strSql  = databaseConnection->prepareSQLStatement(NODE_TREE_SELECT_33501, 
		    SQLStatement strSql;
		    databaseConnection->prepareSQLStatement(strSql, NODE_TREE_SELECT_33501,
				    databaseConnection->escapeQueryString(nodeName), nodeId);

            //
            // Execute the query. The method can throw a DatabaseException.
            // We are responsible for deleting the returned IData object when we're done with it
            //		    
            std::auto_ptr<IData> data(databaseConnection->executeQuery(strSql, columnNames));

            if (NULL == data.get() || 0 == data->getNumRows()) // No entry found with the specified name and parent
            {
                std::ostringstream reasonMessage;
                reasonMessage << "No plan record found for name = " << nodeName << " and parent ID = " << nodeId;
           
                TA_THROW(DataException(reasonMessage.str().c_str(), DataException::NO_VALUE, "" ));
            }
            else if (1 < data->getNumRows()) // More than one entry found for the specified name and parent
            {
                std::ostringstream reasonMessage;
                reasonMessage << "Multiple, non-unique plan records found for name = " << nodeName << " and parent ID = " << nodeId;
            
                TA_THROW(DataException(reasonMessage.str().c_str(), DataException::NOT_UNIQUE, ""));
            }

            //
            // Get the ID corresponding to nodeName. This is the parent of the next node in the path (if there is one)
            //
            nodeId = data->getUnsignedLongData(0, NODE_PKEY_COLUMN);
            
            //
            // Remove the name from the path
            //
            remainingPath.erase(0, (sepPos == std::string::npos)? sepPos : sepPos + 1);         
        }

        databaseConnection->cleanQuery();

        m_nodeId = nodeId;

        FUNCTION_EXIT;
    }


    void PlanDAI::refreshPath()
    {
        FUNCTION_ENTRY( "refreshPath" );

        if ( DatabaseKey::isInvalidKey( m_nodeId ))
        {
            TA_THROW(DataException("The specified plan node is invalid", DataException::INVALID_VALUE, "NODE_ID"));
        }

        // Connect to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Plans_Cd, Read);

        unsigned long ulCurNode = m_nodeId;
        while ( ulCurNode != ROOT_NODE_ID )
        {
            // Set up the columnNames vector to be passed to executeQuery()
            const char* const PARENT_KEY_COLUMN = "PARENT_NODE";
            const char* const PLAN_NAME_COLUMN = "NAME";

            std::vector<std::string> columnNames;
            columnNames.push_back(PLAN_NAME_COLUMN);
            columnNames.push_back(PARENT_KEY_COLUMN);

            SQLStatement strSql;
            databaseConnection->prepareSQLStatement(strSql, NODE_TREE_SELECT_33556,  ulCurNode);

            // Execute the query. The method can throw a DatabaseException.
            // We are responsible for deleting the returned IData object when we're done with it
            std::auto_ptr<IData> data(databaseConnection->executeQuery(strSql, columnNames));

            if (NULL == data.get() || 0 == data->getNumRows()) // No entry found with the specified name and parent
            {
                std::ostringstream reasonMessage;
                reasonMessage << "No plan record found for node_id = " << ulCurNode;
           
                TA_THROW(DataException(reasonMessage.str().c_str(), DataException::NO_VALUE, "" ));
            }
            else if (1 < data->getNumRows()) // More than one entry found for the specified name and parent
            {
                std::ostringstream reasonMessage;
                reasonMessage << "Multiple, non-unique plan records found for node_id = " << ulCurNode;
            
                TA_THROW(DataException(reasonMessage.str().c_str(), DataException::NOT_UNIQUE, ""));
            }

            std::string strPlanName = "";
            strPlanName += CATEGORY_SEPARATOR;
            strPlanName += data->getStringData(0, PLAN_NAME_COLUMN);
            m_path = strPlanName + m_path;
            ulCurNode = data->getUnsignedLongData(0, PARENT_KEY_COLUMN);
        }

        databaseConnection->cleanQuery();

        FUNCTION_EXIT;
    }


    void PlanDAI::validateNodeType()
    {
        FUNCTION_ENTRY("isPlanNode()");

        TA_ASSERT(m_nodeId > 0, "Node ID not set");

        //
        // Set up the columnNames vector to be passed to executeQuery()
        //
        static const std::string NODE_TYPE_NAME_COLUMN("TYPE_NAME");

        std::vector<std::string> columnNames;
        columnNames.push_back(NODE_TYPE_NAME_COLUMN);

        //
        // Set up the SQL query.
        //
        /*std::ostringstream sql;
        sql << " select TYPE_NAME "
               " from NODE_TYPE "
               " where P_KEY = "
               " ( "
                   " select FK_NODE_TYPE "
                   " from NODE_TREE "
                   " where P_KEY = " << m_nodeId << 
               " ) ";*/		

        //
        // Execute the query. The method can throw a DatabaseException.
        // We are responsible for deleting the returned IData object when we're done with it
        //
        IDatabase* database = DatabaseFactory::getInstance().getDatabase(Plans_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), NODE_TREE_STD_SELECT_33502, m_nodeId);
//		std::string strSql  = database->prepareSQLStatement(NODE_TREE_SELECT_33502, m_nodeId);
		SQLStatement strSql;
		database->prepareSQLStatement(strSql, NODE_TREE_SELECT_33502, m_nodeId);
        std::auto_ptr<IData> data(database->executeQuery(strSql, columnNames));

        if (NULL == data.get() || 0 == data->getNumRows()) // No entry found with the specified key
        {
            std::ostringstream reasonMessage;
            reasonMessage << "No plan node type record found for pkey = " << m_nodeId;

            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the key
        {
            std::ostringstream reasonMessage;
            reasonMessage << "Multiple, non-unique plan node type records found for pkey = " << m_nodeId;

            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"") );
        }

        //
        // Determine whether the specified node is a plan.
        //
        if (data->getStringData(0, NODE_TYPE_NAME_COLUMN) != NODE_TYPE_PLAN)
        {
            std::ostringstream reasonMessage;
            reasonMessage << "The specified path (" << m_path << ") denotes a category and not a plan.";
            TA_THROW(DataException(reasonMessage.str().c_str(), DataException::WRONG_TYPE, ""));
        }

        FUNCTION_EXIT;
    }

} //end namespace TA_Base_Core
