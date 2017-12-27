/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/MessageAccessFactory.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * MessageAccessFactory is a singleton that is used to retrieve MessageData objects from the database.
 * It provides both read-write, and read-only objects, but does not create new objects as they are not
 * able to be defined through the system.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "ace/OS.h"

//#include <ctime>
#include <sstream>

#include "core/data_access_interface/src/MessageAccessFactory.h"
#include "core/data_access_interface/src/MessageData.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabaseCallbackUser.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/src/SQLUtilityFun.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/threads/src/Thread.h"

namespace TA_Base_Core
{
	const std::string MessageAccessFactory::SESSION_STARTED_EVENT_NAME ("SessionStarted");
	const std::string MessageAccessFactory::SESSION_ENDED_EVENT_NAME   ("SessionEnded");
	const std::string MessageAccessFactory::OVERRIDE_STARTED_EVENT_NAME("OverrideStarted");
	const std::string MessageAccessFactory::OVERRIDE_ENDED_EVENT_NAME  ("OverrideEnded");

    const unsigned long MessageAccessFactory::INVALID_SEQUENCE_NUMBER(0);
    const unsigned long MessageAccessFactory::MIN_SEQUENCE_NUMBER(1);
    const unsigned long MessageAccessFactory::MAX_SEQUENCE_NUMBER(4000000000);
    const unsigned long MessageAccessFactory::MAX_ROWS(20000);


    MessageAccessFactory* MessageAccessFactory::m_instance = 0;
   
    MessageAccessFactory& MessageAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new MessageAccessFactory();
        }
        return *m_instance;
    }

    
    IMessageData* MessageAccessFactory::getMessage(const std::string& key)
    {
        // Create the Message object to represent this object. Initially only populate it
        // with key. We will populate the rest of the data on demand.
        return new MessageData(key);
    }

    std::vector<IMessageData*> MessageAccessFactory::getAllMessages()
    {
        // get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EVENT_STD_SELECT_17001);
//		std::string strSql  = databaseConnection->prepareSQLStatement(EVENT_SELECT_17001);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EVENT_SELECT_17001);
//		TA_ASSERT(strSql.size() > 0, "MessageAccessFactory::getAllMessages(): Prepare SQL Statement error");

        // create the SQL string to retrieve the key of all messages in the system
        //std::string sql("select message_id from EVENT");

        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn = "message_id";
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // Define the vector for returning the messages
        std::vector<IMessageData*> messages;

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified message_id
        {
            // clean up the pointer
            delete data;
            data = NULL;

            return messages;
        }

        do
        {
            for(unsigned int i = 0;i < data->getNumRows();i++)
            {
                messages.push_back(new MessageData(data->getStringData(i,keyColumn)) );
            }

            // Now that we're finished with the IData object, we need to delete it.
            delete data;
            data = NULL;
        }
        while (databaseConnection->moreData(data));

        // Create the Message object to represent this object. Initially only populate it
        // with key. We will populate the rest of the data on demand.
        return messages;
    }

	std::vector<IMessageData*> MessageAccessFactory::getAuthMessagesForSession( const std::string& session )
	{

		/*std::ostringstream sql;

		sql << "select message_id,TO_CHAR(createtime,'YYYYMMDDHH24MISSFF3'),TO_CHAR(logtime,'YYYYMMDDHH24MISSFF3')," <<
		       "description,param_list,details,eventtype,entitykey,sessionkey,alarm_id,locationkey,incident_id,asset_name," <<
			   "me_message_type.type_name from event,me_message_type where sessionkey = '" << session << "' and " <<
			   "(( eventtype=(select memtyp_id from me_message_type where type_name='" << SESSION_STARTED_EVENT_NAME  << "')) or " <<
			   "( eventtype=(select memtyp_id from me_message_type where type_name='" << SESSION_ENDED_EVENT_NAME    << "')) or " <<
			   "( eventtype=(select memtyp_id from me_message_type where type_name='" << OVERRIDE_STARTED_EVENT_NAME << "')) or " <<
			   "( eventtype=(select memtyp_id from me_message_type where type_name='" << OVERRIDE_ENDED_EVENT_NAME   << "'))) and " <<
		       "event.eventtype = me_message_type.memtyp_id";*/

		// get a connection to the database
	   IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
//	   std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EVENT_Oracle_SELECT_17051, session, SESSION_STARTED_EVENT_NAME,
//	   std::string strSql  = databaseConnection->prepareSQLStatement(EVENT_SELECT_17051, session, SESSION_STARTED_EVENT_NAME,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, EVENT_SELECT_17051, session, SESSION_STARTED_EVENT_NAME,
		   SESSION_ENDED_EVENT_NAME, OVERRIDE_STARTED_EVENT_NAME, OVERRIDE_ENDED_EVENT_NAME);
