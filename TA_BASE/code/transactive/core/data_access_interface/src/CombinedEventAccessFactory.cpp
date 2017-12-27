/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/CombinedEventAccessFactory.cpp $
  * @author:  Bradley Cleaver
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  *
  * CombinedEventAccessFactory is a singleton that is used by the Event Viewer to
  * retrieve all data required to be presented to the user. It allows filtering and
  * sorting to done on the database side, and returns a vector of partially loaded
  * data to improve the efficiency.
  *
  */

// Disable "identifier was trucated to '255' characters" warnings.
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <time.h>
#include "ace/OS.h"
#include "core/data_access_interface/src/CombinedEventAccessFactory.h"
#include "core/data_access_interface/src/CombinedEventData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabaseCallbackUser.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/AlarmSeverityAccessFactory.h"

#include <sstream>

using TA_Base_Core::DatabaseFactory;
using TA_Base_Core::DataException;
using TA_Base_Core::IDatabase;
using TA_Base_Core::Thread;

// Define all constants in an anonymous namespace.
namespace
{
    const unsigned long SOURCE_TABLE_EVENT( 100 );
    const unsigned long SOURCE_TABLE_DATAPOINT( 200 );

    const std::string COLUMN_SOURCE_TABLE( "SOURCE_TABLE" );
    const std::string COLUMN_PKEY( "PKEY" );
    const std::string COLUMN_CREATETIME( "CREATETIME" );
    const std::string COLUMN_CREATEDATETIME( "CREATEDATETIME" );
    const std::string COLUMN_SEVERITY_NAME( "SEVERITYNAME" );
    const std::string COLUMN_ASSET_NAME( "ASSETNAME" );
    const std::string COLUMN_DESCRIPTION( "DESCRIPTION" );
	const std::string COLUMN_EVENT_TYPE_NAME( "EVENT_TYPE_NAME" );
    const std::string COLUMN_VALUE( "VALUE" );
	const std::string COLUMN_OPERATION_MODE("OPERATION_MODE");  // add for 955
    const std::string COLUMN_MMS( "MMS" );
    const std::string COLUMN_DSS( "DSS" );
    const std::string COLUMN_AVL( "AVL" );
    const std::string COLUMN_OPERATOR_NAME( "OPERATORNAME" );
    const std::string COLUMN_ALARM_COMMENT( "ALARMCOMMENT" );
    const std::string COLUMN_ALARM_ID( "ALARM_ID" );
	const std::string COLUMN_ALARM_ACK( "ALARM_ACK" );
	const std::string COLUMN_ALARM_STATUS( "ALARM_STATUS" );
    const std::string COLUMN_MAX_EVENT_KEY( "MaxEventKey" );
    const std::string COLUMN_MAX_DATAPOINT_KEY( "MaxDatapointKey" );

	const std::string COLUMN_LATEST_HOUR( "LatestHour" );
    const std::string ORDER_ASCENDING( "ASC" );
    const std::string ORDER_DESCENDING( "DESC" );

    const std::string ALWAYS_FALSE( "ALWAYS FALSE" );
	const std::string COLUMN_MAX_EVENT_LEVEL ( "MaxEventLevel");
	const std::string COLUMN_EVENT_COUNT ( "EventCount" );

	//source_table 
	const std::string COLUMN_SUBSYSTEM_KEY( "subsystem_key" );
	const std::string COLUMN_PHYSICAL_SUBSYSTEM_KEY( "physical_subsystem_key" );
	const std::string COLUMN_LOCATION_KEY( "location_key" );
	const std::string COLUMN_SEVERITY_KEY( "severity_key" );
	const std::string COLUMN_EVENT_TYPE_KEY( "event_type_key" );
	const std::string COLUMN_ALARM_TYPE_KEY( "alarm_type_key" );
	const std::string COLUMN_OPERATOR_KEY( "operator_key" );
	const std::string COLUMN_EVENT_LEVEL( "event_level" );
	const std::string COLUMN_SESSION_KEY( "session_key" );
	const std::string COLUMN_SESSION_LOCATION( "session_location" );
	const std::string COLUMN_PROFILE_ID( "profile_id" );
	const std::string COLUMN_ACTION_ID( "action_id" );
	const std::string COLUMN_ENTITY_KEY( "entity_key" );
	const std::string COLUMN_AVL_ALARM_HEADID( "avlalarmheadid" );
	const std::string COLUMN_SYSTEM_KEY( "system_key" );
	const std::string COLUMN_EVENT_ID( "event_id" );

	// add this const to judge when sequence meet the maximum value.
	const unsigned long VALVE = 50000000; 

};

namespace TA_Base_Core
{
    CombinedEventAccessFactory* CombinedEventAccessFactory::m_instance = NULL;

    CombinedEventAccessFactory& CombinedEventAccessFactory::getInstance()
    {
        // Create a new instance if it doesn't exist.
        if ( 0 == m_instance )
        {
            m_instance = new CombinedEventAccessFactory();
        }

        return *m_instance;
    }

	std::string CombinedEventAccessFactory::getLastEventID()
	{ 
		return m_latestEventID; 
	}

	//directly set latest peky/eventId by the parameter
	void CombinedEventAccessFactory::setMaxEventPkeyNEventId(CombinedEventAccessFactory::LatestKeyNID latestKeyNId)
	{
		m_curMaxPkey = latestKeyNId.maxPkey;
		if(latestKeyNId.event_id.compare("") != 0)
		{
			m_latestEventID = latestKeyNId.event_id;
		}
	}


	unsigned long CombinedEventAccessFactory::getPkeySequenceMaxValue()
	{

		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, EV_COMBINED_SELECT_13068);
		//		TA_ASSERT(sql.size() > 0, "CombinedEventAccessFactory::getMaxEventPKeyNEventId(): Prepare SQL Statement error");
		// Set up the columnNames vector to be passed to executeQuery()
		std::string maxValueColumn = "max_value";
		std::vector<std::string> columnNames;
		columnNames.push_back( maxValueColumn );

		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		std::auto_ptr<IData> data( databaseConnection->executeQuery( sql, columnNames ) );
		if(data->getNumRows() ==0 )
		{
			return 0;
		}
		unsigned long seqMaxVal = data->getUnsignedLongData(0 , maxValueColumn );

		return seqMaxVal;
	}
		
	CombinedEventAccessFactory::LatestKeyNID  CombinedEventAccessFactory::getMaxEventPKeyNEventId()
	{
		LatestKeyNID latest;
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
		// Create the SQL statement to execute.
		//std::string sql("SELECT last_number as maxpkey, cache_size as cachesize From user_sequences where upper(sequence_name) in ('EV_COMBINED_SEQ')");
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ev_combined_Oracle_SELECT_13051);
//		std::string sql  = databaseConnection->prepareSQLStatement(EV_COMBINED_SELECT_13051);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, EV_COMBINED_SELECT_13051);
//		TA_ASSERT(sql.size() > 0, "CombinedEventAccessFactory::getMaxEventPKey(): Prepare SQL Statement error");
		// Set up the columnNames vector to be passed to executeQuery()
		std::string maxPkeyColumn = "maxpkey";
		std::string cacheSizeColumn = "cachesize";
		std::vector<std::string> columnNames;
		columnNames.push_back( maxPkeyColumn );
		columnNames.push_back( cacheSizeColumn );
		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		std::auto_ptr<IData> data( databaseConnection->executeQuery( sql, columnNames ) );
		if(data->getNumRows() ==0 )
		{
			return latest;
		}
		unsigned long maxpkey= data->getUnsignedLongData(0 , maxPkeyColumn );
		unsigned long cacheSize= data->getUnsignedLongData(0 , cacheSizeColumn );
		cacheSize=cacheSize*2+1;
		latest.maxPkey = maxpkey;
		
		columnNames.clear();
		std::string eventIdColumn = "event_id";
		std::string pkeyColumn = "pkey";
		columnNames.push_back( eventIdColumn );
		columnNames.push_back( pkeyColumn );
		SQLStatement strSql;

		//select event_id , pkey from ev_combined where pkey between %s and %s order by pkey desc 
		//get the latest pkey and event_id
		databaseConnection->prepareSQLStatement(strSql, EV_COMBINED_SELECT_13002, maxpkey>=cacheSize? (maxpkey-cacheSize):0, maxpkey + cacheSize);
		std::auto_ptr<IData> data2( databaseConnection->executeQuery( strSql, columnNames ) );

		if( 0 < data2->getNumRows())
		{
			latest.maxPkey = data2->getUnsignedLongData(0 , pkeyColumn );
			latest.event_id = data2->getStringData(0, eventIdColumn);
		}
		else
		{
          return latest;
		}
		return latest;
	}


	time_t CombinedEventAccessFactory::getEarliestEventTime()
	{
		// Get a connection to the database
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

		// Create the SQL statement to execute.
		//std::string sql("SELECT TO_CHAR( MIN(createtime),'YYYYMMDDHH24MISS') AS earliesttime FROM ev_combined where createtime<sysdate");

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, EV_COMBINED_SELECT_13069);

		// Set up the columnNames vector to be passed to executeQuery()
		std::string earliestTimeColumn = "EARLIESTTIME";
		std::vector<std::string> columnNames;
		columnNames.push_back( earliestTimeColumn );

		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		std::auto_ptr<IData> data( databaseConnection->executeQuery( sql, columnNames ) );

		return data->getDateData( 0, earliestTimeColumn );
	}

	std::vector<ICombinedEventData*> CombinedEventAccessFactory::loadAllEvents( const EventFilter& filter)
	{
        // Now populate the vector with the matching events.
        std::vector<ICombinedEventData*> loadedEvents;

        if ( filterExcludesAll( filter, false ) )
        {
			m_lastWhereClauseAlwaysFalse = true;
            return loadedEvents;
        }

        // Get a connection to the database
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
		int nDbType = databaseConnection->getDbServerType();

        // Generate the where clause.
		// Add the time filters
        // Generate the where clause.
		SQLTypeAdapter sqlParamWhereClause;
		generateWhereClause(sqlParamWhereClause, filter, "", *databaseConnection );
		//std::string whereClause = generateWhereClause( filter, "", *databaseConnection );
		m_lastFilter = filter;

		std::ostringstream eventPkeySql;
		std::ostringstream datapointPkeySql;

		// Get the latest audit pkey from ev_combined
		LatestKeyNID latest=getMaxEventPKeyNEventId();
// 		if (latest.maxPkey>m_curMaxPkey)
// 		{
// 			m_curMaxPkey=latest.maxPkey;
// 			if(latest.event_id.compare("")!=0)
// 			{
// 				m_latestEventID = latest.event_id;
// 			}
// 		}
		setMaxEventPkeyNEventId(latest);

		// wenguang++ TD14188
        //std::ostringstream selectSql;
		SQLTypeAdapter sqlParamSelectSql;

		/*selectSql<< " SELECT ev_combined.source_table AS source_table,"
				   << " ev_combined.pkey AS pkey,"
				   << " TO_CHAR(ev_combined.createtime,'YYYYMMDDHH24MISSFF3') AS createtime,"
				   << " ev_combined.severity_name AS severityname,"
				   << " ev_combined.asset_name AS assetname,"
				   << " ev_combined.description AS description,"
				   << " ev_combined.event_type_name AS event_type_name,"
				   << " ev_combined.value AS value,"
				   << " ev_combined.mms_state AS mms,"
				   << " ev_combined.dss_state AS dss,"
				   << " ev_combined.avl_state AS avl,"
				   << " ev_combined.operator_name AS operatorname,"
				   << " ev_combined.alarm_comment AS alarmcomment,"
				   << " ev_combined.alarm_id AS alarm_id,"
				   << " ev_combined.alarm_ack AS alarm_ack,"
				   << " ev_combined.alarm_status AS alarm_status"
			<< " ev_combined.operation_mode"    // add this column for 955
				   << " FROM ev_combined";*/
 
		/*if (whereClause.empty() == false)
		{
			selectSql << " WHERE " << whereClause;
		}*/

		if (!sqlParamWhereClause.empty())
		{
			sqlParamSelectSql.add(std::string(" WHERE "));
			sqlParamSelectSql.add(sqlParamWhereClause);
		}

		/*std::ostringstream sql;
		sql << selectSql.str(); */
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ev_combined_Oracle_SELECT_13053, selectSql.str());
//		std::string sql  = databaseConnection->prepareSQLStatement(EV_COMBINED_SELECT_13053, selectSql.str());
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, EV_COMBINED_SELECT_13053, sqlParamSelectSql);
//		TA_ASSERT(sql.size() > 0, "CombinedEventAccessFactory::loadAllEvents(): Prepare SQL Statement error");

        // Set up required columns.
        std::vector<std::string> columnNames;
        columnNames.push_back( COLUMN_SOURCE_TABLE );
        columnNames.push_back( COLUMN_PKEY );
        columnNames.push_back( COLUMN_CREATETIME );
        columnNames.push_back( COLUMN_SEVERITY_NAME );
        columnNames.push_back( COLUMN_ASSET_NAME );
        columnNames.push_back( COLUMN_DESCRIPTION );
		columnNames.push_back( COLUMN_EVENT_TYPE_NAME );
        columnNames.push_back( COLUMN_VALUE );
        columnNames.push_back( COLUMN_MMS );
        columnNames.push_back( COLUMN_DSS );
        columnNames.push_back( COLUMN_AVL );
        columnNames.push_back( COLUMN_OPERATOR_NAME );
        columnNames.push_back( COLUMN_ALARM_COMMENT );
        columnNames.push_back( COLUMN_ALARM_ID );
		columnNames.push_back( COLUMN_ALARM_ACK );
		columnNames.push_back( COLUMN_ALARM_STATUS );
		columnNames.push_back(COLUMN_OPERATION_MODE);

        std::auto_ptr<IData> data( databaseConnection->executeQuery( sql, columnNames ) );

        if ( 0 < data->getNumRows() )
        {
            for( unsigned int i=0; i< data->getNumRows(); i++ )
            {
                // Now construct the object and add it to the list.
                loadedEvents.push_back( createCombinedEvent( i, data.get() ) );
            }
		}

        return loadedEvents;
 	}


	std::vector<ICombinedEventData*> CombinedEventAccessFactory::getNextEvents()
	{
		std::vector<ICombinedEventData*> loadedEvents;

		// Get a connection to the database
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

		IData* dataPtr = NULL;
		if ( databaseConnection->moreData( dataPtr ) && ( 0 < dataPtr->getNumRows() ) )
		{
			// Take care of memory management.
			std::auto_ptr<IData> data( dataPtr );

			for( unsigned int i=0; i< data->getNumRows(); i++ )
			{
				// Now construct the object and add it to the list.
				loadedEvents.push_back( createCombinedEvent( i, data.get() ) );
				unsigned long pkey=data->getUnsignedLongData(i,COLUMN_PKEY);

				//update current latest pkey, consider sequence recycle circumstance.
				// need to consider the situation that the sequence meets the maximum and reset to minimum.
				// consider these 2 special situations: 1) m_latestpkey=2, pkey=99999999 2) m_latestpkey=99999999, pkey=2
				if ( ( pkey>m_curMaxPkey && pkey-m_curMaxPkey < VALVE) 
					|| ( pkey< m_curMaxPkey && m_curMaxPkey - pkey > VALVE ) )
				{
					m_curMaxPkey = pkey;
				}

			}
		}

		return loadedEvents;
	}


	std::vector<ICombinedEventData*> CombinedEventAccessFactory::getEventsSinceLastLoad()
	{
		std::vector<ICombinedEventData*> newEvents;

		// If the filter excludes all possible events just return.
		if ( m_lastWhereClauseAlwaysFalse )
		{
			return newEvents;
		}

		// First the event table needs to be checked for new elements.
		LatestKeyNID latest = getLatestPkeys();

		// If the latest pkeys match ours then don't load. The keys will never match exactly, rather
		// they are considered to match if the latest is exactly one ahead of the last loaded.
		if ( latest.maxPkey == m_curMaxPkey )
		{
			return newEvents;
		}	
		std::string pkeyClauseConj = " AND ";
		if(latest.maxPkey < m_curMaxPkey)
		{
			pkeyClauseConj = " OR "; 
		}

		
		// Get a connection to the database
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ev_combined_Oracle_SELECT_13056, m_curMaxPkey, latest.maxPkey, m_lastWhereClause);
		
		//std::string whereClause = generateWhereClause(m_lastFilter, "", *databaseConnection);
		SQLTypeAdapter sqlParamWhereClause;
		generateWhereClause(sqlParamWhereClause, m_lastFilter, "", *databaseConnection);
