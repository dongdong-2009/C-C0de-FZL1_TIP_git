// StatusChecker.cpp: implementation of the StatusChecker class.
//
//////////////////////////////////////////////////////////////////////


#include "IStatusStrategy.h"
#include "IStatusObserver.h"
#include "StatusChecker.h"
#include "core/utilities/src/DebugUtil.h"

#include <ace/OS.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StatusChecker::StatusChecker()
	:m_nErrorStatusCount(0),
	m_nSuccessiveErrorCount(0),
	m_nNotifyErrorCountLimit(BEARABLE_MAX_ERROR_COUNT),
	m_nCurrentErrorCount(0),
	m_bTerminate(false),
	m_HeartBeat(0),
	m_CheckGap(10000),
	m_bPause(false),
	m_CheckCount(0),
	m_bRestartService(false)
{
	m_id = reinterpret_cast<unsigned long>(this);	
}

StatusChecker::~StatusChecker()
{
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"%p StatusChecker destructed!!", this );
}

void StatusChecker::pause()
{
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"++++++Base::StatusChecker pause() called!!!!!!!" );
	m_bPause = true;
	stopWait();
}

void StatusChecker::continues()
{
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"++++++Base::StatusChecker continues() called!!!!!!!" );
	m_bPause = false;
	stopWait();
}

void StatusChecker::terminate()
{
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"++++++Base::StatusChecker terminate() called!!!!!!!" );
	m_bTerminate = true;
	stopWait();
}


/**
 * notifyObservers
 *
 * This function has a logical to judge whether watched object 
 *	error count has reach BEARABLE_MAX_ERROR_COUNT. Only its
 *	error count greater, it will call Observer->onStatusChanged() method
 */
void StatusChecker::notifyObservers()
{
	++m_nCurrentErrorCount;
	if ( !shouldNotifyObserver() )
	{
		return;
	}
	++m_nErrorStatusCount;
	std::set<IStatusObserver*>::iterator it;
	for(it = m_statusObserver.begin(); it != m_statusObserver.end(); ++it)
	{
		IStatusObserver * observer = *it;
		observer->onStatusChanged(std::make_pair(m_id, false));
	}
}

void StatusChecker::addObserver( IStatusObserver * statusObserver )
{
	m_statusObserver.insert(statusObserver);
}

/**
 * run
 *
 * Two responsibility
 *	1: call watched object status check interface 
 *	2: send heart beat signal
 */
void StatusChecker::run()
{
	try
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			" *** StatusChecker [%s] run() start!!!", getName().c_str() );
		init();
		while( !m_bTerminate )
		{
			// set heart beat;
			ACE_OS::time(&m_HeartBeat);

			if (!m_bPause)
			{
				// check
				++m_CheckCount;
				try
				{
					checkMethod();
				}
				catch(...)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
						"checkMethod() [%s] raise exception, checkCnt:%lu", getName().c_str(), m_CheckCount);
				}
			}
			else
			{
				disconnect();	// disconnect() done by status checker itself
				cleanStatus();
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"%s is pausing...", getName().c_str() );
				conditionWait(3600000);	// wait until continues called
				if ( !m_bTerminate )
				{
					connect();
				}
				continue;
			}
			conditionWait(m_CheckGap);
		}
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			"StatusChecker [%s] thread get exception!!!", getName().c_str() );
	}
	disconnect();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"StatusChecker[%s] thread run() finished", getName().c_str() );	
}

bool StatusChecker::shouldNotifyObserver()
{
	++m_nSuccessiveErrorCount;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"shouldNotifyObserver CurrentErrorCnt:%d, SuccessiveErrorCnt:%d, ErrorCntLimit:%d",
		m_nCurrentErrorCount, m_nSuccessiveErrorCount, m_nNotifyErrorCountLimit);
	if ( m_nSuccessiveErrorCount % m_nNotifyErrorCountLimit == 0 )
	{
		return true;
	}
	return false;
}
