// ATSEvent.cpp : Implementation of CATSEvent

#include "stdafx.h"
#include "CallbackHandler.h"
#include "ThreadGuard.h"
#include "Convert.h"
#include "ATSEventSubscription.h"

const std::string g_strSemaphore = "SemaphoreName";
CATSEventSubscription * g_pSubscription = NULL;

// Initialize logger
LoggerPtr CallBackHandler::m_logger(Logger::getLogger("CallbackHandler"));
CSemaphoreLock * CallBackHandler::m_pSemaphore = NULL;
CallBackHandler::CallBackHandler(void * pSubscription) : m_bThreadRunning(false)
{
	g_pSubscription = static_cast<CATSEventSubscription*>(pSubscription);
	m_mapATSEvents.clear();
	m_pSemaphore = new CSemaphoreLock(0, 10, g_strSemaphore);
}

CallBackHandler::~CallBackHandler()
{
	if (m_pSemaphore != NULL)
	{
		delete m_pSemaphore;
		m_pSemaphore = NULL;
	}
}

void CallBackHandler::run()
{
	LOG4CXX_INFO(m_logger, "CallBackHandler::run entry");
	CoInitialize(0); 
	m_bThreadRunning = true;
	m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL); // Create Event for exit signal
	
	DWORD dwWaitResult;
	while(m_bThreadRunning)
	{
		//dwWaitResult = WaitForSingleObject(m_Semaphore, 1000);
		dwWaitResult = m_pSemaphore->acquire(1000);
		switch(dwWaitResult)
		{
		case WAIT_OBJECT_0:
			{
				LOG4CXX_INFO(m_logger, "WaitForSingleObject: WAIT_OBJECT_0");
				if (m_bThreadRunning == false)
				{
					break;
				}
				else
				{
					ThreadGuard guard(m_lock);
					
					if (m_mapATSEvents.size() != 0)
					{
						std::list<IATSEventsAPI::ATSEvent*> atsEvents;
						std::multimap <int, std::list<IATSEventsAPI::ATSEvent*> >::iterator itFirst;

						itFirst = m_mapATSEvents.begin();
						atsEvents = (*itFirst).second;

						IATSEventList* pATSEventList = NULL;

						if ((SHORT)(*itFirst).first == 0)
						{
							CConvert objConverter;
							LOG4CXX_TRACE(m_logger, std::string("Recieved Data: ") + objConverter.ObjectToString(atsEvents));
							LOG4CXX_INFO(m_logger, "ATSSubscriptionError: "<< (int)(*itFirst).first <<" || EventList Size: "<< (int)atsEvents.size());
							objConverter.ATSType2IATSType(atsEvents, &pATSEventList);
							pATSEventList->AddRef();
						}
			
						LOG4CXX_INFO(m_logger, "Before Calling Fire_EventUpdate");
						// fire eventUpdate
						HRESULT hr = g_pSubscription->Fire_EventUpdate((SHORT)(*itFirst).first, pATSEventList);
						if(FAILED(hr))
						{
							LOG4CXX_ERROR(m_logger, "FAILED to Fire_EventUpdate | HRESULT : " << hr);
						}
						else
						{
							LOG4CXX_INFO(m_logger, "After Calling Fire_EventUpdate");
						}

						if(atsEvents.size() != 0)
						{
							LOG4CXX_INFO(m_logger, "+ IATSEventsAPI::FreeATSEventList");
							IATSEventsAPI::FreeATSEventList(atsEvents);
							LOG4CXX_INFO(m_logger, "- IATSEventsAPI::FreeATSEventList");
							atsEvents.clear();
						}
		
						if (pATSEventList != NULL)
						{
							pATSEventList->Release();
						}
						m_mapATSEvents.erase(itFirst);
					}
				}
			}
			break;
		case WAIT_TIMEOUT:
			LOG4CXX_INFO(m_logger, "WaitforSingleObject Timeout");
			break;
		}
	}
	CoUninitialize();
	LOG4CXX_INFO(m_logger, "CallBackHandler::run exit");

	SetEvent(m_hExitEvent); // Signal Thread exit
	//return 0;
}

void CallBackHandler::terminate()
{
	LOG4CXX_INFO(m_logger, "terminate entry");
	m_bThreadRunning = false;
	
	// Wait for thread to exit
	WaitForSingleObject(m_hExitEvent, INFINITE);
	
	// Clean up
	CloseHandle(m_hExitEvent);
	{
		ThreadGuard threadGuard(m_lock);
		if (m_mapATSEvents.size() != 0)
		{
			// Loop through the map and free ATSEvents //
			LOG4CXX_INFO(m_logger, "Data size = " << m_mapATSEvents.size() << " Performing clean up.");
			std::multimap <int, std::list<IATSEventsAPI::ATSEvent*> >::iterator it;
			for (it = m_mapATSEvents.begin(); it != m_mapATSEvents.end(); it++)
			{
				LOG4CXX_INFO(m_logger, "+ IATSEventsAPI::FreeATSEventList");
				FreeATSEventList((*it).second);
				LOG4CXX_INFO(m_logger, "- IATSEventsAPI::FreeATSEventList");
			}
			m_mapATSEvents.clear();
		}
	}
	LOG4CXX_INFO(m_logger, "terminate exit");
}

void CallBackHandler::pushEvents(int atsError, std::list<IATSEventsAPI::ATSEvent*> atsEvents)
{
	LOG4CXX_INFO(m_logger,"pushEvents entry");
	
	std::list<IATSEventsAPI::ATSEvent*> atsEventCopy;
	std::list<IATSEventsAPI::ATSEvent*>::iterator it;
	
	if (THREAD_STATE_RUNNING == getCurrentState())
	{
		// we need to duplicate the data because based on the documentation 
		// IATSEventSubscription will clear the data after calling the 
		// callback function.
		for (it = atsEvents.begin(); it != atsEvents.end(); it++)
		{
			IATSEventsAPI::ATSEvent* pEvent = NULL;
			pEvent = new IATSEventsAPI::ATSEvent(*(*it));
			atsEventCopy.push_back(pEvent);
		}

		{
			ThreadGuard guard(m_lock);
			m_mapATSEvents.insert(make_pair((int)atsError, atsEventCopy));// atsEvents;
		}
		
		LOG4CXX_INFO(m_logger, "m_mapATSEvents size = " << m_mapATSEvents.size());

		m_pSemaphore->release();

	}
	else
	{
		LOG4CXX_ERROR(m_logger, "Thread is not running...");
	}
	
	LOG4CXX_INFO(m_logger,"pushEvents exit");
}