//		std::string sql  = databaseConnection->prepareSQLStatement(EV_COMBINED_SELECT_13056, m_curMaxPkey, latest.maxPkey, whereClause);
		SQLStatement sql;

		databaseConnection->prepareSQLStatement(sql, EV_COMBINED_SELECT_13056, m_curMaxPkey, pkeyClauseConj, latest.maxPkey, sqlParamWhereClause);
//		TA_ASSERT(sql.size() > 0, "CombinedEventAccessFactory::getEventsSinceLastLoad(): Prepare SQL Statement error");

		// Set up required columns.
		std::vector<std::string> columnNames;
		columnNames.push_back( COLUMN_EVENT_ID );
		columnNames.push_back( COLUMN_SOURCE_TABLE );
		columnNames.push_back( COLUMN_PKEY );
		columnNames.push_back( COLUMN_CREATETIME );
		columnNames.push_back( COLUMN_SEVERITY_NAME );
		columnNames.push_back( COLUMN_ASSET_NAME );
		columnNames.push_back( COLUMN_DESCRIPTION );
		columnNames.push_back( COLUMN_EVENT_TYPE_NAME );
		columnNames.push_back( COLUMN_VALUE );
		columnNames.push_back( COLUMN_MMS );
		columnNames.push_back( COLUMN_DSS );
		columnNames.push_back( COLUMN_AVL );
		columnNames.push_back( COLUMN_OPERATOR_NAME );
		columnNames.push_back( COLUMN_ALARM_COMMENT );
		columnNames.push_back( COLUMN_ALARM_ID );
		columnNames.push_back( COLUMN_ALARM_ACK );
		columnNames.push_back( COLUMN_ALARM_STATUS );
		columnNames.push_back( COLUMN_OPERATION_MODE );

		setMaxEventPkeyNEventId(latest);

		// Execute the SQL.
		std::auto_ptr<IData> data( databaseConnection->executeQuery( sql, columnNames ) );

		//Then, use the event records to update current pkey/eventID if it's newer then last step.
		unsigned long tempPkey=0;
		for( unsigned int i=0; i< data->getNumRows(); i++ )
		{
			ICombinedEventData* event = createCombinedEvent( i, data.get() );
			
			// Now construct the object and add it to the list.
			newEvents.push_back( event );

			//update pkey and eventID by the result record
			tempPkey=data->getUnsignedLongData(i,COLUMN_PKEY);			
			if (tempPkey>m_curMaxPkey && tempPkey-m_curMaxPkey<VALVE || tempPkey<m_curMaxPkey && m_curMaxPkey-tempPkey>VALVE)
			{
				m_curMaxPkey=tempPkey;				
				m_latestEventID = data->getStringData(i, COLUMN_EVENT_ID);
			}
		}


		return newEvents;
	}


    void CombinedEventAccessFactory::resetStopFlag()
    {
        m_stopLoad = false;
    }

    void CombinedEventAccessFactory::applicationTerminating()
    {
        m_stopLoad = true;
        m_pauseLoad = false;
    }


    void CombinedEventAccessFactory::applicationPausing()
    {
        m_pauseLoad = true;
    }


    void CombinedEventAccessFactory::applicationResuming()
    {
        m_pauseLoad = false;
    }


    ICombinedEventData* CombinedEventAccessFactory::createCombinedEvent( unsigned long row, IData* data )
    {
        // Get the source table ID and convert it to an event type.
        ECombinedEventType eventType = ( SOURCE_TABLE_EVENT == data->getUnsignedLongData( row, COLUMN_SOURCE_TABLE ) ) ? E_AUDIT_EVENT : E_DPSTATECHANGE_EVENT;

		//get EventID:
		std::string eventID = data->getStringData(row, COLUMN_EVENT_ID);

        // Get the pkey for the event.
        unsigned long pkey = data->getUnsignedLongData( row, COLUMN_PKEY );

        // Get the time the event was created.
        timeb createTime = data->getTimestampData( row, COLUMN_CREATETIME );

        // Get the severity name.
        std::string severityName = data->getStringData( row, COLUMN_SEVERITY_NAME );

        // Get the asset name.
        std::string assetName = data->getStringData( row, COLUMN_ASSET_NAME );

        // Get the description.
        std::string description = data->getStringData( row, COLUMN_DESCRIPTION );
		std::string eventTypeName = data->getStringData( row, COLUMN_EVENT_TYPE_NAME );

        // Get the value.
        std::string value = data->getStringData( row, COLUMN_VALUE );

        // Get the MMS state information.
        std::string mmsState = data->getStringData( row, COLUMN_MMS );

        // Get the DSS state information.
        std::string dssState = data->getStringData( row, COLUMN_DSS );

		std::string avlState = data->getStringData( row, COLUMN_AVL );

        // Get the operator name.
        std::string operatorName = data->getStringData( row, COLUMN_OPERATOR_NAME );

        // Get the alarm comment.
        std::string alarmComment = data->getStringData( row, COLUMN_ALARM_COMMENT );

        // Get the alarm ID.
        std::string alarmId = data->getStringData( row, COLUMN_ALARM_ID );
        if ( alarmId == " " )
        {
            alarmId = "";
        }

        // Get the alarm ack.
        std::string alarmAck = data->getStringData( row, COLUMN_ALARM_ACK );
        if ( alarmAck == " " )
        {
            alarmAck = "";
        }

        // Get the alarm status.
		std::string alarmStatus = data->getStringData( row, COLUMN_ALARM_STATUS );
        if ( alarmStatus == " " )
        {
            alarmStatus = "";
        }



		std::string operation_mode = data->getStringData(row,COLUMN_OPERATION_MODE);
		CombinedEventData * eventData = new CombinedEventData( eventType, pkey, createTime, severityName, assetName, description,
			value, mmsState, dssState, avlState, operatorName, alarmComment, alarmId, alarmAck, alarmStatus, eventTypeName, operation_mode);
		eventData->setEventID(eventID);
		return eventData;
	
    }


    CombinedEventAccessFactory::LatestKeyNID CombinedEventAccessFactory::getLatestPkeys()
    {
        LatestKeyNID keys;
		try
		{
			 
			keys = getMaxEventPKeyNEventId();
			
        }
        catch ( ... )
        {
        }
     
        return keys;
    }


    void CombinedEventAccessFactory::generateWhereClause(SQLTypeAdapter& rSqlParam, const EventFilter& filter, const std::string& timeRestrictionClause, 
														 TA_Base_Core::IDatabase& databaseConnection,unsigned long pkeyBoundary ,bool onNext)
    {
        std::stringstream whereClause;

        // Compliance with the C++ standard would be nice...
        unsigned int i = 0;

		//std::stringstream timeString;
		SQLTypeAdapter sqlParamTimeStr;
		generateTimeStr(filter, timeRestrictionClause ,pkeyBoundary ,onNext, sqlParamTimeStr);
		//std::string timeStr = timeString.str();
		if (/*timeStr.size()>0*/!sqlParamTimeStr.empty())
		{			
			//whereClause << timeStr;
			rSqlParam.add(sqlParamTimeStr);
		}
/*		int nDbType    = databaseConnection.getDbServerType();

		if( strcmp(timeRestrictionClause.c_str(), "") == 0 )
		{
			// Add the time filters
			std::string toTime;
			if ( 0 != filter.toTime )
			{
				const unsigned int bufLen = 15;
				char buf[bufLen];
				struct tm * ptmToTime = localtime(&filter.toTime);
				if(ptmToTime != NULL)
				{
					struct tm tmToTime = *ptmToTime;
					strftime( buf, 15, "%d%m%Y%H%M%S", &tmToTime);

					toTime = buf;
					switch (nDbType)
					{
					case enumOracleDb:
						toTime = "TO_DATE('" + toTime + "','DDMMYYYYHH24MISS')";
						break;
					case enumMysqlDb:
						toTime = "STR_TO_DATE('" + toTime + "','%d%m%Y%H%i%S')";
						break;
					}	
				}
			}

			std::string fromTime;
			if ( 0 == filter.fromTime )
			{
				// Default is 24 hours before to.
				switch(nDbType)
				{
				case enumOracleDb:
					fromTime = (toTime.empty()?"SYSDATE":toTime)+" - INTERVAL '1' DAY";
					break;
				case enumMysqlDb:
					if ( toTime.empty() )
						fromTime = "DATE_SUB( SYSDATE() , INTERVAL 1 DAY)";
					else
						fromTime = "DATE_SUB("+toTime+" , INTERVAL 1 DAY)";
					break;
				}				
			}
			else
			{
				const unsigned int bufLen = 15;
				char buf[bufLen];
				struct tm *ptmFromTime = localtime(&filter.fromTime);
				if(ptmFromTime != NULL)
				{
					struct tm tmFromTime = *ptmFromTime;
					strftime( buf, 15, "%d%m%Y%H%M%S", &tmFromTime);

					fromTime = buf;
					switch(nDbType)
					{
					case enumOracleDb:
						fromTime = "TO_DATE('" + fromTime + "','DDMMYYYYHH24MISS')";
						break;
					case enumMysqlDb:
						fromTime = "STR_TO_DATE('" + fromTime + "','%d%m%Y%H%i%S')";
						break;
					}
				}

				if( 0 == filter.toTime)
				{

					switch(nDbType)
					{
					case enumOracleDb:
						// to = from +1day
						toTime = fromTime + " + INTERVAL '1' DAY ";
						break;
					case enumMysqlDb:
						toTime = "DATE_ADD('"+fromTime+"' , INTERVAL 1 DAY)";
						break;

					}
					
				}


			}

			// Now construct the time portion of the where clause.
			whereClause << "createtime >= " << fromTime;
			if ( !toTime.empty() )
			{
				whereClause << " AND createtime <= " << toTime;
			}
		}
		else
		{
			whereClause << timeRestrictionClause;
		}*/

        // Add the event level.
        whereClause << " AND event_level != 0 AND event_level <= " << filter.eventLevel; 

        // Add filters for locations and subsystems.
        std::string locAndSubsystem = generateLocAndSubsystemExpr( filter, databaseConnection );
        if ( !locAndSubsystem.empty() )
        {
            whereClause << " AND ( " << locAndSubsystem << " )";
        }

        // Filter by operator IDs.
        if ( !filter.operatorIds.empty() )
        {
            whereClause << " AND ( " << generateORList( "operator_key", filter.operatorIds, databaseConnection ) << " )";
        }

		// Filter by system IDs.  // add for 955
		if ( !filter.systemIds.empty() )
		{
			whereClause << " AND ( " << generateORList( "system_key", filter.systemIds, databaseConnection ) << " )";
		}

        // Filter by asset name.
        if ( !filter.assetName.empty() )
        {
            // Use entity name if asset name is NULL???

			//TD15029 azenitha++
			const char* assetNameLwr = strlwr((char*)filter.assetName.c_str());
            //whereClause << " AND asset_name LIKE '" << filter.assetName << "'";
			whereClause << " AND LOWER(asset_name) LIKE '" << assetNameLwr << "'";
			//TD15029 ++azenitha

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "TD15029: Case insensitive search for asset");
        }

		// Filter by TM type:
		if ( !filter.tmType.empty() && ""!=filter.tmType)
		{
			whereClause << " AND operation_mode = '" << filter.tmType.c_str() << "'";
		}

        // Filter by description.
        if ( !filter.description.empty() )
        {
			//TD15029 azenitha++
			const char* descriptionLwr = strlwr((char*)filter.description.c_str());
            //whereClause << " AND ( description LIKE '" << filter.description << "'";
			whereClause << " AND ( LOWER(description) LIKE '" << descriptionLwr << "'";
			//TD15029 ++azenitha

			// TD 14751 add filter by entity type as well as it has been appended to description

			//TD15029 azenitha++
            //whereClause << " OR event_type_name LIKE '" << filter.description << "' )";
			whereClause << " OR LOWER(event_type_name) LIKE '" << descriptionLwr << "' )";
			//TD15029 ++azenitha

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "TD15029: Case insensitive search for description");
        }

        // Filter by severity IDs - this part of the clause must always exist.
        whereClause << " AND ( ";
        if ( filter.showNonAlarmEvents )
        {
			whereClause << " severity_key IS NULL or severity_key=0 ";
        }
        if ( !filter.severityIds.empty() )
        {
            if ( filter.showNonAlarmEvents )
            {
                whereClause << " OR ";
            }
            whereClause << generateORList( "severity_key", filter.severityIds, databaseConnection );
        }
        whereClause << " )";

        // Filter by alarm types IDs.
        if ( !filter.alarmTypes.empty() )
        {
            whereClause << " AND ( " << generateORList( "alarm_type_key", filter.alarmTypes, databaseConnection ) << " )";
        }

        // Filter by alarm ID.
        if ( !filter.alarmId.empty() )
        {
            if (filter.alarmId.size() > 1)
			{
				unsigned int i = 0;
				whereClause << " AND (alarm_id LIKE '" // first alarm
					<< databaseConnection.escapeQueryString(filter.alarmId[i])
					<< "'";

				for (i=1; i<filter.alarmId.size(); ++i) // second alarm onwards
				{
					whereClause << " OR alarm_id LIKE '"
						<< databaseConnection.escapeQueryString(filter.alarmId[i])
						<< "'";
				}
				// close the bracket
				whereClause << ")";
			}
			else // there is only 1 alarm ID
			{
				whereClause << " AND alarm_id LIKE '"
					<< databaseConnection.escapeQueryString(filter.alarmId[0])
					<< "'";
			}
        }

        //return whereClause.str();
		rSqlParam.add(whereClause.str());
    }

    std::string CombinedEventAccessFactory::generateORList( const std::string& columnName,
                                                            const NumericList& keyList,
                                                            IDatabase& databaseConnection)
    {
        // The string to store the list in.
        std::stringstream expression;

        NumericList::const_iterator keyIt;
        for ( keyIt=keyList.begin(); keyIt!=keyList.end(); keyIt++ )
        {
            // If this isn't the first element then it needs an OR added first..
            if ( keyIt != keyList.begin() )
            {
                expression << " OR ";
            }

            // Add the expression to the list.
            expression << databaseConnection.escapeInsertString(columnName) << "=" << *keyIt;
        }

        return expression.str();
    }

    std::string CombinedEventAccessFactory::generateLocAndSubsystemExpr( const EventFilter& filter, IDatabase& databaseConnection )
    {
		//if ( !filter.dutyEnabled )
		{
			// Duty isn't enabled.
			// ( loc1 OR loc2 ) AND ( ss1 OR ss2 OR pss1 OR pss2 )

			// Get the expression that filters on location.
			std::string locationExpr = generateORList( "location_key", filter.locationIds, databaseConnection );
			
			std::string subsystemExpr = joinExpressions(
				generateORList( "subsystem_key", filter.appSubsystemIds, databaseConnection ), "OR",
				generateORList( "physical_subsystem_key", filter.physSubsystemIds, databaseConnection ), false );
			
			// Return the expression.
			return joinExpressions( locationExpr, "AND", subsystemExpr );
		}

        return "";
    }


    std::string CombinedEventAccessFactory::joinExpressions( const std::string& expr1,
                                                             const std::string& operation,
                                                             const std::string& expr2,
                                                             bool bracketExpr /*=true*/ )
    {
        std::stringstream joined;

        if ( !expr1.empty() )
        {
            if ( bracketExpr )
            {
                joined << "( " << expr1 << " )";
            }
            else
            {
                joined << expr1;
            }
        }
        if ( !expr1.empty() && !expr2.empty() )
        {
            joined << " " << operation << " ";
        }
        if ( !expr2.empty() )
        {
            if ( bracketExpr )
            {
                joined << "( " << expr2 << " )";
            }
            else
            {
                joined << expr2;
            }
        }

        return joined.str();
    }

    bool CombinedEventAccessFactory::filterExcludesAll( const EventFilter& filter, bool isVisibilityEnabled)
    {
        bool returnVal = false;

        // If no severites or 'non alarm events'...
        if ( filter.severityIds.empty() && !filter.showNonAlarmEvents )
        {
            returnVal = true;
        }


        return returnVal;
    }

	// queryStartPkey is different from pkeyBoundary.
	// queryStartPkey means this query will be select from [queryStartPkey, queryEndPkey) -- for forward query,
	// or (queryStartPkey , queryEndPkey ] -- for backward query,
	// if queryStartPkey<=0 or queryEndPkey<=0, means this conditions is meaningless, will not use it.
	// pkeyBoundary is the boundary pkey get when user click "->" or "<-" in EventViewer. This pkey shouldn't be included in the query result.
	std::vector<ICombinedEventData*> CombinedEventAccessFactory::loadDesignatedEventsInASection(
		unsigned long queryStartPkey,
		unsigned long queryEndPkey,
		std::string & FirstPartSQLStr,
		bool enableVisibilitRule,  
		const EventFilter& filter,  
		const unsigned long endRow,
		unsigned long pkeyBoundary,
		bool onNext)
	{
		// Now populate the vector with the matching events.
		std::vector<ICombinedEventData*> loadedEvents;

		// Get a connection to the database
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
		int nDbType = databaseConnection->getDbServerType();

		// If the filter excludes all possible events just return.
		if ( filterExcludesAll( filter, true ) )
		{
			m_lastDutyWhereClause = ALWAYS_FALSE;
			m_lastOwnerWhereClause = ALWAYS_FALSE;
			m_lastSystemWhereClause = ALWAYS_FALSE;
			return loadedEvents;
		}


		std::ostringstream FirstPartSQLStream;
		//std::string whereClause;
		//std::string strSelectSql, strSql;
		SQLTypeAdapter sqlParamWhereClause;
		SQLStatement strSql;
		SQLStatement strSelectSql;


		//if(FirstPartSQLStr == "")   // means this loading is happend first time.  
		// will not use last time's sql string because the DB may have been changed
		{
			if (!enableVisibilitRule)
			{
				//whereClause = generateWhereClause(filter, "", *databaseConnection,pkeyBoundary,onNext);
				generateWhereClause(sqlParamWhereClause, filter, "", *databaseConnection,pkeyBoundary,onNext);
				m_lastFilter = filter;
				/*selectSql<< " Select * from ( SELECT / *+  index (ev_combined EVCOM_SUB_TIME_INDX ) * /"*/
				/*selectSql<< " Select * from ( SELECT "	
					<< " source_table AS source_table,"
					<< " pkey AS pkey,"
					<< " TO_CHAR(createtime,'YYYYMMDDHH24MISSFF3') AS createtime,"
					<< " severity_name AS severityname,"
					<< " asset_name AS assetname,"
					<< " description AS description,"
					<< " event_type_name AS event_type_name,"
					<< " value AS value,"
					<< " mms_state AS mms,"
					<< " dss_state AS dss,"
					<< " avl_state AS avl,"
					<< " operator_name AS operatorname,"
					<< " alarm_comment AS alarmcomment,"
					<< " alarm_id AS alarm_id,"
					<< " alarm_ack AS alarm_ack,"
					<< " alarm_status AS alarm_status,"
					<< " operation_mode"
					<< " FROM ev_combined";		*/

				//FirstPartSQLStream << selectSql.str()  <<" WHERE " << m_lastWhereClause;
//				strSelectSql  = databaseConnection->prepareSQLStatement(EV_COMBINED_SELECT_13057, whereClause);

				databaseConnection->prepareSQLStatement(strSelectSql, EV_COMBINED_SELECT_13057, sqlParamWhereClause);
				//FirstPartSQLStream << strSelectSql;
				
				std::string strExSql;
				int nExDbType= databaseConnection->getDbServerType();
				switch (nExDbType)
				{
				case enumOracleDb:
					strExSql = (strSelectSql.strCommon.empty() ? strSelectSql.strOracle : strSelectSql.strCommon);
					break;
				case enumMysqlDb:
					strExSql = (strSelectSql.strCommon.empty() ? strSelectSql.strMySQL : strSelectSql.strCommon);
					break;		
				}
				FirstPartSQLStream << strExSql;

			}
			else
			{
				//whereClause = generateDesignatedWhereClause(filter, "", *databaseConnection, pkeyBoundary, onNext);
				generateDesignatedWhereClause(sqlParamWhereClause, filter, "", *databaseConnection, pkeyBoundary, onNext);
				m_lastFilter = filter;

				/*selectSql<< " Select * from ( SELECT / *+  index (ev_combined EVCOM_SUB_TIME_INDX ) * /"*/
// 				selectSql<< " Select * from ( SELECT "
// 					<< " source_table AS source_table,"
// 					<< " pkey AS pkey,"
// 					<< " TO_CHAR(createtime,'YYYYMMDDHH24MISSFF3') AS createtime,"
// 					<< " severity_name AS severityname,"
// 					<< " asset_name AS assetname,"
// 					<< " description AS description,"
// 					<< " event_type_name AS event_type_name,"
// 					<< " value AS value,"
// 					<< " mms_state AS mms,"
// 					<< " dss_state AS dss,"
// 					<< " avl_state AS avl,"
// 					<< " operator_name AS operatorname,"
// 					<< " alarm_comment AS alarmcomment,"
// 					<< " alarm_id AS alarm_id,"
// 					<< " alarm_ack AS alarm_ack,"
// 					<< " alarm_status AS alarm_status, "
// 					<< " operation_mode as operation_mode";

				//FirstPartSQLStream << selectSql.str() << m_lastWhereClause;
//				strSelectSql  = databaseConnection->prepareSQLStatement(EV_COMBINED_SELECT_13058, whereClause);

				databaseConnection->prepareSQLStatement(strSelectSql, EV_COMBINED_SELECT_13058, sqlParamWhereClause);
				//FirstPartSQLStream << strSelectSql;
				std::string strExSql;
				int nExDbType= databaseConnection->getDbServerType();
				switch (nExDbType)
				{
				case enumOracleDb:
					strExSql = (strSelectSql.strCommon.empty() ? strSelectSql.strOracle : strSelectSql.strCommon);
					break;
				case enumMysqlDb:
					strExSql = (strSelectSql.strCommon.empty() ? strSelectSql.strMySQL : strSelectSql.strCommon);
					break;		
				}
				FirstPartSQLStream << strExSql;

			}

		}
// 		else
// 		{
// 			FirstPartSQLStream << FirstPartSQLStr;  // means it's not the first time loading , can use the sql string(first part) from last time query.
// 		}

		std::ostringstream secondPartSQL;
		std::ostringstream thirdPartSQL;

		if( onNext )
		{
			if( queryStartPkey>0 && queryEndPkey>0 )
			{
				secondPartSQL << " AND pkey>=" <<queryStartPkey <<" AND pkey<"<<queryEndPkey; 
			}else if ( queryStartPkey>0 )
			{
				secondPartSQL << " AND pkey>=" <<queryStartPkey ; 
			}else if ( queryEndPkey>0 )
			{
				secondPartSQL << " AND pkey<"<<queryEndPkey; 
			}

			//thirdPartSQL << " order by createtime asc ) " << " where ROWNUM  <= " << endRow  ;
//			strSql  = databaseConnection->prepareSQLStatement(EV_COMBINED_SELECT_13063, strSelectSql, secondPartSQL.str(), endRow);

			//dynsql delete by lin
			databaseConnection->prepareSQLStatement(strSql, EV_COMBINED_SELECT_13063, strSelectSql, secondPartSQL.str(), endRow);

		}else  // backward query
		{
			if( queryStartPkey>0 && queryEndPkey>0 )
			{
				secondPartSQL << " AND pkey>" << queryStartPkey << " AND pkey<=" << queryEndPkey; 
			}else if( queryStartPkey>0 )
			{
				secondPartSQL << " AND pkey>" << queryStartPkey ; 
			}else if ( queryEndPkey>0 )
			{
				secondPartSQL << " AND pkey<="<<queryEndPkey; 
			}
			//thirdPartSQL << " order by createtime desc ) " << " where ROWNUM  <= " << endRow  ;
//			strSql  = databaseConnection->prepareSQLStatement(EV_COMBINED_SELECT_13059, strSelectSql, secondPartSQL.str(), endRow);

			//dynsql delete by lin
			databaseConnection->prepareSQLStatement(strSql, EV_COMBINED_SELECT_13059, strSelectSql, secondPartSQL.str(), endRow);
		}

// 		std::ostringstream sql ;
// 		sql<<FirstPartSQLStream.str() << secondPartSQL.str() << thirdPartSQL.str();

		// Set up required columns.
		std::vector<std::string> columnNames;
		columnNames.push_back( COLUMN_EVENT_ID );
		columnNames.push_back( COLUMN_SOURCE_TABLE );
		columnNames.push_back( COLUMN_PKEY );
		columnNames.push_back( COLUMN_CREATETIME );
		columnNames.push_back( COLUMN_SEVERITY_NAME );
		columnNames.push_back( COLUMN_ASSET_NAME );
		columnNames.push_back( COLUMN_DESCRIPTION );
		columnNames.push_back( COLUMN_EVENT_TYPE_NAME );
		columnNames.push_back( COLUMN_VALUE );
		columnNames.push_back( COLUMN_MMS );
		columnNames.push_back( COLUMN_DSS );
		columnNames.push_back( COLUMN_AVL );
		columnNames.push_back( COLUMN_OPERATOR_NAME );
		columnNames.push_back( COLUMN_ALARM_COMMENT );
		columnNames.push_back( COLUMN_ALARM_ID );
		columnNames.push_back( COLUMN_ALARM_ACK );
		columnNames.push_back( COLUMN_ALARM_STATUS );
		columnNames.push_back( COLUMN_OPERATION_MODE );
		
// 		for (unsigned long sqlCnt=0; sqlCnt<strSql.size()/100+1;sqlCnt++)
// 		{
// 			std::string newstr=strSql.substr(sqlCnt*100,100);
// 			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "New EV Log:LoadEvent:DatabaseQuery, sql sentence: %s", newstr.c_str());
// 		}

		std::string strExSql;
		int nExDbType= databaseConnection->getDbServerType();
		switch (nExDbType)
		{
		case enumOracleDb:
			strExSql = (strSql.strCommon.empty() ? strSql.strOracle : strSql.strCommon);
			break;
		case enumMysqlDb:
			strExSql = (strSql.strCommon.empty() ? strSql.strMySQL : strSql.strCommon);
			break;		
		}

		for (unsigned long sqlCnt = 0; sqlCnt < strExSql.size() / 100 + 1; sqlCnt++)
		{
			std::string newstr=strExSql.substr(sqlCnt * 100, 100);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "New EV Log:LoadEvent:DatabaseQuery, sql sentence: %s", newstr.c_str());
		}


		std::auto_ptr<IData> data( databaseConnection->executeQuery( strSql, columnNames ) );
		unsigned long firstBatchSize = data->getNumRows();

		unsigned long tempPkey=0;
		for( unsigned int i=0; i< firstBatchSize; i++ )
		{
			// Now construct the object and add it to the list.
			loadedEvents.push_back( createCombinedEvent( i, data.get() ) );

			if( FirstPartSQLStr == "") 
			// when it's first time loading, should update latest pkey/eventId by results because it's might newer then last query("getMaxEventPkeyNEventID").
			{
				tempPkey=data->getUnsignedLongData(i,COLUMN_PKEY);			
				if ( (tempPkey>m_curMaxPkey && tempPkey-m_curMaxPkey<VALVE) || (tempPkey<m_curMaxPkey && m_curMaxPkey-tempPkey>VALVE) )
				{
					m_curMaxPkey=tempPkey;				
					m_latestEventID = data->getStringData(i, COLUMN_EVENT_ID);
				}
			}
			
		}

		if( FirstPartSQLStr == "") // when it's first time loading:
		{
			FirstPartSQLStr = FirstPartSQLStream.str();
		}

		return loadedEvents;

	}

	std::vector<ICombinedEventData*> CombinedEventAccessFactory::loadDesignatedEvents(
		bool enableVisibilitRule,  
		const EventFilter& filter, 
		const std::string pageStartTime, 
		const unsigned long endRow,
		unsigned long pkeyBoundary,
		bool onNext)
	{
		// Now populate the vector with the matching events.
		std::vector<ICombinedEventData*> loadedEvents;

		// Get a connection to the database
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
		int nDbType = databaseConnection->getDbServerType();

		if ( filter.dutyEnabled == false )
		{
			m_isLoadingAll = true;
			return loadAllEvents(filter);
		}
		else
		{
			m_isLoadingAll = false;
		}

		// If the filter excludes all possible events just return.
		if ( filterExcludesAll( filter, true ) )
		{
			m_lastDutyWhereClause = ALWAYS_FALSE;
			m_lastOwnerWhereClause = ALWAYS_FALSE;
			m_lastSystemWhereClause = ALWAYS_FALSE;
			return loadedEvents;
		}
         

		//std::ostringstream selectSql;
		//std::ostringstream sqlPageTime;
		//std::string strSelectSql, strSql;
		SQLTypeAdapter sqlParamPageTime;
		
		SQLStatement strSelectSql;
		SQLStatement strSql;

		//std::string whereClause;
		SQLTypeAdapter sqlParamWhereClause;
		if (!enableVisibilitRule)
		{
			//whereClause = generateWhereClause(filter, "", *databaseConnection, pkeyBoundary, onNext);
			generateWhereClause(sqlParamWhereClause, filter, "", *databaseConnection, pkeyBoundary, onNext);
			m_lastFilter = filter;
				//selectSql<< " SELECT /*+  index (ev_combined EVCOM_SUB_TIME_INDX ) */"
				/*<< " source_table AS source_table,"
				<< " pkey AS pkey,"
				<< " TO_CHAR(createtime,'YYYYMMDDHH24MISSFF3') AS createtime,"
				<< " severity_name AS severityname,"
				<< " asset_name AS assetname,"
				<< " description AS description,"
				<< " event_type_name AS event_type_name,"
				<< " value AS value,"
				<< " mms_state AS mms,"
				<< " dss_state AS dss,"
				<< " avl_state AS avl,"
				<< " operator_name AS operatorname,"
				<< " alarm_comment AS alarmcomment,"
				<< " alarm_id AS alarm_id,"
				<< " alarm_ack AS alarm_ack,"
		    	<< " alarm_status AS alarm_status"
				<< " operation_mode"
		    	<< " FROM ev_combined";		
		     	  
			    sql << selectSql.str()  <<" WHERE " << m_lastWhereClause;*/
//				strSelectSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ev_combined_Oracle_SELECT_13057, m_lastWhereClause);
//				strSelectSql  = databaseConnection->prepareSQLStatement(EV_COMBINED_SELECT_13057, whereClause);

			databaseConnection->prepareSQLStatement(strSelectSql, EV_COMBINED_SELECT_13057, sqlParamWhereClause);
		}
		else
		{
			//whereClause = generateDesignatedWhereClause(filter, "", *databaseConnection, pkeyBoundary, onNext);
			generateDesignatedWhereClause(sqlParamWhereClause, filter, "", *databaseConnection, pkeyBoundary, onNext);
			m_lastFilter = filter;
			//selectSql<< " SELECT /*+  index (ev_combined EVCOM_SUB_TIME_INDX ) */"
				/*<< " ev.source_table AS source_table,"
				<< " ev.pkey AS pkey,"
				<< " TO_CHAR(ev.createtime,'YYYYMMDDHH24MISSFF3') AS createtime,"
				<< " ev.severity_name AS severityname,"
				<< " ev.asset_name AS assetname,"
				<< " ev.description AS description,"
				<< " ev.event_type_name AS event_type_name,"
				<< " ev.value AS value,"
				<< " ev.mms_state AS mms,"
				<< " ev.dss_state AS dss,"
				<< " ev.avl_state AS avl,"
				<< " ev.operator_name AS operatorname,"
				<< " ev.alarm_comment AS alarmcomment,"
				<< " ev.alarm_id AS alarm_id,"
				<< " ev.alarm_ack AS alarm_ack,"
			    << " ev.alarm_status AS alarm_status,"
				<< " ev.operation_mode as operation_mode";			
			
				sql << selectSql.str() << m_lastWhereClause;*/
//				strSelectSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ev_combined_Oracle_SELECT_13058, m_lastWhereClause);
//				strSelectSql  = databaseConnection->prepareSQLStatement(EV_COMBINED_SELECT_13058, whereClause);

			databaseConnection->prepareSQLStatement(strSelectSql, EV_COMBINED_SELECT_13058, sqlParamWhereClause);
		}

		

		bool isloadLatestRecord=false;
		if ( 0 == filter.fromTime  && 0==filter.toTime ) 
		{
			isloadLatestRecord=true;
			m_curMaxPkey=0;
		}


	
		if( pageStartTime != "")
		{
			if (!enableVisibilitRule)
			{
				//sqlPageTime << " AND createtime ";
				sqlParamPageTime.add(std::string(" AND "));
				sqlParamPageTime.add(std::string("createtime "), pageStartTime, enumDateFormat_4, enumFuncFormat_8, enumOperKey_LETHAN);
			}
			else
			{
				//sqlPageTime << " AND ev.createtime ";
				sqlParamPageTime.add(std::string(" AND "));
				sqlParamPageTime.add(std::string("ev.createtime "), pageStartTime, enumDateFormat_4, enumFuncFormat_8, enumOperKey_LETHAN);
			}
			/*if ( enumOracleDb == nDbType )
				sqlPageTime << " <= TO_TIMESTAMP('" << pageStartTime << "', 'YYYYMMDDHH24MISS') ";
			else
				sqlPageTime << " <= DATE_FORMAT('" << pageStartTime << "', '%Y%m%d%H%i%S') ";*/
		}
        

		if( onNext )
		{
			//sql << " order by createtime asc ) " << " where ROWNUM  <= " << endRow  ;
//			strSql  = databaseConnection->prepareSQLStatement(EV_COMBINED_SELECT_13063, strSelectSql, sqlPageTime.str(), endRow);
			
			//dynsql delete by lin
			databaseConnection->prepareSQLStatement(strSql, EV_COMBINED_SELECT_13063, strSelectSql, sqlParamPageTime, endRow);
		}else
		{
			//sql << " order by createtime desc ) " << " where ROWNUM  <= " << endRow  ;
//			strSql  = databaseConnection->prepareSQLStatement(EV_COMBINED_SELECT_13059, strSelectSql, sqlPageTime.str(), endRow);

			//dynsql delete by lin
			databaseConnection->prepareSQLStatement(strSql, EV_COMBINED_SELECT_13059, strSelectSql, sqlParamPageTime, endRow);
		}

		

		// Set up required columns.
		std::vector<std::string> columnNames;
		columnNames.push_back( COLUMN_EVENT_ID);
		columnNames.push_back( COLUMN_SOURCE_TABLE );
		columnNames.push_back( COLUMN_PKEY );
		columnNames.push_back( COLUMN_CREATETIME );
		columnNames.push_back( COLUMN_SEVERITY_NAME );
		columnNames.push_back( COLUMN_ASSET_NAME );
		columnNames.push_back( COLUMN_DESCRIPTION );
		columnNames.push_back( COLUMN_EVENT_TYPE_NAME );
		columnNames.push_back( COLUMN_VALUE );
		columnNames.push_back( COLUMN_MMS );
		columnNames.push_back( COLUMN_DSS );
		columnNames.push_back( COLUMN_AVL );
		columnNames.push_back( COLUMN_OPERATOR_NAME );
		columnNames.push_back( COLUMN_ALARM_COMMENT );
		columnNames.push_back( COLUMN_ALARM_ID );
		columnNames.push_back( COLUMN_ALARM_ACK );
		columnNames.push_back( COLUMN_ALARM_STATUS );
		columnNames.push_back( COLUMN_OPERATION_MODE );




		std::string strExSql;
		int nExDbType= databaseConnection->getDbServerType();
		switch (nExDbType)
		{
		case enumOracleDb:
			strExSql = (strSql.strCommon.empty() ? strSql.strOracle : strSql.strCommon);
			break;
		case enumMysqlDb:
			strExSql = (strSql.strCommon.empty() ? strSql.strMySQL : strSql.strCommon);
			break;		
		}

// 		for (unsigned long sqlCnt=0; sqlCnt<strSql.size()/100+1;sqlCnt++)
// 		{
// 			std::string newstr=strSql.substr(sqlCnt*100,100);
// 			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "New EV Log:LoadEvent:DatabaseQuery, sql sentence: %s", newstr.c_str());
// 		}
		
		for (unsigned long sqlCnt=0; sqlCnt<strExSql.size()/100+1;sqlCnt++)
		{
			std::string newstr=strExSql.substr(sqlCnt*100,100);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "New EV Log:LoadEvent:DatabaseQuery, sql sentence: %s", newstr.c_str());
		}

		std::auto_ptr<IData> data( databaseConnection->executeQuery( strSql, columnNames ) );


		if ( 0 < data->getNumRows() )
		{
			for( unsigned int i=0; i< data->getNumRows(); i++ )
			{
				// Now construct the object and add it to the list.
				loadedEvents.push_back( createCombinedEvent( i, data.get() ) );
// 				unsigned long pkey=data->getUnsignedLongData(i,COLUMN_PKEY);			
// 				if (pkey>m_curMaxPkey) m_curMaxPkey=pkey;				
			}
		}
