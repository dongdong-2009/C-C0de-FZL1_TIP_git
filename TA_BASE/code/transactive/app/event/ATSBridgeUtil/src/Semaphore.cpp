// ATSEvent.cpp : Implementation of CATSEvent

#include "stdafx.h"
#include "Semaphore.h"

// Constructor
CSemaphoreLock::CSemaphoreLock(long initialCount, long maxCount, std::string strLockName)
{
	m_hSemaphore = CreateSemaphore(NULL, initialCount, maxCount, strLockName.c_str());

	if (m_hSemaphore == NULL)
	{
		// Log Error
	}
}

// Destructor
CSemaphoreLock::~CSemaphoreLock()
{
	if (m_hSemaphore != NULL)
	{
		CloseHandle(m_hSemaphore);
	}
}

DWORD CSemaphoreLock::acquire(DWORD dwMillisecond)
{
	DWORD dwRet;
	dwRet = WaitForSingleObject(m_hSemaphore, dwMillisecond);
	return dwRet;
}

void CSemaphoreLock::acquire()
{
	WaitForSingleObject(m_hSemaphore, INFINITE);
}

void CSemaphoreLock::release()
{
	// Release semaphore to signal event //
	if (!ReleaseSemaphore( 
		m_hSemaphore,  // handle to semaphore
		1,            // increase count by one
		NULL) )       
	{
		//LOG4CXX_ERROR(m_logger, "ReleaseSemaphore error: " << GetLastError());
	}
}

