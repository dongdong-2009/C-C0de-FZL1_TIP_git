// WatchedModule.cpp: implementation of the WatchedModule class.
//
//////////////////////////////////////////////////////////////////////

#include "IStatusObserver.h"
#include "WatchedModule.h"
#include "core/utilities/src/DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define TIMER_STOP_SERVICE		0
#define TIMER_START_SERVICE		1


static bool stopService( const char* serviceName ) 
{
	bool sign = false;
	std::string commandLine;
	
	commandLine = "net stop \"";
	commandLine.append( serviceName );
	commandLine.append( "\"" );
	
	if( WinExec(commandLine.c_str(), SW_HIDE) )
	{
		sign = true;
	}
	
	return sign;
}

//start services
static bool startService( const char* serviceName ) 
{
	bool sign = false;
	std::string commandLine;
	
	commandLine = "net start \"";
	commandLine.append( serviceName );
	commandLine.append( "\"" );
	
	if( WinExec(commandLine.c_str(), SW_HIDE) )
	{
		sign = true;
	}
	
	return sign;
}

static int ServiceQueryStatus(const std::string& serviceName)
{
	int ret = -1;
	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	SC_HANDLE schService = OpenService( 
		schSCManager,				// SCM database 
		serviceName.c_str(),        // service name
		SERVICE_QUERY_STATUS); 
	
	SERVICE_STATUS status;
	if(QueryServiceStatus(schService, &status) != 0)
	{
		ret = status.dwCurrentState;
	}
	
	CloseServiceHandle(schSCManager);
	
	return ret;
}

WatchedModule::WatchedModule():m_bTerminate(false), m_nHearBeatGap(10000), m_bRestartService(false)
{
	
}

WatchedModule::~WatchedModule()
{
	TA_Base_Core::ThreadGuard guard(m_mapLock);
	StatusChecker* pChecker = NULL;
	std::map<unsigned long, bool>::iterator itMap = m_statusCheckerMap.begin();
	for( ; itMap != m_statusCheckerMap.end(); ++itMap )
	{
		pChecker = reinterpret_cast<StatusChecker*>(itMap->first);
		if ( pChecker != NULL )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				" clean StatusChecker [%p]", pChecker);
			delete pChecker;
			pChecker = NULL;
		}
	}
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
	" WatchedModule destruction!!!" );

}

void WatchedModule::stopAll()
{
	terminateAndWait();
	TA_Base_Core::ThreadGuard guard(m_mapLock);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		" WatchedModule::stopAll() terminateAndWait() returned, m_statusCheckerMap size:%d",
		m_statusCheckerMap.size() );
	StatusChecker* pChecker = NULL;
	std::map<unsigned long, bool>::iterator itMap = m_statusCheckerMap.begin();
	for( ; itMap != m_statusCheckerMap.end(); ++itMap )
	{
		pChecker = reinterpret_cast<StatusChecker*>(itMap->first);
		if ( pChecker != NULL )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				" stoping StatusChecker [%s]", pChecker->getName().c_str() );
			pChecker->terminateAndWait();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"---StatusChecker [%s] terminateAndWait() function returned", pChecker->getName().c_str() );
		}
	}
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		" WatchedModule::stopAll() finished" );
}

void WatchedModule::onStatusChanged(const ObserverData& obData)
{
	m_statusCheckerMap[obData.first] = obData.second;
}

void WatchedModule::run()
{
	try
	{
		while( !m_bTerminate )
		{
			try
			{
				if ( m_bRestartService )	// some StatusChecker request restart service
				{
					m_bRestartService = false;
					pauseAllChecker();
// 					HANDLE hServer = getProcessHandle(getServiceName());
					stopService(getServiceName().c_str());
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
						"Stop Service [%s]", getServiceName().c_str());
					while ( !m_bTerminate && ServiceQueryStatus(getServiceName()) != SERVICE_STOPPED )
					{
						sleep(3000);
					}
// 					WaitForSingleObject( hServer, INFINITE );
// 					CloseHandle( hServer );
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
						"Service [%s] STOPPED", getServiceName().c_str());
					startService(getServiceName().c_str());
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
						"Start Service [%s]", getServiceName().c_str());
					while ( !m_bTerminate && ServiceQueryStatus(getServiceName()) != SERVICE_RUNNING )
					{
						sleep(3000);
					}
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
							"Service [%s] RUNNING", getServiceName().c_str());
					m_bRestartService = false;
					if ( !m_bTerminate )
					{
						continueAllChecker();
					}
				}
				else	// check heart beat of StatusChecker
				{
					std::map<unsigned long, bool> tmpCheckMap;
					{
						TA_Base_Core::ThreadGuard guard(m_mapLock);
						tmpCheckMap = m_statusCheckerMap;
					}
					std::map<unsigned long, bool>::iterator itMap;
					itMap = tmpCheckMap.begin();
						for(; itMap != tmpCheckMap.end(); ++itMap )
						{
							heartBeatCall(reinterpret_cast<StatusChecker*>(itMap->first));
						}
				}
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
					"heartBeatCall raise exception!!!");
			}
			
			//TA_Base_Core::Thread::sleep(m_nHearBeatGap);
			conditionWait(m_nHearBeatGap);
		}
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			"WatchedModule thread go exception!!!");
	}
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"WatchedModule[%p] thread run() finished!", this );
}

void WatchedModule::terminate()
{
	m_bTerminate = true;
//	m_nHearBeatGap = 0;
	stopWait();
}

void WatchedModule::restartService( const char* serviceName, StatusChecker* pChecker )
{
// 	m_RestartThread.AddJob(serviceName, stopTime, startTime, pChecker);
// 	m_RestartThread.signal();
// 	pChecker->restartServer(serviceName);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
		"%s Notify WatchedModule restart %s!", 
		pChecker->getName().c_str(), getServiceName().c_str() );
	m_bRestartService = true;
	stopWait();
}
void WatchedModule::restartStatusChecker( const StatusChecker* pChecker )
{
	std::map<unsigned long, bool>::iterator itMap = m_statusCheckerMap.find(pChecker->getIdentify());
	if ( itMap == m_statusCheckerMap.end() )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"restartStatusChecker can not find StatusChecker in map!!!" );
	}
	else
	{
		StatusChecker* p = reinterpret_cast<StatusChecker*>(itMap->first);
		if ( p != NULL )
		{
			p->terminateAndWait();
			p->start();
		}
	}
}

void WatchedModule::pauseAllChecker()
{
	TA_Base_Core::ThreadGuard guard(m_mapLock);
	std::map<unsigned long, bool>::iterator itChecker = m_statusCheckerMap.begin();
	StatusChecker* pChecker = NULL;
	for ( ; itChecker != m_statusCheckerMap.end(); ++itChecker )
	{
		pChecker = reinterpret_cast<StatusChecker*>(itChecker->first);
		if ( pChecker != NULL )
		{
			pChecker->pause();
//			pChecker->disconnect();	//	connect and disconnect done by status checker thread
//			pChecker->cleanStatus();
		}
	}
}

void WatchedModule::continueAllChecker()
{
	TA_Base_Core::ThreadGuard guard(m_mapLock);
	std::map<unsigned long, bool>::iterator itChecker = m_statusCheckerMap.begin();
	StatusChecker* pChecker = NULL;
	for ( ; itChecker != m_statusCheckerMap.end(); ++itChecker )
	{
		pChecker = reinterpret_cast<StatusChecker*>(itChecker->first);
		if ( pChecker != NULL )
		{
//			pChecker->connect(); //	connect and disconnect done by status checker thread
			pChecker->continues();
		}
	}
}
