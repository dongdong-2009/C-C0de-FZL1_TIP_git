// StatusChecker.h: interface for the StatusChecker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATUSCHECKER_H__EFDFEDD4_26F9_4BCA_9136_2FFF8D4A6D27__INCLUDED_)
#define AFX_STATUSCHECKER_H__EFDFEDD4_26F9_4BCA_9136_2FFF8D4A6D27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <set>
#include "ThreadRunner.h"

class IStatusObserver;
class IStatusStrategy;

#define BEARABLE_MAX_ERROR_COUNT	3

// Note:

class StatusChecker : public TA_Base_Core::Thread
{
public:
	StatusChecker();
	virtual ~StatusChecker();
public:
	/**
	 * addObserver
	 *
	 */
	void addObserver( IStatusObserver * statusObserver );
	
	/**
	 * notifyObservers
	 *
	 * This function should called in checkMethod when detected a error status
	 *	happen in watched object.
	 */
	void notifyObservers();

	/**
	 * When stop/start service, those function were called by framework
	 * WatchedModule will call those method when restart service
	 */
	 // create connection with new start server, or some init work before connect a server
	virtual bool connect() = 0;
	// end connection with old server which will be stop immediately
	// you can do some clean up work when server is going to stop
	virtual void disconnect() = 0;
	// return connection status with server, you can set this value after call connect()/disconnect() method
	virtual bool isConnect() const = 0;

	// return StatusCheckerName, identification used by WatchedModule
	virtual std::string getName() const = 0;

	// clean successive error count when you do not call notifyObservers() each time in checkMethod()
	void cleanSuccessiveError()
	{
		m_nSuccessiveErrorCount = 0;
	}

	void start()
	{
		m_bTerminate = false;
		Thread::start();
	}

	/**
	 * init 
	 *
	 * chance to do some init work in run() method before enter main loop
	 */
	virtual void init()
	{
	}

	/**
	 * checkMethod
	 *
	 * This method call in main loop thread, implement should call watched object's special
	 *	method to get its status. If returned status indicate error has occur in watched object
	 *	call notifyObservers
	 *
	 * NOTE: Make sure this method will not raise any exception, otherwise run() main loop will
	 *		 ended even m_bTerminate is false
	 */
	virtual long checkMethod() = 0;

	/**
	 * shouldNotifyObserver
	 *
	 * Used in notifyObservers, decide whether we should notify WatchedModule
	 *	if m_successiveErrorCnt >= m_nNotifyErrorCountLimit, 
	 *	registered watched object's onStatusChanged() method will be called
	 *
	 * @return	true		notify watchedModule
	 *			false		not notify
	 */
	virtual bool shouldNotifyObserver();

	/**
	 * flag whether call checkMethod() in main loop
	 */
	virtual void pause();
	
	virtual void continues();
	
	bool isPaused() const
	{
		return m_bPause;
	}

	// clean up Status Checker error status
	void cleanStatus()
	{
		m_nCurrentErrorCount = 0;
		m_nErrorStatusCount = 0;
		m_CheckCount = 0;
		m_nSuccessiveErrorCount = 0;
	}

	// restart service process, not use
	void restartServer(const char* serviceName)
	{
		if ( !m_bRestartService )
		{
			m_bRestartService = true;
		}
		pause();
	}

public:	// get status method

	/**
	 * get identify of StatusChecker
	 */
	unsigned long getIdentify() const
	{
		return m_id;
	}

	/**
	 * isInErrorState
	 *
	 * Check newest error count for current status
	 */
	bool isInErrorState() const
	{
		return ( m_nErrorStatusCount > 0 );
	}

	unsigned long getErrorCount() const
	{
		return m_nCurrentErrorCount;
	}
	unsigned long getSuccessiveErrorCount() const
	{
		return m_nSuccessiveErrorCount;
	}
	unsigned long getErrorStatusCount() const
	{
		return m_nErrorStatusCount;
	}
	unsigned long getErrorNotifyCountLimit() const
	{
		return m_nNotifyErrorCountLimit;
	}
	unsigned long setErrorNotifyCountLimit(unsigned long nNotifyErrorCount)
	{
		unsigned long tmp = m_nNotifyErrorCountLimit;
		m_nNotifyErrorCountLimit = nNotifyErrorCount;
		return tmp;
	}

	unsigned long getCheckCount() const
	{
		return m_CheckCount;
	}

	// Status Checker run() method heartbeat	
	time_t heartBeat() const
	{
		return m_HeartBeat;
	}

	/**
	 * set StatusChecker call checkMethod() interval
	 */
	void setCheckGap( unsigned long ulCheckGap )
	{
		m_CheckGap = ulCheckGap;
	}
	unsigned long getCheckGap() const
	{
		return m_CheckGap;
	}


public:
	virtual void terminate();	// thread virtual method
protected:
	virtual void run();	// thread virtual method
	bool stopService( const char* serviceName );	// stop windows service method
	bool startService( const char* serviceName );	// start windows service method
	int ServiceQueryStatus(const std::string& serviceName);	// query windows service status method

private:
	inline void conditionWait(unsigned long milliseconds)
	{
		m_condTerminate.timedWait(milliseconds);
	}
	
	inline virtual void stopWait()
	{
		m_condTerminate.signal();
	}

private:
	std::set<IStatusObserver*> m_statusObserver;		// Observer set
	unsigned long	m_nErrorStatusCount;			// notify send count
	unsigned long	m_nSuccessiveErrorCount;
	unsigned long	m_id;							// identify
	unsigned long	m_nNotifyErrorCountLimit;		// limit for error count convert to error status count
	unsigned long	m_nCurrentErrorCount;			// call method detect error count

	// for thread run
	bool			m_bTerminate;					// thread end flag
	time_t			m_HeartBeat;					// each loop start time_t
	unsigned long	m_CheckGap;						// check gap
	bool			m_bPause;						// control whether call checkMethod in main loop
	unsigned long	m_CheckCount;					// counts of calling checkMethod

	// start / stop service use
	bool			m_bRestartService;				// flag if you want restart
	unsigned long	m_nStopTime;					//
	unsigned long	m_nStartTime;					//
	std::string		m_strServiceName;				//

	TA_Base_Core::Condition		m_condTerminate;
};

#endif // !defined(AFX_STATUSCHECKER_H__EFDFEDD4_26F9_4BCA_9136_2FFF8D4A6D27__INCLUDED_)
