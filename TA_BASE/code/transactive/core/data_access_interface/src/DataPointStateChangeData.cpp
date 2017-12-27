/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/DataPointStateChangeData.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2016/01/18 15:18:43 $
 * Last modified by:  $Author: Ouyang $
 * 
 * This class implements the IDataPointStateChangeData interface. It provides access to the lo_datapt_state_change table
 */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined(_MSC_VER)

#include "ace/OS.h"

//#include <ctime>

#include "core/data_access_interface/src/DataPointStateChangeData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
//#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "core/uuid/src/TAuuid.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataConfigurationException.h"


namespace TA_Base_Core
{

    DataPointStateChangeData::DataPointStateChangeData(const std::string& key, const std::map< std::string, unsigned long > & alarmMessageDataPointNameToPKeyMap)
        : m_key(key), m_timestamp(0), m_logtime(0), m_entityKey(ULONG_MAX), m_value(""), m_status(""), m_isValidData(false), m_isNewEntry(false),
        m_alarmMessageId(0), m_generalMessageId(-1), m_alarmMessageDataPointNameToPKeyMap(alarmMessageDataPointNameToPKeyMap), m_eventTypeKey(ULONG_MAX)
    {
    }

    DataPointStateChangeData::DataPointStateChangeData(const std::map< std::string, unsigned long > & alarmMessageDataPointNameToPKeyMap)
        : m_key(""), m_timestamp(0), m_logtime(0), 
        m_entityKey(ULONG_MAX), m_value(""), m_status(""), m_isValidData(true), m_isNewEntry(true),
        m_alarmMessageId(0), m_generalMessageId(-1), m_alarmMessageDataPointNameToPKeyMap(alarmMessageDataPointNameToPKeyMap), m_eventTypeKey(ULONG_MAX)
    {
        m_key = TA_Base_Core::TA_Uuid::generateUUID();
    }

    DataPointStateChangeData::DataPointStateChangeData(const std::string& key, const time_t timeStamp, 
        const time_t logTime, const unsigned long entityKey,
        const std::string& value, const std::string& status, const std::string& entityName, 
        const std::string& subsystemName, const std::string& locationName,
        const std::map< std::string, unsigned long > & alarmMessageDataPointNameToPKeyMap)
        : m_key(key), m_timestamp(timeStamp), m_logtime(logTime), m_entityKey(entityKey), m_value(value), m_status(status), 
        m_isValidData(false), m_isNewEntry(false), m_entityName(entityName), m_subsystemName(subsystemName), 
        m_locationName(locationName), m_alarmMessageId(0), m_generalMessageId(-1),
        m_alarmMessageDataPointNameToPKeyMap(alarmMessageDataPointNameToPKeyMap), m_eventTypeKey(ULONG_MAX)
    {
    }

    DataPointStateChangeData::~DataPointStateChangeData()
    {

    }

    const std::string& DataPointStateChangeData::getKey() const
    {
        return m_key;
    }

    const time_t DataPointStateChangeData::getTimestamp()
    {
        if (m_isValidData)
        {
            TA_ASSERT(0 != m_timestamp,"The timestamp must be set before it can be read");
        }
        else
        {
            reload();
        }
        return m_timestamp;
    }

    void DataPointStateChangeData::setTimestamp(const time_t timestamp)
    {
        if (!m_isValidData)
        {
            reload();
        }
        m_timestamp = timestamp;
    }

    const time_t DataPointStateChangeData::getLogtime()
    {
        if (m_isValidData)
        {
            TA_ASSERT(0 != m_logtime,"The logtime must be set before it can be read");
        }
        else
        {
            reload();
        }
        return m_logtime;
    }

    void DataPointStateChangeData::setLogtime(const time_t logtime)
    {
        if (!m_isValidData)
        {
            reload();
        }
        m_logtime = logtime;
    }

    const unsigned long DataPointStateChangeData::getEntityKey()
    {
        if (m_isValidData)
        {
            TA_ASSERT(ULONG_MAX != m_entityKey,"The entity key must be set before it can be read");
        }
        else
        {
            reload();
        }
        return m_entityKey;
    }

