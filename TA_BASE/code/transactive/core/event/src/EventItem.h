/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event/src/EventItem.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __EVENT_ITEM_H_INCLUDED__
#define __EVENT_ITEM_H_INCLUDED__

#include <stdio.h>
#include <string.h>
#include <list>

namespace TA_Base_Core
{
	const int EV_SIZEOF_LONG					= sizeof(long);
	const int EV_SIZEOF_SHORT					= sizeof(short);

	const int EV_EVENT_ID_SIZE					= 38; //based on db
	const int EV_SOURCE_TABLE_SIZE				= 4;
	const int EV_SEVERITY_NAME_SIZE				= 20;
	const int EV_ASSET_NAME_SIZE				= 128;
	const int EV_DESCRIPTION_SIZE				= 400; //db is 1024
	const int EV_EVENT_TYPE_NAME_SIZE			= 128;
	const int EV_ALARM_VALUE_SIZE				= 128; //db is 2000
	const int EV_ALARM_ID_SIZE					= 38;  
	const int EV_MMS_STATE_SIZE					= 4;
	const int EV_DSS_STATE_SIZE					= 30;
	const int EV_AVL_STATE_SIZE					= 4;
	const int EV_OPERATOR_NAME_SIZE				= 85;
	const int EV_ALARM_COMMENT_SIZE				= 300; //db is 1024
	const int EV_ALARM_ACK_SIZE					= 38;
	const int EV_ALARM_STATUS_SIZE				= 4;
	const int EV_SESSION_KEY_SIZE				= 38;
	const int EV_OPERATION_MODE_SIZE			= 4;
	const int EV_AVLALARMHEADID_SIZE			= 38;
	
	const int EV_CREATETIME_TIME_SIZE			= EV_SIZEOF_LONG;
	const int EV_CREATETIME_MILLI_SIZE			= EV_SIZEOF_SHORT;
	const int EV_SUBSYSTEM_KEY_SIZE				= EV_SIZEOF_LONG;
	const int EV_PHYSICAL_SUBSYSTEM_KEY_SIZE	= EV_SIZEOF_LONG;
	const int EV_LOCATION_KEY_SIZE				= EV_SIZEOF_LONG;
	const int EV_SEVERITY_KEY_SIZE				= EV_SIZEOF_LONG;
	const int EV_EVENT_TYPE_KEY_SIZE			= EV_SIZEOF_LONG;
	const int EV_ALARM_TYPE_KEY_SIZE			= EV_SIZEOF_LONG;
	const int EV_OPERATOR_KEY_SIZE				= EV_SIZEOF_LONG;
	const int EV_EVENT_LEVEL_SIZE				= EV_SIZEOF_LONG;
	const int EV_SESSION_LOCATION_SIZE			= EV_SIZEOF_LONG;
	const int EV_PROFILE_ID_SIZE				= EV_SIZEOF_LONG;
	const int EV_ACTION_ID_SIZE					= EV_SIZEOF_LONG;
	const int EV_ENTITY_KEY_SIZE				= EV_SIZEOF_LONG;
	const int EV_SYSTEM_KEY_SIZE				= EV_SIZEOF_LONG;
	const int EV_SEQ_HIGH_ORDER_SIZE			= EV_SIZEOF_LONG;
	const int EV_SEQ_LOW_ORDER_SIZE				= EV_SIZEOF_LONG;
	
