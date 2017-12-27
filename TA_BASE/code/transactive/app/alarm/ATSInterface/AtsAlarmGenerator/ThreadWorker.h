#pragma once
#include "Thread.h"

//forward declaration
class CThreadWorkerMgr;

class CThreadWorker : public TA_Base_Core::Thread
{
public:
	CThreadWorker(CThreadWorkerMgr* pManager, int nSubmitInterval);
	~CThreadWorker(void);
	virtual void run();
	virtual void terminate();

private:
	HANDLE m_hEventClose;
	HANDLE m_hExitEv;
	bool m_bTerminate;

	CThreadWorkerMgr* m_pManager;
	int m_nSubmitInterval;
};
