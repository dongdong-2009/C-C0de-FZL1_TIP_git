#include <vector>
#include <sys/timeb.h>
#include <algorithm>

#include "ATSSimulator.h"
#include "ISCSWorkerHelper.h"
#include "FilterCriteria.h"
#include "ATSRealTimeObserverManager.h"
#include "ATSRealTimeWorker.h"
#include "ConstantItems.h"
#include "core/data_access_interface/src/IAlarmData.h"
#include "core/data_access_interface/src/CombinedEventData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "ATSEventWrapper.h"
#include "core/utilities/src/DebugUtil.h"
/*#include "ConstantItems.h"*/

#include <sstream>
#include "atltime.h"
using namespace TA_Base_Core;
using namespace IATSEventsAPI;


const std::string COLUMN_SOURCE_TABLE( "SOURCE_TABLE" );
const std::string COLUMN_PKEY( "PKEY" );
const std::string COLUMN_CREATETIME( "CREATETIME" );
const std::string COLUMN_CREATEDATETIME( "CREATEDATETIME" );
const std::string COLUMN_SEVERITY_NAME( "SEVERITYNAME" );
const std::string COLUMN_ASSET_NAME( "ASSETNAME" );
const std::string COLUMN_DESCRIPTION( "DESCRIPTION" );
const std::string COLUMN_EVENT_TYPE_NAME( "EVENT_TYPE_NAME" );
const std::string COLUMN_VALUE( "VALUE" );
const std::string COLUMN_OPERATION_MODE("OPERATION_MODE");  
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

ATSRealTimeThread* ATSSimulator::m_atsRealTimeThread=NULL;

ATSRealTimeThread::ATSRealTimeThread()
{
	m_terminated = false;
	m_paused = false;
	/*seq = 1;*/
}

ATSRealTimeThread::~ATSRealTimeThread()
{
	m_terminated = true;
}
void ATSRealTimeThread::terminate()
{
	m_terminated = true;
}
void ATSRealTimeThread::run()
{
	while(!m_terminated )
	{
		if(! m_paused)
		{
			ATSEvent* ev = new ATSEvent();
			ev->AlarmAvalancheType=ATSEventAlarmAvalancheType::ATS_EVENTALARMAVLTYPE_NOALARM;
			ev->AlarmMMSType = ATSEventAlarmMMSType::ATS_EVENTALARMMMSTYPE_SEMIAUTO;
			ev->AlarmSeverity = 3;
			std::stringstream ss ;
			seq = ATSRealTimeWorker::getCurrentMaxKey() +1 ;
			ss<<"ATS_RealTime_pkey="<< seq;
			ev->Asset = ss.str().c_str();
			ev->Description = "ATS real time description";
			ev->EventID = seq+"a";
			CTime curTime = CTime::GetCurrentTime();
			ev->Timestamp = ATSDateTime(curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), curTime.GetHour(), curTime.GetMinute(), curTime.GetSecond(), 0);

			std::list<ATSEvent*> atsEvents ;
			atsEvents.push_back(ev);

			eventSubscriptionCallback (ATSSubscriptionError::ATS_SUBSCRIPTION_OK,  atsEvents);
			
			seq ;
		}

		sleep(/*EventViewerConstants::ISCS_REFRESH_SEC **/1000);
	}
}

ATSSimulator::ATSSimulator(void)
{
	
}

ATSSimulator::~ATSSimulator(void)
{
}

