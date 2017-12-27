#include "core/synchronisation/src/ThreadGuard.h"
#include "ConnectionActorReleaser.h"
#include "WorkTime.h"


NS_BEGIN(TA_DB_Sync)


using  TA_Base_Core::DebugUtil;

ReEntrantThreadLockable ConnectionActorReleaser::m_singletonLock;
ConnectionActorReleaser* ConnectionActorReleaser::m_theClassInstance = NULL;

ConnectionActorReleaser* ConnectionActorReleaser::getInstance()
{
	TA_Base_Core::ThreadGuard guard(m_singletonLock);
	// If guard acquired then make sure the singleton is still NULL
	if ( m_theClassInstance == NULL )
	{
		// Create the one & only object
		m_theClassInstance = new ConnectionActorReleaser();
		m_theClassInstance->start();
	}

	return  m_theClassInstance;
}

ConnectionActorReleaser::ConnectionActorReleaser(void)
{
	m_toTerminate = false;
	m_pMonitorReleaseActorWorkTime = new CWorkTimeLock(PROPAGATE_TIMER_LOG_MonitorReleaseActor);
}

ConnectionActorReleaser::~ConnectionActorReleaser(void)
{
	ThreadGuard guardTimer(m_monitorTimerLock);
	DEF_DELETE(m_pMonitorReleaseActorWorkTime);
}

void ConnectionActorReleaser::push(TcpConnectionActor* invaildActor)
{
	ThreadGuard guard(m_datalock);
    m_invaildActors.push_back(invaildActor);
	unsigned long  count=m_invaildActors.size();
	
	if (count>SERVER_LIMIT_INVAILD_CLIENT_COUNT_MAX)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "push invalid actor into release list, total count=%d",
			m_invaildActors.size());
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "push invalid actor into release list, total count=%d",
			m_invaildActors.size());
	}
}
void ConnectionActorReleaser::terminate()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ConnectionActorReleaser::terminate()");

	m_toTerminate = true;
}

void ConnectionActorReleaser::run()
{
   LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ConnectionActorReleaser begin run");

   while (m_toTerminate==false)
   {	
	   _releaseInvaildActors();
	   Thread::sleep(100);
   }
   LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ConnectionActorReleaser end run");

}

void ConnectionActorReleaser::_releaseInvaildActors()
{
	 vecAcotrT  releaselist;
	 vecActorIterT iter;
	 TcpConnectionActor* actor;
	 std::string	strClientAgtNameGet;
	 unsigned long  nIDGet = 0; 

	{	 
		//copy to a temporary list
		ThreadGuard guard(m_datalock);
		iter=m_invaildActors.begin();
		while (iter!=m_invaildActors.end())
		{
			actor=*iter;
			releaselist.push_back(actor);
			iter++;
		}
		m_invaildActors.clear();
	}
	
	iter=releaselist.begin();
	while (iter!=releaselist.end())
	{
		actor=*iter;
        
		m_strClientID = actor->getClientID();
		strClientAgtNameGet = actor->getClientAgentName();
		nIDGet = actor->getID();	

		//don't need to sleep here
		//TA_Base_Core::Thread::sleep(TIME_BASE_SECOND);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"begin free one server ClientID=%s, ClientAgtName=%s, ID=%lu",
			m_strClientID.c_str(), strClientAgtNameGet.c_str(), nIDGet);
		//begin
		{
			ThreadGuard guardTimer(m_monitorTimerLock);
			m_pMonitorReleaseActorWorkTime->workBegin();
		}		
		actor->serverstopWork(CurrentClientDestroy_Normal);
		DEF_DELETE(actor);
		//end
		{
			ThreadGuard guardTimer(m_monitorTimerLock);
			m_pMonitorReleaseActorWorkTime->workEnd();
		}	
		 
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"end free one server ClientID=%s, ClientAgtName=%s, ID=%lu",
			m_strClientID.c_str(), strClientAgtNameGet.c_str(), nIDGet);


		iter++;
	}
	releaselist.clear();

}

int ConnectionActorReleaser::getWorkTimeInfo( WorkTimeInfo& workTimeInfoGet )
{
	int nFunRes = 0;
	ThreadGuard guardTimer(m_monitorTimerLock);
	
	workTimeInfoGet.m_strClientID = m_strClientID;
	if (NULL != m_pMonitorReleaseActorWorkTime)
	{
		workTimeInfoGet.m_nReleaseActorNotWork = m_pMonitorReleaseActorWorkTime->getNotworkTime();
	}
	else
	{
		workTimeInfoGet.m_nReleaseActorNotWork = 0;
	}
	return nFunRes;	
}



NS_END(TA_DB_Sync)