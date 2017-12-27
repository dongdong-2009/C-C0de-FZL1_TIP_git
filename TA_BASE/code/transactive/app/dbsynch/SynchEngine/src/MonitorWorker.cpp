#include "MonitorWorker.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "ConnectionActorReleaser.h"
#include "WorkTime.h"


NS_BEGIN(TA_DB_Sync)


using  TA_Base_Core::DebugUtil;

ReEntrantThreadLockable CMonitorWorker::m_singletonLock;
CMonitorWorker* CMonitorWorker::m_theClassInstance = NULL;

CMonitorWorker* CMonitorWorker::getInstance()
{
	TA_Base_Core::ThreadGuard guard(m_singletonLock);
	// If guard acquired then make sure the singleton is still NULL
	if ( m_theClassInstance == NULL )
	{
		// Create the one & only object
		m_theClassInstance = new CMonitorWorker();
		m_theClassInstance->start();
	}

	return  m_theClassInstance;
}

CMonitorWorker::CMonitorWorker(void)
{
	m_toTerminate = false;
}

CMonitorWorker::~CMonitorWorker(void)
{
}

void CMonitorWorker::terminate()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CMonitorWorker::terminate()");

	m_toTerminate = true;
}

void CMonitorWorker::run()
{
   LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CMonitorWorker begin run");

   while (m_toTerminate==false)
   {	
	   _MonitorThreads();
	   Thread::sleep(1000);
   }
   LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CMonitorWorker begin run");

}

void CMonitorWorker::_MonitorThreads()
{
	WorkTimeInfo workTimeInfoGet;

	ConnectionActorReleaser::getInstance()->getWorkTimeInfo(workTimeInfoGet);

	//Monitor releaseActor time
	//The oracle client API may hang if disconnect network while doing a select/update/enqueue/dequeue,
	//so add this monitor mechanism to recovery.
	//If this thread found that the release of connctionactor spent more than 5 minutes(a parameter, 'releaseactortimeout', in config file), the thread will stop
	//the whole application, then the monitor shell script will start a new one automatically.
	if (workTimeInfoGet.m_nReleaseActorNotWork > g_system_variables.m_nReleaseActorTimeOut)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "ReleaseActorNotWork %llu > %d, ClientID=%s thread hang SynchEngine must Exit",
			workTimeInfoGet.m_nReleaseActorNotWork, g_system_variables.m_nReleaseActorTimeOut, workTimeInfoGet.m_strClientID.c_str());  
		TA_Base_Core::Thread::sleep(TIME_MIN_FIVE);
		exit(Ret_Success);	
	}
}

NS_END(TA_DB_Sync)