// WatchedModule.h: interface for the WatchedModule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WATCHEDMODULE_H__7D1B21E6_F5A0_4CF4_81CF_691918FF1993__INCLUDED_)
#define AFX_WATCHEDMODULE_H__7D1B21E6_F5A0_4CF4_81CF_691918FF1993__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ThreadRunner.h"
#include "IStatusObserver.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include <vector>

class StatusChecker;

class WatchedModule : public IStatusObserver, public TA_Base_Core::Thread
{
public:
	WatchedModule();
	virtual ~WatchedModule();


	void setHeartBeatGap( unsigned long hbg )
	{
		m_nHearBeatGap = hbg;
	}
	unsigned long getHeartBeatGap() const
	{
		return m_nHearBeatGap;
	}

	virtual std::string getServiceName() const = 0;
	/**
	 * heartBeatCall
	 *
	 * get status checker heart beat timestamp
	 *	call StatusChecker.heartBeat(). If StatusChecker heart beat
	 *	stopped, it may:
	 *	1:	StatusChecker thread stop or quit
	 *	2:	checkMethod() blocked
	 */
	virtual void heartBeatCall(const StatusChecker* pChecker) = 0;

	// IStatusObserver interface
	virtual	void onStatusChanged(const ObserverData& currentStatus);

	void addCheckerMap(const StatusChecker* pChecker)
	{
		TA_Base_Core::ThreadGuard guard(m_mapLock);
		m_statusCheckerMap[pChecker->getIdentify()] = true;
	}

	virtual void run();
	virtual void terminate();

	virtual void show()
	{
	}

public:
	void stopAll();
	virtual void createStatusChecker() = 0;
	void restartService( const char* serviceName, StatusChecker* pChecker );
	void restartStatusChecker( const StatusChecker* pChecker );
	void restartMgr();

private:
	inline void conditionWait(unsigned long milliseconds)
	{
		m_condTerminate.timedWait(milliseconds);
	}
	
	inline virtual void stopWait()
	{
		m_condTerminate.signal();
	}

	void pauseAllChecker();
	void continueAllChecker();
private:
	// first unsigned long is StatusChecker id actually it is the address of StatusChecker
	std::map<unsigned long, bool> m_statusCheckerMap;
	TA_Base_Core::ReEntrantThreadLockable m_mapLock;
	//
	bool			m_bTerminate;
	unsigned long	m_nHearBeatGap;
	bool			m_bRestartService;

	TA_Base_Core::Condition		m_condTerminate;
};

#endif // !defined(AFX_WATCHEDMODULE_H__7D1B21E6_F5A0_4CF4_81CF_691918FF1993__INCLUDED_)
