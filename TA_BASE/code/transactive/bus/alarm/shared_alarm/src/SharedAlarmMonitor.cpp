/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_Review_Branch/TA_BASE/transactive/bus/alarm/alarm_store_library/src/AlarmStoreManager.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2011/01/19 10:41:03 $
  * Last modified by:  $Author: ouyang,zhilin $
  *
  */
///////////////////////////////////////////////////////////
//  SharedAlarmMonitor.cpp
//  Implementation of the Class SharedAlarmMonitor
//  Created on:      19-Jan-2011 10:41:03 AM
///////////////////////////////////////////////////////////

#include <time.h>
#include <windows.h>
#include "SharedAlarmMonitor.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"


using namespace TA_Base_Bus;
using TA_Base_Core::DebugUtil;


SharedAlarmMonitor* SharedAlarmMonitor::m_pSharedAlarmMonitor = NULL;
TA_Base_Core::NonReEntrantThreadLockable SharedAlarmMonitor::m_singletonLock;

/**
 * constructor & destructor
 * 
 */

SharedAlarmMonitor::SharedAlarmMonitor()
{
	
	m_LocationKeyMap.clear();

 	//m_defNormalTimeOut=defNormalTime;       // unit: seconds  AlarmAgentInterface::receiveAlarmMessage to AlarmStoreManager::newAlarm
	m_defDutyChangeTimeOut=defDutyChangeTime;           // unit: seconds  AlarmStoreServant::dutyChanged
	//m_defLoadCurrentAlarmsTimeOut=defLoadCurrentAlarms;        // unit: seconds  AlarmAgentInterface. remove & insert alarms on shared memory
    m_unSeqErrorTimes=0;
	m_nTryDutyTimes=0;
	m_enableDutyCheck=false;
	m_terminate=false;
}

SharedAlarmMonitor::~SharedAlarmMonitor()
{
	mapIterType mapIter = m_LocationKeyMap.begin();
	while ( mapIter != m_LocationKeyMap.end() )
	{
		delete (*mapIter).second;
		(*mapIter).second = 0;
		mapIter++;
	}
	m_LocationKeyMap.clear();
}

SharedAlarmMonitor& SharedAlarmMonitor::getInstance()
{
	TA_Base_Core::ThreadGuard guard( m_singletonLock );

	if ( NULL == m_pSharedAlarmMonitor )
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,
			"SharedAlarmMonitor::getInstance():SharedAlarmMonitor started" );
		m_pSharedAlarmMonitor = new SharedAlarmMonitor();
	}

	return (*m_pSharedAlarmMonitor);
}

void SharedAlarmMonitor::removeInstance()
{
	TA_Base_Core::ThreadGuard guard( m_singletonLock );

	if ( NULL != m_pSharedAlarmMonitor )
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,
			"SharedAlarmMonitor::getInstance():SharedAlarmMonitor stopped" );
        //m_pSharedAlarmMonitor->terminateAndWait();
		delete m_pSharedAlarmMonitor;
		m_pSharedAlarmMonitor = 0;
	}
}

/*
* UpdateTimeBegin ****
*
* update the begin time of processing alarm store messages
* All the locations/threads time store in a map pair, when 
* update begin, just add the new location/thread in the map
* if it's not exist
*/
void SharedAlarmMonitor::updateTimeBegin( unsigned long ulLocationKey )
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	// find if the location/thread already exist
	mapIterType iter = m_LocationKeyMap.find(ulLocationKey);
	if ( iter != m_LocationKeyMap.end() )
	{	 
		if ((*iter).second->enableNormalCheck==false)
		{		
			(*iter).second->ulBeginTime = time(NULL);
			(*iter).second->enableNormalCheck=true;
		}		 
	}
	else
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,
			"SharedAlarmMonitor::updateTimeBegin():a new LocationKey=%ld launched", ulLocationKey );
		LocationKeyItem* pLocationKeyItem = new LocationKeyItem();			
		pLocationKeyItem->ulBeginTime = time(NULL);
		//pLocationKeyItem->ulSeqEndTime   = pLocationKeyItem->ulBeginTime;
		//pLocationKeyItem->ulSeqBeginTime = pLocationKeyItem->ulBeginTime;
		pLocationKeyItem->enableNormalCheck=true;
		m_LocationKeyMap.insert(mapValueType(ulLocationKey, pLocationKeyItem));	
	}
	
}

