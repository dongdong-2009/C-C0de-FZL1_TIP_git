/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/system_controller/src/SystemControllerAccessFactory.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  *
  * SystemControllerAccessFactory is a singleton that is
  * used to retrieve ISystemControllerData objects from
  * the database.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4284)
#endif

#include "core/data_access_interface/system_controller/src/SystemControllerAccessFactory.h"
#include "core/data_access_interface/system_controller/src/SystemControllerData.h"
#include "core/data_access_interface/system_controller/src/ConfigSystemController.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include <string>
#include <vector>
#include <sstream>

namespace TA_Base_Core
{

    SystemControllerAccessFactory* SystemControllerAccessFactory::m_instance = 0;

    SystemControllerAccessFactory& SystemControllerAccessFactory::getInstance()
    {
        if (m_instance == NULL)
        {
            m_instance = new SystemControllerAccessFactory();
        }
        return *m_instance;
    }


    ISystemControllerData* SystemControllerAccessFactory::getSystemControllerDataByName(std::string name)
    {
        FUNCTION_ENTRY("getSystemControllerDataByName");

        // Get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Read);

        // Create the SQL string to retrieve the key of the System Controller.
        // This also doubles as a check to ensure a System Controller with the
        // specified name actually exists.
		// TD #11392
		// Convert to lower case for compare, to remove case sensitivity
        /*char sql[256];
		sprintf(sql,"select snscon_id from sn_system_controller where lower(name) = lower('%s')",
            (databaseConnection->escapeQueryString(name)).c_str());	*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_STD_SELECT_23507,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_SELECT_23507,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_SELECT_23507,
			databaseConnection->escapeQueryString(name));
//		TA_ASSERT(strSql.size() > 0, "SystemControllerAccessFactory::getSystemControllerDataByName(): Prepare SQL Statement error");

        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn = "PKEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object.
        IData* data = databaseConnection->executeQuery(strSql, columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // Clean up the pointer
            delete data;
            data = NULL;

            char reason[256] = {0};
			sprintf(reason, "No data found for System Controller name = %s",name.c_str());			
            TA_THROW(DataException(reason, DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // Clean up the pointer
            delete data;
            data = NULL;

            char reason[256] = {0};
			sprintf(reason, "More than one entry found for System Controller name = %s",name.c_str());			
            TA_THROW(DataException(reason, DataException::NOT_UNIQUE,""));
        }

        // Retrieve the pkey as an unsigned long from the 
        // returned data. There should only be a single returned item (as any other 
        // circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
		unsigned long key = data->getUnsignedLongData(0,keyColumn);

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Create the SystemControllerData object to represent this object. Initially 
        // only populate it with key. We will populate the rest of the data on demand.
        ISystemControllerData* theSystemController = new SystemControllerData(key);

        // Return the SystemControllerData pointer. The class that receives this
        // pointer is responsible for deleting it.
        FUNCTION_EXIT;
        return theSystemController;
    }


    std::vector<std::string> SystemControllerAccessFactory::getSystemControllersInGroup(std::string groupName)
    {
        // Get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Read);

        // Create the SQL to retrieve the System Controllers in the group.
        /*std::ostringstream sql;
		sql << "SELECT name FROM sn_system_controller WHERE snscgr_id = "
            << "(SELECT snscgr_id from sn_system_controller_group WHERE name = '"
            << databaseConnection->escapeQueryString(groupName) << "')";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_STD_SELECT_23508,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_SELECT_23508,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_SELECT_23508,
			databaseConnection->escapeQueryString(groupName));
//		TA_ASSERT(strSql.size() > 0, "SystemControllerAccessFactory::getSystemControllersInGroup(): Prepare SQL Statement error");

        // Set up the columnNames vector to be passed to executeQuery()
        std::string nameColumn = "NAME";
        std::vector<std::string> columnNames;
        columnNames.push_back(nameColumn);

        // Execute the query. 
        IData* data = databaseConnection->executeQuery(strSql, columnNames);

        // Fill a vector with the hostnames returned from the query.
        std::vector<std::string> result;
        unsigned long numRows = data->getNumRows();
        for (unsigned long i = 0; i < numRows; i++)
        {
            result.push_back(data->getStringData(i, nameColumn));
        }

        delete data;
        return result;
    }


    std::vector<ISystemControllerData*> SystemControllerAccessFactory::getAllSystemControllers(bool isReadWrite /*= false*/)
    {
        FUNCTION_ENTRY("getAllSystemControllers");

		// Perform SQL here

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Read);

		// create the SQL string to retrieve all the groups
        /*std::string sql ("SELECT snscon_id FROM sn_system_controller");*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_STD_SELECT_23509);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_SELECT_23509);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_SELECT_23509);
//		TA_ASSERT(strSql.size() > 0, "SystemControllerAccessFactory::getAllSystemControllers(): Prepare SQL Statement error");


        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn( "PKEY" );
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);
		
		std::vector< ISystemControllerData* > systemControllers;

        // Loop for each row returned
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
                    unsigned long key = data->getUnsignedLongData(i, keyColumn);

                    ISystemControllerData* systemController = NULL;
                    if (isReadWrite)
                    {
                        systemController = new ConfigSystemController( key );
                    }
                    else
                    {
                        systemController = new SystemControllerData( key );
                    }
                    systemControllers.push_back( systemController );
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

		return systemControllers;
    }


    IConfigSystemController* SystemControllerAccessFactory::createSystemController()
    {
        return new ConfigSystemController();
    }


    IConfigSystemController* SystemControllerAccessFactory::copySystemController(const IConfigSystemController* systemControllerToCopy)
    {
        FUNCTION_ENTRY("copySystemController");

        TA_ASSERT(systemControllerToCopy !=  NULL, "The System Controller to copy was NULL");
        
        const ConfigSystemController* cast = dynamic_cast<const ConfigSystemController*>(systemControllerToCopy);

        TA_ASSERT(cast != NULL, "System Controller passed could not be converted into a ConfigSystemController");

        FUNCTION_EXIT;
        return new ConfigSystemController(*cast);
    }

} // namespace TA_Base_Core