void ATSSimulator::generateTimeStr ( 
	const TA_Base_Core::CombinedEventAccessFactory::EventFilter& filter, 
	unsigned long pkeyBoundary ,
	bool onNext,
	std::stringstream & timeString)
{
	bool timeSpanLessThan1Sec = false;


	// Add the time filters 
	if( true )
	{
		// if the timeRestrictionClause is empty string
		// generate the time restriction base on the information in the filter

		std::string toTime;
		if ( 0 != filter.toTime )
		{
			const unsigned int bufLen = 15;
			char buf[bufLen];
			struct tm tmToTime = *localtime(&filter.toTime);

			strftime( buf, 15, "%d%m%Y%H%M%S", &tmToTime);

			toTime = buf;
			toTime = "TO_TIMESTAMP('" + toTime + "','DDMMYYYYHH24MISS')";
		}

		std::string fromTime="";
		std::string fromTimeAdd1Sec = "";

		if ( 0 == filter.fromTime) 
		{
			if(toTime.empty())
			{
				fromTime = "SYSDATE - INTERVAL '1' DAY";
				toTime = "SYSDATE";
			}
			else
			{
				fromTime = toTime + " - INTERVAL '1' DAY";
			}				
		}

		else
		{
			// 0 != filter.fromTime
			const unsigned int bufLen = 15;
			char buf[bufLen];
			struct tm tmFromTime = *localtime(&filter.fromTime);

			strftime( buf, 15, "%d%m%Y%H%M%S", &tmFromTime);

			fromTime = buf;
			fromTime = "TO_TIMESTAMP('" + fromTime + "','DDMMYYYYHH24MISS')";

			if( pkeyBoundary!=0 && onNext )
			{
				if (filter.toTime - filter.fromTime <1)
				{
					timeSpanLessThan1Sec = true;
				}
				else 
				{
					fromTimeAdd1Sec = buf;
					fromTimeAdd1Sec = "TO_TIMESTAMP('" + fromTimeAdd1Sec + "','DDMMYYYYHH24MISS') + INTERVAL '1' Second ";
				}
			}

			if(0 == filter.toTime)
			{
				toTime = fromTime + "+ INTERVAL '1' DAY";
			}

		}

		// Now construct the time portion of the where clause.
		if (""!=fromTimeAdd1Sec)
		{
			timeString << "( ( ev.createtime >= " << fromTimeAdd1Sec;
		}else
		{
			timeString << "( ( ev.createtime >= " << fromTime;
		}

		if ( !toTime.empty() )
		{
			if (!fromTime.empty())
			{ 
				timeString << " AND ev.createtime < " << toTime << ")";
			}
			else
			{
				timeString << "  ev.createtime < " << toTime<< ")";
			}

		}

		if ( pkeyBoundary == 0)
		{
			timeString<< " )";
		}
		// add the pkey boundary if needed:
		else if( pkeyBoundary != 0 && !onNext && !toTime.empty() )
		{
			timeString<< " or ( ev.createtime >=" << toTime
				<< " AND ev.createtime<" << toTime <<"+ INTERVAL '1' Second " 
				<< " AND ev.pkey < "<<pkeyBoundary <<") )";
		}
		else if (pkeyBoundary != 0 && onNext && !timeSpanLessThan1Sec)
		{
			timeString<< " or ( ev.createtime >=" << fromTime
				<< " AND ev.createtime<" << fromTimeAdd1Sec 
				<< " AND ev.pkey > "<<pkeyBoundary <<") )";
		}
		else 
		{
			timeString<< " )";
		}
	}
}




IATSEventsAPI::ATSQueryError ATSSimulator::DoStartTimeEndTimeQuery_simulator(
	const TA_Base_Core::CombinedEventAccessFactory::EventFilter* const filterCriteria,
	const ISCSDateTime& startTime,
	const ISCSDateTime& endTime,
	std::list<ATSEvent*>& atsEvents)
{
	std::ostringstream selectSql;
	std::ostringstream sql;
	std::stringstream lastWhereClause;

	 generateTimeStr(*filterCriteria, 0, false,lastWhereClause);

	selectSql<< " Select * from ( SELECT /*+  index (ats_event_test EVCOM_SUB_TIME_INDX ) */"
		<< " ev.source_table AS source_table,"
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
		<< " ev.alarm_status AS alarm_status, "
		<< " ev.operation_mode as operation_mode"
	    << " from ats_event_test ev where ";

	sql << selectSql.str() << lastWhereClause.str();

		sql << " order by createtime desc ) " << " where ROWNUM  <= " << EventViewerConstants::ATS_EVENT_COUNT_DEFAULT<< " order by pkey asc";
		
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
		columnNames.push_back( COLUMN_OPERATION_MODE );


		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);
		std::auto_ptr<IData> data( databaseConnection->executeQuery( sql.str(), columnNames ) );


		if ( 0 < data->getNumRows() )
		{
			for( unsigned int i=0; i< data->getNumRows(); i++ )
			{
				// Now construct the object and add it to the list.
				atsEvents.push_back( createATSEvent( i, data.get() ) );				
			}
		}	

		return ATSQueryError::ATS_QUERY_OK;
}