//	   TA_ASSERT(strSql.size() > 0, "MessageAccessFactory::getAuthMessagesForSession(): Prepare SQL Statement error");

	   // Set up the columnNames vector to be passed to executeQuery()
		std::string message_id  = "message_id";
		std::string createtime  = "createtime";
		std::string logtime     = "logtime";
		std::string description = "description";
		std::string param_list  = "param_list";
		std::string details     = "details";
		std::string eventtype   = "eventtype";
		std::string entitykey   = "entitykey";
		std::string sessionkey  = "sessionkey";
		std::string alarm_id    = "alarm_id";
        // TD13673
        std::string locationkey = "locationkey";
		std::string incident_id = "incident_id";
		std::string asset_name  = "asset_name";
		std::string type_name   = "type_name";

		std::vector<std::string> columnNames;
		columnNames.push_back(message_id );
		columnNames.push_back(createtime );
		columnNames.push_back(logtime    );
		columnNames.push_back(description);
		columnNames.push_back(param_list );
		columnNames.push_back(details    );
		columnNames.push_back(eventtype  );
		columnNames.push_back(entitykey  );
		columnNames.push_back(sessionkey );
		columnNames.push_back(alarm_id   );
        // TD13673
        columnNames.push_back(locationkey );
		columnNames.push_back(incident_id);
		columnNames.push_back(asset_name );
		columnNames.push_back(type_name );

		IData* data = databaseConnection->executeQuery(strSql,columnNames);

		// Define the vector for returning the messages
		std::vector<IMessageData*> messages;

		if( (NULL == data) || (0 == data->getNumRows()) ) // No entry found with the specified message_id
		{
			// clean up the pointer
			if( NULL != data )
			{
				delete data;
				data = NULL;
			}

			return messages;
		}

		do
		{
			for(unsigned int i = 0;i < data->getNumRows();i++)
			{
				MessageData* newMsg = new MessageData(
					data->getStringData      (i,message_id ),
					data->getTimestampData   (i,createtime ),
					data->getTimestampData   (i,logtime    ),
					data->getStringData      (i,description),
					data->getStringData      (i,details    ),
					data->getStringData      (i,param_list ),
					data->getStringData      (i,eventtype  ),
					data->getStringData      (i,type_name  ),
					data->getUnsignedLongData(i,entitykey  ),
					"", // entityName
					"", // subsystemName
					"", // locationName
					data->getStringData      (i,asset_name ),
					data->getStringData      (i,sessionkey ),
					"", // operatorName
					data->getStringData      (i,alarm_id   ),
					"", // severityName
                    // TD13673
                    data->getUnsignedLongData(i, locationkey),
					data->getStringData      (i,incident_id));

				messages.push_back(newMsg);
			}

			delete data;
			data = NULL;
		}
		while (databaseConnection->moreData(data));

		return messages;
	}
	
