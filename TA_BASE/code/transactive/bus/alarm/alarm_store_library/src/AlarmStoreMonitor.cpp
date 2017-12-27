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
//  AlarmStoreMonitor.cpp
//  Implementation of the Class AlarmStoreMonitor
//  Created on:      19-Jan-2011 10:41:03 AM
///////////////////////////////////////////////////////////

#include <time.h>
#include "AlarmStoreMonitor.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
//#include "SharedAlarmAccessFactory.h"
#include "AlarmAccessAdaptor.h"

using namespace TA_Base_Bus;
using TA_Base_Core::DebugUtil;

#define  defMonitorInterval       10*1000      // unit: million seconds

AlarmStoreMonitor* AlarmStoreMonitor::m_pAlarmStoreMonitor = NULL;
TA_Base_Core::NonReEntrantThreadLockable AlarmStoreMonitor::m_singletonLock;

/**
 * constructor & destructor
 * 
 */

AlarmStoreMonitor::AlarmStoreMonitor()
{	 
	m_terminate=false;
}

AlarmStoreMonitor::~AlarmStoreMonitor()
{
 
}

AlarmStoreMonitor& AlarmStoreMonitor::getInstance()
{
	TA_Base_Core::ThreadGuard guard( m_singletonLock );

	if ( NULL == m_pAlarmStoreMonitor )
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,
			"AlarmStoreMonitor::getInstance():AlarmStoreMonitor started" );
		m_pAlarmStoreMonitor = new AlarmStoreMonitor();
	}

	return (*m_pAlarmStoreMonitor);
}

void AlarmStoreMonitor::removeInstance()
{
	TA_Base_Core::ThreadGuard guard( m_singletonLock );

	if ( NULL != m_pAlarmStoreMonitor )
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,
			"AlarmStoreMonitor::getInstance():AlarmStoreMonitor stopped" );
        m_pAlarmStoreMonitor->terminateAndWait();
		delete m_pAlarmStoreMonitor;
		m_pAlarmStoreMonitor = 0;
	}
}

void AlarmStoreMonitor::run()
{
	// to start monitor all the locations/thread state, run till the 
	// application exit
	while (!m_terminate)
	{
		int r=AlarmAccessAdaptor::getInstance().healthMonitor();
//		int r=SharedAlarmAccessFactory::getInstance().healthMonitor();
		if (r>0)
		{
			LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
			"AlarmStoreMonitor::run check timeout reason=%d", r);
			if (r==4)   //update or insert error
			{
				::Sleep(1000*10);
			}
			else  //timeout error
			{			
				::Sleep(1000);
			}
			exit(EXIT_FAILURE);
		}
		// sleep 10 seconds for each turn
		::Sleep(defMonitorInterval);
	}
}

void AlarmStoreMonitor::terminate()
{
	// set the exit monitor thread flag true
	m_terminate=true;
}

 
 











