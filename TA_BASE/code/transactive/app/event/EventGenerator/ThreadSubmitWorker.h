//Author: Noel R. Tajanlangit

#ifndef __THREAD_SUBMIT_WORKER_H_INCLUDED__
#define __THREAD_SUBMIT_WORKER_H_INCLUDED__

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/event/src/EventItem.h"
//Forward declaration
#include "core/event/idl/src/EventAgentCorbaDef.h"
#include "core/event/idl/src/EventDetailCorbaDef.h"
class ThreadSubmitWorker
	: public TA_Base_Core::Thread
{
public:
	ThreadSubmitWorker(ThreadSubmitMgr* pMyManager, 
		int nSubmitInterval, int nSubmitLocation);

	~ThreadSubmitWorker();

	virtual void run();
	virtual void terminate();
	
private:
	void _getEventsToSubmit(TA_Base_Core::T_EventItemInfoList& events);
	void _updateCounters(int nSubmitSize);

private:
	ThreadSubmitMgr* m_pMyManager;
	int m_nSubmitInterval;
	int m_nSubmitLocation;

	bool m_bTerminate;
};


#endif //__THREAD_SUBMIT_WORKER_H_INCLUDED__
