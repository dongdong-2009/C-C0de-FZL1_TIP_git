/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/ConfigurationEditor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  * 
  * ConfigurationEditor holds the data specific to a ConfigurationEditor entity from the
  * database and allows read-only access to that data.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include "core/data_access_interface/entity_access/src/ConfigurationEditor.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DbConnection.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    const std::string ConfigurationEditor::ENTITY_TYPE = "ConfigurationEditor";

    ConfigurationEditor::ConfigurationEditor(unsigned long key) 
        : EntityData(key, getStaticType())
    {
    }

    ConfigurationEditor::~ConfigurationEditor()
    { 
        // Nothing to clean up
    }


    IEntityData* ConfigurationEditor::clone(unsigned long key)
    {
        return new ConfigurationEditor(key);        
    }


    bool ConfigurationEditor::isConfigurationEditorAlreadyRunning(std::string& workstationName)
    {
        FUNCTION_ENTRY("isConfigurationEditorAlreadyRunning");

		// Use RPRAM of --db-connection-file instead of --db-connection to get the schema name
		std::string dbConnectionString;
		TA_Base_Core::DbConnection::getInstance().getConnectionString(OnlineUpdatable_Cd, Read, dbConnectionString);

        std::string::size_type beginOfSchema = dbConnectionString.find_first_of(':') + 1;
        std::string::size_type endOfSchema = dbConnectionString.find_first_of(':',beginOfSchema);

        std::string schemaName = dbConnectionString.substr(beginOfSchema, endOfSchema - beginOfSchema);


        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Schema to check is %s",schemaName.c_str());

        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // Order by login time to ensure that our connection comes at the end and any other connections
        // started before us come first. That way we know we will select the correct machine name and not our
        // own machine name.
        //delete by lin
        //std::string sql("select TERMINAL from local_sessions where lower(program) like '%configurationeditor%'");
        //sql += " and schemaname = upper('";
        //sql += databaseConnection->escapeQueryString(schemaName);
        //sql += "') and status in ('ACTIVE','INACTIVE') order by logon_time";

        
        std::vector<std::string> columns;
        columns.push_back("machine");

        IData* data = NULL;
        try
        {
			//waitforcheck delete by lin
			SQLStatement strSql;
					
			databaseConnection->prepareSQLStatement(strSql, LOCAL_SESSIONS_SELECT_79002,
				databaseConnection->escapeQueryString(schemaName));
			
            //data = databaseConnection->executeQuery(sql,columns);
            data = databaseConnection->executeQuery(strSql,columns);

            if (data->getNumRows() <= 1)
            {
                // This means only we are connected
                delete data;
                data = NULL;

                // No Configuration Editor's are currently connected to this schema
                return false;
            }
            else
            {

				//TD10758 - marc_bugfix
				//Check the timestamp set by the last config editor session
				if (checkConfigurationEditorTimeStamp() == false)
				{
					return false;
				}

                workstationName = data->getStringData(0, "machine");

                delete data;
                data = NULL;

                // A Configuration Editor is already connected to this schema
                return true;
	        }
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Database", "Could not determine if a Configuration Editor is already connected to this schema");
        }

        // If we cannot determine then we must return true for safety so another Configuration Editor
        // cannot be run.
        return true;
    }


    void ConfigurationEditor::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigurationEditor::invalidate");
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
    
        getHelper()->invalidate();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigurationEditor::invalidate");
    }


    std::string ConfigurationEditor::getType()
    {
        return getStaticType();
    }


    std::string ConfigurationEditor::getStaticType()
    {
        return ENTITY_TYPE;
    }


	bool ConfigurationEditor::getShowRegion()
	{
		FUNCTION_ENTRY("getShowRegion");

		// Variable to hold the show region flag.
		// No point in making it a member.
		bool showRegion = false;

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        /*std::ostringstream sql;
		sql << "select VALUE from ENTITYPARAMETERVALUE, ENTITYPARAMETER where ";
		sql << "ENTITYPARAMETER.NAME = 'ShowRegion' and ";
		sql << "ENTITYPARAMETERVALUE.PARAMETERKEY = ENTITYPARAMETER.PKEY";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparameter_STD_SELECT_20503);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITYPARAMETER_SELECT_20503);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITYPARAMETER_SELECT_20503);

		std::string valueColumn = "VALUE";
        std::vector<std::string> columnNames;
        columnNames.push_back(valueColumn);

        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows())
        {
            // clean up the pointer
            delete data;
            data = NULL;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not retrieve show region flag. Setting to default values");

            FUNCTION_EXIT;
            return false;
        }
        else if (1 < data->getNumRows()) 
        {
            // clean up the pointer
            delete data;
            data = NULL;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                "More than expected entries retrieved while retrieving show region flag. Setting to default values");

            FUNCTION_EXIT;
            return false;
        }
        
        try
        {
            showRegion = data->getBooleanData(0, valueColumn);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;

			FUNCTION_EXIT;
            return false;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        FUNCTION_EXIT;

		return showRegion;
	}

    unsigned int ConfigurationEditor::getEventLevels()
    {
		FUNCTION_ENTRY("ConfigurationEditor::getEventLevels");

		// Variable to hold the event level.
		// No point in making it a member.
		unsigned int eventLevels = 5;

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        /*std::ostringstream sql;
		sql << "select VALUE from ENTITYPARAMETERVALUE, ENTITYPARAMETER where ";
		sql << "ENTITYPARAMETER.NAME = 'EventLevels' and ";
		sql << "ENTITYPARAMETERVALUE.PARAMETERKEY = ENTITYPARAMETER.PKEY";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparameter_STD_SELECT_20504);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITYPARAMETER_SELECT_20504);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITYPARAMETER_SELECT_20504);

		std::string valueColumn = "VALUE";
        std::vector<std::string> columnNames;
        columnNames.push_back(valueColumn);

        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows())
        {
            // clean up the pointer
            delete data;
            data = NULL;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not retrieve number of event levels. Setting to default values");

            FUNCTION_EXIT;
            return eventLevels;
        }
        else if (1 < data->getNumRows()) 
        {
            // clean up the pointer
            delete data;
            data = NULL;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
                "More than expected entries retrieved while retrieving number of event levels. Setting to default values");

            FUNCTION_EXIT;
            return eventLevels;
        }
        
        try
        {
            eventLevels = data->getUnsignedLongData(0, valueColumn);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;

			FUNCTION_EXIT;
            return false;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        FUNCTION_EXIT;
		return eventLevels;
    }
	//TD10758 marc_bugfix
	//Perform second-level checking
	bool ConfigurationEditor::checkConfigurationEditorTimeStamp()
	{
		std::string dbConnectionString;
		const int max_interval_seconds = 5;

		TA_Base_Core::DbConnection::getInstance().getConnectionString(OnlineUpdatable_Cd, Read, dbConnectionString);

        std::string::size_type beginOfSchema = dbConnectionString.find_first_of(':') + 1;
        std::string::size_type endOfSchema = dbConnectionString.find_first_of(':',beginOfSchema);

        std::string schemaName = dbConnectionString.substr(beginOfSchema, endOfSchema - beginOfSchema);


        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Schema to check is %s",schemaName.c_str());

        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);



        /*std::string sql("select substr((CURRENT_TIMESTAMP-last_timestamp),instr((CURRENT_TIMESTAMP-last_timestamp),' ')+7,2) seconds,");
		sql += " substr((CURRENT_TIMESTAMP-last_timestamp),instr((CURRENT_TIMESTAMP-last_timestamp),' ')+4,2) minutes, ";
		sql += " substr((CURRENT_TIMESTAMP-last_timestamp),instr((CURRENT_TIMESTAMP-last_timestamp),' ')+1,2) hours, ";
		sql += " trunc(to_number(substr((CURRENT_TIMESTAMP-last_timestamp),1,instr(CURRENT_TIMESTAMP-last_timestamp,' '))))   days, ";
		sql += " trunc(to_number(substr((CURRENT_TIMESTAMP-last_timestamp),1,instr(CURRENT_TIMESTAMP-last_timestamp,' ')))/7) weeks  from CONFIG_EDITOR_TIMESTAMP";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparameter_Oracle_SELECT_20554);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITYPARAMETER_SELECT_20554);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITYPARAMETER_SELECT_20554);

	     
        std::vector<std::string> columns;
        columns.push_back("seconds");
		columns.push_back("minutes");
		columns.push_back("hours");
		columns.push_back("days");
		columns.push_back("weeks");


        IData* data = NULL;
        try
        {
            data = databaseConnection->executeQuery(strSql,columns);

            if (data->getNumRows() == 1)
            {
				//This query should only return 1 row.
				//Purpose of this checking is to check if there is an existing configuration editor
				//New Design of the config editor is that it updates the CONFIG_EDITOR_TIMESTAMP table
				//with a new timestamp every 5 seconds
				//So, for example , if the last config editor session crashed, and if the interval
				//between the current timestamp and the last timestamp is 5 seconds, we should allow
				//a new config editor session to be run


				if (data->getIntegerData(0, "weeks") > 0)
				{
					return false;
				}
				if (data->getIntegerData(0, "days") > 0)
				{
					return false;
				}
				if (data->getIntegerData(0, "hours") > 0)
				{
					return false;
				}
				if (data->getIntegerData(0, "minutes") > 0)
				{
					return false;
				}

				if (data->getIntegerData(0, "seconds") > max_interval_seconds)
				{
					return false;
				}

            }
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Database", "Could not determine if a Configuration Editor is already connected to this schema");
        }

        delete data;
        data = NULL;

		return true;
	
	}

} //close namespace TA_Base_Core