// 		else		 
// 		{
// 			// Get the latest audit pkey from ev_combined
// 			LatestKeyNID latest =getMaxEventPKeyNEventId();
// 			if (latest.maxPkey>m_curMaxPkey)
// 			{
// 				m_curMaxPkey=latest.maxPkey;
// 				if(latest.event_id.compare("")!=0)
// 				{
// 					m_latestEventID = latest.event_id;
// 				}
// 			}
// 					
// 		}

		
		 

		return loadedEvents;
	}
 
	std::vector<ICombinedEventData*> CombinedEventAccessFactory::getDesignatedEventsSinceLastLoad(bool enableVisibilitRule)
	{
		std::vector<ICombinedEventData*> newEvents;

		if( m_isLoadingAll)
		{
			return getEventsSinceLastLoad();
		}

		// If the filter excludes all possible events just return.
		if ( m_lastWhereClauseAlwaysFalse )
		{
			return newEvents;
		}

		// First the event table needs to be checked for new elements.
		LatestKeyNID latest = getLatestPkeys();

		// If the latest pkeys match ours then don't load. The keys will never match exactly, rather
		// they are considered to match if the latest is exactly one ahead of the last loaded.
		//if ( ( latest.auditPkey == m_latestAuditPkey ) && ( latest.datapointPkey == m_latestDatapointPkey ) )
		if (  latest.maxPkey == m_curMaxPkey  )
		{
			return newEvents;
		}

		// Get a connection to the database
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);

		std::stringstream auditKeyRestrictionClause;
		std::stringstream dpKeyRestrictionClause; 
	
		std::ostringstream selectSql;
		std::stringstream  sql;
		//std::string strSelectSql, strSql;
		
		SQLStatement strSelectSql;
		SQLStatement strSql;
		
		std::string pkeyClauseConj = " AND ";
		if(latest.maxPkey < m_curMaxPkey )
		{
			pkeyClauseConj = " OR ";
		}

		//std::string whereClause = "";
		SQLTypeAdapter sqlParamWhereClause;
		if (!enableVisibilitRule)
		{		 
			auditKeyRestrictionClause<<" AND (pkey>" << m_curMaxPkey
				<< pkeyClauseConj<< " pkey<=" << latest.maxPkey <<")";

			//selectSql<< " SELECT /*+  index (ev_combined EVCOM_SUB_TIME_INDX ) */"
				/*<< " source_table AS source_table,"
				<< " pkey AS pkey,"
				<< " TO_CHAR(createtime,'YYYYMMDDHH24MISSFF3') AS createtime,"
				<< " severity_name AS severityname,"
				<< " asset_name AS assetname,"
				<< " description AS description,"
				<< " event_type_name AS event_type_name,"
				<< " value AS value,"
				<< " mms_state AS mms,"
				<< " dss_state AS dss,"
				<< " avl_state AS avl,"
				<< " operator_name AS operatorname,"
				<< " alarm_comment AS alarmcomment,"
				<< " alarm_id AS alarm_id,"
				<< " alarm_ack AS alarm_ack,"
				<< " alarm_status AS alarm_status"
				<< " operation_mode"
				<< " FROM ev_combined  "
				<< "WHERE ";*/	
//			strSelectSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ev_combined_Oracle_SELECT_13060);
//			strSelectSql  = databaseConnection->prepareSQLStatement(EV_COMBINED_SELECT_13060);

			databaseConnection->prepareSQLStatement(strSelectSql, EV_COMBINED_SELECT_13060);
			//whereClause = generateWhereClause(m_lastFilter, "", *databaseConnection);
			generateWhereClause(sqlParamWhereClause, m_lastFilter, "", *databaseConnection);
		}
		else
		{
		 
			auditKeyRestrictionClause<<" AND (ev.pkey>" << m_curMaxPkey
				<< pkeyClauseConj << " ev.pkey<=" << latest.maxPkey <<")";

		    //selectSql<< " SELECT  /*+  index (ev_combined EVCOM_PKEY_INDX ) */  ev.source_table AS source_table,"
				/*<< " ev.pkey AS pkey,"
				<< " TO_CHAR(ev.createtime,'YYYYMMDDHH24MISSFF3') AS createtime,"
				<< " ev.severity_name AS severityname,"
				<< " ev.asset_name AS assetname,"
				<< " ev.description AS description,"
				<< " ev.event_type_name AS event_type_name,"
				<< " ev.value AS value,"
				<< " ev.mms_state AS mms,"
				<< " ev.dss_state AS dss,"
				<< " ev.avl_state AS avl,"
				<< " ev.operator_name AS operatorname,"
				<< " ev.alarm_comment AS alarmcomment,"
				<< " ev.alarm_id AS alarm_id,"
				<< " ev.alarm_ack AS alarm_ack,"
				<< " ev.alarm_status AS alarm_status,"
				<< " ev.operation_mode ";*/
//		strSelectSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ev_combined_Oracle_SELECT_13061);
//		strSelectSql  = databaseConnection->prepareSQLStatement(EV_COMBINED_SELECT_13061);

			databaseConnection->prepareSQLStatement(strSelectSql, EV_COMBINED_SELECT_13061);
			//whereClause = generateDesignatedWhereClause(m_lastFilter, "", *databaseConnection);
			generateDesignatedWhereClause(sqlParamWhereClause, m_lastFilter, "", *databaseConnection);
			
		}
         //sql << selectSql.str() << m_lastWhereClause << auditKeyRestrictionClause.str()  << " and rownum<=2000 order by  createtime desc" ;	
