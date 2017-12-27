// AlarmProvider.h: interface for the CAlarmProvider class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATSALARM_H__A8817BFB_5FB3_4EB5_B0CE_B256D962EA50__INCLUDED_)
#define AFX_ATSALARM_H__A8817BFB_5FB3_4EB5_B0CE_B256D962EA50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataStructInfo.h"
#include "core/utilities/src/DebugUtil.h"

class CAlarmProvider : public IAlarmProvider  
{
public:
	CAlarmProvider(unsigned long _ulProviderID);
	virtual ~CAlarmProvider();

	void submitAlarm(const AlarmDetail& details, const ProfileInfo& profileInfo); //May catch ATS_ALARM_QUEUE_OVERFLOW exception
	void submitAlarmList(std::list<AlarmDetail*> details, const ProfileInfo& profileInfo); //May catch ATS_ALARM_QUEUE_OVERFLOW exception
	bool getProfileInfo(ProfileInfo& profileInfo);
	unsigned long retrieveAlarmAckUpdate(std::list<AlarmAckUpdate*>& updates);
	unsigned long retrieveMmsJobRequest(std::list<MmsJobRequest*>& updates);
	unsigned long retrieveAlarmRequestUpdate( std::list<AlarmRequest*>& updates );
	void setUpdateTime(time_t updateTime);
private:
	unsigned long m_ulProviderID;

};

#endif // !defined(AFX_ATSALARM_H__A8817BFB_5FB3_4EB5_B0CE_B256D962EA50__INCLUDED_)