// 	std::vector<IMessageData*> MessageAccessFactory::getMessagesBetweenTimes(
// 		const time_t fromTime,
// 		const time_t toTime,
//         const unsigned long maxEvents,
//         MessageFilter* filter, 
//         IDatabaseCallbackUser* callbackUser,
//         bool eventOnly
// 	)
//     {
//         // If not filtering on any severities or events unassociated with an alarm,
//         // then there isn't anything to load.
//         if (0 == filter->severityIds.size() && !eventOnly)
//         {
//             std::vector<IMessageData*> messages;
//             return messages;
//         }
// 
//         // get a connection to the database
//         IDatabase* databaseConnection = 
//                 TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
// 		int nDbType = databaseConnection->getDbServerType();
// 
//         // Calculate the times as strings...
//         std::string timeSql("");
// 
//         // Use ACE routine ctime_r()
//         // Wek Mon DD HH:MM:SS YYYY\n\0
//         const unsigned int bufLen = 27;
//         char buf[bufLen];
//         std::string fromTimeString(ACE_OS::ctime_r(&fromTime,buf,bufLen));
// 
//         // and remove the characters we don't want - staring with the last two
//         fromTimeString.erase(24,2);
// 
//         // and then the first 4
//         fromTimeString.erase(0,4);
//        
//         // If the alarmID filter is set, that is a special case, and the time filter is ignored (we want to see the alarm regardless of when
//         // it happened).
//         //if (0 != fromTime)
//         {
// 			if (enumOracleDb == nDbType)
// 				timeSql = " e.createtime >= TO_TIMESTAMP('" + fromTimeString + "','Mon DD HH24:MI:SS YYYY')";
// 			else
// 				timeSql = " e.createtime >= STR_TO_DATE('" + fromTimeString + "','%b %d %H:%i:%S %Y')";
//         }
//         
// 
//         if (0 != toTime)
//         {
//             // if the toTime is not the default, then there is a restriction on that too
//             const unsigned int bufLen = 27;
//             char buf[bufLen];
//             std::string toTimeString(ACE_OS::ctime_r(&toTime,buf,bufLen));
// 
//             // and remove the characters we don't want - staring with the last two
//             toTimeString.erase(24,2);
// 
//             // and then the first 4
//             toTimeString.erase(0,4);
// 
//             // If the from time filter was added need to put in an "and" here
//             if (0 != fromTime)
//             {
//                 timeSql += " and";
//             }
// 
// 			if (enumOracleDb == nDbType)
// 				timeSql += " createtime <= TO_TIMESTAMP('" + toTimeString + "','Mon DD HH24:MI:SS YYYY')";
// 			else
// 				timeSql += " createtime <= STR_TO_DATE('" + toTimeString + "','%b %d %H:%i:%S %Y')";
//         }
// 		else
// 		{
// 			if (enumOracleDb == nDbType)
// 				timeSql+= " AND e.createtime <= sysdate";
// 			else
// 				timeSql+= " AND e.createtime <= sysdate()";
// 		}
// 
//         // If the filter object has been set, work out the various where clauses
//         std::ostringstream filterSql;
//         std::ostringstream tablesSql;
//         tablesSql << " from event e";
//         if (NULL != filter)
//         {
//             bool hasEntity = false;
//             bool hasSession = false;
//             bool hasAlarm = false;
//             // Description
//             if (0 < filter->description.length())
//             {
//                 filterSql << " and e.description like '" << databaseConnection->escapeQueryString(filter->description) << "'";
//             }
// 
//             unsigned long i = 0;
//             // Event Type(s)
//             if (0 < filter->eventTypes.size())
//             {
//                 filterSql << " and e.eventtype = ";
//                 for (i = 0;i < filter->eventTypes.size();i++)
//                 {
//                     filterSql << filter->eventTypes[i];
//                 
//                     if (i != filter->eventTypes.size() - 1)
//                     {
//                         filterSql << " or e.eventtype = ";
//                     }
//                 }
//             }
// 
//             // Asset name (or entity name if asset name = "")
//             // TODO: It is quite ugly making clients of the EVENT.ASSET_NAME column have to handle 
//             // an empty asset name as a special case.  It would be preferable to populate ASSET_NAME
//             // with the entity name when the event is added, but this is difficult.
//             if (0 < filter->assetName.length())
//             {
//                 // We have to include the ENTITY table in the query whether the asset name is "" or not.
//                 tablesSql << ", entity ent";
//                 hasEntity = true;
// 
//                 filterSql << " and e.entitykey = ent.pkey and (e.ASSET_NAME like '" << databaseConnection->escapeQueryString(filter->assetName) << "'"
//                     << " or (e.asset_name IS NULL and ent.name like '" << databaseConnection->escapeQueryString(filter->assetName) << "'))";
//             }
// 
//             // Operator Ids
//             if (0 < filter->operatorIds.size())
//             {
//                 if (!hasSession)
//                 {
//                     tablesSql << ", ta_session s" << ", operator op";
//                     hasSession = true;
//                 }
// 
//           //      filterSql << " and e.sessionkey = s.pkey(+) and s.operatorkey = op.pkey(+) and (op.pkey =  ";
// 				filterSql << " and (op.pkey = ";
//                 for (i = 0;i < filter->operatorIds.size();i++)
//                 {
//                     filterSql << filter->operatorIds[i];
// 
//                     // If the key is 0, the operator is NULL (i.e., need a special query)
//                     if (0 == filter->operatorIds[i])
//                     {
//                         filterSql << " or op.pkey is NULL";
//                     }
//                 
//                     if (i != filter->operatorIds.size() - 1)
//                     {
//                         filterSql << " or op.pkey = ";
//                     }
//                 }
//                 filterSql << ")";
//             }
// 
//             // Severity Ids
//             if (0 < filter->severityIds.size())
//             {
//                 if (!hasAlarm)
//                 {
//                     tablesSql << ", alarm a";
//                     hasAlarm = true;
//                 }
// 
//                 //filterSql << " and e.alarm_id = a.alarm_id(+) and ";
// 				filterSql << " and " ;
//                 
//                 
//                 if(eventOnly)
//                 {
//                     filterSql << " (";
//                 }
//                 
//                 filterSql << "(a.severity = ";
// 
//                 for (i = 0;i < filter->severityIds.size();i++)
//                 {
//                     filterSql << filter->severityIds[i];
//                 
//                     if (i != filter->severityIds.size() - 1)
//                     {
//                         filterSql << " or a.severity = ";
//                     }
//                 }
//                 filterSql << ")";
// 
//                 
//             }
// 
//             
//             if(eventOnly)
//             {
//                 if(0 < filter->severityIds.size())
//                 {
//                     filterSql << " or";
//                 }
//                 else
//                 {
//                     filterSql << " and";
//                 }
// 
//                 filterSql << " (e.alarm_id is NULL or e.alarm_id = ' ')";
// 
//                 if(0 < filter->severityIds.size())
//                 {
//                     filterSql << ")";
//                 }
//             }
// 
//             // Subsystem Ids
//             if (0 < filter->subsystemIds.size())
//             {
//                 if (!hasEntity)
//                 {
//                     tablesSql << ", entity ent";
//                     hasEntity = true;
//                 }
// 
//                 filterSql << " and e.entitykey = ent.pkey and (ent.subsystemkey = ";
//                 for (i = 0;i < filter->subsystemIds.size();i++)
//                 {
//                     filterSql << filter->subsystemIds[i];
//                 
//                     if (i != filter->subsystemIds.size() - 1)
//                     {
//                         filterSql << " or ent.subsystemkey = ";
//                     }
//                 }
//                 filterSql << ")";
//             }
// 
//             // Location Ids
//             if (0 < filter->locationIds.size())
//             {
//                 if (!hasEntity)
//                 {
//                     tablesSql << ", entity ent";
//                     hasEntity = true;
//                 }
// 
//                 filterSql << " and e.entitykey = ent.pkey and (ent.locationkey = ";
//                 for (i = 0;i < filter->locationIds.size();i++)
//                 {
//                     filterSql << filter->locationIds[i];
//                 
//                     if (i != filter->locationIds.size() - 1)
//                     {
//                         filterSql << " or ent.locationkey = ";
//                     }
//                 }
//                 filterSql << ")";
//             }
// 
//             // Alarm Types
//             if (0 < filter->alarmTypes.size())
//             {
//                 if (!hasAlarm)
//                 {
//                     tablesSql << ", alarm a";
//                     hasAlarm = true;
//                 }
// 
//                 filterSql << " and e.alarm_id = a.alarm_id and (a.type = ";
//                 for (i = 0;i < filter->alarmTypes.size();i++)
//                 {
//                     filterSql << filter->alarmTypes[i];
//                 
//                     if (i != filter->alarmTypes.size() - 1)
//                     {
//                         filterSql << " or a.type = ";
//                     }
//                 }
//                 filterSql << ")";
//             }
// 
//             // Alarm Id
//             if (0 < filter->alarmId.length())
//             {
//                 filterSql << " and e.alarm_id like '" << databaseConnection->escapeQueryString(filter->alarmId) << "'";
//             }
//         }
// 
//         // If the callback user has been passed in, get the total event count first.
//         if (NULL != callbackUser)
//         {
// 			callbackUser->setCount(maxEvents);
//         }
// 
//   //      std::string sql("select /*+ index (e createtime) */ e.message_id, \
// 						 TO_CHAR(e.createtime,'YYYYMMDDHH24MISSFF3'), \
// 						 TO_CHAR(e.logtime,'YYYYMMDDHH24MISSFF3'), e.description, e.param_list, e.details, \
// 						 e.EVENTTYPE, e.entitykey, e.alarm_id, e.sessionkey ,e.locationkey, e.incident_id, \
// 						 mt.type_name as TypeName, ent.name as EntityName, op.name ");
//         
// 	//	sql += ", sev.name, subsys.name as SubsystemName, loc.name as LocationName, e.ASSET_NAME \
// 			   from event e, me_message_type mt, operator op, entity ent, ta_session s, \
// 			   alarmseverity sev, location loc, subsystem subsys, alarm a \
// 			   where e.entitykey = ent.pkey AND e.sessionkey = s.pkey(+) AND s.operatorkey = op.pkey(+) \
// 			   AND mt.memtyp_id = e.eventtype AND ent.locationkey = loc.pkey \
// 			   AND ent.SUBSYSTEMKEY = subsys.PKEY AND e.ALARM_ID = a.ALARM_ID(+) \
// 			   AND a.SEVERITY = sev.PKEY(+)";
// 
//         //sql += " and " + timeSql;
// 		std::string sql;
//         if (0 < filterSql.str().length())
//         {
//             sql += filterSql.str();
//         }
//         if (0 < maxEvents)
//         {
//             //std::ostringstream rownumSql;
//             //rownumSql << " and rownum < " << (maxEvents + 1);
//             //sql += rownumSql.str();
// 
//             // AL : 6/2/04 : Fix for TD1260. It is now possible to configure a max number of events to display.
//             // If there are more events matching the filter, the most recent events will be displayed first.
// 
// 			// GK : Matt Ch. assures me that if the e.createtime column is indexed (and toad tells me it is)
// 			// then an "order by" is no longer required.
//             //sql += " order by e.createtime desc";
//     
//             std::ostringstream rownumSql;
// 			if (enumOracleDb == nDbType)
// 				rownumSql << " and rownum < " << (maxEvents + 1);
// 			else
// 				rownumSql << " LIMIT " << (maxEvents + 1);
// 
//             sql += rownumSql.str();
//         }
// //		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EVENT_Oracle_SELECT_17052, timeSql, sql);
// 		std::string strSql  = databaseConnection->prepareSQLStatement(EVENT_SELECT_17052, timeSql, sql);
// 		TA_ASSERT(strSql.size() > 0, "MessageAccessFactory::getMessagesBetweenTimes(): Prepare SQL Statement error");
// 
//         // Set up the columnNames vector to be passed to executeQuery()
//         std::string keyColumn = "MESSAGEID";
//         std::string createTimeColumn = "CREATETIME";
//         std::string logTimeColumn = "LOGTIME";
//         std::string descriptionColumn = "DESCRIPTION";
//         std::string paramListColumn = "PARAMLIST";
//         std::string detailsColumn = "DETAILS";
//         std::string eventTypeKeyColumn = "EVENTTYPEKEY";
//         std::string entityKeyColumn = "ENTITYKEY";
//         std::string alarmIdColumn = "ALARMID";
//         std::string sessionIdColumn = "SESSIONID";
// 
//         // TD13673
//         std::string locationKeyColumn = "LOCATIONKEY";
//         std::string incidentIdColumn = "INCIDENTID";
//         std::string eventTypeNameColumn = "EVENTTYPENAME";
//         std::string entityNameColumn = "ENTITYNAME";
//         std::string operatorColumn = "OPERATOR";
//         std::string severityColumn = "SEVERITY";
//         std::string subsystemColumn = "SUBSYSTEM";
//         std::string locationColumn = "LOCATION";
//         std::string assetColumn = "ASSET";
//         std::vector<std::string> columnNames;
//         columnNames.push_back(keyColumn);
//         columnNames.push_back(createTimeColumn);
//         columnNames.push_back(logTimeColumn);
//         columnNames.push_back(descriptionColumn);
//         columnNames.push_back(paramListColumn);
//         columnNames.push_back(detailsColumn);
//         columnNames.push_back(eventTypeKeyColumn);
//         columnNames.push_back(entityKeyColumn);
//         columnNames.push_back(alarmIdColumn);
//         columnNames.push_back(sessionIdColumn);
//         // TD13673
//         columnNames.push_back(locationKeyColumn);
//         columnNames.push_back(incidentIdColumn);
//         columnNames.push_back(eventTypeNameColumn);
//         columnNames.push_back(entityNameColumn);
//         columnNames.push_back(operatorColumn);
//         columnNames.push_back(severityColumn);
//         columnNames.push_back(subsystemColumn);
//         columnNames.push_back(locationColumn);
//         columnNames.push_back(assetColumn);
// 
//         // Execute the query. The method can throw a DatabaseException.
//         // This is documented in the comment of this method.
//         // We are responsible for deleting the returned IData object when we're done with it
//         IData* data = databaseConnection->executeQuery(strSql,columnNames);
// 
//         // Don't need the filter object any more, so delete it
//         try
//         {
//             if (filter != NULL)
//             {
//                 delete filter;
//                 filter = NULL;
//             }
//         }
//         catch(...)
//         {
//             // A problem deleting the filter should not cause the rest of the operation to fail, so log and continue
//             LOG_GENERIC(SourceInfo,DebugUtil::DebugWarn,"An error occurred deleting the filter object in the MessageAccessFactory.");
//         }
//         
//         // Define the vector for returning the messages
//         std::vector<IMessageData*> messages;
// 
//         if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified message_id
//         {
//             // clean up the pointer
//             delete data;
//             data = NULL;
// 
//             return messages;
//         }
// 
//         do
//         {
//             for(unsigned int i = 0; i < data->getNumRows() && !m_stopLoad; i++)
//             {
//                 
//                 std::string severityName = data->getStringData(i,severityColumn);
//                 if (0 == severityName.compare("<null>"))
//                 {
//                     severityName = " ";
//                 }
//                     
//                 messages.push_back(new MessageData(
//                     data->getStringData(i,keyColumn),                    
//                     data->getTimestampData(i,createTimeColumn),
//                     data->getTimestampData(i,logTimeColumn),
//                     data->getStringData(i,descriptionColumn),
//                     data->getStringData(i,detailsColumn),
//                     data->getStringData(i,paramListColumn),
//                     data->getStringData(i,eventTypeKeyColumn),
//                     data->getStringData(i,eventTypeNameColumn),
//                     data->getUnsignedLongData(i,entityKeyColumn),
//                     data->getStringData(i,entityNameColumn),
//                     data->getStringData(i,subsystemColumn),
//                     data->getStringData(i,locationColumn),
//                     data->getStringData(i, assetColumn),
//                     data->getStringData(i,sessionIdColumn),
//                     data->getStringData(i,operatorColumn),
//                     data->getStringData(i,alarmIdColumn),
//                     severityName,
//                     // TD13673
//                     data->getUnsignedLongData(i,locationKeyColumn),
//                     data->getStringData(i,incidentIdColumn)
//                     ));
// 
//                 while(m_pauseLoad)
//                 {
//                     TA_Base_Core::Thread::sleep(200);
//                 }
//             }
// 
// 			if (NULL != callbackUser)
//             {
//                 callbackUser->notifyLoad(data->getNumRows());
//             }
// 
//             // Now that we're finished with the IData object, we need to delete it.
//             delete data;
//             data = NULL;
//             
//         }
//         while (databaseConnection->moreData(data) && !m_stopLoad);
// 
//         // Reset the m_stopLoad variable
//         if(m_stopLoad)
//         {
//             m_stopLoad = false;
//         }
// 
//         // Create the Message object to represent this object. Initially only populate it
//         // with key. We will populate the rest of the data on demand.
//         return messages;
//     }

 
//     time_t MessageAccessFactory::getEarliestMessageTime()
//     {
//         // get a connection to the database
//         IDatabase* databaseConnection = 
//                 TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
// //		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EVENT_Oracle_SELECT_17053);
// 		std::string strSql  = databaseConnection->prepareSQLStatement(EVENT_SELECT_17053);
// 		TA_ASSERT(strSql.size() > 0, "MessageAccessFactory::getEarliestMessageTime(): Prepare SQL Statement error");
// 
//         //std::string sql("SELECT /*+ INDEX_COMBINE (EVENT IDX_EVENT_CREATETIME)*/ TO_CHAR(MIN(createtime),'YYYYMMDDHH24MISSFF3') FROM EVENT");
// 
//         // Set up the columnNames vector to be passed to executeQuery()
//         std::string createTimeColumn = "CREATETIME";
//         std::vector<std::string> columnNames;
//         columnNames.push_back(createTimeColumn);
// 
//         // Execute the query. The method can throw a DatabaseException.
//         // This is documented in the comment of this method.
//         // We are responsible for deleting the returned IData object when we're done with it
//         IData* data = databaseConnection->executeQuery(strSql,columnNames);
//      
//         time_t earliestTime = data->getDateData(0,createTimeColumn);
// 
//         delete data;
//         data = NULL;
// 
//         return earliestTime;
//     }

