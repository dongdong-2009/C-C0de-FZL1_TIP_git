/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/system_controller/src/GroupMonitorAccessFactory.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  *
  * This class is a singleton that is used to retrieve
  * data from the sn_group_monitor table.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <vector>
#include <string>
#include <sstream>

#include "core/data_access_interface/system_controller/src/GroupMonitorAccessFactory.h"
#include "core/data_access_interface/system_controller/src/ConfigGroupMonitor.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::IDatabase;
using TA_Base_Core::IData;

namespace TA_Base_Core
{
    GroupMonitorAccessFactory* GroupMonitorAccessFactory::m_instance = 0;

    GroupMonitorAccessFactory& GroupMonitorAccessFactory::getInstance()
    {
        if (m_instance == NULL)
        {
            m_instance = new GroupMonitorAccessFactory();
        }
        return *m_instance;
    }

    std::vector<std::string> GroupMonitorAccessFactory::getGroupsMonitoredBySystemController(std::string hostname)
    {
        // Get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_GROUP_STD_SELECT_23003,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_GROUP_SELECT_23003,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_GROUP_SELECT_23003,
			databaseConnection->escapeQueryString(hostname));
//		TA_ASSERT(strSql.size() > 0, "GroupMonitorAccessFactory::getGroupsMonitoredBySystemController(): Prepare SQL Statement error");

        /*std::ostringstream sql;
        sql << "SELECT name FROM sn_system_controller_group WHERE snscgr_id = ";
        sql << "(SELECT snscgr_id FROM sn_group_monitor WHERE snscon_id = ";
        sql << "(SELECT snscon_id FROM sn_system_controller WHERE name = '";
        sql << databaseConnection->escapeQueryString(hostname) << "'))";*/

        // Set up the columnNames vector to be passed to executeQuery()
        std::string nameColumn = "NAME";
        std::vector<std::string> columnNames;
        columnNames.push_back(nameColumn);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object.
        IData* data = databaseConnection->executeQuery(strSql, columnNames);

        // Retrieve each row from the IData object, and put it in a vector.
        std::vector<std::string> result;
        int numRows = data->getNumRows();
        for (int i = 0; i < numRows; i++)
        {
            result.push_back(data->getStringData(i, nameColumn));
        }
    
        delete data;
        return result;
    }

    std::vector<std::string> GroupMonitorAccessFactory::getSystemControllersMonitoringGroup(std::string groupName)
    {
        // Get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_STD_SELECT_23502,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_SELECT_23502,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_SELECT_23502,
			databaseConnection->escapeQueryString(groupName));
