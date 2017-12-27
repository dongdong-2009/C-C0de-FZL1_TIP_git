// AlarmStoreStatusChecker.cpp: implementation of the AlarmStoreStatusChecker class.
//
//////////////////////////////////////////////////////////////////////

#include "AlarmStoreStatusChecker.h"

#include "core/process_management/src/UtilityInitialiser.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/alarm/IDL/src/AlarmStoreCorbaDef.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/ObjectResolutionException.h"

#include <iostream>
#include <sstream>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using TA_Base_Core::DebugUtil;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AlarmStoreStatusChecker::AlarmStoreStatusChecker()
	:lCurrentTime(0), IPreviseTime(0), 
	m_strAddr("corbaloc::localhost:"),
	m_port(TA_Base_Core::IAlarmStoreCorbaDef::DEFAULT_ALARM_STORE_PORT),
	m_strName(TA_Base_Core::IAlarmStoreCorbaDef::DEFAULT_ALARM_STORE_NAME),
	m_bIsConnect(false)
{
//	char * argv[] = {""};
//	TA_Base_Core::UtilityInitialiser::initialiseUtilities(0, argv);
	resolveAlarmStore();
}
AlarmStoreStatusChecker::~AlarmStoreStatusChecker()
{
	// m_alarmStore var do not need artificially destruct
	// thread will stop when StatucChecker destruct
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "AlarmStoreStatusChecker destruction!" );
}

bool AlarmStoreStatusChecker::resolveAlarmStore()
{
	std::ostringstream ior;
	ior << m_strAddr;
	ior << m_port;
	ior << "/" << m_strName;

	m_IOR = ior.str();

	int nTryCount = 0;
	
	while( nTryCount < 4 )
	{
		try
		{	
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Resolving Alarm Store - Attempt %d", ++nTryCount );
			
			CORBA::Object_var obj = TA_Base_Core::CorbaUtil::getInstance().stringToObject( m_IOR );
			if ( true == CORBA::is_nil(obj) )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Failed to retrieve Corba object with IOR %s",
					m_IOR.c_str() );
				continue;
			}

			if ( !CORBA::is_nil(m_alarmStore) )
			{
				m_alarmStore = NULL;
			}
			
			m_alarmStore = TA_Base_Core::IAlarmStoreCorbaDef::_narrow( obj );
			if ( true == CORBA::is_nil(m_alarmStore) )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Failed to narrow obj into IAlarmStoreCorbaDef" );
				continue;
			}
			
			// Calls were successful so the alarm store is now valid
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Alarm Store Connected.");
			break;
		}
		catch( const TA_Base_Core::ObjectResolutionException& ore )
		{
			m_alarmStore = NULL;
			std::ostringstream desc;
			desc << "Caught ObjectResolutionException while resolving AlarmStore with IOR "
				<< m_IOR << "; Reason: " << ore.what();
			LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", desc.str().c_str() );
		}
		catch( const TA_Base_Core::IAlarmStoreCorbaDef::InvalidFilterException& ife )
		{
			//TODO: How to get this back out to the calling class
			m_alarmStore = NULL;
			std::ostringstream desc;
			desc << "Filter specified is invalid; Reason: " << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ife);
			LOG_EXCEPTION_CATCH( SourceInfo, "InvalidFilterException", desc.str().c_str() );
		}
		catch ( const CORBA::SystemException& se )
		{
			m_alarmStore = NULL;
			std::ostringstream desc;
			desc << "Caught CORBA::SystemException resolving AlarmStore with IOR "
				<< m_IOR << "; Reason: " 
				<< TA_Base_Core::CorbaUtil::getInstance().exceptionToString(se);
			LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::SystemException", desc.str().c_str() );
		}
		catch ( const std::exception& e )
		{
			m_alarmStore = NULL;
			std::ostringstream desc;
			desc << "Caught std::exception resolving AlarmStore with IOR "
				<< m_IOR << "; Reason: " << e.what();
			LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", desc.str().c_str() );
		}
		catch( ... )
		{
			m_alarmStore = NULL;
			// Catch all exception to prevent thread from pre-maturely terminating
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Caught unknown exception in AlarmStoreMonitorImpl::run()!" );
		}
	}// end while
	if ( nTryCount >= 4 )
	{
		return false;
	}
	else
	{
		return true;
	}
}

long AlarmStoreStatusChecker::checkMethod()
{
	bool bCheckRet = false;
	// no connect, return 

	// check Alarm Store
	try
	{
		if ( !connect() )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Alarm Store is disconnectted, disable notify action." );
			return IPreviseTime;
		}
		IPreviseTime = lCurrentTime;
		lCurrentTime = m_alarmStore->getAliveTimeStamp();
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "AlarmStoreServant Heart beat:%d", lCurrentTime );
	}
	catch (CORBA::Exception& ce)
	{
		bCheckRet = true;
		LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", 
			TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ce).c_str());
		
		// Start trying to reconnect
// 		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Signalling m_resolveAlarmStore..." );
//		resolveAlarmStore();
	}
	catch (const std::exception& e)
	{
		bCheckRet = true;
		std::ostringstream desc;
		desc << "Caught std::exception in generateAlarmCache; Reason: " << e.what();
		LOG_EXCEPTION_CATCH(SourceInfo, "std::exception", desc.str().c_str());
		
		// Start trying to reconnect
// 		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Signalling m_resolveAlarmStore..." );
//		resolveAlarmStore();
	}
	catch(...)
	{
		bCheckRet = true;
		LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Caught unknown exception in AlarmStoreStatusChecker::checkMethod");
		
		// Start trying to reconnect
// 		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Signalling m_resolveAlarmStore..." );
//		resolveAlarmStore();
	}
	// Alarm Store Time stamp stopped
	try
	{
		if ( lCurrentTime == IPreviseTime )
		{
			bCheckRet = true;
		}
		if (bCheckRet)
		{
			notifyObservers();
			bCheckRet = false;
		}
		else
		{
			cleanSuccessiveError();
		}
	}
	catch(...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Caught unknown exception in AlarmStoreStatusChecker::checkMethod when notifyObservers");
	}
	return lCurrentTime;
}

bool AlarmStoreStatusChecker::connect()
{
	if ( !m_bIsConnect )
	{
		m_bIsConnect = resolveAlarmStore();
	}
	return m_bIsConnect;
}