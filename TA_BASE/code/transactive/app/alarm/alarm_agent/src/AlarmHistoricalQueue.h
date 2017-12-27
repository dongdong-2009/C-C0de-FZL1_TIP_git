/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3002_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/AlarmHistoricalQueue.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by:  $Author: Noel R. Tajanlangit $
  *
  * 
  */
#if !defined(__ALARM_HISTORICAL_QUEUE_H_INCLUDED__)
#define __ALARM_HISTORICAL_QUEUE_H_INCLUDED__

#include <queue>
#include <set>
#include <string>

namespace TA_Base_App
{
	typedef std::queue< std::string > T_AlarmIDQueue;
	typedef std::set< std::string > T_AlarmIDSet;

	class AlarmHistoricalQueue
	{
	public:
		/**
          * Constructor
          */
		AlarmHistoricalQueue();
		/**
          * Destructor
          */
		~AlarmHistoricalQueue();

		/**
          * insert
		  * 
		  * inserts the new alarm ID into the cache
		  *
		  * @Param const std::string the alarmID
          */
		void insert(const std::string& alarmID);

		/**
          * isAlarmSubmitted
		  * 
		  * check if the alarmID is aleady existing in the cache.
		  *
		  * @Param const std::string the alarmID
		  * @Return true if ID is found otherwise false
          */
		bool isAlarmSubmitted(const std::string& alarmID);

	private:

		/**
          * _removeOldestID
          */
		void _removeOldestID();
		
		// Cached according to insertion.
		T_AlarmIDQueue m_alarmIDQueue;
		// Cached sorted by ID
		T_AlarmIDSet m_alarmIDSet;
		// Maximum size for the cache
		unsigned long m_ulMaxQueueSize;
	};

} //TA_Base_App

#endif //__ALARM_HISTORICAL_QUEUE_H_INCLUDED__