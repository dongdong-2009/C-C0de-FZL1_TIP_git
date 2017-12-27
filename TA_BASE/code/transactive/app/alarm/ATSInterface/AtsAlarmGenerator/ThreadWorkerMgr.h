#pragma once
#include "CommonDefine.h"
#include "ThreadWorker.h"
#include "NonReEntrantThreadLockable.h"

typedef std::vector<CThreadWorker *> THREAD_WORKER_LIST;
class CThreadWorkerMgr
{
public:

	CThreadWorkerMgr(HWND hwndDlg, int nThreadCount, int nSubmitInterval, ULONG ulAlarmCount);
	~CThreadWorkerMgr(void);
	void startAutoSubmit();
	void stopAutoSubmit();
	bool decrementAlarmCount();
	void notifySubmitFinish();

private:
	TA_Base_Bus::NonReEntrantThreadLockable m_threadLock;
	THREAD_WORKER_LIST m_threadWorkers;
	int m_nThreadCount;
	int m_nSubmitInterval;
	ULONG m_ulAlarmCount;

	bool m_bThreadWorkerStop;
	bool m_bSubmitFinish;
	bool m_bTerminating;
	HWND m_hwndDlg;
};
