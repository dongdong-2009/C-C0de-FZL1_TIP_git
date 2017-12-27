/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/DataPointStateChangeAccessFactory.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * DataPointStateChangeAccessFactory is a singleton that is used to retrieve DataPointStateChangeData objects from the 
 * database.
 * It provides read-write objects already in the database, and creates new read-write objects for addition to the database.
 */


#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined(_MSC_VER)

#include "ace/OS.h" 

//#include <ctime>
#include <sstream>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IData.h"

#include "core/data_access_interface/src/DataPointStateChangeAccessFactory.h"
#include "core/data_access_interface/src/DataPointStateChangeData.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"

#include "core/threads/src/Thread.h"

#include "core/synchronisation/src/ThreadGuard.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    DataPointStateChangeAccessFactory* DataPointStateChangeAccessFactory::s_instance = NULL;
    NonReEntrantThreadLockable DataPointStateChangeAccessFactory::s_singletonLock;

    DataPointStateChangeAccessFactory& DataPointStateChangeAccessFactory::getInstance()
    {
        TA_THREADGUARD(s_singletonLock);

        if (NULL == s_instance)
        {
            s_instance = new DataPointStateChangeAccessFactory();
        }

        return *s_instance;
    }


    void DataPointStateChangeAccessFactory::getDataPointStateChangeBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IDataPointStateChangeData*>& stateChangePointers)
    {
        // get a connection to the database
        //IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
		TA_ASSERT(0 != pDatabase, "Database connection is null");

        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn = "lodsch_id";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = pDatabase->executeQuery(sql,columnNames);

        // Create the vector of pointers to IDataPointStateChangeData
        for (unsigned long i = 0; i < data->getNumRows() ; ++i )
        {
            std::string stateChangeKey = data->getStringData(i, keyColumn);
            stateChangePointers.push_back( new DataPointStateChangeData(stateChangeKey, m_alarmMessageDataPointNameToPKeyMap) );
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;
    }


    IDataPointStateChangeData* DataPointStateChangeAccessFactory::getDataPointStateChange(const std::string& key)
    {
		FUNCTION_ENTRY("getDataPointStateChange");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

        // create the SQL string to retrieve the key of the message
        // this is a check to ensure an message with the specified key
		/*std::ostringstream sql;
        sql << "select lodsch_id from lo_datapt_state_change where lodsch_id = '" << databaseConnection->escapeQueryString(key) << "'";*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), lo_datapt_state_change_STD_SELECT_13501,databaseConnection->escapeQueryString(key) );
//		std::string sql  = databaseConnection->prepareSQLStatement(LO_DATAPT_STATE_CHANGE_SELECT_13501,databaseConnection->escapeQueryString(key) );
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, LO_DATAPT_STATE_CHANGE_SELECT_13501,databaseConnection->escapeQueryString(key) );
//		TA_ASSERT(sql.size() > 0, "DataPointStateChangeAccessFactory::getDataPointStateChange(): Prepare SQL Statement error");

		std::vector<IDataPointStateChangeData*> stateChangePointers;
		getDataPointStateChangeBySql(databaseConnection, sql, stateChangePointers);

		if ( 0 == stateChangePointers.size())
        {
            std::ostringstream message;
			message << "No data found for StateChangePointer with key " << key;
           // TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql ) );
            
			std::string strExSql;
			int nExDbType= databaseConnection->getDbServerType();
			switch (nExDbType)
			{
			case enumOracleDb:
				strExSql = (sql.strCommon.empty() ? sql.strOracle : sql.strCommon);
				break;
			case enumMysqlDb:
				strExSql = (sql.strCommon.empty() ? sql.strMySQL : sql.strCommon);
				break;		
			}
            TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, strExSql ) );
        }

		TA_ASSERT(1 == stateChangePointers.size(), "StateChangePointers key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;		
		return stateChangePointers[0];
    }


    std::vector<IDataPointStateChangeData*> DataPointStateChangeAccessFactory::getDataPointStateChanges()
    {
		FUNCTION_ENTRY("DataPointStateChangeAccessFactory");

		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
        // create the SQL string to retrieve the key of the message
        // this is a check to ensure an message with the specified
        // message_id actually exists.
		/*std::ostringstream sql;
        sql << "select lodsch_id from lo_datapt_state_change";*/
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), lo_datapt_state_change_STD_SELECT_13502);
//		std::string sql  = databaseConnection->prepareSQLStatement(LO_DATAPT_STATE_CHANGE_SELECT_13502);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, LO_DATAPT_STATE_CHANGE_SELECT_13502);
//		TA_ASSERT(sql.size() > 0, "DataPointStateChangeAccessFactory::getDataPointStateChanges(): Prepare SQL Statement error");

		std::vector<IDataPointStateChangeData*> stateChangePointers;
		getDataPointStateChangeBySql(databaseConnection, sql, stateChangePointers);

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;		
		return stateChangePointers;
	}

    std::vector<IDataPointStateChangeData*> DataPointStateChangeAccessFactory::getStateChangesBetweenTimes
        (const time_t fromTime,const time_t toTime, 
        const unsigned long maxEvents,
        StateChangeFilter* filter, 
        IDatabaseCallbackUser* callbackUser)
    {
        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
		//int nDbType = databaseConnection->getDbServerType();

        // Calculate the times as strings...
        //std::string timeSql("");
		SQLTypeAdapter sqlParamTimeStr;

        // Use ACE routine ctime() which always returns a 26-character string:
        // Wek Mon DD HH:MM:SS YYYY\n\0
        const unsigned int bufLen = 27;
        char buf[bufLen];
        std::string fromTimeString(ACE_OS::ctime_r(&fromTime,buf,bufLen));

        // and remove the characters we don't want - staring with the last two
        fromTimeString.erase(24,2);

        // and then the first 4
        fromTimeString.erase(0,4);
       
        // If the alarmID filter is set, that is a special case, and the time filter is ignored (we want to see the alarm regardless of when
        // it happened).
        //if (0 != fromTime)
        /*{
			if (enumOracleDb == nDbType)
				timeSql = " dp.timestamp > TO_DATE('" + fromTimeString + "','Mon DD HH24:MI:SS YYYY')";
			else
				timeSql = " dp.timestamp > STR_TO_DATE('" + fromTimeString + "','%b %d %H:%i:%S %Y')";
        }*/
		sqlParamTimeStr.add(std::string(" dp.timestamp "), fromTimeString, enumDateFormat_5, enumFuncFormat_2, enumOperKey_ABOVE);        

        if (0 != toTime)
        {
            const unsigned int bufLen = 27;
            char buf[bufLen];
            // if the toTime is not the default, then there is a restriction on that too
            std::string toTimeString(ACE_OS::ctime_r(&fromTime,buf,bufLen));

            // and remove the characters we don't want - staring with the last two
            toTimeString.erase(24,2);

            // and then the first 4
            toTimeString.erase(0,4);

            // If the from time filter was added need to put in an "and" here
            if (0 != fromTime)
            {
                //timeSql += " and";
				sqlParamTimeStr.add(std::string(" and "));
            }			

			/*if (enumOracleDb == nDbType)
				timeSql += " timestamp < TO_DATE('" + toTimeString + "','Mon DD HH24:MI:SS YYYY')";
			else
				timeSql += " timestamp < STR_TO_DATE('" + toTimeString + "','%b %d %H:%i:%S %Y')";*/
			sqlParamTimeStr.add(std::string(" timestamp "), toTimeString, enumDateFormat_5, enumFuncFormat_2, enumOperKey_BELOW);
        }
		else
		{
			/*if ( enumOracleDb == nDbType)
				timeSql += " and dp.timestamp <= sysdate";
			else
				timeSql += " and dp.timestamp <= sysdate()";*/
			
			sqlParamTimeStr.add(enumFuncFormat_3, std::string(" and dp.timestamp "), enumOperKey_LETHAN);
		}

        // If the filter object has been set, work out the various where clauses
        std::ostringstream filterSql;
        std::ostringstream tablesSql;
        //tablesSql << " from lo_datapt_state_change dp";
        if (NULL != filter)
        {
            bool hasEntity = false;
            unsigned int i = 0;

            // Entity name
            if (0 < filter->entityName.length())
            {
                if (!hasEntity)
                {
                    tablesSql << ", entity ent";
                    hasEntity = true;
                }

                filterSql << " and dp.entitykey = ent.pkey and ent.name like '" << databaseConnection->escapeQueryString(filter->entityName) << "'";
            }

            // Values
            if (0 < filter->value.size())
            {
                filterSql << " and (dp.value = '";
                for (i = 0;i < filter->value.size();i++)
                {
                    filterSql << databaseConnection->escapeQueryString(filter->value[i]);
                
                    if (i != filter->value.size() - 1)
                    {
                        filterSql << "' or dp.value = '";
                    }
                }
                filterSql << "')";
            }

            // Status(es)
            if (0 < filter->status.size())
            {
                filterSql << " and (dp.status = '";
                for (i = 0;i < filter->status.size();i++)
                {
                    filterSql << databaseConnection->escapeQueryString(filter->status[i]);
                
                    if (i != filter->status.size() - 1)
                    {
                        filterSql << "' or dp.status = '";
                    }
                }
                filterSql << "')";
            }

            // Subsystem Ids
            if (0 < filter->subsystemIds.size())
            {
                if (!hasEntity)
                {
                    tablesSql << ", entity ent";
                    hasEntity = true;
                }

                filterSql << " and dp.entitykey = ent.pkey and (ent.subsystemkey = ";
                for (i = 0;i < filter->subsystemIds.size();i++)
                {
                    filterSql << filter->subsystemIds[i];
                
                    if (i != filter->subsystemIds.size() - 1)
                    {
                        filterSql << " or ent.subsystemkey = ";
                    }
                }
                filterSql << ")";
            }

            // Location Ids
            if (0 < filter->locationIds.size())
            {
                if (!hasEntity)
                {
                    tablesSql << ", entity ent";
                    hasEntity = true;
                }

                filterSql << " and dp.entitykey = ent.pkey and (ent.locationkey = ";
                for (i = 0;i < filter->locationIds.size();i++)
                {
                    filterSql << filter->locationIds[i];
                
                    if (i != filter->locationIds.size() - 1)
                    {
                        filterSql << " or ent.locationkey = ";
                    }
                }
                filterSql << ")";
            }
        }

        // If the callback user has been passed in, get the total event count first.
        if (NULL != callbackUser)
        {
            /*std::string countSql("select count(*)");
            countSql += tablesSql.str();
            countSql += " where " + timeSql;
            if (0 < filterSql.str().length())
            {
                countSql += filterSql.str();
            } */    
            
            // Set up the columnNames vector to be passed to executeQuery()
            std::string countColumn = "count";
            std::vector<std::string> columnNames;
            columnNames.push_back(countColumn);

            // Execute the query. The method can throw a DatabaseException.
            // This is documented in the comment of this method.
            // We are responsible for deleting the returned IData object when we're done with it
            
            //waitforcheck delete by lin
            //IData* data = databaseConnection->executeQuery(countSql,columnNames);
            SQLStatement strSql;
			databaseConnection->prepareSQLStatement(strSql, LO_DATAPT_STATE_CHANGE_SELECT_13554, tablesSql.str(), sqlParamTimeStr, filterSql.str());
            //strSql.strCommon = countSql;
            IData* data = databaseConnection->executeQuery(strSql,columnNames);
            

            unsigned long count = 0;
            if (0 != data->getNumRows()) // At least one entry found with the specified message_id
            {
                count = data->getUnsignedLongData(0,countColumn);
            } 

            // clean up the pointer
            delete data;
            data = NULL;
            
            // If the total event count is greater than the database limit, then notify the callback user, else
            // set the count to be what was loaded.
            if (maxEvents != 0 && maxEvents < count)
            {
                callbackUser->limitEnforced();
                callbackUser->setCount(maxEvents);
            }    
            else
            {
                callbackUser->setCount(count);
            }

        }

        // create the SQL string to retrieve the key of all messages in the system
   //     std::string sql("select /*+ (dp timestamp) */ dp.lodsch_id, TO_CHAR(dp.timestamp,'YYYYMMDDHH24MISS'), TO_CHAR(dp.logtime,'YYYYMMDDHH24MISS'), \
//dp.entitykey, dp.value, dp.status, ent.name as EntityName, subsys.name as SubsystemName, loc.name as LocationName \
//from lo_datapt_state_change dp, entity ent, location loc, subsystem subsys where dp.entitykey = ent.pkey AND ent.locationkey \
//= loc.pkey AND ent.SUBSYSTEMKEY = subsys.PKEY");
   //     sql += " and " + timeSql;
		//std::string sql;
		SQLTypeAdapter sqlParam;
        if (0 < filterSql.str().length())
        {
            //sql += filterSql.str();
			sqlParam.add(filterSql.str());
        }
        if (0 < maxEvents)
        {
            //std::ostringstream rownumSql;
            //rownumSql << " and rownum < " << (maxEvents+1);
            //sql += rownumSql.str();

            // AL : 6/2/04 : Fix for TD1260. It is now possible to configure a max number of events to display.
            // If there are more events matching the filter, the most recent events will be displayed first.

            //sql += " order by dp.timestamp desc";
    
           /* std::ostringstream rownumSql;
			if (enumOracleDb == nDbType)
				rownumSql << " and rownum < " << (maxEvents + 1);
			else
				rownumSql << " LIMIT " << (maxEvents + 1);

            sql += rownumSql.str();*/
			sqlParam.add(enumSyntax_Format_1);
        }

	//	std::string strSql = defPrepareSQLStatement(nDbType, lo_datapt_state_change_Oracle_SELECT_13552, timeSql, sql);
//		std::string strSql = databaseConnection->prepareSQLStatement(LO_DATAPT_STATE_CHANGE_SELECT_13552, timeSql, sql);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, LO_DATAPT_STATE_CHANGE_SELECT_13552, sqlParamTimeStr, sqlParam, maxEvents + 1);
//		TA_ASSERT(strSql.size() > 0, "DataPointStateChangeAccessFactory::getStateChangesBetweenTimes(): Prepare SQL Statement error");

        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn = "MESSAGEID";
        std::string timeStampColumn = "TIMESTAMP";
        std::string logTimeColumn = "LOGTIME";
        std::string entityKeyColumn = "ENTITYKEY";        
        std::string valueColumn = "VALUE";
        std::string statusColumn = "STATUS";
        std::string entityNameColumn = "ENTITYNAME";
        std::string subsystemColumn = "SUBSYSTEM";
        std::string locationColumn = "LOCATION";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);
        columnNames.push_back(timeStampColumn);
        columnNames.push_back(logTimeColumn);
        columnNames.push_back(entityKeyColumn);
        columnNames.push_back(valueColumn);
        columnNames.push_back(statusColumn);
        columnNames.push_back(entityNameColumn);
        columnNames.push_back(subsystemColumn);
        columnNames.push_back(locationColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Don't need the filter object any more, so delete it
        if (filter != NULL)
        {

            delete filter;
            filter = NULL;
        }
        
        // Define the vector for returning the messages
        std::vector<IDataPointStateChangeData*> messages;

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified message_id
        {
            // clean up the pointer
            delete data;
            data = NULL;

            return messages;
        }

        do
        {
            for(unsigned int i = 0; i < data->getNumRows() && !m_stopLoad; i++)
            {
                try
                {
                    messages.push_back(new DataPointStateChangeData(
                        data->getStringData(i,keyColumn),                    
                        data->getDateData(i,timeStampColumn),
                        data->getDateData(i,logTimeColumn),
                        data->getUnsignedLongData(i,entityKeyColumn),
                        data->getStringData(i,valueColumn),
                        data->getStringData(i,statusColumn),
                        data->getStringData(i,entityNameColumn),
                        data->getStringData(i,subsystemColumn),
                        data->getStringData(i,locationColumn),
                        m_alarmMessageDataPointNameToPKeyMap
                        ));
                }
                catch(...)
                {
                    // An error when creating one object shouldn't effect the others
                    continue;
                }

                while(m_pauseLoad)
                {
                    TA_Base_Core::Thread::sleep(200);
                }
            }

            // Now that we're finished with the IData object, we need to delete it.
            delete data;
            data = NULL;
            if (NULL != callbackUser)
            {
                callbackUser->notifyLoad(500);
            }
        }
        while (databaseConnection->moreData(data) && !m_stopLoad);

        // Reset the m_stopLoad variable
        if(m_stopLoad)
        {
            m_stopLoad = false;
        }

        // Create the Message object to represent this object. Initially only populate it
        // with key. We will populate the rest of the data on demand.
        return messages;
    }


    time_t DataPointStateChangeAccessFactory::getEarliestMessageTime()
    {
        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

        //std::string sql("SELECT TO_CHAR(MIN(TIMESTAMP),'YYYYMMDDHH24MISS') FROM LO_DATAPT_STATE_CHANGE");
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), lo_datapt_state_change_Oracle_SELECT_13551);
//		std::string sql  = databaseConnection->prepareSQLStatement(LO_DATAPT_STATE_CHANGE_SELECT_13551);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, LO_DATAPT_STATE_CHANGE_SELECT_13551);
//		TA_ASSERT(sql.size() > 0, "DataPointStateChangeAccessFactory::getEarliestMessageTime(): Prepare SQL Statement error");

        // Set up the columnNames vector to be passed to executeQuery()
        std::string timestampColumn = "TIMESTAMP";
        std::vector<std::string> columnNames;
        columnNames.push_back(timestampColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql,columnNames);
     
        time_t earliestTime = data->getDateData(0,timestampColumn);

        delete data;
        data = NULL;

        return earliestTime;
    }


    IDataPointStateChangeData* DataPointStateChangeAccessFactory::createDataPointStateChange()
    {
        return new DataPointStateChangeData(m_alarmMessageDataPointNameToPKeyMap);
    }


    void DataPointStateChangeAccessFactory::applicationTerminating()
    {
        m_stopLoad = true;
        m_pauseLoad = false;
    }

    void DataPointStateChangeAccessFactory::applicationPausing()
    {
        m_pauseLoad = true;
    }


    void DataPointStateChangeAccessFactory::applicationResuming()
    {
        m_pauseLoad = false;
    }


    // Raymond Pau++
    // Bug#802 TD7194
    DataPointStateChangeAccessFactory::DataPointStateChangeAccessFactory()
        : m_stopLoad(false), m_pauseLoad(false) 
    {
        FUNCTION_ENTRY("Constructor");

        // Prepare a map of Datapoint AlarmMessage Name to their PKey
        try
        {
            std::string name("");
            unsigned long pkey = 0;

            IDatabase* databaseConnection =
                    TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);

            /*std::ostringstream sql;
            sql << "select ep.pkey, ep.name"
                << " from entityparameter ep, entitytype et "
                << "where ep.typekey=et.pkey"
                << " and et.name = '" << databaseConnection->escapeQueryString(DataPointEntityData::getStaticType()) << "'"
                << " and ep.name in ("
                << "'" << DataPointEntityData::DATAPOINT_FALSE_ALARM_MESSAGE << "',"
                << "'" << DataPointEntityData::DATAPOINT_TRUE_ALARM_MESSAGE << "',"
		        << "'" << DataPointEntityData::DATAPOINT_OH_ALARM_MESSAGE << "',"
                << "'" << DataPointEntityData::DATAPOINT_HIHI_ALARM_MESSAGE << "',"
                << "'" << DataPointEntityData::DATAPOINT_HI_ALARM_MESSAGE << "',"
                << "'" << DataPointEntityData::DATAPOINT_LO_ALARM_MESSAGE << "',"
                << "'" << DataPointEntityData::DATAPOINT_LOLO_ALARM_MESSAGE << "',"
		        << "'" << DataPointEntityData::DATAPOINT_OL_ALARM_MESSAGE << "',"
		        << "'" << DataPointEntityData::DATAPOINT_EVENT_MESSAGE << "'"
                << ")";*/
//			std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entitytype_STD_SELECT_4016, databaseConnection->escapeQueryString(DataPointEntityData::getStaticType()),
//			std::string sql  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4016, databaseConnection->escapeQueryString(DataPointEntityData::getStaticType()),
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ENTITYTYPE_SELECT_4016, databaseConnection->escapeQueryString(DataPointEntityData::getStaticType()),
				DataPointEntityData::DATAPOINT_FALSE_ALARM_MESSAGE, DataPointEntityData::DATAPOINT_TRUE_ALARM_MESSAGE, DataPointEntityData::DATAPOINT_OH_ALARM_MESSAGE,
				DataPointEntityData::DATAPOINT_HIHI_ALARM_MESSAGE, DataPointEntityData::DATAPOINT_HI_ALARM_MESSAGE, DataPointEntityData::DATAPOINT_LO_ALARM_MESSAGE,
				DataPointEntityData::DATAPOINT_LOLO_ALARM_MESSAGE, DataPointEntityData::DATAPOINT_OL_ALARM_MESSAGE, DataPointEntityData::DATAPOINT_EVENT_MESSAGE);