	const int EV_DATA_SIZE = 
			EV_EVENT_ID_SIZE + 
			EV_SOURCE_TABLE_SIZE +
			EV_SEVERITY_NAME_SIZE +
			EV_ASSET_NAME_SIZE +		
			EV_DESCRIPTION_SIZE +		
			EV_EVENT_TYPE_NAME_SIZE +	
			EV_ALARM_VALUE_SIZE +		
			EV_ALARM_ID_SIZE +			
			EV_MMS_STATE_SIZE +			
			EV_DSS_STATE_SIZE +			
			EV_AVL_STATE_SIZE +			
			EV_OPERATOR_NAME_SIZE +		
			EV_ALARM_COMMENT_SIZE +		
			EV_ALARM_ACK_SIZE +			
			EV_ALARM_STATUS_SIZE +		
			EV_SESSION_KEY_SIZE +		
			EV_OPERATION_MODE_SIZE +	
			EV_AVLALARMHEADID_SIZE +	
			EV_CREATETIME_TIME_SIZE +
			EV_CREATETIME_MILLI_SIZE +
			EV_SUBSYSTEM_KEY_SIZE +
			EV_PHYSICAL_SUBSYSTEM_KEY_SIZE +
			EV_LOCATION_KEY_SIZE +
			EV_SEVERITY_KEY_SIZE +
			EV_EVENT_TYPE_KEY_SIZE +
			EV_ALARM_TYPE_KEY_SIZE +
			EV_OPERATOR_KEY_SIZE +
			EV_EVENT_LEVEL_SIZE +
			EV_SESSION_LOCATION_SIZE +
			EV_PROFILE_ID_SIZE +
			EV_ACTION_ID_SIZE +
			EV_ENTITY_KEY_SIZE +
			EV_SYSTEM_KEY_SIZE +
			EV_SEQ_HIGH_ORDER_SIZE +
			EV_SEQ_LOW_ORDER_SIZE;

	const int EV_DATA_FIXED_SIZE = 
		EV_EVENT_ID_SIZE + 
		EV_SOURCE_TABLE_SIZE +
		EV_SEVERITY_NAME_SIZE +
		EV_ALARM_ID_SIZE +			
		EV_MMS_STATE_SIZE +			
		EV_DSS_STATE_SIZE +			
		EV_AVL_STATE_SIZE +			
		EV_ALARM_ACK_SIZE +			
		EV_ALARM_STATUS_SIZE +		
		EV_SESSION_KEY_SIZE +		
		EV_OPERATION_MODE_SIZE +	
		EV_AVLALARMHEADID_SIZE +	
		EV_CREATETIME_TIME_SIZE +
		EV_CREATETIME_MILLI_SIZE +
		EV_SUBSYSTEM_KEY_SIZE +
		EV_PHYSICAL_SUBSYSTEM_KEY_SIZE +
		EV_LOCATION_KEY_SIZE +
		EV_SEVERITY_KEY_SIZE +
		EV_EVENT_TYPE_KEY_SIZE +
		EV_ALARM_TYPE_KEY_SIZE +
		EV_OPERATOR_KEY_SIZE +
		EV_EVENT_LEVEL_SIZE +
		EV_SESSION_LOCATION_SIZE +
		EV_PROFILE_ID_SIZE +
		EV_ACTION_ID_SIZE +
		EV_ENTITY_KEY_SIZE +
		EV_SYSTEM_KEY_SIZE +
		EV_SEQ_HIGH_ORDER_SIZE +
		EV_SEQ_LOW_ORDER_SIZE;

	struct MillisecondTimeEx 
	{
		unsigned long time;
		unsigned short millitm;
		MillisecondTimeEx():time(0), millitm(0) {};
		MillisecondTimeEx(unsigned long _time, unsigned short _millitm)
			:time(_time),millitm(_millitm)
		{
		}
		MillisecondTimeEx(const MillisecondTimeEx& rhs)
			:time(rhs.time),millitm(rhs.millitm)
		{
		}
		/*************************************************************************/
		/* Please note that these overload comparison operators are being use in */
		/* sorting method, for the STL containers, if changes are needed please  */
		/* consider the sort order.												 */
		/*************************************************************************/
		bool operator < (const MillisecondTimeEx& rhs) const
		{
			if(time == rhs.time)
			{
				return millitm<rhs.millitm;
			}
			return time < rhs.time;
		}

		bool operator > (const MillisecondTimeEx& rhs) const
		{
			if(time == rhs.time)
			{
				return millitm>rhs.millitm;
			}
			return time > rhs.time;
		}

		bool operator == (const MillisecondTimeEx& rhs) const
		{
			if (time == rhs.time)
			{
				return millitm == rhs.millitm;
			}

			return false;
		}

		MillisecondTimeEx& operator=( const MillisecondTimeEx& rhs )
		{
			MillisecondTimeEx tmp(rhs);

			time = tmp.time;
			millitm = tmp.millitm;

			return *this;
		}

	};

