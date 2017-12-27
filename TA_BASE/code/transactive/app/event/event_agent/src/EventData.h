/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/EventDataIndices.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __EVENT_DATA_H_INCLUDED__
#define __EVENT_DATA_H_INCLUDED__

#include <string>

namespace TA_Base_App
{
	struct MillisecondTimeEx 
	{
		unsigned long time;
		unsigned short millitm;
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

	class EventData 
	{
	public:
		inline ~EventData(){};
		inline EventData(
			std::string _eventID, std::string _sourceTable, MillisecondTimeEx _createTime,
			unsigned long _subsystemKey, long _physicalSubsystemKey,
			unsigned long _locationKey, unsigned long _severityKey,
			std::string _severityname, std::string _assetname, std::string _description,
			unsigned long _eventTypeKey, std::string _eventTypeName,
			std::string _alarmID, unsigned long _alarmTypeKey, std::string _value,
			std::string _mms, std::string _decisionSupportState, std::string _avalanceState,
			unsigned long _operatorKey,	std::string _operatorName, std::string _alarmComment,
			unsigned long _eventLevel, std::string _alarmAckedBy, std::string _alarmStatus,
			std::string _sessionID, unsigned long _sessionLocation, long _profileID,
			unsigned long _actionID, std::string _operatorMode, unsigned long _entitykey,
			std::string _avlheadID, unsigned long _systemKey,
			EvMgsSequenceNumber _eventSequence): 
				eventID(_eventID),sourceTable(_sourceTable),
				createTime(_createTime),subsystemKey(_subsystemKey),
				physicalSubsystemKey(_physicalSubsystemKey),
				locationKey(_locationKey),severityKey(_severityKey),
				severityname(_severityname),assetname(_assetname),
				description(_description),eventTypeKey(_eventTypeKey),
				eventTypeName(_eventTypeName),alarmID(_alarmID),
				alarmTypeKey(_alarmTypeKey),value(_value),mms(_mms),
				decisionSupportState(_decisionSupportState),
				avalanceState(_avalanceState),operatorKey(_operatorKey),
				operatorName(_operatorName),alarmComment(_alarmComment),
				eventLevel(_eventLevel),alarmAckedBy(_alarmAckedBy),
				alarmStatus(_alarmStatus),sessionID(_sessionID),
				sessionLocation(_sessionLocation),profileID(_profileID),
				actionID(_actionID),operatorMode(_operatorMode),
				entitykey(_entitykey),avlheadID(_avlheadID),
				systemKey(_systemKey),eventSequence(_eventSequence)				
			{};



		/**
		 * Unique identifier for the event data
		 */
		std::string eventID;

		/**
		 * Indicates the source db table of the event
		 */
		std::string sourceTable;

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
		std::string severityname;

		/**
		 * Asset name
		 */
		std::string assetname;

		/**
		 * description
		 */
		std::string description;

		/**
		 * Event type key
		 */
		unsigned long eventTypeKey;

		/**
		 * Event type name
		 */
		std::string eventTypeName;

		/**
		 * Alarm ID
		 */
		std::string alarmID;

		/**
		 * Alarm Type key
		 */
		unsigned long alarmTypeKey;

		/**
		 * value
		 */
		std::string value;

		/**
		 * Maintenance Management State
		 */
		std::string mms;

		/**
		 * Decision Support State
		 */
		std::string decisionSupportState;

		/**
		 * avalance state
		 */
		std::string avalanceState;

		/**
		 * operator key
		 */
		unsigned long operatorKey;

		/**
		 * operator key
		 */
		std::string operatorName;

		/**
		 * alarm comment
		 */
		std::string alarmComment;

		/**
		 * Event Level
		 */
		unsigned long eventLevel;

		/**
		 * Alarm Acknowledge by
		 */
		std::string alarmAckedBy;

		/**
		 * alarm status
		 */
		std::string alarmStatus;
		
		/**
		 * Session ID
		 */
		std::string sessionID;

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
		std::string operatorMode;

		/**
		 * entity key
		 */
		unsigned long entitykey;

		/**
		 * avalance head ID
		 */
		std::string avlheadID;

		/**
		 * system key
		 */
		unsigned long systemKey;

		/**
		 * Event Sequence
		 */
		EvMgsSequenceNumber eventSequence;

	private:

	};

}


#endif //__EVENT_DATA_H_INCLUDED__