//		TA_ASSERT(strSql.size() > 0, "GroupMonitorAccessFactory::getSystemControllersMonitoringGroup(): Prepare SQL Statement error");

        /*std::ostringstream sql;
        sql << "SELECT name FROM sn_system_controller WHERE snscon_id IN ";
        sql << "(SELECT snscon_id FROM sn_group_monitor WHERE snscgr_id = ";
        sql << "(SELECT snscgr_id FROM sn_system_controller_group WHERE name = '";
        sql << databaseConnection->escapeQueryString(groupName) << "'))";*/

        // Set up the columnNames vector to be passed to executeQuery()
        std::string nameColumn = "NAME";
        std::vector<std::string> columnNames;
        columnNames.push_back(nameColumn);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object.
        IData* data = databaseConnection->executeQuery(strSql, columnNames);

        // Retrieve each row from the IData object, and put it in a vector.
        std::vector<std::string> result;
        int numRows = data->getNumRows();
        for (int i = 0; i < numRows; i++)
        {
            result.push_back(data->getStringData(i, nameColumn));
        }
    
        delete data;
        return result;
    }


    std::string GroupMonitorAccessFactory::getGroupName(unsigned long key )
    {
        FUNCTION_ENTRY("getGroupName");

        // Get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_GROUP_STD_SELECT_23004,key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_GROUP_SELECT_23004,key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_GROUP_SELECT_23004,key);
//		TA_ASSERT(strSql.size() > 0, "GroupMonitorAccessFactory::getGroupName(): Prepare SQL Statement error");

        // Create a query to retreive the name of the group.
        /*std::ostringstream sql;
        sql <<  "SELECT name FROM sn_system_controller_group WHERE snscgr_id = " <<  key;*/

        // Create the columns vector.
        std::string nameColumn = "NAME";
        std::vector<std::string> columnNames;
        columnNames.push_back(nameColumn);

        // Execute the query. The method can throw a DatabaseException.
        // We are responsible for deleting the returned IData object when we're 
        // done with it.
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            char reason[256] = {0};
			sprintf(reason, "No System Controller group found for snscgr_id = %lu", key);			
                TA_THROW(DataException(reason, DataException::NO_VALUE, ""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            char reason[256] = {0};
			sprintf(reason, "More than one System Controller group found for snscgr_id = %lu", key);
                TA_THROW(DataException(reason, DataException::NOT_UNIQUE, ""));
        }

        std::string groupName = data->getStringData(0, nameColumn);
        delete data;

        FUNCTION_EXIT;
        return groupName;
    }

        
    std::vector<IConfigGroupMonitor*> GroupMonitorAccessFactory::getAllMonitorGroups()
    {
        FUNCTION_ENTRY("getAllMonitorGroups");

		// Perform SQL here
		std::vector< IConfigGroupMonitor* > groups;

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_GROUP_Oracle_SELECT_23054);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_GROUP_SELECT_23054);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_GROUP_SELECT_23054);
//		TA_ASSERT(strSql.size() > 0, "GroupMonitorAccessFactory::getAllMonitorGroups(): Prepare SQL Statement error");

		// create the SQL string to retrieve all the groups
        /*std::ostringstream sql;
        sql << "select SNSCGR_ID, NAME, DESCRIPTION, ";
        sql << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'), ";
        sql << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM SN_SYSTEM_CONTROLLER_GROUP";*/

        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn( "PKEY" );
		std::string nameColumn( "NAME" );
		std::string descriptionColumn( "DESCRIPTION" );
		std::string dateModifiedColumn( "MODIFIED" );
		std::string dateCreatedColumn( "CREATED" );
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);
		columnNames.push_back(nameColumn);
		columnNames.push_back(descriptionColumn);
		columnNames.push_back(dateModifiedColumn);
		columnNames.push_back(dateCreatedColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);
		
        if (0 == data->getNumRows()) // No entry found with the specified alarm_id
        {
            // clean up the pointer
            delete data;
            data = NULL;

            return groups;
        }

        // Loop for each row returned
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
                    time_t dateModified = data->getDateData(i,dateModifiedColumn, 0);

                    // Create a shiny new group that has all the information pre-loaded!
                    IConfigGroupMonitor* group = new ConfigGroupMonitor( data->getUnsignedLongData( i, keyColumn ),
							                                             data->getStringData( i, nameColumn ),
							                                             data->getStringData( i, descriptionColumn ),
                                                                         data->getDateData( i, dateCreatedColumn ),
                                                                         dateModified );
                    groups.push_back( group );
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

		return groups;
    }


    IConfigGroupMonitor* GroupMonitorAccessFactory::createGroup()
    {
        return new ConfigGroupMonitor();
    }


    IConfigGroupMonitor* GroupMonitorAccessFactory::copyGroup(const IConfigGroupMonitor* groupToCopy)
    {
        FUNCTION_ENTRY("copyGroup");

        TA_ASSERT(groupToCopy !=  NULL, "The group to copy was NULL");
        
        const ConfigGroupMonitor* cast = dynamic_cast<const ConfigGroupMonitor*>(groupToCopy);

        TA_ASSERT(cast != NULL, "Group passed could not be converted into a ConfigGroupMonitor");

        FUNCTION_EXIT;
        return new ConfigGroupMonitor(*cast);
    }


} // namespace TA_Base_Core