	struct EvMgsSequenceNumber 
	{
		unsigned long ulHighOrder;
		unsigned long ulLowOrder;
		EvMgsSequenceNumber(): ulHighOrder(0), ulLowOrder(0) {}
		EvMgsSequenceNumber(unsigned long _ulHighOrder, unsigned long _ulLowOrder)
			:ulHighOrder(_ulHighOrder),ulLowOrder(_ulLowOrder)
		{
		}
		EvMgsSequenceNumber(const EvMgsSequenceNumber& rhs)
			:ulHighOrder(rhs.ulHighOrder),ulLowOrder(rhs.ulLowOrder)
		{
		}
		/*************************************************************************/
		/* Please note that these overload comparison operators are being use in */
		/* sorting method, for the STL containers, if changes are needed please  */
		/* consider the sort order.												 */
		/*************************************************************************/
		bool operator < (const EvMgsSequenceNumber& rhs) const
		{
			if(ulHighOrder == rhs.ulHighOrder)
			{
				return ulLowOrder<rhs.ulLowOrder;
			}
			return ulHighOrder < rhs.ulHighOrder;
		}

		bool operator > (const EvMgsSequenceNumber& rhs) const
		{
			if(ulHighOrder == rhs.ulHighOrder)
			{
				return ulLowOrder>rhs.ulLowOrder;
			}
			return ulHighOrder > rhs.ulHighOrder;
		}

		bool operator == (const EvMgsSequenceNumber& rhs) const
		{
			if (ulHighOrder == rhs.ulHighOrder)
			{
				return ulLowOrder == rhs.ulLowOrder;
			}

			return false;
		}

		EvMgsSequenceNumber& operator=( const EvMgsSequenceNumber& rhs )
		{
			EvMgsSequenceNumber tmp(rhs);

			ulHighOrder = tmp.ulHighOrder;
			ulLowOrder = tmp.ulLowOrder;

			return *this;
		}
	};

	class EventItem
	{
	public:
		EventItem()
			: eventID(m_eventID)
			  ,sourceTable(m_sourceTable)
			  ,severityname(m_severityname)
			  ,assetname(m_assetname)
			  ,description(m_description)
			  ,eventTypeName(m_eventTypeName)
			  ,alarmID(m_alarmID)
			  ,value(m_value),mms(m_mms)
			  ,decisionSupportState(m_decisionSupportState)
			  ,avalanceState(m_avalanceState)
			  ,operatorName(m_operatorName)
			  ,alarmComment(m_alarmComment)
			  ,alarmAckedBy(m_alarmAckedBy)
			  ,alarmStatus(m_alarmStatus)
			  ,sessionID(m_sessionID)
			  ,operatorMode(m_operatorMode)
			  ,avlheadID(m_avlheadID)
			  ,createTime(0,0) ,subsystemKey(0),
			  physicalSubsystemKey(0),
			  locationKey(0),severityKey(0),
			  eventTypeKey(0),alarmTypeKey(0),
			  operatorKey(0),eventLevel(0),
			  sessionLocation(0),profileID(0),
			  actionID(0),entitykey(0),
			  systemKey(0),eventSequence(0,0)	
		  {};
		inline ~EventItem()
		