//		 strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ev_combined_Oracle_SELECT_13062, strSelectSql, m_lastWhereClause,		

//		strSql  = databaseConnection->prepareSQLStatement(EV_COMBINED_SELECT_13062, strSelectSql, whereClause,
		//dynsql delete by lin
		databaseConnection->prepareSQLStatement(strSql, EV_COMBINED_SELECT_13062, strSelectSql, sqlParamWhereClause, auditKeyRestrictionClause.str());

		// Set up required columns.
		std::vector<std::string> columnNames;
		columnNames.push_back( COLUMN_EVENT_ID);
		columnNames.push_back( COLUMN_SOURCE_TABLE );
		columnNames.push_back( COLUMN_PKEY );
		columnNames.push_back( COLUMN_CREATETIME );
		columnNames.push_back( COLUMN_SEVERITY_NAME );
		columnNames.push_back( COLUMN_ASSET_NAME );
		columnNames.push_back( COLUMN_DESCRIPTION );
		columnNames.push_back( COLUMN_EVENT_TYPE_NAME );
		columnNames.push_back( COLUMN_VALUE );
		columnNames.push_back( COLUMN_MMS );
		columnNames.push_back( COLUMN_DSS );
		columnNames.push_back( COLUMN_AVL );
		columnNames.push_back( COLUMN_OPERATOR_NAME );
		columnNames.push_back( COLUMN_ALARM_COMMENT );
		columnNames.push_back( COLUMN_ALARM_ID );
		columnNames.push_back( COLUMN_ALARM_ACK );
		columnNames.push_back( COLUMN_ALARM_STATUS );
		columnNames.push_back( COLUMN_OPERATION_MODE );
		
		std::string strExSql;
		int nExDbType= databaseConnection->getDbServerType();
		switch (nExDbType)
		{
		case enumOracleDb:
			strExSql = (strSql.strCommon.empty() ? strSql.strOracle : strSql.strCommon);
			break;
		case enumMysqlDb:
			strExSql = (strSql.strCommon.empty() ? strSql.strMySQL : strSql.strCommon);
			break;		
		}
		
