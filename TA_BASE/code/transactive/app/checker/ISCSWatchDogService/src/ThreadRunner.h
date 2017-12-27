// ThreadRunner.h: interface for the ThreadRunner class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREADRUNNER_H__BB3E1E3E_D9D7_43F4_A5DF_918F6D300E8E__INCLUDED_)
#define AFX_THREADRUNNER_H__BB3E1E3E_D9D7_43F4_A5DF_918F6D300E8E__INCLUDED_

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Condition.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ThreadRunner: public TA_Base_Core::Thread
{
public:
	ThreadRunner()
	{
	}
	virtual ~ThreadRunner()
	{
		terminateAndWait();
	}
	/**
	 * run
	 *
	 * you should call terminate() method in thread loop struct,
	 *	if you want
	 */
	virtual void run() = 0;

	/**
	 * conditionWait
	 * 
	 * use to instead sleep function, can be interrupt by terminate()
	 *
	 * @param milliseconds	sleep time
	 */
	inline void conditionWait(unsigned long milliseconds)
	{
		m_condTerminate.timedWait(milliseconds);
	}

	inline virtual void stopWait()
	{
		m_condTerminate.signal();
	}

private:
	TA_Base_Core::Condition		m_condTerminate;
};

#endif // !defined(AFX_THREADRUNNER_H__BB3E1E3E_D9D7_43F4_A5DF_918F6D300E8E__INCLUDED_)
