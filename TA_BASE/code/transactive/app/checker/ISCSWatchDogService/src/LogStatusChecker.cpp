

#include "LogStatusChecker.h"

using TA_Base_Core::DebugUtil;

bool LogStatusChecker::connect()
{
	// connect called, means restart service has finish
	m_logAnalyser.resetProcessID();
	m_logAnalyser.reAnalyze();
	m_bIsConnect = true;
	return m_bIsConnect;
}

void LogStatusChecker::disconnect()
{
	// when dissonnect call, means server will stop immediately
	// do nothing
	m_bIsConnect = false;
}

// before enter run() loop in StatusChecker, init() call
void LogStatusChecker::init()
{
	m_logAnalyser.resetProcessID();
	m_logAnalyser.reAnalyze();
}

long LogStatusChecker::checkMethod()
{
	if ( !isConnect() )
	{
		connect();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			" LogStatusChecker disconnected, ignore check action!");
		return 0;
	}
	unsigned long ulKeyCnt = 0;
	try
	{
		std::map<std::string, unsigned long> tmpMap;
		bool bRet = false;
		{
			TA_Base_Core::ThreadGuard guard( m_LockKeyMap );
			tmpMap = m_KeyLimitMap;
		}
		for ( std::map<std::string, unsigned long>::iterator itMap = tmpMap.begin();
		itMap != tmpMap.end(); ++itMap )
		{
			ulKeyCnt = m_logAnalyser.getKeyCount(itMap->first);
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Key:[%s] count:[%lu]",
				itMap->first.c_str(), ulKeyCnt );
			if ( ulKeyCnt > itMap->second )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Key:[%s] count:[%lu] comes to limit, Notify Watcher",
					itMap->first.c_str(), ulKeyCnt );
				bRet = true;
				break;
			}
		}
		if ( bRet )
		{
			notifyObservers();
		}
		else
		{
			cleanSuccessiveError();
		}
	}
	catch(...)
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "LogStatusChecker checkMethod raise execption!!!" );
	}
	return ulKeyCnt;
}
