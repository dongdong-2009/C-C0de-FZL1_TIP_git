// AlarmStoreCmdWatcher.h: interface for the AlarmStoreCmdWatcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALARMSTORECMDWATCHER_H__9B5A6BA5_1E56_49C3_8BC9_F09B543B1901__INCLUDED_)
#define AFX_ALARMSTORECMDWATCHER_H__9B5A6BA5_1E56_49C3_8BC9_F09B543B1901__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "WatchedModule.h"

class AlarmStoreCmdWatcher : public WatchedModule  
{
public:
	AlarmStoreCmdWatcher();
	virtual ~AlarmStoreCmdWatcher();

	static std::string ServiceName;

	virtual std::string getServiceName() const
	{
		return ServiceName;
	}


public:
	virtual void createStatusChecker();

	virtual void onStatusChanged(const ObserverData& currentStatus);

	virtual void heartBeatCall(const StatusChecker* pChecker);

private:
	time_t			m_preHeartBeat;
	time_t			m_currentHearBeat;
	unsigned long	m_HeartBeatStopCnt;
	unsigned long	m_HeartBeatStopLimit;
};

#endif // !defined(AFX_ALARMSTORECMDWATCHER_H__9B5A6BA5_1E56_49C3_8BC9_F09B543B1901__INCLUDED_)