		{};
		inline EventItem(
			const char* _eventID, const char* _sourceTable, MillisecondTimeEx _createTime,
			unsigned long _subsystemKey, long _physicalSubsystemKey,
			unsigned long _locationKey, unsigned long _severityKey,
			const char* _severityname, const char* _assetname, const char* _description,
			unsigned long _eventTypeKey, const char* _eventTypeName,
			const char* _alarmID, unsigned long _alarmTypeKey, const char* _value,
			const char* _mms, const char* _decisionSupportState, const char* _avalanceState,
			unsigned long _operatorKey,	const char* _operatorName, const char* _alarmComment,
			unsigned long _eventLevel, const char* _alarmAckedBy, const char* _alarmStatus,
			const char* _sessionID, unsigned long _sessionLocation, long _profileID,
			unsigned long _actionID, const char* _operatorMode, unsigned long _entitykey,
			const char* _avlheadID, unsigned long _systemKey,
			EvMgsSequenceNumber _eventSequence): 
					eventID(m_eventID)
					,sourceTable(m_sourceTable)
					,severityname(m_severityname)
					,assetname(m_assetname)
					,description(m_description)
					,eventTypeName(m_eventTypeName)
					,alarmID(m_alarmID)
					,value(m_value),mms(m_mms)
					,decisionSupportState(m_decisionSupportState)
					,avalanceState(m_avalanceState)
					,operatorName(m_operatorName)
					,alarmComment(m_alarmComment)
					,alarmAckedBy(m_alarmAckedBy)
					,alarmStatus(m_alarmStatus)
					,sessionID(m_sessionID)
					,operatorMode(m_operatorMode)
					,avlheadID(m_avlheadID)
					,createTime(_createTime),subsystemKey(_subsystemKey),
					physicalSubsystemKey(_physicalSubsystemKey),
					locationKey(_locationKey),severityKey(_severityKey),
					eventTypeKey(_eventTypeKey),alarmTypeKey(_alarmTypeKey),
					operatorKey(_operatorKey),eventLevel(_eventLevel),
					sessionLocation(_sessionLocation),profileID(_profileID),
					actionID(_actionID),entitykey(_entitykey),
					systemKey(_systemKey),eventSequence(_eventSequence)				
			{
				assign(m_eventID, _eventID, EV_EVENT_ID_SIZE);
				assign(m_sourceTable, _sourceTable, EV_SOURCE_TABLE_SIZE);
				assign(m_severityname, _severityname, EV_SEVERITY_NAME_SIZE);
				assign(m_assetname, _assetname, EV_ASSET_NAME_SIZE);
				assign(m_description, _description, EV_DESCRIPTION_SIZE);
				assign(m_eventTypeName, _eventTypeName, EV_EVENT_TYPE_NAME_SIZE);
				assign(m_alarmID, _alarmID, EV_ALARM_ID_SIZE);
				assign(m_value, _value, EV_ALARM_VALUE_SIZE);
				assign(m_mms, _mms, EV_MMS_STATE_SIZE);
				assign(m_decisionSupportState, _decisionSupportState, EV_DSS_STATE_SIZE);
				assign(m_avalanceState, _avalanceState, EV_AVL_STATE_SIZE);
				assign(m_operatorName, _operatorName, EV_OPERATOR_NAME_SIZE);
				assign(m_alarmComment, _alarmComment, EV_ALARM_COMMENT_SIZE);
				assign(m_alarmAckedBy, _alarmAckedBy, EV_ALARM_ACK_SIZE);
				assign(m_alarmStatus, _alarmStatus, EV_ALARM_STATUS_SIZE);
				assign(m_sessionID, _sessionID, EV_SESSION_KEY_SIZE);
				assign(m_operatorMode, _operatorMode, EV_OPERATION_MODE_SIZE);
				assign(m_avlheadID, _avlheadID, EV_AVLALARMHEADID_SIZE);
			};