ATSEvent* ATSSimulator::createATSEvent( unsigned long row, IData* data )
{
	// Get the source table ID and convert it to an event type.
	ECombinedEventType eventType =  E_AUDIT_EVENT ;

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

	std::string alarmStatusEventType = "";    // add this for column "Alarm Status"
	if(eventTypeName.empty() == false && 
		(eventTypeName.find("AlarmAcknowledged") != std::string::npos
		|| eventTypeName.find("AlarmClosed") != std::string::npos
		|| eventTypeName.find("AlarmSubmitted") != std::string::npos
		|| eventTypeName.find("AlarmSupressed") != std::string::npos))
	{
		alarmStatusEventType = eventTypeName;
		if(eventTypeName.find("AlarmClosed")!=std::string::npos)
		{
			alarmStatusEventType = "AlarmNormalized";
		}
	}
	// TD 14751 get the expanded description with event type appended to front
	// 		std::string expandedDescription("");
	// 		if (eventTypeName.empty() == false) expandedDescription = eventTypeName + "- ";
	// 		expandedDescription += description;

	// Get the value.
	std::string value = data->getStringData( row, COLUMN_VALUE );

	// Get the MMS state information.
	std::string mmsState = data->getStringData( row, COLUMN_MMS );

	// Get the DSS state information.
	std::string dssState = data->getStringData( row, COLUMN_DSS );

	// Get the AVL state information.
	std::string avlState = "";
	if(eventTypeName.empty() == false && 
		(eventTypeName.find("AlarmIsAvalancheHead") != std::string::npos ) )
	{
		avlState = "H";
	}
	else if(eventTypeName.empty() == false && 
		(eventTypeName.find("AlarmIsAvalancheChild") != std::string::npos ) )
	{
		avlState = "F";
	}

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

	std::stringstream eventIDstream;
	eventIDstream<<pkey<<"a";
	std::string eventID = eventIDstream.str();

	ATSEvent * res = new ATSEvent();
	res->EventID = eventID;
	
	res->Operator = operatorName;
    res->Value = value;
	res->AlarmID = alarmId;
	
	// alarm type
	int randomNum = rand()%4;
	switch(randomNum)
	{
	case 0:
	    res->AlarmType = IATSEventsAPI::ATSEventAlarmType::ATS_EVENTALARMTYPE_BOTH;
		res->AlarmMMSType = IATSEventsAPI::ATSEventAlarmMMSType::ATS_EVENTALARMMMSTYPE_AUTO;
		res->AlarmAvalancheType= IATSEventsAPI::ATSEventAlarmAvalancheType::ATS_EVENTALARMAVLTYPE_HEAD;
		res->AlarmSeverity = randomNum;
		break;
	case 1:
		res->AlarmType = IATSEventsAPI::ATSEventAlarmType::ATS_EVENTALARMTYPE_MAINTENANCE;
		res->AlarmMMSType = IATSEventsAPI::ATSEventAlarmMMSType::ATS_EVENTALARMMMSTYPE_NOALARM;
		res->AlarmAvalancheType= IATSEventsAPI::ATSEventAlarmAvalancheType::ATS_EVENTALARMAVLTYPE_CHILD;
		res->AlarmSeverity = randomNum;
		break;
	case 2:
			 res->AlarmType = IATSEventsAPI::ATSEventAlarmType::ATS_EVENTALARMTYPE_OPERATIONAL;
		res->AlarmMMSType = IATSEventsAPI::ATSEventAlarmMMSType::ATS_EVENTALARMMMSTYPE_NONE;
		res->AlarmAvalancheType= IATSEventsAPI::ATSEventAlarmAvalancheType::ATS_EVENTALARMAVLTYPE_NOALARM;
		res->AlarmSeverity = randomNum;
		break;
	case 3:
			 res->AlarmType = IATSEventsAPI::ATSEventAlarmType::ATS_EVENTLARMTYPE_NOALARM;
		res->AlarmMMSType = IATSEventsAPI::ATSEventAlarmMMSType::ATS_EVENTALARMMMSTYPE_SEMIAUTO;
		res->AlarmAvalancheType= IATSEventsAPI::ATSEventAlarmAvalancheType::ATS_EVENTALARMAVLTYPE_NOALARM;
		res->AlarmSeverity = randomNum;
		break;
	default:
		break;

	}


	res->Description = "ATS_description "+ eventID;
	res->Asset = "ATS_Asset" + eventID;
	res->EventType = "System message";
	CTime ctime1 = (createTime.time);
	res->Timestamp = IATSEventsAPI::ATSDateTime(ctime1.GetYear(),ctime1.GetMonth(), ctime1.GetDay(), ctime1.GetHour(), ctime1.GetMinute(), ctime1.GetSecond(),0);
	return res;

}



