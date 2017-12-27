// OpcBridgeWatcher.cpp: implementation of the OpcBridgeWatcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OpcBridgeWatcher.h"
#include "OpcBridgeStatusChecker.h"
#include "core/utilities/src/DebugUtil.h"

#include "LogStatusChecker.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using TA_Base_Core::DebugUtil;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OpcBridgeWatcher::OpcBridgeWatcher()
{
	m_nHeartBeat = 0;
	m_nHeartBeatOld = 0;
	
	createStatusChecker();
	createLogStatusChecker();
	start();
}

OpcBridgeWatcher::~OpcBridgeWatcher()
{
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "OpcBridgeWatcher destruction!" );
}

void OpcBridgeWatcher::heartBeatCall( const StatusChecker* pChecker )
{	
	// TODO: Add your message handler code here and/or call default
	if ( pChecker->getName().compare("OpcBridgeStatusChecker") == 0 )
	{
		m_nHeartBeatOld = m_nHeartBeat;
		m_nHeartBeat = pChecker->heartBeat();
	}
}

void OpcBridgeWatcher::onStatusChanged( const ObserverData& currentStatus )
{
	StatusChecker *pChecker = reinterpret_cast<StatusChecker*>(currentStatus.first);
	if ( pChecker != NULL && pChecker->getName().compare("OpcLogStatusChecker") == 0 )
	{
		restartService( "Opc Bridge", pChecker );
	}
	else if( m_nHeartBeat!=m_nHeartBeatOld )
	{
//		StatusChecker* pChecker = reinterpret_cast<StatusChecker*>(currentStatus.first);
		static int number = 0;
		static int dayOld = 0;
		
		time_t currentTime;
		time( &currentTime );
		struct tm hb = *gmtime( &currentTime );
		
		if( (number++)<RESTARTLIMITNUMBER )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "restart Opc Bridge" );
			
			pChecker->pause();
			restartService( "Opc Bridge", pChecker );
		}
		
		if( number>=10000 )
		{
			number = RESTARTLIMITNUMBER;
		}
		
		if( dayOld!=hb.tm_mday )
		{
			number = 0;
		}
		
		dayOld = hb.tm_mday;
	}
}

void OpcBridgeWatcher::createStatusChecker()
{
	m_pStatusChecker = new OpcBridgeStatusChecker;
	m_pStatusChecker->addObserver( this );
	addCheckerMap( m_pStatusChecker );
	m_pStatusChecker->setCheckGap( 10000 );
	m_pStatusChecker->start();
}

void OpcBridgeWatcher::createLogStatusChecker()
{
	// create LogStatusChecker with ExeFileName and Log file pathname
	LogStatusChecker* pLogChecker = new LogStatusChecker(
		"TA_OpcBridgeService.exe",
		"C:\\transactive\\logs\\Log_OpcBridge" );
	pLogChecker->addObserver( this );
	addCheckerMap( pLogChecker );

	// LogStatusChecker can be used by anyone, so specify its name
	pLogChecker->setName("OpcLogStatusChecker");

	// add search key and specify compare function if you need special method
	pLogChecker->addKey( 
		"as no subscribers are interested, firstEvent is <{(domain_name,", // basic match key
		10, // key match notify limit count
		findUnPrintableASCIICharacter // compare function
		);
	// set LogAnalyser refresh directory time interval
	pLogChecker->setFileRefreshInterval( 3000 );
	// set StatusChecker interval
	pLogChecker->setCheckGap( 30000 );
	pLogChecker->start();
}
