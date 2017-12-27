/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/system_controller/src/SystemControllerHelper.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/10/18 11:29:58 $
  * Last modified by:  $Author: Ouyang $
  *
  * This class contains the data associated with one 
  * entry in the sn_system_controller table.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <ctime>
#include <string>

#include "core/data_access_interface/system_controller/src/SystemControllerHelper.h"
#include "core/data_access_interface/system_controller/src/GroupMonitorAccessFactory.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    SystemControllerHelper::SystemControllerHelper(unsigned long key)
        : m_isValidData(false),
          m_isNew(false),
          m_key(key),
          m_name(""),
          m_agentPollPeriod(0),
          m_heartbeatSendPeriod(0),
          m_heartbeatSendNumber(0),
          m_agentResponseTimeout(0),
          m_serverEntityKey(0),
          m_groupId(0),
          m_debugLogDir(""),
          m_binDir(""),
          m_dateCreated(0),
          m_dateModified(0)
    {
    }


    SystemControllerHelper::SystemControllerHelper()
        : m_isValidData(false),
          m_isNew(true),
          m_key(0),
          m_name(""),
          m_agentPollPeriod(0),
          m_heartbeatSendPeriod(0),
          m_heartbeatSendNumber(0),
          m_agentResponseTimeout(0),
          m_serverEntityKey(0),
          m_groupId(0),
          m_debugLogDir(""),
          m_binDir(""),
          m_dateCreated(0),
          m_dateModified(0)
    {
    }

    
    SystemControllerHelper::SystemControllerHelper( const SystemControllerHelper& rhs)
        : m_isValidData(false),
          m_isNew(true),
          m_key(rhs.m_key),
          m_name(rhs.m_name),
          m_agentPollPeriod(rhs.m_agentPollPeriod),
          m_heartbeatSendPeriod(rhs.m_heartbeatSendPeriod),
          m_heartbeatSendNumber(rhs.m_heartbeatSendNumber),
          m_agentResponseTimeout(rhs.m_agentResponseTimeout),
          m_serverEntityKey(rhs.m_serverEntityKey),
          m_groupId(rhs.m_groupId),
          m_debugLogDir(rhs.m_debugLogDir),
          m_binDir(rhs.m_binDir),
          m_dateCreated(0),
          m_dateModified(0)
    {
    }

        
    SystemControllerHelper::~SystemControllerHelper()
    {
    }


    unsigned long SystemControllerHelper::getKey()
    {
        return m_key;
    }

    std::string SystemControllerHelper::getName()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_name;
    }

    unsigned long SystemControllerHelper::getAgentPollPeriod()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_agentPollPeriod;
    }

    unsigned long SystemControllerHelper::getHeartbeatSendPeriod()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_heartbeatSendPeriod;
    }

    unsigned long SystemControllerHelper::getHeartbeatSendNumber()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_heartbeatSendNumber;
    }

    unsigned long SystemControllerHelper::getAgentResponseTimeout()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_agentResponseTimeout;
    }

    unsigned long SystemControllerHelper::getServerEntityKey()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_serverEntityKey;
    }

    unsigned long SystemControllerHelper::getGroupId()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_groupId;
    }


    std::string SystemControllerHelper::getGroupName(unsigned long key)
    {
        FUNCTION_ENTRY("getGroupName");

        if (key == 0)
        {
            FUNCTION_EXIT;
            return "";
        }

        std::string group("");

        try
        {
            group = GroupMonitorAccessFactory::getInstance().getGroupName( key );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Database", "Could not retrieve the name for the group from the database. Returning an empty string");
        }

        FUNCTION_EXIT;
        return group;
    }


    std::string SystemControllerHelper::getEntityName(unsigned long key)
    {
        FUNCTION_ENTRY("getEntityName");

        if (key == 0)
        {
            FUNCTION_EXIT;
            return "";
        }

        std::string server("");
        IEntityData* serverObject = NULL;

        try
        {
            serverObject = EntityAccessFactory::getInstance().getEntity( key );
            server = serverObject->getName();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Database", "Could not retrieve the name for the entity from the database. Returning an empty string");
        }


        if (serverObject != NULL)
        {
            delete serverObject;
            serverObject = NULL;
        }
           

        FUNCTION_EXIT;
        return server;
    }


    std::string SystemControllerHelper::getDebugLogDir()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_debugLogDir;
    }
   
    
    std::string SystemControllerHelper::getBinDir()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_binDir;
    }
   
    
    time_t SystemControllerHelper::getDateCreated()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_dateCreated;
    }


    time_t SystemControllerHelper::getDateModified()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_dateModified;
    }
        

    void SystemControllerHelper::setName(const std::string& name)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_name = name;
    }

    
    void SystemControllerHelper::setAgentPollPeriod(unsigned long agentPollPeriod)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_agentPollPeriod = agentPollPeriod;
    }


    void SystemControllerHelper::setHeartbeatSendPeriod(unsigned long heartbeatSendPeriod)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_heartbeatSendPeriod = heartbeatSendPeriod;
    }


    void SystemControllerHelper::setHeartbeatSendNumber(unsigned long heartbeatSendNumber)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_heartbeatSendNumber = heartbeatSendNumber;
    }


    void SystemControllerHelper::setAgentResponseTimeout(unsigned long agentResponseTimeout)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_agentResponseTimeout = agentResponseTimeout;
    }


    void SystemControllerHelper::setServerEntityKey(unsigned long serverEntityKey)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_serverEntityKey = serverEntityKey;
    }


    void SystemControllerHelper::setGroupId(unsigned long groupId)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_groupId = groupId;
    }


    void SystemControllerHelper::setDebugLogDir(const std::string& debugLogDir)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_debugLogDir = debugLogDir;
    }


    void SystemControllerHelper::setBinDir(const std::string& binDir)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_binDir = binDir;
    }


    void SystemControllerHelper::deleteThisSystemController()
    {
         FUNCTION_ENTRY("deleteThisSystemController");

        TA_ASSERT(!m_isNew, "This system controller does not yet exist in the database, and therefore cannot be deleted");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Write);
        
        // We must now attempt to delete the system controller

        /*std::ostringstream sql;
        sql << "delete SN_SYSTEM_CONTROLLER where SNSCON_ID = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_STD_DELETE_23901, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_DELETE_23901, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_DELETE_23901, m_key);
//		TA_ASSERT(strSql.size() > 0, "SystemControllerHelper::deleteThisSystemController(): Prepare SQL Statement error");

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
   }


    void SystemControllerHelper::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        // don't need to check key - the key will never be "invalid"
        
        // Now need to determine which of the attributes are empty, and add each failure
        // to a vector of failed field names.

        std::vector<std::string> fieldNames;
        if (m_name.empty())
        {
            fieldNames.push_back("Name");
        }
        if (m_debugLogDir.empty())
        {
            fieldNames.push_back("Log Directory");
        }
        if (m_binDir.empty())
        {
            fieldNames.push_back("Bin Directory");
        }
        if (m_serverEntityKey == 0)
        {
            fieldNames.push_back("Server");
        }
        if (m_agentPollPeriod == 0)
        {
            fieldNames.push_back("Agent Poll Period");
        }
        if (m_heartbeatSendPeriod == 0)
        {
            fieldNames.push_back("Heartbeat Send Period");
        }
        if (m_heartbeatSendNumber == 0)
        {
            fieldNames.push_back("Heartbeat Send Number");
        }
        if (m_agentResponseTimeout ==0)
        {
            fieldNames.push_back("Agent Response Timeout");
        }

        // Now, if we have at least one entry in the vector, something is not right.
        if (0 < fieldNames.size())
        {
            TA_THROW( TA_Base_Core::DataConfigurationException("System Controller data not fully specified. Monitor Group cannot be written to database" ,fieldNames) );
        }

        // Write the system controller to the database

        // Need to check if this is a new system controller or an existing system controller - 
        // as a new system controller will be inserted, while an existing system controller will be updated
        if (m_isNew)
        {
             // This is a new system controller
            addNew();
        }
        else 
        {
            // This is an existing system controller
            modifyExisting();
        }

        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }

        
    void SystemControllerHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        TA_ASSERT(!m_isNew, "Cannot call invalidate on a new object");

        // Set the flag to false
        m_isValidData = false;

        FUNCTION_EXIT;
    }


    void SystemControllerHelper::reload()
    {
        FUNCTION_ENTRY("reload");

        // Get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Read);
        
        // Create the SQL string to retrieve the data of the system controller
        // based upon the key
        /*std::ostringstream sql;
        sql << "SELECT name, agent_poll_period, heartbeat_send_period, agent_response_timeout,server_entity_pkey, nvl(snscgr_id, 0) as snscgr_id, debug_log_dir, bin_dir, ";
        sql << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'), ";
        sql << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM sn_system_controller WHERE snscon_id = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_Oracle_SELECT_23551, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_SELECT_23551, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_SELECT_23551, m_key);
//		TA_ASSERT(strSql.size() > 0, "SystemControllerHelper::reload(): Prepare SQL Statement error");

        // Set up the columnNames vector to be passed to executeQuery()
        std::string nameColumn = "NAME";
        std::string agentPollPeriodColumn = "AGENT_POLL_PERIOD";
        std::string heartbeatSendPeriodColumn = "HEARTBEAT_SEND_PERIOD";
        std::string heartbeatSendNumberColumn = "HEARTBEAT_SEND_NUMBER";
        std::string agentResponseTimeoutColumn = "AGENT_RESPONSE_TIMEOUT";
        std::string serverEntityKeyColumn = "SERVER_ENTITY_PKEY";
        std::string groupIdColumn = "SNSCGR_ID";
        std::string debugLogDirColumn = "DEBUG_LOG_DIR";
        std::string binDirColumn = "BIN_DIR";
        std::string dateModifiedColumn = "DATE_MODIFIED";
        std::string dateCreatedColumn = "DATE_CREATED";

        std::vector<std::string> columnNames;
        columnNames.push_back(nameColumn);
        columnNames.push_back(agentPollPeriodColumn);
        columnNames.push_back(heartbeatSendPeriodColumn);
        columnNames.push_back(heartbeatSendNumberColumn);
        columnNames.push_back(agentResponseTimeoutColumn);
        columnNames.push_back(serverEntityKeyColumn);
        columnNames.push_back(groupIdColumn);
        columnNames.push_back(debugLogDirColumn);
        columnNames.push_back(binDirColumn);
        columnNames.push_back(dateModifiedColumn);
        columnNames.push_back(dateCreatedColumn);

        // Execute the query. The method can throw a DatabaseException.
        // We are responsible for deleting the returned IData object when we're 
        // done with it.
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            std::ostringstream reason;
            reason << "No System Controller found for snscon_id = " << m_key;
            TA_THROW(DataException(reason.str().c_str(), DataException::NO_VALUE, ""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            std::ostringstream reason;
            reason << "More than one System Controller found for snscon_id = " << m_key;
            TA_THROW(DataException(reason.str().c_str(), DataException::NOT_UNIQUE, ""));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions.
        m_name = data->getStringData(0, nameColumn);
        m_agentPollPeriod = data->getUnsignedLongData(0, agentPollPeriodColumn);
        m_heartbeatSendPeriod = data->getUnsignedLongData(0, heartbeatSendPeriodColumn);
        m_heartbeatSendNumber = data->getUnsignedLongData(0, heartbeatSendNumberColumn);
        m_agentResponseTimeout = data->getUnsignedLongData(0, agentResponseTimeoutColumn);
        m_serverEntityKey = data->getUnsignedLongData(0, serverEntityKeyColumn);
        m_groupId = data->getUnsignedLongData(0, groupIdColumn);
        m_debugLogDir = data->getStringData(0, debugLogDirColumn);
        m_binDir = data->getStringData(0, binDirColumn);
        m_dateCreated = data->getDateData( 0, dateCreatedColumn );
        m_dateModified = data->getDateData(0,dateModifiedColumn, 0);

        // Now that we're done with the IData object, it's our responsibility to 
        // delete it.
        delete data;
        data = NULL;

        // Need to record that we now have valid data.
        m_isValidData = true; 

        FUNCTION_EXIT;
    }


    void SystemControllerHelper::addNew()
    {
        FUNCTION_ENTRY("addNew");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                "System Controller %s is a new system controller. It will be added to the database.",
                m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Write);
        
        // First check there is not alreay an system controller with this name (it is a uniqueness
        // constraint).
        /*std::ostringstream formatSQL;
        formatSQL << "select SNSCON_ID from SN_SYSTEM_CONTROLLER where NAME = '";
        formatSQL << databaseConnection->escapeQueryString(m_name) << "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_STD_SELECT_23510,
//		std::string strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_SELECT_23510,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_SELECT_23510,
			databaseConnection->escapeQueryString(m_name));
//		TA_ASSERT(strSql.size() > 0, "SystemControllerHelper::addNew(): Prepare SQL Statement error");

        std::string keyColumn = "PKEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Check the number of rows returned (the actual data is not important) - it
        // should be 0
        if (0 != data->getNumRows()) // Name already exists
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "A system controller already exists with name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
        }

        // delte the IData pointer now that it's finised with
        delete data;
        data = NULL;

        // Now we can get on with writing the entity to the database.
        // generate the SQL string to write the entity (reuse the string)
		std::ostringstream osIdColumn, osIdColumnVal;
       /* formatSQL.str("");
        formatSQL << "insert into SN_SYSTEM_CONTROLLER(SNSCON_ID,NAME,AGENT_POLL_PERIOD,HEARTBEAT_SEND_PERIOD,";
        formatSQL << "AGENT_RESPONSE_TIMEOUT,SERVER_ENTITY_PKEY,DEBUG_LOG_DIR,BIN_DIR";*/
        if (m_groupId != 0)
        {
            osIdColumn << ",SNSCGR_ID";
        }
        /*formatSQL << ") ";
        formatSQL << "values (SYSTEM_CONTROL_SEQ.NEXTVAL,'";
		formatSQL << databaseConnection->escapeInsertString(m_name) << "',";
        formatSQL << m_agentPollPeriod << "," << m_heartbeatSendPeriod << "," << m_heartbeatSendNumber << ",";
        formatSQL << m_agentResponseTimeout << "," << m_serverEntityKey << ",'";
		formatSQL << databaseConnection->escapeInsertString(m_debugLogDir) << "','";
        formatSQL << databaseConnection->escapeInsertString(m_binDir) << "'";*/
        if (m_groupId != 0)
        {
            osIdColumnVal << "," << m_groupId;
        }
        //formatSQL << ")";
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_Oracle_INSERT_23851, osIdColumn.str(),
//		strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_INSERT_23851, osIdColumn.str(),

		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_INSERT_23851, osIdColumn.str(),
            databaseConnection->getNextPkey("SYSTEM_CONTROL_SEQ"),
			databaseConnection->escapeInsertString(m_name), m_agentPollPeriod, m_heartbeatSendPeriod, m_heartbeatSendNumber, m_agentResponseTimeout, m_serverEntityKey,
			databaseConnection->escapeInsertString(m_debugLogDir), databaseConnection->escapeInsertString(m_binDir), osIdColumnVal.str() );