ActionCommand  ATSSimulator::doStartTimeEndTimeQuery(std::vector<TA_Base_Core::ICombinedEventData*> & events, 
														FilterCriteria filterCriteria, unsigned long atsMaxLength)
{
	ActionCommand actionCommand = ActionCommand::Reconnect;
	ATSQueryError err;
	std::list<ATSEvent*> atsEvents;
	ATSFilterCriteria atsFilter;
	TA_Base_Core::CombinedEventAccessFactory::EventFilter eventFilter;
	TA_Base_App::ISCSWorkerHelper::populatePublicFilter(filterCriteria, eventFilter);

    transformToATSFilter(atsFilter, filterCriteria);
	err = DoStartTimeEndTimeQuery_simulator(
		&eventFilter,
		filterCriteria.StartTimePeriod,
		filterCriteria.EndTimePeriod,
		atsEvents);

	if( err == ATS_QUERY_OK )
	{
		// cut the length to be within atsMaxLength
		truncateATSEvents (atsEvents, atsMaxLength, true);

		//query succesful:
		transformToISCSEvents ( atsEvents, events ); 

		// free the temporary cache 
		IATSEventsAPI::FreeATSEventList(atsEvents);  
		actionCommand =  ActionCommand::NoCommand;

	}
	else
	{
		if(atsEvents.size()>0)
		{
			IATSEventsAPI::FreeATSEventList(atsEvents); 
		}

		actionCommand = ActionCommand::Reconnect;
	}

	return actionCommand;

}

