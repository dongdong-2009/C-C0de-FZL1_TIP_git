// ATSEvent.h : Declaration of the CATSEvent
#ifndef __SEMAPHORE_INCLUDE__
#define __SEMAPHORE_INCLUDE__
#include <string>
#include "IThreadLockable.h"
#pragma once

class CSemaphoreLock : public TA_Base_Bus::IThreadLockable
{
public:
	CSemaphoreLock(long initialCount, long maxCount, std::string strLockName);
	~CSemaphoreLock(void);
	DWORD acquire(DWORD dwMillisecond);
	virtual void acquire();
	virtual void release();
private:

	// Handle to semaphore
	HANDLE m_hSemaphore;
};




#endif //__SEMAPHORE_INCLUDE__