		inline EventItem(const EventItem& rhs) :
			eventID(m_eventID)
			,sourceTable(m_sourceTable)
			,severityname(m_severityname)
			,assetname(m_assetname)
			,description(m_description)
			,eventTypeName(m_eventTypeName)
			,alarmID(m_alarmID)
			,value(m_value),mms(m_mms)
			,decisionSupportState(m_decisionSupportState)
			,avalanceState(m_avalanceState)
			,operatorName(m_operatorName)
			,alarmComment(m_alarmComment)
			,alarmAckedBy(m_alarmAckedBy)
			,alarmStatus(m_alarmStatus)
			,sessionID(m_sessionID)
			,operatorMode(m_operatorMode)
			,avlheadID(m_avlheadID)
			,createTime(rhs.createTime),subsystemKey(rhs.subsystemKey),
			physicalSubsystemKey(rhs.physicalSubsystemKey),
			locationKey(rhs.locationKey),severityKey(rhs.severityKey),
			eventTypeKey(rhs.eventTypeKey),alarmTypeKey(rhs.alarmTypeKey),
			operatorKey(rhs.operatorKey),eventLevel(rhs.eventLevel),
			sessionLocation(rhs.sessionLocation),profileID(rhs.profileID),
			actionID(rhs.actionID),entitykey(rhs.entitykey),
			systemKey(rhs.systemKey),eventSequence(rhs.eventSequence)	
		{

			assign(m_eventID, rhs.eventID, EV_EVENT_ID_SIZE);
			assign(m_sourceTable, rhs.sourceTable, EV_SOURCE_TABLE_SIZE);
			assign(m_severityname, rhs.severityname, EV_SEVERITY_NAME_SIZE);
			assign(m_assetname, rhs.assetname, EV_ASSET_NAME_SIZE);
			assign(m_description, rhs.description, EV_DESCRIPTION_SIZE);
			assign(m_eventTypeName, rhs.eventTypeName, EV_EVENT_TYPE_NAME_SIZE);
			assign(m_alarmID, rhs.alarmID, EV_ALARM_ID_SIZE);
			assign(m_value, rhs.value, EV_ALARM_VALUE_SIZE);
			assign(m_mms, rhs.mms, EV_MMS_STATE_SIZE);
			assign(m_decisionSupportState, rhs.decisionSupportState, EV_DSS_STATE_SIZE);
			assign(m_avalanceState, rhs.avalanceState, EV_AVL_STATE_SIZE);
			assign(m_operatorName, rhs.operatorName, EV_OPERATOR_NAME_SIZE);
			assign(m_alarmComment, rhs.alarmComment, EV_ALARM_COMMENT_SIZE);
			assign(m_alarmAckedBy, rhs.alarmAckedBy, EV_ALARM_ACK_SIZE);
			assign(m_alarmStatus, rhs.alarmStatus, EV_ALARM_STATUS_SIZE);
			assign(m_sessionID, rhs.sessionID, EV_SESSION_KEY_SIZE);
			assign(m_operatorMode, rhs.operatorMode, EV_OPERATION_MODE_SIZE);
			assign(m_avlheadID, rhs.avlheadID, EV_AVLALARMHEADID_SIZE);
		}
		
		inline  unsigned long getDataSize()
		{
			unsigned long ulRetVal = 0;
			ulRetVal += EV_EVENT_ID_SIZE +
				EV_SOURCE_TABLE_SIZE +
				EV_ALARM_STATUS_SIZE +
				EV_OPERATION_MODE_SIZE +
				EV_CREATETIME_TIME_SIZE +
				EV_CREATETIME_MILLI_SIZE +
				EV_SUBSYSTEM_KEY_SIZE +
				EV_PHYSICAL_SUBSYSTEM_KEY_SIZE +
				EV_LOCATION_KEY_SIZE +
				EV_SEVERITY_KEY_SIZE +
				EV_EVENT_TYPE_KEY_SIZE +
				EV_ALARM_TYPE_KEY_SIZE +
				EV_OPERATOR_KEY_SIZE +
				EV_EVENT_LEVEL_SIZE +
				EV_SESSION_LOCATION_SIZE +
				EV_PROFILE_ID_SIZE +
				EV_ACTION_ID_SIZE +
				EV_ENTITY_KEY_SIZE +
				EV_SYSTEM_KEY_SIZE +
				EV_SEQ_HIGH_ORDER_SIZE +
				EV_SEQ_LOW_ORDER_SIZE;

			ulRetVal += strlen(severityname);
			ulRetVal += strlen(assetname);
			ulRetVal += strlen(description);
			ulRetVal += strlen(eventTypeName);
			ulRetVal += strlen(value);
			ulRetVal += strlen(alarmID);
			ulRetVal += strlen(mms);
			ulRetVal += strlen(decisionSupportState);
			ulRetVal += strlen(avalanceState);
			ulRetVal += strlen(operatorName);
			ulRetVal += strlen(alarmComment);
			ulRetVal += strlen(alarmAckedBy);
			ulRetVal += strlen(sessionID);
			ulRetVal += strlen(avlheadID);

			return ulRetVal;
		}

		/**
		 * Unique identifier for the event data
		 */
		char* eventID;

		/**
		 * Indicates the source db table of the event
		 */
		char* sourceTable;

		/** 
		 * Create time the same with the source table
		 */
		MillisecondTimeEx createTime;

		/**
		 * sub systemkey derived from the source table
		 */
		unsigned long subsystemKey;

		/**
		 * physical sub system derived from the source table
		 */
		long physicalSubsystemKey;