ActionCommand ATSSimulator::doPreviousPageQuery(std::vector<TA_Base_Core::ICombinedEventData*> & events, 
												   FilterCriteria filterCriteria, unsigned long atsMaxLength)
{
	ActionCommand actionCommand = ActionCommand::Reconnect;
	ATSQueryError err;
	ATSFilterCriteria atsFilter;
	TA_Base_Core::CombinedEventAccessFactory::EventFilter eventFilter;
	std::list<ATSEvent*> atsEvents1;
	std::list<ATSEvent*> atsEvents2;


	ATSEventWrapper::transformToATSFilter(atsFilter, filterCriteria);

	TA_Base_App::ISCSWorkerHelper::populatePublicFilter(filterCriteria, eventFilter);

	//query [start, end)
	err = DoStartTimeEndTimeQuery_simulator(
		&eventFilter,
		filterCriteria.StartTimePeriod,
		filterCriteria.EndTimePeriod,
		atsEvents1);

	if( err == ATS_QUERY_OK )
	{
		// go on query [end, end+1s)
		filterCriteria.StartTimePeriod = filterCriteria.EndTimePeriod;
		ISCSDateTime tempTime = (filterCriteria.StartTimePeriod);
		tempTime.addSec(1);
		filterCriteria.EndTimePeriod = tempTime;
		TA_Base_App::ISCSWorkerHelper::populatePublicFilter(filterCriteria, eventFilter);

		err = DoStartTimeEndTimeQuery_simulator(
			&eventFilter,
			filterCriteria.StartTimePeriod,
			filterCriteria.EndTimePeriod,
			atsEvents2);

		cutIfIDEqualOrBiggerThan ( filterCriteria.pkeyBoundary_ATS, atsEvents2 );

		// atsEvents1 = atsEvents1 + atsEvents2
		mergeATSEvnts(atsEvents1, atsEvents2);

		// cut the length to be within atsMaxLength
		truncateATSEvents (atsEvents1, atsMaxLength, true);

		//query succesful:
		transformToISCSEvents ( atsEvents1, events ); 

		// free the temporary cache 
		IATSEventsAPI::FreeATSEventList(atsEvents1);  

		actionCommand =  ActionCommand::NoCommand;

	}
	else
	{
		if(atsEvents1.size()>0)
		{
			IATSEventsAPI::FreeATSEventList(atsEvents1);  
		}

		actionCommand = ActionCommand::Reconnect;
	}

	return actionCommand;

}
ActionCommand ATSSimulator::doNextPageQuery(std::vector<TA_Base_Core::ICombinedEventData*> & events, 
											   FilterCriteria filterCriteria, unsigned long atsMaxLength)
{
	ATSQueryError err;
	ATSFilterCriteria atsFilter;
	std::list<ATSEvent*> atsEvents1;
	std::list<ATSEvent*> atsEvents2;
	TA_Base_Core::CombinedEventAccessFactory::EventFilter eventFilter;

	transformToATSFilter(atsFilter, filterCriteria);
	TA_Base_App::ISCSWorkerHelper::populatePublicFilter(filterCriteria, eventFilter);

	if( ATSFilterTimeSpanLessThan1Sec(atsFilter))
	{
		// if the timespan is <1sec, query [start, end)
		err = DoStartTimeEndTimeQuery_simulator(
			&eventFilter,
			filterCriteria.StartTimePeriod,
			filterCriteria.EndTimePeriod,
			atsEvents1);
		if(err!=ATS_QUERY_OK)
		{
			if(atsEvents1.size()>0)
			{
				IATSEventsAPI::FreeATSEventList(atsEvents1);  
			}
			return  ActionCommand::Reconnect;
		}

	}
	else
	{   // if the timespan is >=1sec, startTime= startTime+1sec, query [start+1s, end);
		add1SecToATSFilterStartTime(atsFilter);
		ISCSDateTime oldStartTime = filterCriteria.StartTimePeriod;
		filterCriteria.StartTimePeriod.addSec(1);
		TA_Base_App::ISCSWorkerHelper::populatePublicFilter(filterCriteria, eventFilter);

		err = DoStartTimeEndTimeQuery_simulator(
			&eventFilter,
			filterCriteria.StartTimePeriod,
			filterCriteria.EndTimePeriod,
			atsEvents1);

		if(err != ATS_QUERY_OK)
		{
			if(atsEvents1.size()>0)
			{
				IATSEventsAPI::FreeATSEventList(atsEvents1); 
			}
			return  ActionCommand::Reconnect;
		}

		// then, query [ startTime, starTime+1s):
		filterCriteria.EndTimePeriod = filterCriteria.StartTimePeriod;
		filterCriteria.StartTimePeriod = oldStartTime;
		TA_Base_App::ISCSWorkerHelper::populatePublicFilter(filterCriteria, eventFilter);

		// [start, start+1s);
		err = DoStartTimeEndTimeQuery_simulator(
			&eventFilter,
			filterCriteria.StartTimePeriod,
			filterCriteria.EndTimePeriod,
			atsEvents2);
		if(err!=ATS_QUERY_OK)
		{
			if(atsEvents1.size()>0)
			{
				IATSEventsAPI::FreeATSEventList(atsEvents1);  
			}
			return  ActionCommand::Reconnect;
		}

		cutIfIDEqualOrSmallerThan ( filterCriteria.pkeyBoundary_ATS, atsEvents2 );
		// atsEvents1 = atsEvents2 + atsEvents1
		mergeATSEvnts(atsEvents1, atsEvents2);

	}
	// cut the length to be within atsMaxLength
	truncateATSEvents (atsEvents1, atsMaxLength, true);

	//query succesful:
	transformToISCSEvents ( atsEvents1, events ); 

	// free the temporary cache 
	IATSEventsAPI::FreeATSEventList(atsEvents1);  

	return  ActionCommand::NoCommand;

}

