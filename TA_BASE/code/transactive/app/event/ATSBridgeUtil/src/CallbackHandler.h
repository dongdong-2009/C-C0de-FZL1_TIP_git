// ATSEvent.h : Declaration of the CATSEvent
#ifndef __CALLBACK_HANDLER__
#define __CALLBACK_HANDLER__
#pragma once

#include <utility>
#include "Thread.h"
#include "IATSEventsAPI.h"
#include "NonReEntrantThreadLockable.h"
#include "Semaphore.h"
#include <map>

#ifdef ENABLE_LOG4CXX
	#include "log4cxx/logger.h" 
	#include "log4cxx/helpers/exception.h"
	using namespace log4cxx;  
	using namespace log4cxx::helpers;  
#else
	#include "logger.h"  
	using namespace log4cxx;  
#endif

using namespace TA_Base_Core;
using namespace TA_Base_Bus;


class CallBackHandler : public Thread
{
public:

	CallBackHandler(void *pSubscription);
	~CallBackHandler(void);
	virtual void run();
	virtual void terminate();
	void pushEvents(int atsError, std::list<IATSEventsAPI::ATSEvent*> atsEvents);

private:
	std::multimap <int, std::list<IATSEventsAPI::ATSEvent*> > m_mapATSEvents;
	NonReEntrantThreadLockable m_lock;
	bool m_bThreadRunning;
	HANDLE m_hExitEvent;
	static LoggerPtr m_logger;
	static CSemaphoreLock * m_pSemaphore;
};


#endif //__CALLBACK_HANDLER__