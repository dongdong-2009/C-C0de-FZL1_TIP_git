/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3002_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/AlarmHistoricalQueue.cpp $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by:  $Author: Noel R. Tajanlangit $
  *
  * Implementation file
  */

#include "app/alarm/alarm_agent/src/AlarmHistoricalQueue.h"
#include "core/utilities/src/DebugUtil.h"

#define DEFAULT_HISTORICAL_QUEUE_SIZE 10000

using namespace TA_Base_Core;


namespace TA_Base_App
{
	AlarmHistoricalQueue::AlarmHistoricalQueue()
		: m_ulMaxQueueSize(0)
	{
		m_ulMaxQueueSize = DEFAULT_HISTORICAL_QUEUE_SIZE;
	}

	AlarmHistoricalQueue::~AlarmHistoricalQueue()
	{}

	void AlarmHistoricalQueue::insert(const std::string& alarmID)
	{
		FUNCTION_ENTRY("insert");

		T_AlarmIDSet::iterator itFind = m_alarmIDSet.find(alarmID);
		if (itFind != m_alarmIDSet.end())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Alarm ID already exist in the queue.");
			FUNCTION_EXIT;
			return;
		}

		if (m_alarmIDQueue.size() >= m_ulMaxQueueSize)
		{
			// Remove oldest ID if we reach maximum allowed size
			_removeOldestID();
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Inserting alarm ID %s", alarmID.c_str());
		m_alarmIDSet.insert(alarmID);
		m_alarmIDQueue.push(alarmID);
	
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Queue size=%u, Set size=%u", m_alarmIDQueue.size(), m_alarmIDSet.size());
		FUNCTION_EXIT;
	}

	bool AlarmHistoricalQueue::isAlarmSubmitted(const std::string& alarmID)
	{
		FUNCTION_ENTRY("isAlarmSubmitted");
		bool bRetVal = false;
		
		T_AlarmIDSet::iterator itFind = m_alarmIDSet.find(alarmID);
		if (itFind != m_alarmIDSet.end())
		{
			bRetVal = true;
		}

		FUNCTION_EXIT;
		return bRetVal;
	}

	void AlarmHistoricalQueue::_removeOldestID()
	{
		FUNCTION_ENTRY("_removeOldestID");

		std::string delID = m_alarmIDQueue.front();
		
		m_alarmIDSet.erase(delID);
		m_alarmIDQueue.pop();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Removing oldest data from historical queue, ID=%s", delID.c_str() );

		FUNCTION_EXIT;
	}
}