    void DataPointStateChangeData::setEntityKey(const unsigned long entityKey)
    {
        if (!m_isValidData)
        {
            reload();
        }
        m_entityKey = entityKey;
    }

    const std::string& DataPointStateChangeData::getValue()
    {
        if (m_isValidData)
        {
            TA_ASSERT("" != m_value,"The value must be set before it can be read");
        }
        else
        {
            reload();
        }
        return m_value;
    }

    void DataPointStateChangeData::setValue(const std::string& value)
    {
        if (!m_isValidData)
        {
            reload();
        }
        m_value = value;
    }

    const std::string& DataPointStateChangeData::getStatus()
    {
        if (m_isValidData)
        {
            TA_ASSERT("" != m_status,"The status must be set before it can be read");
        }
        else
        {
            reload();
        }
        return m_status;
    }

    void DataPointStateChangeData::setStatus(const std::string& value)
    {
        if (!m_isValidData)
        {
            reload();
        }
        m_status = value;
    }
    
    std::string& DataPointStateChangeData::getSubsystemName()
    {
        if (m_isValidData)
        {
            TA_ASSERT("" != m_subsystemName,"The status must be set before it can be read");
        }
        else
        {
            reload();
        }
        return m_subsystemName;
    }

    void DataPointStateChangeData::setSubsystemName(const std::string& subsystemName)
    {
        if (!m_isValidData)
        {
            reload();
        }
        m_subsystemName = subsystemName;
    }
       
    std::string& DataPointStateChangeData::getLocationName()
    {
        if (m_isValidData)
        {
            TA_ASSERT("" != m_locationName,"The status must be set before it can be read");
        }
        else
        {
            reload();
        }
        return m_locationName;
    }

    void DataPointStateChangeData::setLocationName(const std::string& locationName)
    {
        if (!m_isValidData)
        {
            reload();
        }
        m_locationName = locationName;
    }
        
    std::string& DataPointStateChangeData::getEntityName()
    {
        if (m_isValidData)
        {
            TA_ASSERT("" != m_entityName,"The status must be set before it can be read");
        }
        else
        {
            reload();
        }
        return m_entityName;
    }

    void DataPointStateChangeData::setEntityName(const std::string& entityName)
    {
        if (!m_isValidData)
        {
            reload();
        }
        m_entityName = entityName;
    }

    // Raymond Pau++
    // Bug#802 TD7194
    void DataPointStateChangeData::setMessage(long generalMessage)
    {
        FUNCTION_ENTRY("setMessage");

        m_generalMessageId = generalMessage;
        m_alarmMessageId = 0;
        
        FUNCTION_EXIT;
    }

    void DataPointStateChangeData::setMessage(const std::string& messageType)
    {
        FUNCTION_ENTRY("setMessage");

        std::map< std::string, unsigned long >::const_iterator it =
            m_alarmMessageDataPointNameToPKeyMap.find(messageType);

        if (it != m_alarmMessageDataPointNameToPKeyMap.end())
        {
            m_alarmMessageId = it->second;
            m_generalMessageId = -1;
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Unknown messageType - %s", messageType.c_str());
        }

        FUNCTION_EXIT;
    }

    
    const unsigned long DataPointStateChangeData::getEventTypeKey()
    {
        if (m_isValidData)
        {
            TA_ASSERT(ULONG_MAX != m_eventTypeKey, "The entity key must be set before it can be read");
        }
        else
        {
            reload();
        }
        return m_eventTypeKey;
    }

    void DataPointStateChangeData::setEventTypeKey(const unsigned long eventTypeKey)
    {
        if (!m_isValidData)
        {
            reload();
        }
        m_eventTypeKey = eventTypeKey;
    }
    // Bug#802 TD7194
    // ++Raymond Pau

    void DataPointStateChangeData::invalidate()
    {
        FUNCTION_ENTRY("invalidate");
        // Cannot invalidate a new entry (it hasn't been written to the database, so it will never successfull reload).
        TA_ASSERT(!m_isNewEntry,"Attempted to invalidate a new data point change object");

        m_isValidData = false;
        FUNCTION_EXIT;
    }

    std::string DataPointStateChangeData::getLocalDatabaseName()
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