// 		for (unsigned long sqlCnt=0; sqlCnt<strSql.size()/100+1;sqlCnt++)
// 		{
// 			std::string newstr=strSql.substr(sqlCnt*100,100);
// 			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:LoadEvent:DatabaseQuery, sql sentence: %s", newstr.c_str());
// 		}
		
		for (unsigned long sqlCnt=0; sqlCnt<strExSql.size()/100+1;sqlCnt++)
		{
			std::string newstr=strExSql.substr(sqlCnt*100,100);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:LoadEvent:DatabaseQuery, sql sentence: %s", newstr.c_str());
		}

		setMaxEventPkeyNEventId(latest);

		// Execute the SQL.
		IData* data= databaseConnection->executeQuery( strSql, columnNames ) ;
		unsigned long tempPkey =0;
		do
		{
			for( unsigned int i=0; i< data->getNumRows(); i++ )
			{
				ICombinedEventData* event = createCombinedEvent( i, data );

				// Now construct the object and add it to the list.
				newEvents.push_back( event );

				//update pkey and event_id by the selected result
				tempPkey = data->getUnsignedLongData(i,COLUMN_PKEY);
				if ( ( tempPkey>m_curMaxPkey && tempPkey-m_curMaxPkey < VALVE) 
					|| ( tempPkey< m_curMaxPkey && m_curMaxPkey - tempPkey > VALVE ) )
				{
					m_curMaxPkey = tempPkey;
					m_latestEventID = data->getStringData(i, COLUMN_EVENT_ID);
				}
			}

			delete data;
			data = NULL;
		}
		while ( databaseConnection->moreData(data) );		

		return newEvents;
	}
	void CombinedEventAccessFactory::generateTimeStr ( 
		const EventFilter& filter, 
		const std::string& timeRestrictionClause ,
		unsigned long pkeyBoundary ,
		bool onNext,
		/*std::stringstream & timeString,*/
		SQLTypeAdapter& rSqlTypeApt)
	{
		bool timeSpanLessThan1Sec = false;
		//timeString<<timeRestrictionClause;
		rSqlTypeApt.add(timeRestrictionClause);

		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
		int nDbType  = databaseConnection->getDbServerType();

		// Add the time filters 
		if( strcmp( timeRestrictionClause.c_str(), "" ) == 0 )
		{
			// if the timeRestrictionClause is empty string
			// generate the time restriction base on the information in the filter

		//	std::string toTime;
			std::string strToTimeOra, strToTimeMy;
			if ( 0 != filter.toTime )
			{
				const unsigned int bufLen = 15;
				char buf[bufLen];
				struct tm *ptmToTime = localtime(&filter.toTime);
				if(ptmToTime != NULL)
				{
					struct tm tmToTime = *ptmToTime;

					strftime( buf, 15, "%d%m%Y%H%M%S", &tmToTime);

				//	toTime = buf;
					strToTimeOra = buf;
					strToTimeMy = buf;

					strToTimeOra = "TO_date('" + strToTimeOra + "','DDMMYYYYHH24MISS')";
					strToTimeMy = "STR_TO_DATE('" + strToTimeMy + "','%d%m%Y%H%i%S')";
					/*switch(nDbType)
					{
					case enumOracleDb:
						toTime = "TO_TIMESTAMP('" + toTime + "','DDMMYYYYHH24MISS')";
						break;
					case enumMysqlDb:
						toTime = "STR_TO_DATE('" + toTime + "','%d%m%Y%H%i%S')";
						break;
					}*/
				}
			}

			/*std::string fromTime="";
			std::string fromTimeAdd1Sec = "";*/

			std::string strFromTimeOra, strFromTimeMy;
			std::string strFromTimeAdd1SecOra, strFromTimeAdd1SecMy;

			if ( 0 == filter.fromTime) // 0 == filter.fromTime
			{
				if(strToTimeOra.empty())
				{
				//	fromTime = "SYSDATE - INTERVAL '1' DAY";
					strFromTimeOra = "SYSDATE - INTERVAL '1' DAY";
					strFromTimeMy = "DATE_SUB( SYSDATE() , INTERVAL 1 DAY)";
					/*switch(nDbType)
					{
					case enumOracleDb:
						fromTime = "SYSDATE - INTERVAL '1' DAY";
						break;
					case enumMysqlDb:
						fromTime = "DATE_SUB( SYSDATE() , INTERVAL 1 DAY)";
						break;
					}*/
				}
				else
				{
					//fromTime = toTime + " - INTERVAL '1' DAY";
					strFromTimeOra = strToTimeOra + " - INTERVAL '1' DAY";
					strFromTimeMy = "DATE_SUB("+strToTimeMy+" , INTERVAL 1 DAY)";
					/*switch(nDbType)
					{
					case enumOracleDb:
						fromTime = toTime + " - INTERVAL '1' DAY";
						break;
					case enumMysqlDb:
						fromTime = "DATE_SUB("+toTime+" , INTERVAL 1 DAY)";
						break;
					}*/
				}				
			}

			else // 0 != filter.fromTime
			{
				const unsigned int bufLen = 15;
				char buf[bufLen];
				struct tm *ptmFromTime = localtime(&filter.fromTime);
				if(ptmFromTime != NULL)
				{
					struct tm tmFromTime = *ptmFromTime;

					strftime( buf, 15, "%d%m%Y%H%M%S", &tmFromTime);

					/*fromTime = buf;*/
					strFromTimeOra = buf;
					strFromTimeMy = buf;
					//fromTime = "TO_TIMESTAMP('" + fromTime + "','DDMMYYYYHH24MISS')";
					strFromTimeOra = "TO_date('" + strFromTimeOra + "','DDMMYYYYHH24MISS')";
					strFromTimeMy = "STR_TO_DATE('" + strFromTimeMy + "','%d%m%Y%H%i%S')";

					/*switch(nDbType)
					{
					case enumOracleDb:
						fromTime = "TO_TIMESTAMP('" + fromTime + "','DDMMYYYYHH24MISS')";
						break;
					case enumMysqlDb:
						fromTime = "STR_TO_DATE('" + fromTime + "','%d%m%Y%H%i%S')";
						break;
					}*/

					if( pkeyBoundary!=0 && onNext && 0 != filter.toTime)
					{
						if (filter.toTime - filter.fromTime <1)
						{
							timeSpanLessThan1Sec = true;
						}
						else 
						{
							//fromTimeAdd1Sec = buf;
							strFromTimeAdd1SecOra = buf;
							strFromTimeAdd1SecMy  = buf;
							//fromTimeAdd1Sec = "TO_TIMESTAMP('" + fromTimeAdd1Sec + "','DDMMYYYYHH24MISS') + INTERVAL '1' Second ";
							strFromTimeAdd1SecOra = "TO_date('" + strFromTimeAdd1SecOra + "','DDMMYYYYHH24MISS') + INTERVAL '1' Second ";
							strFromTimeAdd1SecMy  = "STR_TO_DATE('" + strFromTimeAdd1SecMy + "','%d%m%Y%H%i%S')";
							strFromTimeAdd1SecMy  = "DATE_ADD("+ strFromTimeAdd1SecMy +" , INTERVAL 1 Second)";

							/*switch(nDbType)
							{
							case enumOracleDb:
								fromTimeAdd1Sec = "TO_TIMESTAMP('" + fromTimeAdd1Sec + "','DDMMYYYYHH24MISS') + INTERVAL '1' Second ";
								break;
							case enumMysqlDb:
								fromTimeAdd1Sec = "STR_TO_DATE('" + fromTimeAdd1Sec + "','%d%m%Y%H%i%S')";
								fromTimeAdd1Sec = "DATE_ADD('"+ fromTimeAdd1Sec +"' , INTERVAL 1 DAY)";
								break;
							}*/

						}
					}
				}
			}


			// Now construct the time portion of the where clause.
			if (""!=strFromTimeAdd1SecOra)
			{
				/*timeString << "( ( createtime >= " << fromTimeAdd1Sec;*/
				rSqlTypeApt.add(std::string("( ( CREATEDATETIME >= "));
				rSqlTypeApt.add(strFromTimeAdd1SecOra, strFromTimeAdd1SecMy);
			}else
			{
				/*timeString << "( ( createtime >= " << fromTime;*/
				rSqlTypeApt.add(std::string("( ( CREATEDATETIME >= "));
				rSqlTypeApt.add(strFromTimeOra, strFromTimeMy);
			}

			if ( /*!toTime.empty()*/ !strToTimeOra.empty() )
			{
				if (/*!fromTime.empty()*/ !strFromTimeOra.empty())
				{ 
					/*timeString << " AND createtime < " << toTime << ")";*/
					rSqlTypeApt.add(std::string(" AND CREATEDATETIME < "));
					rSqlTypeApt.add(strToTimeOra, strToTimeMy);
					rSqlTypeApt.add(std::string(")"));
				}
				else
				{
					/*timeString << "  createtime < " << toTime<< ")";*/
					rSqlTypeApt.add(std::string("  CREATEDATETIME < "));
					rSqlTypeApt.add(strToTimeOra, strToTimeMy);
					rSqlTypeApt.add(std::string(")"));
				}

			}else
			{
				/*timeString << "  )";*/
				rSqlTypeApt.add(std::string("  )"));
			}

			if ( pkeyBoundary == 0)
			{
				/*timeString<< " )";*/
				rSqlTypeApt.add(std::string(" )"));
			}
			// add the pkey boundary if needed:
			else if( pkeyBoundary != 0 && !onNext && !strToTimeOra.empty() )
			{
				std::stringstream timeStringOra, timeStringMy;
				timeStringOra<< " or ( CREATEDATETIME >=" << strToTimeOra
					<< " AND CREATEDATETIME<" << strToTimeOra <<"+ INTERVAL '1' Second "   //delete this for test
					<< " AND pkey < "<<pkeyBoundary <<") )";

				timeStringMy<< " or ( CREATEDATETIME >=" << strToTimeMy
					<< " AND CREATEDATETIME< DATE_ADD(" << strToTimeMy <<" , INTERVAL 1 Second) "   //delete this for test
					<< " AND pkey < "<<pkeyBoundary <<") )";

				rSqlTypeApt.add(timeStringOra.str(), timeStringMy.str());

			/*	switch(nDbType)
				{
				case enumOracleDb:
				    timeString<< " or ( createtime >=" << toTime
						<< " AND createtime<" << toTime <<"+ INTERVAL '1' Second "   //delete this for test
						<< " AND pkey < "<<pkeyBoundary <<") )";
					break;
				case enumMysqlDb:
					timeString<< " or ( createtime >=" << toTime
						<< " AND createtime< DATE_ADD('" << toTime <<"', INTERVAL 1 Second) "   //delete this for test
						<< " AND pkey < "<<pkeyBoundary <<") )";
					break;
			
				} */
			}
			else if (pkeyBoundary != 0 && onNext && !timeSpanLessThan1Sec)
			{
				std::stringstream timeStringOra, timeStringMy;

				timeStringOra<< " or ( CREATEDATETIME >=" << strFromTimeOra
					<< " AND CREATEDATETIME<" << strFromTimeAdd1SecOra 
					<< " AND pkey > "<<pkeyBoundary <<") )";

				timeStringMy<< " or ( CREATEDATETIME >=" << strFromTimeMy
					<< " AND CREATEDATETIME<" << strFromTimeAdd1SecMy 
					<< " AND pkey > "<<pkeyBoundary <<") )";
				rSqlTypeApt.add(timeStringOra.str(), timeStringMy.str());

				/*timeString<< " or ( createtime >=" << fromTime
					<< " AND createtime<" << fromTimeAdd1Sec 
					<< " AND pkey > "<<pkeyBoundary <<") )";*/
			}
			else 
			{
				/*timeString<< " )";*/

				rSqlTypeApt.add(std::string(" )"));
			}
		}
		
	}

	void CombinedEventAccessFactory::generateDesignatedWhereClause( 
		SQLTypeAdapter& rSqlParam,
		const EventFilter& filter, 
		const std::string& timeRestrictionClause ,
		TA_Base_Core::IDatabase& databaseConnection,
		unsigned long pkeyBoundary ,
		bool onNext )
	{
		//std::stringstream whereClause;

		//std::stringstream timeString;
		SQLTypeAdapter sqlParam;

		//whereClause<< " FROM ev_combined ev, t6 WHERE ";
		rSqlParam.add(std::string(" FROM ev_combined ev, t6 WHERE "));

		// Compliance with the C++ standard would be nice...
		unsigned int i = 0;
		generateTimeStr(filter, timeRestrictionClause ,pkeyBoundary ,onNext, sqlParam);
		 
		// Add the restriction to retrieve physical subsystem events 
		//disable duty function
		// 		if(filter.dutyEnabled && !filter.fullDuty.empty())
		// 		{
		// 			whereClause << " AND "<< generateDesignatedDutyExpr( filter.fullDuty, databaseConnection, !(filter.appSubsystemIds.empty() && !filter.physSubsystemIds.empty()));
		// 		}else{
		// 			whereClause << " AND ev.physical_subsystem_key = -1 AND t6.sessta_id=0 ";
		// 		}

		//disable duty function

	 
		//whereClause << "  t6.sessta_id=0  ";
		rSqlParam.add(std::string("  t6.sessta_id=0  "));

		if( !filter.profiles.empty())
		{
			/*whereClause<<" AND ("<<generateORList("t6.seprof_id", filter.profiles, databaseConnection)<<")";*/
			rSqlParam.add(std::string(" AND ("));
			rSqlParam.add(generateORList("t6.seprof_id", filter.profiles, databaseConnection));
			rSqlParam.add(std::string(")"));
		}

        //std::string str=timeString.str();

        if (/*str.size()>0*/!sqlParam.empty())
		{
           /* whereClause << " AND "+ str;*/
			rSqlParam.add(std::string(" AND "));
			rSqlParam.add(sqlParam);
		}

		std::stringstream whereClause;
		whereClause << " AND ev.action_id = t6.seacti_id";


		whereClause << " AND ev.physical_subsystem_key = t6.physical_subsystem_key"
			<< " AND ev.subsystem_key = t6.subsystemkey";

		if( !filter.locationIds.empty())
		{
			whereClause<<" AND ("<<generateORList("ev.location_key", filter.locationIds, databaseConnection)<<")";
		}	 

		if (!(filter.appSubsystemIds.empty()))
		{			 
			whereClause << " AND  ("<<generateORList("subsystem_key", filter.appSubsystemIds, databaseConnection)<<" )" ;
			//whereClause<<" OR "<<generateORList("ev.physical_subsystem_key", filter.appSubsystemIds, databaseConnection) <<" )" ;
			

		}
		if (!(filter.physSubsystemIds.empty()))
		{			 
			whereClause << " AND  ("<<generateORList("ev.physical_subsystem_key", filter.physSubsystemIds, databaseConnection) <<" )" ;
			//whereClause<<" OR "<<generateORList("ev.subsystem_key", filter.physSubsystemIds, databaseConnection) <<" )" ;

		}


		// Filter by operator IDs.
		if ( !filter.operatorIds.empty() )
		{
			whereClause << " AND ( " << generateORList( "operator_key", filter.operatorIds, databaseConnection ) << " )";
		}

		// Filter by system IDs.
		if ( !filter.systemIds.empty() )
		{
			whereClause << " AND ( " << generateORList( "system_key", filter.systemIds, databaseConnection ) << " )";
		}

		// Filter by event type IDs.
		if ( !filter.eventTypeIds.empty() )
		{
			whereClause << " AND ( " << generateORList( "event_type_key", filter.eventTypeIds, databaseConnection ) << " )";
		}

		// Filter by asset name.
		if ( !filter.assetName.empty() )
		{
			// Use entity name if asset name is NULL???

			//TD15029 azenitha++
			const char* assetNameLwr = strlwr((char*)filter.assetName.c_str());
			//whereClause << " AND ev_combined.asset_name LIKE '" << filter.assetName << "'";
			whereClause << " AND LOWER(ev.asset_name) LIKE '" << assetNameLwr << "'";
			//TD15029 ++azenitha

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "TD15029: Case insensitive search for asset");
		}

		if ( !filter.tmType.empty())
		{
			whereClause << " AND operation_mode = '" << filter.tmType.c_str() << "'";
		}
		// Filter by description.
		if ( !filter.description.empty() )
		{
			//TD15029 azenitha++
			const char* descriptionLwr = strlwr((char*)filter.description.c_str());
			//whereClause << " AND ( ev_combined.description LIKE '" << filter.description << "'";
			whereClause << " AND ( LOWER(ev.description) LIKE '" << descriptionLwr << "'";
			//TD15029 ++azenitha

			// TD 14751 add filter by entity type as well as it has been appended to description

			//TD15029 azenitha++
			//whereClause << " OR event_type_name LIKE '" << filter.description << "' )";
			whereClause << " OR LOWER(event_type_name) LIKE '" << descriptionLwr << "' )";
			//TD15029 ++azenitha

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "TD15029: Case insensitive search for description");
		}

		// Filter by severity IDs - this part of the clause must always exist.
		if(!filter.showNonAlarmEvents || filter.severityIds.size() != TA_Base_Core::AlarmSeverityAccessFactory::getInstance().getAllAlarmSeverities().size())
		{
			whereClause << " AND ( ";
			if ( filter.showNonAlarmEvents )
			{
				whereClause << " ev.severity_key IS NULL or ev.severity_key=0 ";
			}
			if ( !filter.severityIds.empty() )
			{
				if ( filter.showNonAlarmEvents )
				{
					whereClause << " OR ";
				}
				whereClause << generateORList( "ev.severity_key", filter.severityIds, databaseConnection );
			}
			whereClause << " )";
		}

		// Filter by alarm types IDs.
		if ( !filter.alarmTypes.empty() )
		{
			whereClause << " AND ( " << generateORList( "ev.alarm_type_key", filter.alarmTypes, databaseConnection ) << " )";
		}

		// Filter by alarm ID.
		if ( !filter.alarmId.empty() )
		{
			if (filter.alarmId.size() > 1)
			{
				unsigned int i = 0;
				whereClause << " AND (ev.alarm_id LIKE '" // first alarm
					<< databaseConnection.escapeQueryString(filter.alarmId[i])
					<< "'";

				for (i=1; i<filter.alarmId.size(); ++i) // second alarm onwards
				{
					whereClause << " OR ev.alarm_id LIKE '"
						<< databaseConnection.escapeQueryString(filter.alarmId[i])
						<< "'";
				}
				// close the bracket
				whereClause << ")";
			}
			else // there is only 1 alarm ID
			{
				whereClause << " AND ev.alarm_id LIKE '"
					<< databaseConnection.escapeQueryString(filter.alarmId[0])
					<< "'";
			}
		}


		// Add the event level.
		
		//waitforcheck delete by lin
		//std::string evLevelSql = "select max(view_level) view_level from me_message_type";
		SQLStatement evLevelSql;
		databaseConnection.prepareSQLStatement(evLevelSql, ME_MESSAGE_TYPE_SELECT_10002);
		unsigned long maxViewLevelCount;
		std::vector<std::string> maxViewLevel;
		maxViewLevel.push_back( COLUMN_MAX_EVENT_LEVEL );
		std::auto_ptr<IData> maxViewLevelData( databaseConnection.executeQuery( evLevelSql, maxViewLevel ) );
		if( 0 < maxViewLevelData->getNumRows())
		{
			try
			{ 
				maxViewLevelCount = maxViewLevelData->getUnsignedLongData(0, COLUMN_MAX_EVENT_LEVEL);
			}
			catch(TA_Base_Core::DataException& ex)
			{
				// Most likely, no audit events in database.
				LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
			}
		}
		whereClause << " AND ev.event_level != 0 "; 

		if(filter.eventLevel<maxViewLevelCount)
		{
			whereClause << " AND ev.event_level <= " << filter.eventLevel; 
		}

		rSqlParam.add(whereClause.str());

		/*return whereClause.str();*/
	}

	std::list<ICombinedEventData*> CombinedEventAccessFactory::getLatest24hrsEvents(unsigned long ulMaxEvents, unsigned long & ulLatestSeqId)
	{
        // Now populate the vector with the matching events.
        std::list<ICombinedEventData*> loadedEvents;

        // Get a connection to the database
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
		int nDbType = databaseConnection->getDbServerType();
	
		time_t now = time(NULL);
		now -= 24 * 60 * 60; // minus 1 day to query the 24hrs events
		struct tm querytm;
		char timeBuff[24] = {0};
		if (ACE_OS::localtime_r(&now, &querytm) != NULL)
		{
			sprintf(timeBuff, "%02d/%02d/%04d %02d:%02d:%02d",
				querytm.tm_mday , 
				querytm.tm_mon + 1,
				querytm.tm_year + 1900,
				querytm.tm_hour,
				querytm.tm_min,
				querytm.tm_sec);
		}

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, EV_COMBINED_SELECT_13066, std::string(timeBuff), ulMaxEvents);

        // Set up required columns.
        std::vector<std::string> columnNames;
		columnNames.push_back( COLUMN_EVENT_ID );
		columnNames.push_back( COLUMN_SOURCE_TABLE );
        columnNames.push_back( COLUMN_PKEY );
        columnNames.push_back( COLUMN_CREATETIME );
		columnNames.push_back( COLUMN_SUBSYSTEM_KEY);
		columnNames.push_back( COLUMN_PHYSICAL_SUBSYSTEM_KEY );
		columnNames.push_back( COLUMN_LOCATION_KEY );
		columnNames.push_back( COLUMN_SEVERITY_KEY );
		columnNames.push_back( COLUMN_SEVERITY_NAME );
        columnNames.push_back( COLUMN_ASSET_NAME );
        columnNames.push_back( COLUMN_DESCRIPTION );
		columnNames.push_back( COLUMN_EVENT_TYPE_KEY );
		columnNames.push_back( COLUMN_EVENT_TYPE_NAME );
		columnNames.push_back( COLUMN_ALARM_ID );
		columnNames.push_back( COLUMN_ALARM_TYPE_KEY );
        columnNames.push_back( COLUMN_VALUE );
        columnNames.push_back( COLUMN_MMS );
        columnNames.push_back( COLUMN_DSS );
        columnNames.push_back( COLUMN_AVL );
		columnNames.push_back( COLUMN_OPERATOR_KEY );
        columnNames.push_back( COLUMN_OPERATOR_NAME );
        columnNames.push_back( COLUMN_ALARM_COMMENT );
		columnNames.push_back( COLUMN_EVENT_LEVEL );
        columnNames.push_back( COLUMN_ALARM_ACK );
		columnNames.push_back( COLUMN_ALARM_STATUS );
		columnNames.push_back( COLUMN_SESSION_KEY );
		columnNames.push_back( COLUMN_SESSION_LOCATION );
		columnNames.push_back( COLUMN_PROFILE_ID );
		columnNames.push_back( COLUMN_ACTION_ID );
		columnNames.push_back( COLUMN_OPERATION_MODE );
		columnNames.push_back( COLUMN_ENTITY_KEY );
		columnNames.push_back( COLUMN_AVL_ALARM_HEADID );
		columnNames.push_back( COLUMN_SYSTEM_KEY );

		LatestKeyNID latest = getMaxEventPKeyNEventId();
		ulLatestSeqId = latest.maxPkey;

		// Execute the SQL.
		IData* data= databaseConnection->executeQuery( sql, columnNames ) ;

		unsigned long tempPkey =0;
        if ( 0 < data->getNumRows() )
        {
			do
			{
				for( unsigned int i=0; i< data->getNumRows(); i++ )
				{
					ICombinedEventData* event = newCreateCombinedEvent( i, data );

					// Now construct the object and add it to the list.
					loadedEvents.push_back( event );

					tempPkey = data->getUnsignedLongData(i,COLUMN_PKEY);
					if ( ( tempPkey>ulLatestSeqId && tempPkey-ulLatestSeqId < VALVE) 
						|| ( tempPkey< ulLatestSeqId && ulLatestSeqId - tempPkey > VALVE ) )
					{
						ulLatestSeqId = tempPkey;

					}
				}

				delete data;
				data = NULL;
			}
			while ( databaseConnection->moreData(data) );
		}

        return loadedEvents;
	}

	ICombinedEventData* CombinedEventAccessFactory::newCreateCombinedEvent( unsigned long row, IData* data )
	{
		// Get the source table ID and convert it to an event type.
		ECombinedEventType eventType = ( SOURCE_TABLE_EVENT == data->getUnsignedLongData( row, COLUMN_SOURCE_TABLE ) ) ? E_AUDIT_EVENT : E_DPSTATECHANGE_EVENT;

		// Get the event ID
		std::string eventID = data->getStringData( row, COLUMN_EVENT_ID );

		// Get the pkey for the event.
		unsigned long pkey = data->getUnsignedLongData( row, COLUMN_PKEY );

		// Get the time the event was created.
		timeb createTime = data->getTimestampData( row, COLUMN_CREATETIME );

		// Get the severity name.
		std::string severityName = data->getStringData( row, COLUMN_SEVERITY_NAME );

		// Get the asset name.
		std::string assetName = data->getStringData( row, COLUMN_ASSET_NAME );

		// Get the description.
		std::string description = data->getStringData( row, COLUMN_DESCRIPTION );
		std::string eventTypeName = data->getStringData( row, COLUMN_EVENT_TYPE_NAME );

		// Get the value.
		std::string value = data->getStringData( row, COLUMN_VALUE );

		// Get the MMS state information.
		std::string mmsState = data->getStringData( row, COLUMN_MMS );

		// Get the DSS state information.
		std::string dssState = data->getStringData( row, COLUMN_DSS );

		std::string avlState = data->getStringData( row, COLUMN_AVL );

		// Get the operator name.
		std::string operatorName = data->getStringData( row, COLUMN_OPERATOR_NAME );

		// Get the alarm comment.
		std::string alarmComment = data->getStringData( row, COLUMN_ALARM_COMMENT );

		// Get the alarm ID.
		std::string alarmId = data->getStringData( row, COLUMN_ALARM_ID );
		if ( alarmId == " " )
		{
			alarmId = "";
		}

		// Get the alarm ack.
		std::string alarmAck = data->getStringData( row, COLUMN_ALARM_ACK );
		if ( alarmAck == " " )
		{
			alarmAck = "";
		}

		// Get the alarm status.
		std::string alarmStatus = data->getStringData( row, COLUMN_ALARM_STATUS );
		if ( alarmStatus == " " )
		{
			alarmStatus = "";
		}
		std::string operation_mode = data->getStringData(row,COLUMN_OPERATION_MODE);

		std::string source_table = data->getStringData(row, COLUMN_SOURCE_TABLE); 
		unsigned long subsystem_key = data->getUnsignedLongData( row, COLUMN_SUBSYSTEM_KEY);
		long physical_subsystem_key = data->getIntegerData( row, COLUMN_PHYSICAL_SUBSYSTEM_KEY);
		unsigned long location_key = data->getUnsignedLongData( row, COLUMN_LOCATION_KEY);
		unsigned long severity_key = data->getUnsignedLongData( row, COLUMN_SEVERITY_KEY);
		unsigned long event_type_key = data->getUnsignedLongData( row, COLUMN_EVENT_TYPE_KEY );
		unsigned long alarm_type_key = data->getUnsignedLongData( row, COLUMN_ALARM_TYPE_KEY );
		unsigned long operator_key = data->getUnsignedLongData( row, COLUMN_OPERATOR_KEY );
		unsigned long event_level = data->getUnsignedLongData( row, COLUMN_EVENT_LEVEL );
		std::string session_key = data->getStringData(row, COLUMN_SESSION_KEY );
		unsigned long session_location = data->getUnsignedLongData( row, COLUMN_LOCATION_KEY );
		long profile_id = data->getIntegerData( row, COLUMN_PROFILE_ID );
		unsigned long action_id = data->getUnsignedLongData( row, COLUMN_ACTION_ID );
		unsigned long entity_key = data->getUnsignedLongData( row, COLUMN_ENTITY_KEY );
		std::string avlalarmheadid = data->getStringData(row, COLUMN_AVL_ALARM_HEADID );
		unsigned long system_key = data->getUnsignedLongData( row,COLUMN_SYSTEM_KEY );

		return new CombinedEventData( eventID, eventType, pkey, createTime, severityName, assetName, description,
			value, mmsState, dssState, avlState, operatorName, alarmComment, alarmId, alarmAck, alarmStatus, 
			source_table,subsystem_key,physical_subsystem_key,location_key,severity_key,event_type_key,
			alarm_type_key,operator_key,event_level,session_key,session_location,profile_id,action_id,
			entity_key,avlalarmheadid,system_key,eventTypeName, operation_mode);
	}

	std::list<ICombinedEventData*> CombinedEventAccessFactory::getLatestEvents(unsigned long ulLastSequenceNum, unsigned long & ulLatestSeqId)
	{
		// Now populate the vector with the matching events.
		std::list<ICombinedEventData*> loadedEvents;

		LatestKeyNID latest = getMaxEventPKeyNEventId();
		if(latest.maxPkey == ulLastSequenceNum)
		{
			return loadedEvents;
		}

		ulLatestSeqId = latest.maxPkey;
		std::string pkeyClauseConj = " AND ";
		if(latest.maxPkey< ulLastSequenceNum) 
		{
			pkeyClauseConj = " OR ";
		}
		// Get a connection to the database
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
		int nDbType = databaseConnection->getDbServerType();

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, EV_COMBINED_SELECT_13067, ulLastSequenceNum, pkeyClauseConj, latest.maxPkey, 2000);

		// Set up required columns.
		std::vector<std::string> columnNames;
		columnNames.push_back( COLUMN_EVENT_ID );
		columnNames.push_back( COLUMN_SOURCE_TABLE );
		columnNames.push_back( COLUMN_PKEY );
		columnNames.push_back( COLUMN_CREATETIME );
		columnNames.push_back( COLUMN_SUBSYSTEM_KEY);
		columnNames.push_back( COLUMN_PHYSICAL_SUBSYSTEM_KEY );
		columnNames.push_back( COLUMN_LOCATION_KEY );
		columnNames.push_back( COLUMN_SEVERITY_KEY );
		columnNames.push_back( COLUMN_SEVERITY_NAME );
		columnNames.push_back( COLUMN_ASSET_NAME );
		columnNames.push_back( COLUMN_DESCRIPTION );
		columnNames.push_back( COLUMN_EVENT_TYPE_KEY );
		columnNames.push_back( COLUMN_EVENT_TYPE_NAME );
		columnNames.push_back( COLUMN_ALARM_ID );
		columnNames.push_back( COLUMN_ALARM_TYPE_KEY );
		columnNames.push_back( COLUMN_VALUE );
		columnNames.push_back( COLUMN_MMS );
		columnNames.push_back( COLUMN_DSS );
		columnNames.push_back( COLUMN_AVL );
		columnNames.push_back( COLUMN_OPERATOR_KEY );
		columnNames.push_back( COLUMN_OPERATOR_NAME );
		columnNames.push_back( COLUMN_ALARM_COMMENT );
		columnNames.push_back( COLUMN_EVENT_LEVEL );
		columnNames.push_back( COLUMN_ALARM_ACK );
		columnNames.push_back( COLUMN_ALARM_STATUS );
		columnNames.push_back( COLUMN_SESSION_KEY );
		columnNames.push_back( COLUMN_SESSION_LOCATION );
		columnNames.push_back( COLUMN_PROFILE_ID );
		columnNames.push_back( COLUMN_ACTION_ID );
		columnNames.push_back( COLUMN_OPERATION_MODE );
		columnNames.push_back( COLUMN_ENTITY_KEY );
		columnNames.push_back( COLUMN_AVL_ALARM_HEADID );
		columnNames.push_back( COLUMN_SYSTEM_KEY );

		// Execute the SQL.
		IData* data= databaseConnection->executeQuery( sql, columnNames ) ;

		unsigned long tempPkey = 0;
		if ( 0 < data->getNumRows() )
		{
			do
			{
				for( unsigned int i=0; i< data->getNumRows(); i++ )
				{
					ICombinedEventData* event = newCreateCombinedEvent( i, data );

					// Now construct the object and add it to the list.
					loadedEvents.push_back( event );

					tempPkey = data->getUnsignedLongData(i,COLUMN_PKEY);
					if ( ( tempPkey>ulLatestSeqId && tempPkey-ulLatestSeqId < VALVE) 
						|| ( tempPkey< ulLatestSeqId && ulLatestSeqId - tempPkey > VALVE ) )
					{
						ulLatestSeqId = tempPkey;

					}
				}

				delete data;
				data = NULL;
			}
			while ( databaseConnection->moreData(data) );
		}

		return loadedEvents;
	}
};