//     std::vector<IData*> MessageAccessFactory::getMessagesFromView(const std::string& viewName, const std::vector<std::string>& columnNames,
//                                                                   unsigned long precedingMsgPkey, unsigned long maxMsgs)
//     {
//         TA_ASSERT(!viewName.empty(), "Message view name has not been specified");
//         TA_ASSERT(!columnNames.empty(), "Message view column names have not been specified");
//         TA_ASSERT(columnNames.size() <= static_cast<unsigned long>(SimpleDb::MAX_COLS), "Too many message view column names have been specified");
//         
//         IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
//         
//         std::vector<IData*> messages;
// 
//         // Build up a string of comma separated column names.
//         std::string selectableColumns("");
//  
//         for (std::vector<std::string>::const_iterator nameIt(columnNames.begin()); nameIt != columnNames.end(); nameIt++)
//         {
//             if (!selectableColumns.empty())
//             {
//                 selectableColumns += ", ";
//             }
// 
//             selectableColumns += *nameIt;
//         }
// 
//         // Get the required messages up to and including MAX_SEQUENCE_NUMBER.
//         /*std::ostringstream sql;
//         sql << "SELECT " << selectableColumns 
//             << "  FROM " << databaseConnection->escapeQueryString(viewName)
//             << "  WHERE PKEY > " << precedingMsgPkey 
//             << "    AND ROWNUM <= " << maxMsgs
// 			<< " ORDER BY PKEY";*/		
// 		const char *pszViewName = strlwr(const_cast<char*>(databaseConnection->escapeQueryString(viewName).c_str()));
// 
// //		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EVENT_Oracle_SELECT_17054, selectableColumns,
// 		std::string strSql  = databaseConnection->prepareSQLStatement(EVENT_SELECT_17054, selectableColumns,
// 			pszViewName, precedingMsgPkey, maxMsgs);
// 		TA_ASSERT(strSql.size() > 0, "MessageAccessFactory::getMessagesFromView(): Prepare SQL Statement error");
// 
//         IData* data = databaseConnection->executeQuery(strSql, columnNames);
// 
//         unsigned long msgCount(0);
// 
//         do
//         {
//             messages.push_back(data);
//             msgCount += data->getNumRows();
//             
//             data = NULL;       
//         }
//         while (databaseConnection->moreData(data));
// 
//         // If necessary, wrap around to the beginning of the pkey sequence and get the remaining messages.
//         if (msgCount < maxMsgs)
//         {
//             unsigned long nextMsgPkey = getNextMessageSequenceNumber();
// 
//             if (nextMsgPkey < precedingMsgPkey)
//             {				
//                /* std::ostringstream sql;
//                 sql << "SELECT " << selectableColumns 
//                     << "  FROM " << databaseConnection->escapeQueryString(viewName)
//                     << "  WHERE PKEY BETWEEN " << MIN_SEQUENCE_NUMBER << " AND " << nextMsgPkey
//                     << "    AND ROWNUM <= " << maxMsgs - msgCount
// 					<< " ORDER BY PKEY" ;*/
// 
// //				std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), EVENT_Oracle_SELECT_17055, selectableColumns,
// 				std::string strSql  = databaseConnection->prepareSQLStatement(EVENT_SELECT_17055, selectableColumns,
// 					pszViewName, MIN_SEQUENCE_NUMBER, nextMsgPkey, maxMsgs - msgCount);
// 				TA_ASSERT(strSql.size() > 0, "MessageAccessFactory::getMessagesFromView(): Prepare SQL Statement error");
// 
//                 data = databaseConnection->executeQuery(strSql, columnNames);
// 
//                 do
//                 {
//                     messages.push_back(data);
//                     data = NULL;       
//                 }
//                 while (databaseConnection->moreData(data));            
//             }
//         }
// 
//         return messages;
//     }
// 

