// AlarmStoreCmdWatcher.cpp: implementation of the AlarmStoreCmdWatcher class.
//
//////////////////////////////////////////////////////////////////////

#include "AlarmStoreCmdWatcher.h"
#include "AlarmStoreStatusChecker.h"

#include <time.h>
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

// #ifdef _DEBUG
// #undef THIS_FILE
// static char THIS_FILE[]=__FILE__;
// #define new DEBUG_NEW
// #endif

#define HEART_BEAT_STOP_LIMIT		3

using TA_Base_Core::DebugUtil;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

std::string AlarmStoreCmdWatcher::ServiceName("Alarm Store Service");
AlarmStoreCmdWatcher::AlarmStoreCmdWatcher()
:m_preHeartBeat(0), m_currentHearBeat(0), m_HeartBeatStopCnt(0)
{
	m_HeartBeatStopLimit = HEART_BEAT_STOP_LIMIT;	
	createStatusChecker();

	start();
}

AlarmStoreCmdWatcher::~AlarmStoreCmdWatcher()
{
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "AlarmStoreCmdWatcher destruction!" );
}

void AlarmStoreCmdWatcher::createStatusChecker()
{
	StatusChecker* pStatusChecker = new AlarmStoreStatusChecker();
	pStatusChecker->addObserver(this);
	addCheckerMap(pStatusChecker);

	// set AlarmStoreStatusChecker parameter
	unsigned long ulTmp = 60000;
	std::string tmp = TA_Base_Core::RunParams::getInstance().get("AlarmStoreCheckGap");
	if ( !tmp.empty() )
	{
		ulTmp = atol( tmp.c_str() );
	}
	unsigned long ulTmpTry = 3;
	tmp = TA_Base_Core::RunParams::getInstance().get("AlarmStoreStopLimit");
	if ( !tmp.empty() )	
	{
		ulTmpTry = atol( tmp.c_str() );	
	}

	if ( ulTmp < 20000 || ulTmp > 60000 )	// between 20s and 180s 
	{
		ulTmp = 60000;	// check interval
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			"createStatusChecker RunParam AlarmStoreCheckGap should between 20000 and 180000, set to 60000" );
	}

	if ( (ulTmpTry * ulTmp) > 300000 || (ulTmpTry * ulTmp) < 180000 )
	{
		ulTmpTry = 180000 / ulTmp;	// error notify limit
	}
	pStatusChecker->setCheckGap(ulTmp);
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"createStatusChecker RunParam AlarmStoreCheckGap=%d", ulTmp );
	pStatusChecker->setErrorNotifyCountLimit(ulTmpTry);
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"createStatusChecker RunParam AlarmStoreStopLimit=%d", ulTmpTry );


	// set AlarmStoreWatcher parameter
	unsigned long ulTmpWatcher = 60000;
	tmp = TA_Base_Core::RunParams::getInstance().get("AlarmDogHeartBeatGap");
	if ( !tmp.empty() )
	{
		ulTmpWatcher = atol( tmp.c_str() );
	}
	if ( ulTmpWatcher < ulTmp )	// AlarmDogHeartBeatGap >= AlarmStoreCheckGap
	{
		ulTmpWatcher = ulTmp;
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			"Construct AlarmStoreWatcher RunParam AlarmDogHeartBeatGap should >= AlarmStoreCheckGap" );
	}
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"Construct AlarmStoreWatcher RunParam AlarmDogHeartBeatGap=%d", ulTmpWatcher );
	setHeartBeatGap(ulTmpWatcher);
	
	unsigned long ulTmpWatcherTry = 3;
	tmp = TA_Base_Core::RunParams::getInstance().get("AlarmDogStopLimit");
	if ( !tmp.empty() )
	{
		ulTmpWatcherTry = atoi(tmp.c_str());
	}
	if ( ulTmpWatcherTry <= ulTmpTry )
	{
		ulTmpWatcher = ulTmp;
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			"Construct AlarmStoreWatcher RunParam AlarmDogStopLimit should > AlarmStoreStopLimit" );
	}

	m_HeartBeatStopLimit = ulTmpWatcherTry;
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"Construct AlarmStoreWatcher RunParam AlarmDogStopLimit=%d", m_HeartBeatStopLimit );
	m_HeartBeatStopCnt = 0;

	pStatusChecker->start();
}

void AlarmStoreCmdWatcher::onStatusChanged( const ObserverData& currentStatus )
{
	StatusChecker* pChecker = reinterpret_cast<StatusChecker*>(currentStatus.first);
	restartService(ServiceName.c_str(), pChecker);
	LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
		"onStatusChanged restart:%s, errCnt:%lu, errStatusCnt:%lu, checkCnt:%lu",
		ServiceName.c_str(), pChecker->getErrorCount(), 
		pChecker->getErrorStatusCount(), pChecker->getCheckCount() );
}

/**
 * Implement restart AlarmStore Service when watch dog heart beat stop
 *	make sure:
 *	1: watch dog is running, isPaused() == false
 *	2: Alarm Store Service connection has established, isConnect() == true
 *	3: Successive heart beat stop count is larger than StopLimit
 */
void AlarmStoreCmdWatcher::heartBeatCall( const StatusChecker* pChecker )
{
	m_preHeartBeat = m_currentHearBeat;
	m_currentHearBeat = pChecker->heartBeat();

	struct tm hb = *localtime(&m_currentHearBeat);
	LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
		"heartBeatCall(%02d:%02d:%02d) [%s] errCnt:%lu, successiveErrorCnt:%lu, errStatusCnt:%lu, checkCnt:%lu, heartBeatStopCnt:%d",
		hb.tm_hour, hb.tm_min, hb.tm_sec,
		ServiceName.c_str(), pChecker->getErrorCount(), pChecker->getSuccessiveErrorCount(),
		pChecker->getErrorStatusCount(), pChecker->getCheckCount(),
		m_HeartBeatStopCnt );
		
	// check heart beat stop for dogs when it is not paused
	if ( pChecker->isPaused() )
	{
		m_HeartBeatStopCnt = 0;
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
			"heartBeatCall Paused" );
	}
	else if ( !pChecker->isConnect() )
	{
		m_HeartBeatStopCnt = 0;
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
			"heartBeatCall Disconnected" );
	}
	else if ( m_currentHearBeat == m_preHeartBeat )
	{
		++m_HeartBeatStopCnt;
		if ( m_HeartBeatStopCnt >= m_HeartBeatStopLimit )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
				"heartBeatCall Restart:%s, m_HeartBeatStopCnt:%d, errCnt:%lu, errStatusCnt:%lu, checkCnt:%lu",
				ServiceName.c_str(), m_HeartBeatStopCnt, pChecker->getErrorCount(), 
				pChecker->getErrorStatusCount(), pChecker->getCheckCount() );
			m_HeartBeatStopCnt = 0;
			restartStatusChecker( pChecker );
		}
	}
	else
	{
		m_HeartBeatStopCnt = 0;
	}
}