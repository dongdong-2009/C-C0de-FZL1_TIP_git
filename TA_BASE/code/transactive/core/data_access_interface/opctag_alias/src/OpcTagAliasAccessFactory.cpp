/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/opctag_alias/src/OpcTagAliasAccessFactory.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * OpcTagAliasAccessFactory is a singleton that is used to retrieve OpcTagAlias objects either from the
  * database or newly created. All OpcTagAlias objects returned will adhear to the IOpcTagAlias interface.
  * Data is primarily retrieved from the CO_OPC_TAG table
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/opctag_alias/src/OpcTagAliasAccessFactory.h"
#include "core/data_access_interface/opctag_alias/src/OpcTagAlias.h"
#include "core/data_access_interface/opctag_alias/src/ConfigOpcTagAlias.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    //table name 
    static const std::string SCOPCTAGALIAS_TBL    ("SC_OPC_TAG_ALIAS");
    //index column
    static const std::string INDEX_COL       ("SCOTAL_ID");
    //OPC alias column
    static const std::string ALIAS_COL   ("ALIAS");
    //entity id column
    static const std::string ENTITYIDDP_COL   ("ENTITY_ID_DP");
    //attribute column
    static const std::string ATTRIBUTE_COL   ("ATTRIBUTE_DP");

    OpcTagAliasAccessFactory::OpcTagAliasAccessFactory()
    {
        FUNCTION_ENTRY("Constructor");
        FUNCTION_EXIT;
    }

    OpcTagAliasAccessFactory& OpcTagAliasAccessFactory::getInstance()
    {
        FUNCTION_ENTRY("getInstance");
        static OpcTagAliasAccessFactory factory;
        FUNCTION_EXIT;
        return factory;
    }


    IOpcTagAlias* OpcTagAliasAccessFactory::getOpcTagAlias( const unsigned long opcTagAliasKey, const bool readWrite )
    {
        FUNCTION_ENTRY("getOpcTagAlias");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);

        // create the SQL string to retrieve the name and key of the OpcTagAlias
        // this is a check to ensure a OpcTagAlias with the specified key actually exists.
        /*std::ostringstream sql;
        sql << "select " << ALIAS_COL  << " , " << ENTITYIDDP_COL << " from " << SCOPCTAGALIAS_TBL << " where " << INDEX_COL << " = " << opcTagAliasKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_OPC_TAG_ALIAS_STD_SELECT_33001, opcTagAliasKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_OPC_TAG_ALIAS_SELECT_33001, opcTagAliasKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_OPC_TAG_ALIAS_SELECT_33001, opcTagAliasKey);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		columnNames.push_back(ALIAS_COL);
        columnNames.push_back(ENTITYIDDP_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // Bring the DataException into the namespace
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified key
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "No data found for key = " << opcTagAliasKey;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one entry found for key = " << opcTagAliasKey;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,""));
        }

        // Create the OpcTagAlias object to represent this object. Initially only populate it
        // with entitykey and less name. We will populate the rest of the data on demand.
        IOpcTagAlias* opcTagAlias = NULL;
        if (readWrite)
        {
            opcTagAlias = new ConfigOpcTagAlias(data->getUnsignedLongData(0, ENTITYIDDP_COL), data->getStringData(0,ALIAS_COL));
        }
        else
        {
            opcTagAlias = new OpcTagAlias(data->getUnsignedLongData(0, ENTITYIDDP_COL), data->getStringData(0,ALIAS_COL));
        }
        
        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
        FUNCTION_EXIT;
        return opcTagAlias;
    }


    std::vector<IOpcTagAlias*> OpcTagAliasAccessFactory::getAllOpcTagAliases(const bool readWrite)
    {
        FUNCTION_ENTRY("getAllOpcTagAliases");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);

		// create the SQL string to retrieve all the locations of the alarm
        /*std::ostringstream sql;
        sql << "select " << INDEX_COL << " , "
            << ALIAS_COL << " , " 
            << ENTITYIDDP_COL << " , " 
            << ATTRIBUTE_COL << " from " << SCOPCTAGALIAS_TBL;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_OPC_TAG_ALIAS_STD_SELECT_33002);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_OPC_TAG_ALIAS_SELECT_33002);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_OPC_TAG_ALIAS_SELECT_33002);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(INDEX_COL);
		columnNames.push_back(ALIAS_COL);
        columnNames.push_back(ENTITYIDDP_COL);
		columnNames.push_back(ATTRIBUTE_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);
		
        // Loop for each row returned
        std::vector< IOpcTagAlias* > opcTagAliases;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // Catch any exceptions so we can clean up the pointers
                try
                {
                    unsigned long index = data->getUnsignedLongData(i, INDEX_COL);
                    unsigned long entitykey = data->getUnsignedLongData(i, ENTITYIDDP_COL);
                    std::string aliasname = data->getStringData(i, ALIAS_COL);
                    std::string attribute = data->getStringData(i, ATTRIBUTE_COL);
                    if (readWrite)
                    {
                        opcTagAliases.push_back( new ConfigOpcTagAlias(index, entitykey, aliasname, attribute ));
                    }
                    else
                    {
                        opcTagAliases.push_back( new OpcTagAlias( index, entitykey, aliasname, attribute));
                    }
                }
                catch (TA_Base_Core::DataException&)
                {
                    // Clean up the data pointer
                    delete data;
                    data = NULL;
                    throw;
                }
            }
			delete data;
			data = NULL;
        }
        while ( databaseConnection->moreData(data) );

        FUNCTION_EXIT;
		return opcTagAliases;
    }


    IConfigOpcTagAlias* OpcTagAliasAccessFactory::createOpcTagAlias()
    {
        FUNCTION_ENTRY("createOpcTagAlias");
        FUNCTION_EXIT;
        return new ConfigOpcTagAlias();
    }

    IConfigOpcTagAlias* OpcTagAliasAccessFactory::createOpcTagAlias(const unsigned long entitykey, const std::string& name)
    {
        FUNCTION_ENTRY("createOpcTagAlias");
        FUNCTION_EXIT;
        return new ConfigOpcTagAlias(entitykey, name);
    }

    IConfigOpcTagAlias* OpcTagAliasAccessFactory::copyOpcTagAlias(const IConfigOpcTagAlias* opcTagAliasToCopy)
    {
        FUNCTION_ENTRY("copyOpcTagAlias");

        TA_ASSERT(opcTagAliasToCopy !=  NULL, "The OpcTagAlias to copy was NULL");
        
        const ConfigOpcTagAlias* cast = dynamic_cast<const ConfigOpcTagAlias*>(opcTagAliasToCopy);

        TA_ASSERT(cast != NULL, "OpcTagAlias passed could not be converted into a ConfigOpcTagAlias");

        FUNCTION_EXIT;
        return new ConfigOpcTagAlias(*cast);
    }
	
} // closes TA_Base_Core