//     unsigned long MessageAccessFactory::getNextMessageSequenceNumber()
//     {
//         FUNCTION_ENTRY("getNextMessageSequenceNumber()");
//       
// 
//         const std::string NEXTVAL_COLUMN("NEXTVAL");
//         std::vector<std::string> columnNames;
//         columnNames.push_back(NEXTVAL_COLUMN);
// 
//         std::string sql("SELECT EVENT_SEQ.NEXTVAL FROM DUAL");
// 
//         std::auto_ptr<IData> data(TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read)->executeQuery(sql, columnNames));
//         TA_ASSERT(data.get() != NULL, "executeQuery() returned NULL data pointer");
// 
//         if (data->getNumRows() == 0)
//         {
//             TA_THROW(DataException("Query returned an empty data set.", DataException::NO_VALUE, sql));
//         }
//  
//         if (data->getNumRows() > 1)
//         {
//             TA_THROW(DataException("Query returned multiple records in the data set.", DataException::NOT_UNIQUE, sql));
//         }
//         
//         FUNCTION_EXIT;
//         return data->getUnsignedLongData(0, NEXTVAL_COLUMN);
//     }
// 

    IMessageData* MessageAccessFactory::createMessage()
    {
        return new MessageData();
    }


    void MessageAccessFactory::applicationTerminating()
    {
        m_stopLoad = true;
        m_pauseLoad = false;
    }


    void MessageAccessFactory::applicationPausing()
    {
        m_pauseLoad = true;
    }


    void MessageAccessFactory::applicationResuming()
    {
        m_pauseLoad = false;
    }

    void MessageAccessFactory::resetStopFlag()
    {
        m_stopLoad = false;
    }


    MessageAccessFactory::MessageAccessFactory() 
        : m_stopLoad(false), m_pauseLoad(false), m_operatorName("")
    {
        try
        {
            //IOperator* operatorData = OperatorAccessFactory::getInstance().getOperator(0, false);
            //m_operatorName = operatorData->getName();
            // Fix for TD2576 - do not want operatorName to default to "<Unallocated>"

            m_operatorName = " ";
        }
        catch(const DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            m_operatorName = "";
        }
        catch(const DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            m_operatorName = "";
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
            m_operatorName = "";
        }
    }