ActionCommand ATSSimulator::stopATSSubscription(void){

	ActionCommand actCommand = NoCommand;

	ATSRealTimeThread* subscriber = getATSSubscriberThread();

	if(  subscriber != NULL )
	{
		subscriber->m_paused = true;
		ATSSubscriptionError atsSubErr= ATSSubscriptionError::ATS_SUBSCRIPTION_OK;
		if(atsSubErr == ATSSubscriptionError::ATS_SUBSCRIPTION_OK){
			actCommand = ActionCommand::NoCommand;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ATS Subscription stopped successfully ");
		}else if(atsSubErr == ATSSubscriptionError::ATS_SUBSCRIPTION_FAILED_COMMS_ERROR){
			
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Get ATSSubscriptionError: %d when try to stopSubscription ", atsSubErr);

		}
	}			

	return actCommand;
}

ATSRealTimeThread* ATSSimulator::getATSSubscriberThread(){
	if ( m_atsRealTimeThread == NULL )
	{
		try{
			m_atsRealTimeThread = new ATSRealTimeThread();
			m_atsRealTimeThread->start();
			if(m_atsRealTimeThread != NULL)
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "New ATS event subscription created.");
		}

		catch (...)
		{				
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Exception caught when call ATSEventSubscription::CreateATSEventSubscription().");
		}

	}
	return m_atsRealTimeThread;
}

ActionCommand ATSSimulator::startATSSubscription(
	FilterCriteria filterCriteria,
	const unsigned int backloglength,				
	std::vector<TA_Base_Core::ICombinedEventData*>& events){


		ActionCommand actCommand = Reconnect;
		ATSFilterCriteria atsFilter;

		transformToATSFilter(atsFilter, filterCriteria );
		std::list<IATSEventsAPI::ATSEvent*> atsEvents;


		ATSRealTimeThread* subscriber = getATSSubscriberThread();
		if( subscriber!=NULL )
		{
			/*ATSSubscriptionError atsSubErr = subscriber ->startSubscription( &atsFilter, backloglength ,&TA_Base_App::eventSubscriptionCallback,  atsEvents);*/
			subscriber->m_paused = false;

			if(/*atsSubErr == ATSSubscriptionError::ATS_SUBSCRIPTION_OK*/ true ) 
			{
				// subscribe successfully:
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ATS subscriber has started a new subscription successfully, and returned total backlog events: %d ", atsEvents.size());

				// retrieve data
				/*transferToISCSEvents ( atsEvents, events ); */

				actCommand = NoCommand;
			}
			else 
			{
				// get a communication error
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Get Error when call ATS:startSubscription, ATSSubscriptionError:");

				if (atsEvents.size()>0)
				{
					IATSEventsAPI::FreeATSEventList(atsEvents);  
				}

				actCommand = ActionCommand::Reconnect;
			}

		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can't start ATS subscription due to getATSSubscriber unsuccessfully.");
			return ActionCommand::Reconnect;
		}


		return actCommand;

}
