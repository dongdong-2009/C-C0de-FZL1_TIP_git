#pragma once
#include "CommonDefine.h"
#include "NonReEntrantThreadLockable.h"
#include "Thread.h"

class CThreadAutoAckClose : public TA_Base_Core::Thread
{
public:
	CThreadAutoAckClose(int nAckCloseInterval, HWND hWndTarget);
	~CThreadAutoAckClose(void);
	virtual void run();
	virtual void terminate();
	void insert(AlarmQueueItem* pQueueItem);
	void remove(I_ALARM_PROVIDER::AlarmDetail* pAlarmDetail);
	void finishedAutoAck();

private:
	TA_Base_Bus::NonReEntrantThreadLockable m_threadLock;
	ALARMDETAIL_QUEUE m_pAlarmQueue;	
	
	int m_nAckCloseInterval;
	HANDLE m_hExitEv;
	bool m_bTerminate;
	bool m_bfinishAutoAck;
	HWND m_hWndTarget;
};