/*
* UpdateTimeEnd ****
* 
* update the end time of processing alarm messages
* the rules as: normally all the alarm processing should completed
* within 4 minutes, for duty change and reload current alarms, however,
* the completed time adjust to 5 minutes
*
* @ ulLocationKey: the identifier of the location/thread
* @ ulTime: the begin time of the location/thread
*/
void SharedAlarmMonitor::updateTimeEnd(unsigned long ulLocationKey)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

	// first check if the location key or thread exists, if not, then something
	// wrong may happened.
	mapIterType iter = m_LocationKeyMap.find(ulLocationKey);
	if ( iter != m_LocationKeyMap.end() )
	{
// 		LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,
// 			"SharedAlarmMonitor::updateTimeEnd():LocationKey=%d", ulLocationKey );	 	 
		(*iter).second->enableNormalCheck=false;		
	}
	else
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
			"SharedAlarmMonitor::updateTimeEnd():Cannot found the locationKey, LocationKey=%d",ulLocationKey );
	}
}

void SharedAlarmMonitor::updateDutyChange(bool blnStart)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

	LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,
			"SharedAlarmMonitor::updateDutyChange(): blnStart:%d", blnStart );
	if (blnStart)
	{
		m_ulDutyBeginTime=time(NULL);
		m_enableDutyCheck=true;
	}
	else
	{		 
		m_enableDutyCheck=false;
	}

	//RefreshTimeOut(blnStart, defDutyChangeTime);
}

//*******
void SharedAlarmMonitor::updateLoad(bool blnStart, unsigned long ulLocationKey )
{
	TA_Base_Core::ThreadGuard guard( m_lock );

	LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,
			"SharedAlarmMonitor::updateLoad(): blnStart:%d, LocationKey:%ld", blnStart, ulLocationKey );

	mapIterType mapIter = m_LocationKeyMap.find(ulLocationKey);
	if ( mapIter != m_LocationKeyMap.end() )
	{
		if ( blnStart )	
		{		 
			(*mapIter).second->ulLoadBeginTime  = time(NULL);		
			(*mapIter).second->enableLoadCheck=true;
		}
		else
		{			 
			(*mapIter).second->enableLoadCheck=false;
		}		
	
	}
	else
	{
		LocationKeyItem* pLocationKeyItem = new LocationKeyItem();
		pLocationKeyItem->ulLoadBeginTime  = time(NULL);		
		pLocationKeyItem->enableLoadCheck=true;
		m_LocationKeyMap.insert(mapValueType(ulLocationKey, pLocationKeyItem));

	}			
}
 

void SharedAlarmMonitor::updateSeqTime()
{
	TA_Base_Core::ThreadGuard guard( m_lock );
     m_unSeqErrorTimes++;	
}

 

void SharedAlarmMonitor::terminate()
{
	// set the exit monitor thread flag true
	m_terminate=true;
}

/*
* MonitorThreadsState
* 
* monitor all the locations/threads conform to the rules as below
* the rules as: normally all the alarm processing should completed
* within 4 minutes, for duty change and reload current alarms, however,
* the completed time adjust to 5 minutes
*
* @ ulLocationKey: the identifier of the location/thread
* @ ulTime: the begin time of the location/thread
*/
int SharedAlarmMonitor::MonitorThreadsState()
{
    TA_Base_Core::ThreadGuard guard( m_lock );
 

	return MonitorState();		
}

bool SharedAlarmMonitor::MonitorSeqError()
{	
	if (m_unSeqErrorTimes>=defSeqFailedTimes)
	{
		//::Sleep(1000*90);		 
		LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
					"SharedAlarmMonitor::MonitorSeqError(): times=%ld Exit the Process",m_unSeqErrorTimes );
		return true;
	}
	return false;

}


