//Author: Noel R. Tajanlangit

#ifndef __THREAD_SUBMIT_MANAGER_H_INCLUDED__
#define __THREAD_SUBMIT_MANAGER_H_INCLUDED__

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include <vector>

//Forward declaration
class ThreadSubmitWorker;

class ThreadSubmitMgr
{
public:
	ThreadSubmitMgr(int nSubmitInterval, 
			int nBatchSubmitSize,
			int nNumberOfThreads,
			unsigned long ulNumberOfEvents,
			int nSubmitLocation);

	~ThreadSubmitMgr();

	void startAutoSubmit();

	void stopAutoSubmit();

	int getEventSize();

	unsigned long getUnsubmittedSize();

private:
	int m_nSubmitInterval;
	int m_nBatchSubmitSize;
	int m_nNumberOfThreads;
	unsigned long m_ulNumberOfEvents;
	int m_nSubmitLocation;
	
	TA_Base_Core::NonReEntrantThreadLockable m_threadLock;
	
	typedef std::vector< ThreadSubmitWorker* > T_vecThreadWorkers;
	T_vecThreadWorkers m_thrdSubmitWorkers;

};


#endif //__THREAD_SUBMIT_MANAGER_H_INCLUDED__