//     std::vector<IMessageData*> MessageAccessFactory::getMessageOfEntityType(
//         const unsigned long entityType, 
//         const time_t fromTime /*= 0*/,
//         const time_t toTime /*= 0*/,
//         const unsigned long maxEvents /*= 0*/)
//     {
//         FUNCTION_ENTRY("getMessageOfEntityType");
// 
// 		IDatabase* databaseConnection = 
// 			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
// 		int nDbType = databaseConnection->getDbServerType();
// 
//         // create the SQL string to retrieve the key of all messages in the system
//         std::ostringstream sql, rownumSql;
//         //sql << "select message_id from EVENT where entitykey in (select pkey from ENTITY where typekey = " << entityType << ")";       
// 
//         if (0 != fromTime)
//         {
//             // Use ACE routine ctime_r()
//             // Wek Mon DD HH:MM:SS YYYY\n\0
//             const unsigned int bufLen = 27;
//             char buf[bufLen];
//             std::string fromTimeString(ACE_OS::ctime_r(&fromTime,buf,bufLen));
// 
//             // and remove the characters we don't want - staring with the last two
//             fromTimeString.erase(24,2);
// 
//             // and then the first 4
//             fromTimeString.erase(0,4);
//        
// 			if (enumOracleDb == nDbType)
// 				sql << " AND createtime >= TO_TIMESTAMP('" << fromTimeString << "','Mon DD HH24:MI:SS YYYY')";
// 			else
// 				sql << " AND createtime >= STR_TO_DATE('" << fromTimeString << "','%b %d %H:%i:%S %Y')";
//         }
// 
//         if (0 != toTime)
//         {
//             // if the toTime is not the default, then there is a restriction on that too
//             const unsigned int bufLen = 27;
//             char buf[bufLen];
//             std::string toTimeString(ACE_OS::ctime_r(&toTime,buf,bufLen));
// 
//             // and remove the characters we don't want - staring with the last two
//             toTimeString.erase(24,2);
// 
//             // and then the first 4
//             toTimeString.erase(0,4);
// 
// 			if (enumOracleDb == nDbType)
// 				sql << " AND createtime <= TO_TIMESTAMP('" << toTimeString << "','Mon DD HH24:MI:SS YYYY')";
// 			else
// 				sql << " AND createtime <= STR_TO_DATE('" << toTimeString << "','%b %d %H:%i:%S %Y')";
//         }
// 
// 		// If the maximum number of events is not specified, then just use the default
// 		if (maxEvents != ULONG_MAX)
// 		{
// 			if (enumOracleDb == nDbType)
// 				rownumSql << " AND rownum <= " << ((maxEvents == 0) ? MAX_ROWS : maxEvents); 
// 			else
// 				rownumSql << " LIMIT " << ((maxEvents == 0) ? MAX_ROWS : maxEvents); 
// 		}
// 
//         //sql << " order by createtime desc";
// 	//	std::string strSql = defPrepareSQLStatement(nDbType, EVENT_Oracle_SELECT_17056, entityType, sql.str(), rownumSql.str());
// 		std::string strSql = databaseConnection->prepareSQLStatement(EVENT_SELECT_17056, entityType, sql.str(), rownumSql.str());
// 		TA_ASSERT(strSql.size() > 0, "MessageAccessFactory::getMessageOfEntityType(): Prepare SQL Statement error");
// 
//         // Set up the columnNames vector to be passed to executeQuery()
//         std::string keyColumn = "message_id";
//         std::vector<std::string> columnNames;
//         columnNames.push_back(keyColumn);
// 
//         LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"SQL statement is ", strSql.c_str()); 
// 
//         // Execute the query. The method can throw a DatabaseException.
//         // This is documented in the comment of this method.
//         // We are responsible for deleting the returned IData object when we're done with it
//        
//         IData* data = databaseConnection->executeQuery(strSql,columnNames);
//         
//         // Define the vector for returning the messages
//         std::vector<IMessageData*> messages;
// 
//         if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified message_id
//         {
//             // clean up the pointer
//             delete data;
//             data = NULL;
// 
//             return messages;
//         }
// 
//         do
//         {
//             for(unsigned int i = 0;i < data->getNumRows();i++)
//             {
//                 messages.push_back(new MessageData(data->getStringData(i,keyColumn)) );
//             }
// 
//             // Now that we're finished with the IData object, we need to delete it.
//             delete data;
//             data = NULL;
//         }
//         while (databaseConnection->moreData(data));
// 
//         // Create the Message object to represent this object. Initially only populate it
//         // with key. We will populate the rest of the data on demand.
//         return messages;
// 
// 
//         FUNCTION_EXIT;
//     }
	//TD 15056
	//zhou yuan++