    void DataPointStateChangeData::reload()
    {
        FUNCTION_ENTRY("reload");
        TA_ASSERT(!m_isNewEntry,"Cannot reload an entry that has not been written to the database");
        
        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

        /*std::string sql = "select TO_CHAR(TIMESTAMP,'YYYYMMDDHH24MISS'),";
        sql += "TO_CHAR(LOGTIME,'YYYYMMDDHH24MISS'),ENTITYKEY,VALUE,STATUS,ALARMMESSAGEKEY,GENERALMESSAGEKEY";
        sql += " from lo_datapt_state_change where lodsch_id = '" + m_key + "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), lo_datapt_state_change_Oracle_SELECT_13553, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(LO_DATAPT_STATE_CHANGE_SELECT_13553, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, LO_DATAPT_STATE_CHANGE_SELECT_13553, m_key);

        std::string timestampColumn("TIMESTAMP");
        std::string logtimeColumn("LOGTIME");
        std::string entityKeyColumn("ENTITYKEY");
        std::string valueColumn("VALUE");
        std::string statusColumn("STATUS");
        std::string alarmMessageKeyColumn("ALARMMESSAGEKEY");
        std::string generalMessageKeyColumn("GENERALMESSAGEKEY");
        std::vector<std::string> columnNames;
        columnNames.push_back(timestampColumn);
        columnNames.push_back(logtimeColumn);
        columnNames.push_back(entityKeyColumn);
        columnNames.push_back(valueColumn);
        columnNames.push_back(statusColumn);
        columnNames.push_back(alarmMessageKeyColumn);
        columnNames.push_back(generalMessageKeyColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified lodsch_id
        {
            delete data;
            data = NULL;

            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "No messasge found for message_id = %s",m_key.c_str());			
            throw DataException(reasonMessage,DataException::NO_VALUE,"");
        }
        else if (1 < data->getNumRows()) // More than one entry found for the lodsch_id
        {
            delete data;
            data = NULL;
            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More than one message found for message_id = %s",m_key.c_str());			
            throw DataException(reasonMessage,DataException::NOT_UNIQUE,"");
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_timestamp = data->getDateData(0,timestampColumn);
        m_logtime = data->getDateData(0,logtimeColumn);
        m_entityKey = data->getUnsignedLongData(0,entityKeyColumn);
        m_value = data->getStringData(0,valueColumn);
        m_status = data->getStringData(0,statusColumn);

        try
        {
            m_alarmMessageId = data->getUnsignedLongData(0,alarmMessageKeyColumn);
        }
        catch(const DataException &)
        {
            m_alarmMessageId = 0;
        }

        try
        {
            m_generalMessageId = data->getUnsignedLongData(0,generalMessageKeyColumn);
        }
        catch(const DataException &)
        {
            m_generalMessageId = -1;
        }

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        
        // Need to record that we now have valid data
        m_isValidData = true;

        FUNCTION_EXIT;
    }

    void DataPointStateChangeData::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");
        
        // Set up the boolean variables that specify if something is wrong.
        bool timestampValid = ULONG_MAX != m_timestamp;
        bool logtimeValid = ULONG_MAX != m_logtime;
        bool entityKeyValid = ULONG_MAX != m_entityKey;
        bool valueValid = 0 != m_value.compare("");
        bool statusValid = 0 != m_status.compare("");

        // Determine if any of the tests failed, and if so, add the name of the field they represent to a vector.
        std::vector<std::string> fieldNames;

        if (!timestampValid)
        {
            fieldNames.push_back("TIMESTAMP");
        }
        if (!logtimeValid)
        {
            fieldNames.push_back("LOGTIME");
        }
        if (!entityKeyValid)
        {
            fieldNames.push_back("ENTITYKEY");
        }
        if (!valueValid)
        {
            fieldNames.push_back("VALUE");
        }
        if (!statusValid)
        {
            fieldNames.push_back("STATUS");
        }

        // If there is one or more entry in the vector, then at least one of the tests failed, and an exception need to be thrown.
        if (0 < fieldNames.size())
        {
            // Put all the field names into a vector
            std::string fields("");
            for (unsigned long i = 0;i < fieldNames.size();i++)
            {
                fields += fieldNames[i] + ", ";
            }

            // Then pull off the last two characters (don't need the trailing comma and space)
            fields.resize(fields.size()-2);

            std::string message = "Message data not fully specified. Entity cannot be written to database due to the following fields: " + fields;
            TA_THROW(TA_Base_Core::DataConfigurationException(message.c_str(),fieldNames));
        }

        // Need to determine if this is a new entry, or an existing one, as they are treated differently.
        if (m_isNewEntry)
        {
            addNewStateChange();
        }
        else
        {
            modifyExistingStateChange();
        }

        FUNCTION_EXIT;
    }

    void DataPointStateChangeData::addNewStateChange()
    {
        // This is a new entry so need to use the SQL "INSERT" statement
        
        // First need to convert the time data into formatted strings.
        struct tm timestampStruct;
        // Two options are available for time - GMT or localtime. Using localtime for now, but to 
        // use GMT simply comment out the line above this comment, and uncomment the line below it.
        //struct tm timestampStruct;
        //ACE_OS::gmtime_r(&m_timestamp, &timestampeStruct);

        // extract the time from the struct and make it into a string
        char timestampString[22] = {0};
        if (ACE_OS::localtime_r(&m_timestamp, &timestampStruct) != NULL)
        {
	        sprintf(timestampString, "%02d/%02d/%04d %02d:%02d:%02d", timestampStruct.tm_mday, timestampStruct.tm_mon + 1, 
		        timestampStruct.tm_year + 1900, timestampStruct.tm_hour, timestampStruct.tm_min, timestampStruct.tm_sec);
        }

        // do it again with the logtime
        struct tm logtimeStruct;
        //struct tm logtimeStruct;
        //ACE_OS::gmtime_r(&m_logtime,logtimeStruct);
        char logtimeString[24] = {0};
        if (ACE_OS::localtime_r(&m_logtime, &logtimeStruct) != NULL)
        {
	        sprintf(logtimeString, "%02d/%02d/%04d %02d:%02d:%02d", logtimeStruct.tm_mday, logtimeStruct.tm_mon + 1, 
		        logtimeStruct.tm_year + 1900, logtimeStruct.tm_hour, logtimeStruct.tm_min, logtimeStruct.tm_sec);
        }

        // Get the database connection
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Write);
        
        std::ostringstream ssAlarmMessageId;
        if (m_alarmMessageId == 0)
        {
            ssAlarmMessageId << "NULL";
        }
        else
        {
            ssAlarmMessageId << m_alarmMessageId;
        }
   
        std::ostringstream ssGeneralMessageId;
        if (m_generalMessageId == -1)
        {
            ssGeneralMessageId << "NULL";
        }
        else
        {
            ssGeneralMessageId << m_generalMessageId;
        }
        // generate the SQL string to write the entry
        /*std::ostringstream formatSQL;
        formatSQL << "BEGIN ";
        formatSQL << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','GROUP', '";
        formatSQL << "INSERT INTO lo_datapt_state_change (lodsch_id,timestamp,logtime,entitykey,";
        formatSQL << "value,status,alarmmessagekey,generalmessagekey,eventtypekey) VALUES (''" << m_key << "'',TO_DATE(''" << timestampString;
        formatSQL << "'',''dd/mm/yyyy hh24:mi:ss''),TO_DATE(''" << logtimeString << "'',''dd/mm/yyyy hh24:mi:ss''),";
		formatSQL << m_entityKey << ",''";
        formatSQL << databaseConnection->escapeAQSQLString(m_value) << "'',''";
        formatSQL << databaseConnection->escapeAQSQLString(m_status) <<  "'',";
        formatSQL << ssAlarmMessageId.str() << ",";
        formatSQL << ssGeneralMessageId.str() << ",";
        formatSQL << m_eventTypeKey << ")";
        
        formatSQL << "');";
        formatSQL << "END;";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), lo_datapt_state_change_Oracle_INSERT_13851, getLocalDatabaseName(),
//		std::string strSql  = databaseConnection->prepareSQLStatement(LO_DATAPT_STATE_CHANGE_INSERT_13851, getLocalDatabaseName(),
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, LO_DATAPT_STATE_CHANGE_INSERT_13851, getLocalDatabaseName(),
			m_key, timestampString, logtimeString, m_entityKey, databaseConnection->escapeAQSQLString(m_value), databaseConnection->escapeAQSQLString(m_status),
			ssAlarmMessageId.str(), ssGeneralMessageId.str(), m_eventTypeKey);
        databaseConnection->executeModification(strSql);  

        m_isNewEntry = false;
    }

    void DataPointStateChangeData::modifyExistingStateChange()
    {
         // This is an existing entry so need to use the SQL "UPDATE" statement
        
        // First need to convert the time data into formatted strings.
        struct tm timestampStruct;
        // Two options are available for time - GMT or localtime. Using localtime for now, but to 
        // use GMT simply comment out the line above this comment, and uncomment the line below it.
        //struct tm timestampStruct;
        //ACE_OS::gmtime_r(&m_timestamp, &timestampStruct);

        // extract the time from the struct and make it into a string
        char timestampString[22] = {0};
        if (ACE_OS::localtime_r(&m_timestamp, &timestampStruct) != NULL)
        {
	        sprintf(timestampString, "%02d/%02d/%04d %02d:%02d:%02d", timestampStruct.tm_mday, timestampStruct.tm_mon + 1, 
		        timestampStruct.tm_year + 1900, timestampStruct.tm_hour, timestampStruct.tm_min, timestampStruct.tm_sec);
        }

        // do it again with the logtime
        struct tm logtimeStruct;
        //struct tm logtimeStruct;
        //ACE_OS::gmtime_r(&m_logtime, &logtimeStruct);
        char logtimeString[24] = {0};
        if (ACE_OS::localtime_r(&m_logtime, &logtimeStruct) != NULL)
        {
	        sprintf(logtimeString, "%02d/%02d/%04d %02d:%02d:%02d", logtimeStruct.tm_mday, logtimeStruct.tm_mon + 1, 
		        logtimeStruct.tm_year + 1900, logtimeStruct.tm_hour, logtimeStruct.tm_min, logtimeStruct.tm_sec);
        }

        // Get the database connection
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Write);
        
        std::ostringstream ssAlarmMessageId;
        if (m_alarmMessageId == 0)
        {
            ssAlarmMessageId << "NULL";
        }
        else
        {
            ssAlarmMessageId << m_alarmMessageId;
        }
   
        std::ostringstream ssGeneralMessageId;
        if (m_generalMessageId == -1)
        {
            ssGeneralMessageId << "NULL";
        }
        else
        {
            ssGeneralMessageId << m_generalMessageId;
        }

        // generate the SQL string to write the entry
       /* std::ostringstream formatSQL;
        formatSQL << "BEGIN ";
        formatSQL << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','GROUP', '";
        formatSQL << "update lo_datapt_state_change set timestamp = TO_DATE(''" << timestampString;
        formatSQL << "'',''dd/mm/yyyy hh24:mi:ss''),logtime = TO_DATE(''" << logtimeString << "'',''dd/mm/yyyy hh24:mi:ss''),";
        formatSQL << "entitykey = " << m_entityKey << ",value = ''" << databaseConnection->escapeAQSQLString(m_value);
        formatSQL << "'',status = ''" << databaseConnection->escapeAQSQLString(m_status) << "''";
        formatSQL << ",alarmmessagekey = " << ssAlarmMessageId.str();
        formatSQL << ",generalmessagekey = " << ssGeneralMessageId.str();
        formatSQL << ",eventtypekey = " << m_eventTypeKey;
        formatSQL << " where lodsch_id = ''" << m_key << "''";
        formatSQL << " ');";
        formatSQL << "END;";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), lo_datapt_state_change_Oracle_UPDATE_13701, 
//		std::string strSql  = databaseConnection->prepareSQLStatement(LO_DATAPT_STATE_CHANGE_UPDATE_13701, 
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, LO_DATAPT_STATE_CHANGE_UPDATE_13701,
			getLocalDatabaseName(), timestampString, logtimeString, m_entityKey,databaseConnection->escapeAQSQLString(m_value),
			databaseConnection->escapeAQSQLString(m_status), ssAlarmMessageId.str(), ssGeneralMessageId.str(), m_eventTypeKey, m_key);
        databaseConnection->executeModification(strSql);
    }
}
