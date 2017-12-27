/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/MessageData.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2016/01/18 15:18:43 $
 * Last modified by:  $Author: Ouyang $
 * 
 * IMessageData is an interface to the Message table. It provides both read and write access
 * for agents and user applicaitons.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "ace/OS.h"

#include "core/data_access_interface/src/MessageData.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
//#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "core/message/types/SchedulingAudit_MessageTypes.h"


#include <time.h>

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    
    std::string MessageData::getKey()
    {
        TA_ASSERT(std::string("") != m_key, "A newly created message must be written to the database before the key can be retrieved");

        return(m_key);
    }


    std::string MessageData::getDescription()
    {
        if (m_isValidData || std::string("") == m_key)
        {
			TA_ASSERT(std::string("") != m_description,"The description must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_description;
    }


    void MessageData::setDescription(const std::string& description)
    {
        if (!m_isValidData && std::string("") != m_key)
        {
            reload();
        }
        m_description = description;
    }


    std::string MessageData::getParamList()
    {
        if (m_isValidData || std::string("") == m_key)
        {
                TA_ASSERT(std::string("") != m_paramString,"The description must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_paramString;
    }


    void MessageData::setParamList(const std::string& paramString)
    {
        if (!m_isValidData && std::string("") != m_key)
        {
            reload();
        }
        m_paramString = paramString;
    }


    timeb MessageData::getCreatetime()
    {
        if (m_isValidData || std::string("") == m_key)
        {
            TA_ASSERT(0 != m_createTime.time,"The create time must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_createTime;
    }


    void MessageData::setCreatetime(const timeb createTime)
    {
        if (!m_isValidData && std::string("") != m_key)
        {
            reload();
        }
        m_createTime = createTime;
    }


    timeb MessageData::getLogtime()
    {
        if (m_isValidData || std::string("") == m_key)
        {
            TA_ASSERT(0 != m_logTime.time,"The log time must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_logTime;
    }


    void MessageData::setLogtime(const timeb logTime)
    {
        if (!m_isValidData && std::string("") != m_key)
        {
            reload();
        }
        m_logTime = logTime;
    }


    std::string MessageData::getDetails()
    {
        if (!m_isValidData && std::string("") != m_key)
        {
            reload();
        }
        return m_details;
    }


    void MessageData::setDetails(const std::string& details)
    {
        if (!m_isValidData && std::string("") != m_key)
        {
            reload();
        }
        m_details = details;
    }


    std::string MessageData::getType()
    {
        if (m_isValidData || std::string("") == m_key)
        {
                TA_ASSERT(std::string("") != m_type,"The type must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_type;
    }


    void MessageData::setType(const unsigned long typeKey)
    {
        if (!m_isValidData && std::string("") != m_key)
        {
            reload();
        }
        m_type = "TODO: THIS ONE";
    }


    void MessageData::setType(const std::string& typeName)
    {
        if (!m_isValidData && std::string("") != m_key)
        {
            reload();
        }
        m_type = typeName;
    }

    
    std::string& MessageData::getTypeName()
    {
        if (m_isValidData || std::string("") == m_key)
        {
                TA_ASSERT(std::string("") != m_type,"The type name must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_typeName;
    }

    void MessageData::setTypeName (const std::string& typeName)
    {
        if (!m_isValidData && std::string("") != m_key)
        {
            reload();
        }
        m_typeName = typeName;
    }

    std::string& MessageData::getEntityName()
    {
        if (m_isValidData || std::string("") == m_key)
        {
                TA_ASSERT(std::string("") != m_type,"The entity name must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_entityName;
    }

    void MessageData::setEntityName(const std::string& entityName)
    {
        if (!m_isValidData && std::string("") != m_key)
        {
            reload();
        }
        m_entityName = entityName;
    }
    

    std::string& MessageData::getOperatorName()
    {
        if (m_isValidData || std::string("") == m_key)
        {
                TA_ASSERT(std::string("") != m_type,"The operator name must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_operatorName;
    }

    void MessageData::setOperatorName(const std::string& operatorName)
    {
        if (!m_isValidData && std::string("") != m_key)
        {
            reload();
        }
        m_operatorName = operatorName;
    }
    
    
    std::string& MessageData::getSeverityName()
    {
        if (m_isValidData || std::string("") == m_key)
        {
                TA_ASSERT(std::string("") != m_type,"The severity name must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_severityName;
    }

    void MessageData::setSeverityName(const std::string& severityName)
    {
        if (!m_isValidData && std::string("") != m_key)
        {
            reload();
        }
        m_severityName = severityName;
    }
    

    std::string& MessageData::getSubsystemName()
    {
        if (m_isValidData || std::string("") == m_key)
        {
                TA_ASSERT(std::string("") != m_type,"The subsystem name must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_subsystemName;
    }

    void MessageData::setSubsystemName(const std::string& subsystemName)
    {
        if (!m_isValidData && std::string("") != m_key)
        {
            reload();
        }
        m_subsystemName = subsystemName;
    }
    

    std::string& MessageData::getLocationName()
    {
        if (m_isValidData || std::string("") == m_key)
        {
                TA_ASSERT(std::string("") != m_type,"The location name must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_locationName;
    }

    void MessageData::setLocationName(const std::string& locationName)
    {
        if (!m_isValidData && std::string("") != m_key)
        {
            reload();
        }
        m_locationName = locationName;
    }


    std::string& MessageData::getAssetName()
    {
        if (m_isValidData || std::string("") == m_key)
        {
            // TODO: It looks like there might be a copy & paste mistake with these assertions, 
            // as many of them are not testing the condition they claim to detect.
			// In the case of asset name, it is allowed to be empty so we shouldn't need to assert.
            TA_ASSERT(std::string("") != m_type, "The asset name must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_assetName;
    }

    void MessageData::setAssetName(const std::string& assetName)
    {
        if (!m_isValidData && std::string("") != m_key)
        {
            reload();
        }
        m_assetName = assetName;
    }


    unsigned long MessageData::getEntityKey()
    {
        if (m_isValidData || std::string("") == m_key)
        {
            TA_ASSERT(ULONG_MAX != m_entityKey,"The entity key must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_entityKey;
    }


    void MessageData::setEntityKey(const unsigned long key)
    {
        if (!m_isValidData && std::string("") != m_key)
        {
            reload();
        }
        m_entityKey = key;
    }


	std::string MessageData::getSessionID()
    {
        if (m_isValidData || std::string("") == m_key)
        {
            // Nothing to implement as session key can be null TD 10246
			//TA_ASSERT( "" != m_sessionID, "The sessionID must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_sessionID;
    }


    void MessageData::setSessionID( const std::string& sessionID )
    {
        if (!m_isValidData && std::string("") != m_key)
        {
            reload();
        }
        m_sessionID = sessionID;
    }


    std::string MessageData::getAlarmKey()
    {
        if (m_isValidData || std::string("") == m_key)
        {
                TA_ASSERT(std::string("") != m_alarmKey,"The alarm key must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_alarmKey;
    }


    void MessageData::setAlarmKey(const std::string& key)
    {
        if (!m_isValidData && std::string("") != m_key)
        {
            reload();
        }
        m_alarmKey = key;
    }

    // TD13673
    unsigned long MessageData::getLocationKey()
    {
        if ( m_isValidData || "" == m_key )
        {
                TA_ASSERT( ULONG_MAX != m_locationKey, "The location ID must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
		return m_locationKey;
    }

    void MessageData::setLocationKey(unsigned long locationKey)
    {
        if (!m_isValidData && ULONG_MAX != m_locationKey)
        {
            reload();
        }
        m_locationKey = locationKey;
    }

	std::string MessageData::getIncidentID()
	{
        if ( m_isValidData || "" == m_key )
        {
                TA_ASSERT( "" != m_incidentID, "The incident ID must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
		return m_incidentID;
	}


	void MessageData::setIncidentID( const std::string& incidentID )
	{
        if (!m_isValidData && std::string("") != m_key)
        {
            reload();
        }
        m_incidentID = incidentID;
	}


    void MessageData::invalidate()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "MessageData::invalidate" );
        // This method cannot be called until the key has been set
            TA_ASSERT(std::string("") != m_key,"Attempted to call invalidate() on a new message");

        m_isValidData = false;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "MessageData::invalidate" );
    }


    void MessageData::reload()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "MessageData::reload" );
            TA_ASSERT(std::string("") != m_key, "This message has not yet been written to the database, therefore it cannot be reloaded");
        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
        
        /*std::string sql = "select TO_CHAR(CREATETIME,'YYYYMMDDHH24MISSFF3'),";
        sql += "TO_CHAR(LOGTIME,'YYYYMMDDHH24MISSFF3'),DESCRIPTION,PARAM_LIST,DETAILS,EVENTTYPE,";
        sql += "ENTITYKEY, nvl(SESSIONKEY, ' ') as SESSIONKEY, alarm_id, LOCATIONKEY, incident_id, asset_name from event where message_id = '" + m_key + "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EVENT_Oracle_SELECT_17058, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EVENT_SELECT_17058, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EVENT_SELECT_17058, m_key);

        // Set up the columnNames vector to be passed to executeQuery()
        std::string createTimeColumn = "CREATETIME";
        std::string logTimeColumn = "LOGTIME";
        std::string descriptionColumn = "DESCRIPTION";
        std::string paramColumn = "PARAMLIST";
        std::string detailsColumn = "DETAILS";
        std::string typeColumn = "TYPE";
        std::string entityKeyColumn = "ENTITYKEY";
        std::string sessionIDColumn = "SESSIONKEY";
        std::string alarmKeyColumn = "ALARMKEY";
        // TD13673
        std::string locationKeyColumn = "LOCATIONKEY";
		std::string incidentIDColumn = "INCIDENT_ID";
        std::string assetNameColumn = "ASSET_NAME";
        std::vector<std::string> columnNames;
        columnNames.push_back(createTimeColumn);
        columnNames.push_back(logTimeColumn);
        columnNames.push_back(descriptionColumn);
        columnNames.push_back(paramColumn);
        columnNames.push_back(detailsColumn);
        columnNames.push_back(typeColumn);
        columnNames.push_back(entityKeyColumn);
        columnNames.push_back(sessionIDColumn);
        columnNames.push_back(alarmKeyColumn);
        // TD13673
        columnNames.push_back(locationKeyColumn);
        columnNames.push_back(incidentIDColumn);
        columnNames.push_back(assetNameColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified message_id
        {
            delete data;
            data = NULL;
            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "No messasge found for message_id = %s",m_key.c_str());			
            throw DataException(reasonMessage,DataException::NO_VALUE,"");
        }
        else if (1 < data->getNumRows()) // More than one entry found for the message_id
        {
            delete data;
            data = NULL;
            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More than one message found for message_id = %s",m_key.c_str());			
            throw DataException(reasonMessage,DataException::NOT_UNIQUE,"");
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_createTime = data->getTimestampData(0,createTimeColumn);
        m_logTime = data->getTimestampData(0,logTimeColumn);
        m_description = data->getStringData(0,descriptionColumn);
        m_paramString = data->getStringData(0,paramColumn);
        m_details = data->getStringData(0,detailsColumn);
        m_type = data->getStringData(0,typeColumn);
        m_entityKey = data->getUnsignedLongData(0,entityKeyColumn);

		// Translate the stupid empty space into an empty string
        m_sessionID = data->getStringData(0,sessionIDColumn);
		if ( " " == m_sessionID )
		{
			m_sessionID = "";
		}

        m_alarmKey = data->getStringData(0,alarmKeyColumn);
        // TD13673
        m_locationKey = data->getUnsignedLongData(0,locationKeyColumn);
		m_incidentID = data->getStringData( 0, incidentIDColumn );
        m_assetName = data->getStringData(0, assetNameColumn);

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        
        // Need to record that we now have valid data
        m_isValidData = true;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "MessageData::reload" );
    }

    void MessageData::applyChanges()
    {
        applyChanges(m_key);
    }

    void MessageData::applyChanges(const std::string& key)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "MessageData::applyChanges" );

        // Set up the boolean variables that specify if something is wrong. These checks have been
        // updated to reflect the constraints on the database (TD10246 and TD11450).
        bool descriptionValid = std::string("") != m_description;
        bool createTimeValid = ULONG_MAX != m_createTime.time;
        bool logTimeValid = ULONG_MAX != m_logTime.time;
        bool typeValid = std::string("") != m_type;
        bool keyValid = std::string("") != key;
        bool entityKeyValid = ULONG_MAX != m_entityKey;
        
        // Now need to determine which of the above tests failed, and add each failure
        // to a vector of failed field names. Unfortunately, this leads to a lot of if 
        // statements, but there's no way around it.
        std::vector<std::string> fieldNames;
        if (!descriptionValid)
        {
            fieldNames.push_back("DESCRIPTION");
        }
        if (!createTimeValid)
        {
            fieldNames.push_back("CREATETIME");
        }
        if (!logTimeValid)
        {
            fieldNames.push_back("LOGTIME");
        }
        if (!entityKeyValid)
        {
            fieldNames.push_back("ENTITYKEY");
        }
        if (!typeValid)
        {
            fieldNames.push_back("TYPE");
        }
        if (!keyValid)
        {
            fieldNames.push_back("KEY");
        }

        // Now, if we have at least one entry in the vector, something is not right.
        if (0 < fieldNames.size())
        {
                throw TA_Base_Core::DataConfigurationException("Message data not fully specified. Entity cannot be written to database"
                                                                ,fieldNames);
        }
        // Write the message to the database
        
        // Need to check if this is a new message or an existing message - 
        // as a new message will be inserted, while an existing message will be updated
        if (m_isNew) // This is a new entity
        {
            m_key = key;
            createNewMessage();
        }
        else // This is an existing entity
        {
            // Ensure this method has been called with the correct key (cannot change
            // an existing key)
            TA_ASSERT(key == m_key,"Cannot set the key for an existing Message");
            updateExistingMessage();
        }
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "MessageData::applyChanges" );
    }

    std::string MessageData::getLocalDatabaseName()
    {
        if (m_localDatabase.empty())
        {
            // have to find out what is the local db name
            // this should be the first in the connection list so grab it
            if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
            {
                throw DatabaseException("db-connection-file not set");
            }
            try
            {
				std::string strDbConnFile(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
				m_localDatabase = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strDbConnFile, Event_Ad, Write);

                /*DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
                DataConnections connections = connectionStrings.getConnectionList(Event_Ad, Write);
                m_localDatabase = connections[0].first;  */
            }
            catch(...) // most likely a file not found error
            {
                throw DatabaseException("Unable to find database connection infomation");
            }

        }
        return m_localDatabase;
    }

    // CL-21508 begin
    void  MessageData::createNewMessage()
    {
        if ( m_alarmKey.empty() )
        {
            createOthersMessage();
        }
        else
        {
            createAlarmNewMessage();
        }
    }

//     void MessageData::createNewMessage()
    void  MessageData::createOthersMessage()
    // CL-21508 end
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "MessageData::createNewMessage");
        

        // Now we can get on with writing the entity to the database.
        // Convert non-string elements to char*'s
		char entityKeyString[50] = {0}; 
		sprintf(entityKeyString, "%lu", m_entityKey);
        // TD13673
        char locationKeyString[50] = {0};
		sprintf(locationKeyString, "%lu", m_locationKey);

        // Convert time data into formatted strings
        struct tm createTimeStruct;
        // Note for code review: Which one of these should we use? Currently getting
        // the local time. Could get GMT instead.
        //struct tm t;
        //ACE_OS::gmtime_r(&currentTime, &t);

        // extract the time from the struct and make it into a string
        char createTimeString[24] = {0};
        if (ACE_OS::localtime_r(&m_createTime.time, &createTimeStruct) != NULL)
        {
	        sprintf(createTimeString, "%02d/%02d/%04d %02d:%02d:%02d.%03d", createTimeStruct.tm_mday, createTimeStruct.tm_mon + 1, 
		        createTimeStruct.tm_year + 1900, createTimeStruct.tm_hour, createTimeStruct.tm_min, createTimeStruct.tm_sec,
                m_createTime.millitm);
        }

        // do it again with the logtime
        struct tm logTimeStruct;
        char logTimeString[24] = {0};	// buffer [22] not long enough to contain "%02d/%02d/%04d %02d:%02d:%02d.%03d"
        if (ACE_OS::localtime_r(&m_logTime.time, &logTimeStruct) != NULL)
        {
	        sprintf(logTimeString, "%02d/%02d/%04d %02d:%02d:%02d.%03d", logTimeStruct.tm_mday, logTimeStruct.tm_mon + 1, 
		        logTimeStruct.tm_year + 1900, logTimeStruct.tm_hour, logTimeStruct.tm_min, logTimeStruct.tm_sec,
                m_logTime.millitm);
        }

        
        // Get the database connection
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Write);
        //std::string formatSQL = "BEGIN ";
		SQLStatement strSql;

		if( m_type.compare( TA_Base_Core::SchedulingAudit::SchedulingAddJob.getTypeKey())==0 
			|| m_type.compare( TA_Base_Core::SchedulingAudit::SchedulingDeleteJob.getTypeKey())==0 
			|| m_type.compare( TA_Base_Core::SchedulingAudit::SchedulingModifyJob.getTypeKey())==0 
			|| m_type.compare( TA_Base_Core::SchedulingAudit::SchedulingEnableJob.getTypeKey())==0 
			|| m_type.compare( TA_Base_Core::SchedulingAudit::SchedulingDisableJob.getTypeKey())==0 
			|| m_type.compare( TA_Base_Core::SchedulingAudit::SchedulingJobCompleted.getTypeKey())==0 
			|| m_type.compare( TA_Base_Core::SchedulingAudit::SchedulingJobStarting.getTypeKey())==0 )
		{     
			// generate the SQL string to write the entity (reuse the string)
			//formatSQL += "audit_data_queue_pkg.enqueue_audit_data('" + getLocalDatabaseName() + "','PUBLIC', '";
			//formatSQL += "insert into EVENT (message_id,CREATETIME,LOGTIME,DESCRIPTION,";
			//formatSQL += "PARAM_LIST,DETAILS,EVENTTYPE,ENTITYKEY,SESSIONKEY,ALARM_ID,LOCATIONKEY,INCIDENT_ID,ASSET_NAME) ";
			//formatSQL += "values (''" + m_key + "'',TO_TIMESTAMP(''";
			//formatSQL += std::string(createTimeString) + "'',''dd/mm/yyyy hh24:mi:ss.ff3''),";
			//formatSQL += "TO_TIMESTAMP(''" + std::string(logTimeString) + "'',''dd/mm/yyyy hh24:mi:ss.ff3''),''";
			//formatSQL += databaseConnection->escapeAQSQLString(m_description) + "'',''";
			//formatSQL += databaseConnection->escapeAQSQLString(m_paramString) + "'',''";
			//formatSQL += databaseConnection->escapeAQSQLString(m_details) + "'',''";
			//formatSQL += databaseConnection->escapeAQSQLString(m_type) + "'',";
			//formatSQL += std::string(entityKeyString) + ",''";
			//formatSQL += databaseConnection->escapeAQSQLString( m_sessionID ) + "'',''";
			//formatSQL += databaseConnection->escapeAQSQLString( m_alarmKey )+ "'',";
			//// TD13673
			//formatSQL += std::string(locationKeyString) + ",''";
			//formatSQL += databaseConnection->escapeAQSQLString( m_incidentID ) + "'',''";
			//formatSQL += databaseConnection->escapeAQSQLString( m_assetName ) + "'')";
			//formatSQL += "');";
			//formatSQL += "END;";
//			strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EVENT_Oracle_INSERT_17351, getLocalDatabaseName(), m_key, std::string(createTimeString),
//			strSql  = databaseConnection->prepareSQLStatement(EVENT_INSERT_17351, getLocalDatabaseName(), m_key, std::string(createTimeString),

		databaseConnection->prepareSQLStatement(strSql, EVENT_INSERT_17351, getLocalDatabaseName(), m_key, std::string(createTimeString),
				std::string(logTimeString), databaseConnection->escapeAQSQLString(m_description), databaseConnection->escapeAQSQLString(m_paramString), 
				databaseConnection->escapeAQSQLString(m_details), databaseConnection->escapeAQSQLString(m_type), std::string(entityKeyString),
				databaseConnection->escapeAQSQLString( m_sessionID ), databaseConnection->escapeAQSQLString( m_alarmKey ), std::string(locationKeyString),
				databaseConnection->escapeAQSQLString( m_incidentID ), databaseConnection->escapeAQSQLString( m_assetName ) );
		}
		else
		{
			// generate the SQL string to write the entity (reuse the string)
			//formatSQL += "audit_data_queue_pkg.enqueue_audit_data('" + getLocalDatabaseName() + "','GROUP', '";
			//formatSQL += "insert into EVENT (message_id,CREATETIME,LOGTIME,DESCRIPTION,";
			//formatSQL += "PARAM_LIST,DETAILS,EVENTTYPE,ENTITYKEY,SESSIONKEY,ALARM_ID,LOCATIONKEY,INCIDENT_ID,ASSET_NAME) ";
			//formatSQL += "values (''" + m_key + "'',TO_TIMESTAMP(''";
			//formatSQL += std::string(createTimeString) + "'',''dd/mm/yyyy hh24:mi:ss.ff3''),";
			//formatSQL += "TO_TIMESTAMP(''" + std::string(logTimeString) + "'',''dd/mm/yyyy hh24:mi:ss.ff3''),''";
			//formatSQL += databaseConnection->escapeAQSQLString(m_description) + "'',''";
			//formatSQL += databaseConnection->escapeAQSQLString(m_paramString) + "'',''";
			//formatSQL += databaseConnection->escapeAQSQLString(m_details) + "'',''";
			//formatSQL += databaseConnection->escapeAQSQLString(m_type) + "'',";
			//formatSQL += std::string(entityKeyString) + ",''";
			//formatSQL += databaseConnection->escapeAQSQLString( m_sessionID ) + "'',''";
			//formatSQL += databaseConnection->escapeAQSQLString( m_alarmKey )+ "'',";
			//// TD13673
			//formatSQL += std::string(locationKeyString) + ",''";
			//formatSQL += databaseConnection->escapeAQSQLString( m_incidentID ) + "'',''";
			//formatSQL += databaseConnection->escapeAQSQLString( m_assetName ) + "'')";
			//formatSQL += "');";
			//formatSQL += "END;";
//			strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EVENT_Oracle_INSERT_17352, getLocalDatabaseName(), m_key, std::string(createTimeString),
//			strSql  = databaseConnection->prepareSQLStatement(EVENT_INSERT_17352, getLocalDatabaseName(), m_key, std::string(createTimeString),

		databaseConnection->prepareSQLStatement(strSql, EVENT_INSERT_17352, getLocalDatabaseName(), m_key, std::string(createTimeString),
				std::string(logTimeString), databaseConnection->escapeAQSQLString(m_description), databaseConnection->escapeAQSQLString(m_paramString),
				databaseConnection->escapeAQSQLString(m_details), databaseConnection->escapeAQSQLString(m_type), std::string(entityKeyString),
				databaseConnection->escapeAQSQLString( m_sessionID ), databaseConnection->escapeAQSQLString( m_alarmKey ), std::string(locationKeyString),
				databaseConnection->escapeAQSQLString( m_incidentID ), databaseConnection->escapeAQSQLString( m_assetName ) );

		}
 
        databaseConnection->executeModification(strSql);  
        
        LOG ( SourceInfo, DebugUtil::FunctionExit, "MessageData::createNewMessage");
    }

    void MessageData::updateExistingMessage()
	{
		if ( m_alarmKey.empty() )
		{
			updateOthersExistingMessage();
		}
		else
		{
			updateAlarmExistingMessage();
		}
	}
	void MessageData::updateAlarmExistingMessage()
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MessageData::updateAlarmExistingMessage(),it should not be excuted.");
	}
	void MessageData::updateOthersExistingMessage()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "MessageData::updateExistingMessage");
        
        // Convert the non-string fields to char*'s
        // Now we can get on with writing the entity to the database.
        // Convert non-string elements to char*'s
		char entityKeyString[50] = {0}; 
		sprintf(entityKeyString, "%lu", m_entityKey);
        // TD13673
        char locationKeyString[50] = {0};
		sprintf(locationKeyString, "%lu", m_locationKey);
        
        // Convert time data into formatted strings
        struct tm createTimeStruct;
        // Note for code review: Which one of these should we use? Currently getting
        // the local time. Could get GMT instead.
        //struct tm t;
        //ACE_OS::gmtime_r(&currentTime, &t);

        // extract the time from the struct and make it into a string
        char createTimeString[24] = {0};
        if (ACE_OS::localtime_r(&m_createTime.time, &createTimeStruct) != NULL)
        {
	        sprintf(createTimeString, "%02d/%02d/%04d %02d:%02d:%02d.%03d", createTimeStruct.tm_mday, createTimeStruct.tm_mon + 1, 
		        createTimeStruct.tm_year + 1900, createTimeStruct.tm_hour, createTimeStruct.tm_min, createTimeStruct.tm_sec,
                m_createTime.millitm);
        }

        // do it again with the logtime
        struct tm logTimeStruct;
        char logTimeString[24] = {0};
        if (ACE_OS::localtime_r(&m_logTime.time, &logTimeStruct) != NULL)
        {
	        sprintf(logTimeString, "%02d/%02d/%04d %02d:%02d:%02d.%03d", logTimeStruct.tm_mday, logTimeStruct.tm_mon + 1, 
		        logTimeStruct.tm_year + 1900, logTimeStruct.tm_hour, logTimeStruct.tm_min, logTimeStruct.tm_sec,
                m_logTime.millitm);
        }

        // Get the database connetion
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Write);

		//std::string formatSQL = "BEGIN ";
		SQLStatement strSql;

		if( m_type.compare( TA_Base_Core::SchedulingAudit::SchedulingAddJob.getTypeKey())==0 
			|| m_type.compare( TA_Base_Core::SchedulingAudit::SchedulingDeleteJob.getTypeKey())==0 
			|| m_type.compare( TA_Base_Core::SchedulingAudit::SchedulingModifyJob.getTypeKey())==0 
			|| m_type.compare( TA_Base_Core::SchedulingAudit::SchedulingEnableJob.getTypeKey())==0 
			|| m_type.compare( TA_Base_Core::SchedulingAudit::SchedulingDisableJob.getTypeKey())==0 
			|| m_type.compare( TA_Base_Core::SchedulingAudit::SchedulingJobCompleted.getTypeKey())==0 
			|| m_type.compare( TA_Base_Core::SchedulingAudit::SchedulingJobStarting.getTypeKey())==0 )
		{
			//formatSQL += "audit_data_queue_pkg.enqueue_audit_data('" + getLocalDatabaseName() + "','PUBLIC', '";
			//formatSQL += "update EVENT set CREATETIME = TO_TIMESTAMP(''";
			//formatSQL += std::string(createTimeString) + "'',''dd/mm/yyyy hh24:mi:ss.ff3''),";
			//formatSQL += " LOGTIME = TO_TIMESTAMP(''" + std::string(logTimeString);
			//formatSQL += "'',''dd/mm/yyyy hh24:mi:ss.ff3''),";
			//formatSQL += " DESCRIPTION = ''" + databaseConnection->escapeAQSQLString(m_description);
			//formatSQL += "'', PARAM_LIST = ''" + databaseConnection->escapeAQSQLString(m_paramString);
			//formatSQL += "'', DETAILS = ''" + databaseConnection->escapeAQSQLString(m_details);
			//formatSQL += "'', EVENTTYPE = ''" + databaseConnection->escapeAQSQLString(m_type);
			//formatSQL += "'', ENTITYKEY = " + std::string( entityKeyString );
			//formatSQL += ", SESSIONKEY = ''" + databaseConnection->escapeAQSQLString(m_sessionID);
			//formatSQL += "'', ALARM_ID = ''" + databaseConnection->escapeAQSQLString( m_alarmKey );
			//// TD13673
			//formatSQL += "'', LOCATIONKEY = " + std::string( locationKeyString );
			//formatSQL += ", INCIDENT_ID = ''" + databaseConnection->escapeAQSQLString( m_incidentID );
			//formatSQL += "'', ASSET_NAME = ''" + databaseConnection->escapeAQSQLString( m_assetName );
			//formatSQL += "'' where message_id = ''" + m_key + "''";
			//formatSQL += "');";
			//formatSQL += "END;";
//			strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EVENT_Oracle_UPDATE_17201, getLocalDatabaseName(), std::string(createTimeString),
//			strSql  = databaseConnection->prepareSQLStatement(EVENT_UPDATE_17201, getLocalDatabaseName(), std::string(createTimeString),

		databaseConnection->prepareSQLStatement(strSql, EVENT_UPDATE_17201, getLocalDatabaseName(), std::string(createTimeString),
				std::string(logTimeString), databaseConnection->escapeAQSQLString(m_description), databaseConnection->escapeAQSQLString(m_paramString),
				databaseConnection->escapeAQSQLString(m_details), databaseConnection->escapeAQSQLString(m_type), std::string( entityKeyString ),
				databaseConnection->escapeAQSQLString(m_sessionID), databaseConnection->escapeAQSQLString( m_alarmKey ), std::string( locationKeyString ),
				databaseConnection->escapeAQSQLString( m_incidentID ), databaseConnection->escapeAQSQLString( m_assetName ), m_key);

		}
		else
		{
			//formatSQL += "audit_data_queue_pkg.enqueue_audit_data('" + getLocalDatabaseName() + "','GROUP', '";
			//formatSQL += "update EVENT set CREATETIME = TO_TIMESTAMP(''";
			//formatSQL += std::string(createTimeString) + "'',''dd/mm/yyyy hh24:mi:ss.ff3''),";
			//formatSQL += " LOGTIME = TO_TIMESTAMP(''" + std::string(logTimeString);
			//formatSQL += "'',''dd/mm/yyyy hh24:mi:ss.ff3''),";
			//formatSQL += " DESCRIPTION = ''" + databaseConnection->escapeAQSQLString(m_description);
			//formatSQL += "'', PARAM_LIST = ''" + databaseConnection->escapeAQSQLString(m_paramString);
			//formatSQL += "'', DETAILS = ''" + databaseConnection->escapeAQSQLString(m_details);
			//formatSQL += "'', EVENTTYPE = ''" + databaseConnection->escapeAQSQLString(m_type);
			//formatSQL += "'', ENTITYKEY = " + std::string( entityKeyString );
			//formatSQL += ", SESSIONKEY = ''" + databaseConnection->escapeAQSQLString(m_sessionID);
			//formatSQL += "'', ALARM_ID = ''" + databaseConnection->escapeAQSQLString( m_alarmKey );
			//// TD13673
			//formatSQL += "'', LOCATIONKEY = " + std::string( locationKeyString );
			//formatSQL += ", INCIDENT_ID = ''" + databaseConnection->escapeAQSQLString( m_incidentID );
			//formatSQL += "'', ASSET_NAME = ''" + databaseConnection->escapeAQSQLString( m_assetName );
			//formatSQL += "'' where message_id = ''" + m_key + "''";
			//formatSQL += "');";
			//formatSQL += "END;";
//			strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EVENT_Oracle_UPDATE_17202, getLocalDatabaseName(), std::string(createTimeString),
//			strSql  = databaseConnection->prepareSQLStatement(EVENT_UPDATE_17202, getLocalDatabaseName(), std::string(createTimeString),

		databaseConnection->prepareSQLStatement(strSql, EVENT_UPDATE_17202, getLocalDatabaseName(), std::string(createTimeString),
				std::string(logTimeString), databaseConnection->escapeAQSQLString(m_description), databaseConnection->escapeAQSQLString(m_paramString),
				databaseConnection->escapeAQSQLString(m_details), databaseConnection->escapeAQSQLString(m_type), std::string( entityKeyString ),
				databaseConnection->escapeAQSQLString(m_sessionID), databaseConnection->escapeAQSQLString( m_alarmKey ), std::string( locationKeyString ),
				databaseConnection->escapeAQSQLString( m_incidentID ), databaseConnection->escapeAQSQLString( m_assetName ), m_key);
		}

        databaseConnection->executeModification(strSql);
        // nothing more to be done here
        LOG ( SourceInfo, DebugUtil::FunctionExit, "MessageData::updateExistingMessage");
    }

   // CL-21508 begin
    void MessageData::setAlarmDescription( const std::string& alarmDescription )
    {
        m_alarmDescription = alarmDescription;
    }
    
    void MessageData::setAlarmValue( const std::string& alarmValue )
    {
        m_alarmValue = alarmValue;
    }
    
    void MessageData::setAlarmComments( const std::string& alarmComments )
    {
        m_alarmComments = alarmComments;
    }
    
    void MessageData::setAlarmMMSState( const std::string& alarmMMSState )
    {
        m_alarmMMSState = alarmMMSState;
    }
    
    void MessageData::setAlarmDSSState( const std::string& alarmDSSState )
    {
        m_alarmDSSState = alarmDSSState;
    }

    void MessageData::setAlarmAVLstate( const std::string& alarmAVLstate )
    {
        m_alarmAVLstate = alarmAVLstate;
    }

    void MessageData::setAlarmACKedBy( const std::string& alarmACKedBy )
    {
        m_alarmACKedBy = alarmACKedBy;
    }

    void MessageData::setAlarmStatus( const std::string& alarmStatus )
    {
        m_alarmStatus = alarmStatus;
    }
    void MessageData::setOMFlag( const std::string& OMFlag )
    {
        m_OMFlag = OMFlag;
    }

    void MessageData::setEventLelOrSeverityKey( const unsigned long eventLelOrSeverityKey )
    {
        m_eventLelOrSeverityKey = eventLelOrSeverityKey;
    }

    void MessageData::setAlarmTypeKey( const unsigned long alarmTypeKey )
    {
        m_alarmTypeKey= alarmTypeKey;
    }
	//update this SQL to insert EventID to EV_COMBINED for realtime event
    void MessageData::createAlarmNewMessage()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "MessageData::createNewMessageWithAlarm");

        // do it again with the logtime
        tm logTimeStruct;
        char logTimeString[24] = { '\0', };
        if (ACE_OS::localtime_r(&m_createTime.time, &logTimeStruct) != NULL)
        {
            sprintf(logTimeString, "%02d/%02d/%04d %02d:%02d:%02d.%03d", logTimeStruct.tm_mday, logTimeStruct.tm_mon + 1, 
                logTimeStruct.tm_year + 1900, logTimeStruct.tm_hour, logTimeStruct.tm_min, logTimeStruct.tm_sec,
                m_createTime.millitm);
        }
        // Get the database connection
        IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Write);
        std::stringstream formatSQL;

// 		const char* const Q_MARK = "''";
//         const char* const COMMA = ", ";
//        // generate the SQL string to write the entity
//         formatSQL << "BEGIN ";
//         formatSQL << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','GROUP', '";
//         formatSQL << "insert into ev_combined (SOURCE_TABLE, CREATETIME, ";
//         formatSQL << "LOCATION_KEY, SEVERITY_KEY, ASSET_NAME, DESCRIPTION, EVENT_TYPE_KEY, ";
//         formatSQL << "ALARM_ID, ALARM_TYPE_KEY, VALUE, MMS_STATE, DSS_STATE, AVL_STATE, ";
//         formatSQL << "ALARM_COMMENT, ALARM_ACK, ALARM_STATUS, SESSION_KEY, OPERATION_MODE, ENTITY_KEY) ";
//         formatSQL << "values (" << Q_MARK << 100 << Q_MARK << COMMA;
//         formatSQL << "TO_TIMESTAMP(" << Q_MARK << std::string(logTimeString) << Q_MARK << COMMA << Q_MARK << "dd/mm/yyyy hh24:mi:ss.ff3" << Q_MARK << "), ";
//         formatSQL << Q_MARK << m_locationKey << Q_MARK << COMMA << Q_MARK << m_eventLelOrSeverityKey << Q_MARK << COMMA;
//         formatSQL << Q_MARK << databaseConnection->escapeAQSQLString( m_assetName ) << Q_MARK << COMMA;
//         formatSQL << Q_MARK << databaseConnection->escapeAQSQLString( m_alarmDescription ) << Q_MARK << COMMA;
//         formatSQL << "TO_NUMBER(" << Q_MARK << databaseConnection->escapeAQSQLString( m_type ) << Q_MARK << "), ";
//         formatSQL << Q_MARK << databaseConnection->escapeAQSQLString( m_alarmKey ) << Q_MARK << COMMA;
//         formatSQL << Q_MARK << m_alarmTypeKey << Q_MARK << COMMA;
//         formatSQL << Q_MARK << databaseConnection->escapeAQSQLString( m_alarmValue ) << Q_MARK << COMMA;
//         formatSQL << Q_MARK << databaseConnection->escapeAQSQLString( m_alarmMMSState ) << Q_MARK << COMMA;
//         formatSQL << Q_MARK << databaseConnection->escapeAQSQLString( m_alarmDSSState ) << Q_MARK << COMMA;
//         formatSQL << Q_MARK << databaseConnection->escapeAQSQLString( m_alarmAVLstate ) << Q_MARK << COMMA;
//         formatSQL << Q_MARK << databaseConnection->escapeAQSQLString( m_alarmComments ) << Q_MARK << COMMA;
//         formatSQL << Q_MARK << databaseConnection->escapeAQSQLString( m_alarmACKedBy ) << Q_MARK << COMMA;
//         formatSQL << Q_MARK << databaseConnection->escapeAQSQLString( m_alarmStatus ) << Q_MARK << COMMA;
//         formatSQL << Q_MARK << databaseConnection->escapeAQSQLString( m_sessionID ) << Q_MARK << COMMA;
// 		formatSQL << Q_MARK << databaseConnection->escapeAQSQLString( m_OMFlag) << Q_MARK << COMMA;
//         formatSQL << Q_MARK << m_entityKey << Q_MARK << ") ";
//         formatSQL << "');";
// 		formatSQL << "END;";

		//EVENT_INSERT_17353=CALL enqueue_audit_data('%s','GROUP', 'insert into ev_combined (SOURCE_TABLE, CREATETIME,LOCATION_KEY, 
		//	+ SEVERITY_KEY, ASSET_NAME, DESCRIPTION, EVENT_TYPE_KEY,

// 			+ ALARM_ID, ALARM_TYPE_KEY, VALUE, MMS_STATE, DSS_STATE, AVL_STATE,
// 			+ ALARM_COMMENT, ALARM_ACK, ALARM_STATUS, SESSION_KEY, OPERATION_MODE, ENTITY_KEY) 
// 			+ values (%s, STR_TO_DATE(''%s'',''%%d/%%m/%%Y %%H:%%i:%%S.%%f''), %s,
// 			+ %s,''%s'',''%s'',%s,
// 			+ ''%s'',%s,''%s'',''%s'',''%s'',''%s'',
// 			+ ''%s'',''%s'',''%s'',''%s'',''%s'',%s
// 			)') 

		SQLStatement strSql;

//		strSql  = databaseConnection->prepareSQLStatement(EVENT_INSERT_17353, getLocalDatabaseName(), 100, databaseConnection->escapeAQSQLString(logTimeString),m_locationKey,

		//change EVENT_INSERT_17353 for realtime event, we insert one more column: event_id
		databaseConnection->prepareSQLStatement(strSql, EVENT_INSERT_17353, getLocalDatabaseName(), 100, m_key, databaseConnection->escapeAQSQLString(logTimeString),m_locationKey,
			m_eventLelOrSeverityKey,  databaseConnection->escapeAQSQLString(m_assetName), databaseConnection->escapeAQSQLString(m_alarmDescription),  atol(m_type.c_str()),
			databaseConnection->escapeAQSQLString( m_alarmKey ),m_alarmTypeKey, databaseConnection->escapeAQSQLString( m_alarmValue ),
			databaseConnection->escapeAQSQLString(m_alarmMMSState),databaseConnection->escapeAQSQLString(m_alarmDSSState),databaseConnection->escapeAQSQLString(m_alarmAVLstate),
			databaseConnection->escapeAQSQLString( m_alarmComments ), databaseConnection->escapeAQSQLString( m_alarmACKedBy ), 
			databaseConnection->escapeAQSQLString( m_alarmStatus ), databaseConnection->escapeAQSQLString( m_sessionID ), databaseConnection->escapeAQSQLString( m_OMFlag ),m_entityKey );


        databaseConnection->executeModification(strSql);
        LOG ( SourceInfo, DebugUtil::FunctionExit, "MessageData::createNewMessageWithAlarm");
    }
    // CL-21058 end
} // closes TA_Base_Core