		/**
		 * location key 
		 */
		unsigned long locationKey;

		/**
		 * Severity key
		 */
		unsigned long severityKey;

		/**
		 * Severity name
		 */
		char* severityname;

		/**
		 * Asset name
		 */
		char* assetname;

		/**
		 * description
		 */
		char* description;

		/**
		 * Event type key
		 */
		unsigned long eventTypeKey;

		/**
		 * Event type name
		 */
		char* eventTypeName;

		/**
		 * Alarm ID
		 */
		char* alarmID;

		/**
		 * Alarm Type key
		 */
		unsigned long alarmTypeKey;

		/**
		 * value
		 */
		char* value;

		/**
		 * Maintenance Management State
		 */
		char* mms;

		/**
		 * Decision Support State
		 */
		char* decisionSupportState;

		/**
		 * avalance state
		 */
		char* avalanceState;

		/**
		 * operator key
		 */
		unsigned long operatorKey;

		/**
		 * operator key
		 */
		char* operatorName;

		/**
		 * alarm comment
		 */
		char* alarmComment;

		/**
		 * Event Level
		 */
		unsigned long eventLevel;

		/**
		 * Alarm Acknowledge by
		 */
		char* alarmAckedBy;

		/**
		 * alarm status
		 */
		char* alarmStatus;
		
		/**
		 * Session ID
		 */
		char* sessionID;

		/**
		 * session location
		 */
		unsigned long sessionLocation;

		/**
		 * profile ID
		 */
		long profileID;

		/**
		 * action ID
		 */
		unsigned long actionID;

		/**
		 * Operator mode
		 */
		char* operatorMode;

		/**
		 * entity key
		 */
		unsigned long entitykey;

		/**
		 * avalanche head ID
		 */
		char* avlheadID;

		/**
		 * system key
		 */
		unsigned long systemKey;

		/**
		 * Event Sequence
		 */
		EvMgsSequenceNumber eventSequence;

	private:
		inline size_t assign(char buf[], const char* value, size_t bufSize)
		{
			size_t cpLen = strlen(value) >= bufSize ? bufSize - 1 : strlen(value);
			memcpy(buf, value, cpLen);
			buf[cpLen] = 0;

			return cpLen;
		}

		char m_eventID[EV_EVENT_ID_SIZE];
		char m_sourceTable[EV_SOURCE_TABLE_SIZE];
		char m_severityname[EV_SEVERITY_NAME_SIZE];
		char m_assetname[EV_ASSET_NAME_SIZE];
		char m_description[EV_DESCRIPTION_SIZE];
		char m_eventTypeName[EV_EVENT_TYPE_NAME_SIZE];
		char m_alarmID[EV_ALARM_ID_SIZE];
		char m_value[EV_ALARM_VALUE_SIZE];
		char m_mms[EV_MMS_STATE_SIZE];
		char m_decisionSupportState[EV_DSS_STATE_SIZE];
		char m_avalanceState[EV_AVL_STATE_SIZE];
		char m_operatorName[EV_OPERATOR_NAME_SIZE];
		char m_alarmComment[EV_ALARM_COMMENT_SIZE];
		char m_alarmAckedBy[EV_ALARM_ACK_SIZE];
		char m_alarmStatus[EV_ALARM_STATUS_SIZE];
		char m_sessionID[EV_SESSION_KEY_SIZE];
		char m_operatorMode[EV_OPERATION_MODE_SIZE];
		char m_avlheadID[EV_AVLALARMHEADID_SIZE];
	
	};

	//Note: This structure will delete its internal pointer
	//so clients don't need to delete the pointer
	struct EventItemInfo 
	{
		bool isEntityInfoValid;
		bool isSessionInfoValid;
		EventItem* eventItemDetail;

		EventItemInfo()
			:eventItemDetail(0)
			,isEntityInfoValid(false)
			,isSessionInfoValid(false)
			{};
		~EventItemInfo() {
			if (0 != eventItemDetail)
			{
				delete eventItemDetail;
				eventItemDetail=0;
			}
		}
	};

	typedef std::list< EventItem* > T_EventItemList;
	typedef std::list< EventItemInfo* > T_EventItemInfoList;
}


#endif //__EVENT_ITEM_H_INCLUDED__