//			TA_ASSERT(sql.size() > 0, "DataPointStateChangeAccessFactory(): Prepare SQL Statement error");

            std::string pkeyColumn = "ep.PKEY";
            std::string nameColumn = "ep.NAME";
            std::vector<std::string> columnNames;
            columnNames.push_back(pkeyColumn);
            columnNames.push_back(nameColumn);

            IData* data = databaseConnection->executeQuery(sql, columnNames);

            // Loop for each row returned
            do
            {
                for (unsigned long i = 0; i < data->getNumRows(); ++i)
                {
                    try
                    {
                        pkey = data->getUnsignedLongData(i, pkeyColumn);
                        name = data->getStringData(i, nameColumn);
                        m_alarmMessageDataPointNameToPKeyMap[name] = pkey;

                        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Loading Datapoint Alarm Message Key... %s -> %d", name.c_str(), pkey);
                    }
                    catch (TA_Base_Core::DataException& e)
                    {
                        std::string text = "DataPointStateChangeAccessFactory Constructor - " + std::string(e.what());
                        LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text.c_str());
                    }
                }
                delete data;
                data = NULL;
            }
            while (databaseConnection->moreData(data));
        }
        catch(DatabaseException & e)
        {
            std::string text = "DataPointStateChangeAccessFactory Constructor - " + std::string(e.what());
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text.c_str());
        }

        FUNCTION_EXIT;
    }
    // Bug#802 TD7194
    // ++Raymond Pau
}