// 	std::vector<IMessageData*> MessageAccessFactory::getMessageOfEntity( const unsigned long entityKey, 
// 								const bool isGetMessageOfOneDay /* = false*/,
// 								const time_t fromTime /* = 0 */, 
// 								const time_t toTime /* = 0 */, 
// 								const unsigned long maxEvents /* = 0 */)
// 	{
// 		FUNCTION_ENTRY("getMessageOfEntity");
// 
// 		IDatabase* databaseConnection = 
// 			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
// 		int nDbType = databaseConnection->getDbServerType();
// 		
//         // create the SQL string to retrieve the key of all messages in the system
//         std::ostringstream sql,rownSql;
//         //sql << "select message_id from event where entitykey = " << entityKey << " ";
// 		
//         // If the maximum number of events is not specified, then just use the default
//         if (maxEvents != ULONG_MAX)
//         {
// 			if (enumOracleDb == nDbType)
// 				rownSql << " AND rownum <= " << ((maxEvents == 0) ? MAX_ROWS : maxEvents); 
// 			else
// 				rownSql << " LIMIT " << ((maxEvents == 0) ? MAX_ROWS : maxEvents); 
//         }
// 		
// 		if(false == isGetMessageOfOneDay)
// 		{
// 			if (0 != fromTime)
// 			{
// 				// Use ACE routine ctime_r()
// 				// Wek Mon DD HH:MM:SS YYYY\n\0
// 				const unsigned int bufLen = 27;
// 				char buf[bufLen];
// 				std::string fromTimeString(ACE_OS::ctime_r(&fromTime,buf,bufLen));
// 				
// 				// and remove the characters we don't want - staring with the last two
// 				fromTimeString.erase(24,2);
// 				
// 				// and then the first 4
// 				fromTimeString.erase(0,4);
// 			
// 				if (enumOracleDb == nDbType)
// 					sql << " AND createtime >= TO_TIMESTAMP('" << fromTimeString << "','Mon DD HH24:MI:SS YYYY')";
// 				else
// 					sql << " AND createtime >= STR_TO_DATE('" << fromTimeString << "','%b %d %H:%i:%S %Y')";
// 			}
// 			
// 			if (0 != toTime)
// 			{
// 				// if the toTime is not the default, then there is a restriction on that too
// 				const unsigned int bufLen = 27;
// 				char buf[bufLen];
// 				std::string toTimeString(ACE_OS::ctime_r(&toTime,buf,bufLen));
// 				
// 				// and remove the characters we don't want - staring with the last two
// 				toTimeString.erase(24,2);
// 				
// 				// and then the first 4
// 				toTimeString.erase(0,4);
// 				
// 				if (enumOracleDb == nDbType)
// 					sql << " AND createtime <= TO_TIMESTAMP('" << toTimeString << "','Mon DD HH24:MI:SS YYYY')";
// 				else
// 					sql << " AND createtime <= STR_TO_DATE('" << toTimeString << "','%b %d %H:%i:%S %Y')";
// 			}
// 		}
// 		else
// 		{
// 			time_t now = time(NULL);
// 			const unsigned int bufLen = 27;
// 			char buf[bufLen];
// 			std::string toTimeString(ACE_OS::ctime_r(&now,buf,bufLen));
// 			
// 			// and remove the characters we don't want - staring with the last two
// 			toTimeString.erase(24,2);
// 			
// 			// and then the first 4
// 			toTimeString.erase(0,4);
// 			
// 			if (enumOracleDb == nDbType)
// 			{
// 				sql << " AND createtime <= TO_DATE('" << toTimeString << "','Mon DD HH24:MI:SS YYYY')";
// 				sql << " AND createtime >= (TO_DATE('" << toTimeString << "','Mon DD HH24:MI:SS YYYY') - 1)";
// 			}
// 			else
// 			{
// 				sql << " AND createtime <= STR_TO_DATE('" << toTimeString << "','%b %d %H:%i:%S %Y')";
// 				sql << " AND createtime >= DATE_SUB(STR_TO_DATE('" << toTimeString << "','%b %d %H:%i:%S %Y'), INTERVAL 1 DAY)";
// 			}
// 		}
// 		
//         //sql << " order by createtime desc";
// 	//	std::string strSql = defPrepareSQLStatement(nDbType, EVENT_Oracle_SELECT_17057, entityKey, sql.str(), rownSql.str());
// 		std::string strSql = databaseConnection->prepareSQLStatement(EVENT_SELECT_17057, entityKey, sql.str(), rownSql.str());
// 		TA_ASSERT(strSql.size() > 0, "MessageAccessFactory::getMessageOfEntity(): Prepare SQL Statement error");
// 		
//         // Set up the columnNames vector to be passed to executeQuery()
//         std::string keyColumn = "message_id";
//         std::vector<std::string> columnNames;
//         columnNames.push_back(keyColumn);
// 		
//         LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"SQL statement is %s", sql.str().c_str()); 
// 		
//         // Execute the query. The method can throw a DatabaseException.
//         // This is documented in the comment of this method.
//         // We are responsible for deleting the returned IData object when we're done with it        
//         IData* data = databaseConnection->executeQuery(strSql,columnNames);
//         
//         // Define the vector for returning the messages
//         std::vector<IMessageData*> messages;
// 		
//         if (0 == data->getNumRows()) // No entry found with the specified message_id
//         {
//             // clean up the pointer
//             delete data;
//             data = NULL;
// 			
//             return messages;
//         }
// 		
//         do
//         {
//             for(unsigned int i = 0;i < data->getNumRows();i++)
//             {
//                 messages.push_back(new MessageData(data->getStringData(i,keyColumn)) );
//             }
// 			
//             // Now that we're finished with the IData object, we need to delete it.
//             delete data;
//             data = NULL;
//         }
//         while (databaseConnection->moreData(data));
// 		
//         // Create the Message object to represent this object. Initially only populate it
//         // with key. We will populate the rest of the data on demand.
//         return messages;
// 		
// 		
//         FUNCTION_EXIT;		
// 	}
	//++zhou yuan

} // closes TA_Base_Core