bool SharedAlarmMonitor::MonitorNormal(unsigned long location, LocationKeyItem* item)
{
	if (item->enableNormalCheck)
	{
		unsigned long curTime= time(NULL);
		long tDelta  =curTime-item->ulBeginTime;	
		long timeOut = item->ulTimeOut ;
		if (m_enableDutyCheck) 
		{
			timeOut=m_defDutyChangeTimeOut;
		}
		else if  (item->enableLoadCheck)
		{
			timeOut=item->ulLoadTimeOut;
		}
		if ( tDelta > timeOut )
		{		
			item->nTryTimes += 1;
			LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
				"SharedAlarmMonitor::MonitorNormal(): locationKey=%ld blocked, Delta=%ld ,Times=%d, timeout=%d",location, tDelta, item->nTryTimes,timeOut );
			if (item->nTryTimes >= defTryTimes)
			{	
				LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
					"SharedAlarmMonitor::MonitorNormal(): the LocationKey=%ld Exit the Process",location );
				//::Sleep(1000);
				//exit(EXIT_FAILURE);
				return true;
			}
		}
		else
		{
			item->nTryTimes=0;
		}
	}	
	return false;
}


bool SharedAlarmMonitor::MonitorDutyChange()
{
	if (m_enableDutyCheck)
	{
		unsigned long curTime= time(NULL);
		long tDelta  =curTime-m_ulDutyBeginTime;	
		long timeOut =  defDutyChangeTime ;
		if ( tDelta > timeOut )
		{		
			m_nTryDutyTimes += 1;
			LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
				"SharedAlarmMonitor::MonitorDutyChange" );
			if (m_nTryDutyTimes >= defTryTimes)
			{	
				LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
					"SharedAlarmMonitor::MonitorDutyChange Exit the Process");
				//::Sleep(1000);
				//exit(EXIT_FAILURE);
				return true;
			}
		}
		else
		{
			m_nTryDutyTimes=0;
		}
	}
	return false;
}

bool SharedAlarmMonitor::MonitorLoad(unsigned long location, LocationKeyItem* item)
{
	if (item->enableLoadCheck)
	{
		unsigned long curTime= time(NULL);
		long tDelta  =curTime-item->ulLoadBeginTime;	
		long timeOut = item->ulLoadTimeOut ;
		if (m_enableDutyCheck) 
		{
			timeOut=m_defDutyChangeTimeOut;
		}
		if ( tDelta > timeOut )
		{		
			item->nTryLoadTimes += 1;
			LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
				"SharedAlarmMonitor::MonitorLoad(): locationKey=%ld blocked, Delta=%ld ,Times=%d, timeout=%d",location, tDelta, item->nTryTimes,timeOut );
			if (item->nTryLoadTimes >= defTryTimes)
			{	
				LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
					"SharedAlarmMonitor::MonitorLoad(): the LocationKey=%ld Exit the Process",location );
				//::Sleep(1000);
				//exit(EXIT_FAILURE);
				return true;
			}
		}
		else
		{
			item->nTryLoadTimes=0;
		}
	}	
	return false;
}


 
int  SharedAlarmMonitor::MonitorState()
{
	bool isTimeOut=false;
	isTimeOut=MonitorDutyChange();
	if (isTimeOut)
	{
		return 1;
	}
	
	mapIterType mapIter = m_LocationKeyMap.begin();    
	while ( mapIter != m_LocationKeyMap.end() )
	{
		long tDelta=0;
		// check every exists items
		isTimeOut=MonitorNormal((*mapIter).first,(*mapIter).second);
		if (isTimeOut)
		{
			return 2;
		}
		isTimeOut=MonitorLoad((*mapIter).first,(*mapIter).second);
		if (isTimeOut)
		{
			return 3;
		}
		isTimeOut=MonitorSeqError();
		if (isTimeOut)
		{
			return 4;
		}
		
		// for sequence failed items
		long tSeqDelta   = (*mapIter).second->ulSeqEndTime - (*mapIter).second->ulSeqBeginTime;
		long tSeqTimeOut = (*mapIter).second->ulSeqFailedTimeOut;
		
		if ( tSeqDelta > tSeqTimeOut)
		{			
			
			LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
				"SharedAlarmMonitor::MonitorState(): locationKey=%ld failed, tSeqDelta=%ld ,Times=%d, timeout=%d",(*mapIter).first, tSeqDelta, (*mapIter).second->ulSeqFailedTimes,tSeqTimeOut );
			if ((*mapIter).second->ulSeqFailedTimes > defSeqFailedTimes)
			{	
				LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
					"SharedAlarmMonitor::MonitorState(): the LocationKey=%ld Exit the Process",(*mapIter).first );
				//::Sleep(1000);
				//exit(EXIT_FAILURE);			 
				return 3;
				 
			}
		}
		// next item
		mapIter++;
	}
	return 0;
}

 