//		TA_ASSERT(strSql.size() > 0, "SystemControllerHelper::addNew(): Prepare SQL Statement error");
 
        databaseConnection->executeModification(strSql);

        //now need to get the PKEY back out for this SC group and update the date fields
        /*std::ostringstream sql;
		sql << "select SNSCON_ID, TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from SN_SYSTEM_CONTROLLER where NAME = '";
        sql << databaseConnection->escapeQueryString(m_name).c_str() << "'";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_Oracle_SELECT_23552,
//		strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_SELECT_23552,

		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_SELECT_23552,
			databaseConnection->escapeQueryString(m_name));
//		TA_ASSERT(strSql.size() > 0, "SystemControllerHelper::addNew(): Prepare SQL Statement error");

        // Can use the same columnNames vector from above. Key is already in it so we can just add
        // the new fields
        std::string createdColumn = "DATE_CREATED";
        columnNames.push_back(createdColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) // No entry found with the specified name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "No data found for name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one entry found for name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
        }

        // Retrieve the pkey as an unsigned long from the 
        // returned data. There should only be a single returned item (as any other 
        // circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_key = data->getUnsignedLongData(0,keyColumn);
            m_dateCreated = data->getDateData(0,createdColumn);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        FUNCTION_EXIT;
    }


    void SystemControllerHelper::modifyExisting()
    {
        FUNCTION_ENTRY("modifyExisting");
    
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                "System Controller %s already exists. It's details will be updated.",
                m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Write);

        // Create a select statement that will ensure the uniqueness of the entity name.
        /*std::string formatSQL = "select SNSCON_ID from SN_SYSTEM_CONTROLLER where NAME = '"
            + databaseConnection->escapeQueryString(m_name) + "'";*/
		//std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_STD_SELECT_23510,
		//std::string strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_SELECT_23510,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_SELECT_23510,
			databaseConnection->escapeQueryString(m_name));

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        std::string keyColumn = "KEY";
        columnNames.push_back(keyColumn);

        // execute the query
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        //check the returned data - we should have 1 or 0 row returned
        if (1 == data->getNumRows()) // The name was found - need to check if it was for this pkey
        {
            if (m_key != data->getUnsignedLongData(0,keyColumn))
            {
                // A name already exists that is the same as the currently specified name
                // clean up the pointer
                delete data;
                data = NULL;

                std::ostringstream reasonMessage;
			    reasonMessage << "A system controller already exists with name = " << m_name.c_str();
                TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
            }
            // else - it matches, so that's okay - it means the name wasn't changed.
        }
        else if (1 < data->getNumRows()) // Two or more entities already exist with this name
        {   // This code will not be executed unless there is an error in the database
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one system controller already exists name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
        }

        delete data;
        data =  NULL;

        std::ostringstream sql;
        /*sql << "update SN_SYSTEM_CONTROLLER set NAME = '" << (databaseConnection->escapeQueryString(m_name)).c_str();
        sql << "', AGENT_POLL_PERIOD = " << m_agentPollPeriod;
        sql << ", HEARTBEAT_SEND_PERIOD = " << m_heartbeatSendPeriod;
        sql << ", HEARTBEAT_SEND_NUMBER = " << m_heartbeatSendNumber;
        sql << ", AGENT_RESPONSE_TIMEOUT = " << m_agentResponseTimeout;
        sql << ", SERVER_ENTITY_PKEY = " << m_serverEntityKey;
        sql << ", DEBUG_LOG_DIR = '"  << (databaseConnection->escapeQueryString(m_debugLogDir)).c_str();
        sql << "', BIN_DIR = '"  << (databaseConnection->escapeQueryString(m_binDir)).c_str();
        sql << "', SNSCGR_ID = ";*/
        if (m_groupId != 0)
        {
            sql << m_groupId;
        }
        else
        {
            sql << "NULL ";
        }
        //sql << " where SNSCON_ID = " << m_key;
		//strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_STD_UPDATE_23651, databaseConnection->escapeQueryString(m_name),
		//strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_UPDATE_23651, databaseConnection->escapeQueryString(m_name),

		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_UPDATE_23651, databaseConnection->escapeQueryString(m_name),
			m_agentPollPeriod, m_heartbeatSendPeriod, m_heartbeatSendNumber, m_agentResponseTimeout, m_serverEntityKey, databaseConnection->escapeQueryString(m_debugLogDir),
			databaseConnection->escapeQueryString(m_binDir), sql.str(), m_key);

        databaseConnection->executeModification(strSql);


        //now need to update the date fields
        /*sql.str("");
        sql  << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') ";
        sql << "from SN_SYSTEM_CONTROLLER where SNSCON_ID = " << m_key;*/
		//strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SN_SYSTEM_CONTROLLER_Oracle_SELECT_23553, m_key);
		//strSql  = databaseConnection->prepareSQLStatement(SN_SYSTEM_CONTROLLER_SELECT_23553, m_key);

		databaseConnection->prepareSQLStatement(strSql, SN_SYSTEM_CONTROLLER_SELECT_23553, m_key);

        // Can use the same columnNames vector from above.
        columnNames.clear();
        std::string createdColumn = "DATE_CREATED";
        std::string modifiedColumn = "DATE_MODIFIED";
        columnNames.push_back(createdColumn);
        columnNames.push_back(modifiedColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) // No entry found with the specified name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for name = " << m_name.c_str();			
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "More than one entry found for name = " << m_name.c_str();			
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
        }

        try
        {
            m_dateCreated = data->getDateData(0,createdColumn);
            m_dateModified = data->getDateData(0,modifiedColumn);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        FUNCTION_EXIT;
    }

}
