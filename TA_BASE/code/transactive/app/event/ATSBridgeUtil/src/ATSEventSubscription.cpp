// ATSEventSubscription.cpp : Implementation of CATSEventSubscription

#include "stdafx.h"
#include "ATSEventSubscription.h"
#include "Convert.h"
// CATSEventSubscription
//CATSEventSubscription* CATSEventSubscription::pThis = NULL;
CallBackHandler * CATSEventSubscription:: m_pCallbackHandler = NULL;
LoggerPtr CATSEventSubscription::m_logger(LoggerPtr(Logger::getLogger("CATSEventSubscription")));

using namespace IATSEventsAPI;

HRESULT CATSEventSubscription::FinalConstruct()
{
	LOG4CXX_INFO(m_logger, "FinalConstruct entry");

	try 
	{
		LOG4CXX_INFO(m_logger, "Before IATSEventsAPI::ATSEventSubscription::CreateATSEventSubscription");
		m_pATSEventSubscription = IATSEventsAPI::ATSEventSubscription::CreateATSEventSubscription();

		if(!m_pATSEventSubscription)
		{
			LOG4CXX_ERROR(m_logger, "E_FAIL | Failed to CreateATSEventSubscription");
			return E_FAIL;
		}

		m_pCallbackHandler = new CallBackHandler(this);
	}
	catch(...)
	{
		LOG4CXX_ERROR(m_logger, "Unknown exception ocurred..");
		return E_FAIL;
	}

	LOG4CXX_INFO(m_logger, "S_OK | Success CreateATSEventSubscription");
	LOG4CXX_INFO(m_logger, "FinalConstruct exit");
	return S_OK;
}

void CATSEventSubscription::SubscriptionCallback(const IATSEventsAPI::ATSSubscriptionError error, std::list<IATSEventsAPI::ATSEvent*>& atsEvents)
{
	LOG4CXX_INFO(m_logger, "SubscriptionCallback entry");
	//SYSTEMTIME st, et;
	//GetLocalTime(&st);

	if (error != ATS_SUBSCRIPTION_OK)
	{
// 		m_pCallbackHandler->pushEvents((int)error, atsEvents);
// 	}
// 	else
// 	{
		LOG4CXX_ERROR(m_logger, "ATSSubscriptionError: " << (int)error);
	}

	m_pCallbackHandler->pushEvents((int)error, atsEvents);
	//GetLocalTime(&et);
	
	//LOG4CXX_INFO(m_logger, "SubscriptionCallback Start Time: " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << ":" << st.wMilliseconds);
	//LOG4CXX_INFO(m_logger, "SubscriptionCallback End Time: " << et.wHour << ":" << et.wMinute << ":" << et.wSecond << ":" << et.wMilliseconds);
	LOG4CXX_INFO(m_logger, "SubscriptionCallback exit");
}

STDMETHODIMP CATSEventSubscription::startSubscription(IATSFilterCriteria* pfilterCriteria, LONG backloglength, IATSEventList** pATSEvents, LONG* pRetVal)
{
	// TODO: Add your implementation code here
	LOG4CXX_INFO(m_logger, "startSubscription entry");
	CConvert objConverter;
	
	IATSEventsAPI::ATSFilterCriteria filterCriteria;
	IATSEventsAPI::ATSSubscriptionError subscribeRet;
	std::list<IATSEventsAPI::ATSEvent*> atsEvents;

	objConverter.IATSType2ATSType(pfilterCriteria, filterCriteria);

	LOG4CXX_TRACE(m_logger, "Filter Criteria: " << objConverter.ObjectToString(pfilterCriteria));
	LOG4CXX_TRACE(m_logger, "backloglength = " << backloglength);

	if (m_pCallbackHandler->getCurrentState() != Thread::THREAD_STATE_RUNNING)
	{
		m_pCallbackHandler->start();
	}

	try
	{
		LOG4CXX_TRACE(m_logger, "Filter Criteria Converted: " << objConverter.ObjectToString(filterCriteria));
		LOG4CXX_INFO(m_logger, "Before Calling startSubscription");

		subscribeRet = m_pATSEventSubscription->startSubscription(&filterCriteria, backloglength, SubscriptionCallback, atsEvents);

		*pRetVal = (long)subscribeRet;

		if(subscribeRet != ATS_SUBSCRIPTION_OK)
		{
			if (m_pCallbackHandler->getCurrentState() == Thread::THREAD_STATE_RUNNING)
			{
				m_pCallbackHandler->terminateAndWait();
			}

			LOG4CXX_ERROR(m_logger, "After calling startSubscription | failed | Result: " << (int) subscribeRet);
		}
		else
		{
			objConverter.ATSType2IATSType(atsEvents, pATSEvents);
			LOG4CXX_INFO(m_logger, "After Calling startSubscription | Result: 0");	
		}

		if(atsEvents.size() != 0)
		{
			LOG4CXX_INFO(m_logger, "+ IATSEventsAPI::FreeATSEventList");
			IATSEventsAPI::FreeATSEventList(atsEvents);
			LOG4CXX_INFO(m_logger, "- IATSEventsAPI::FreeATSEventList");
		}
	}
	catch(...)
	{
		LOG4CXX_ERROR(m_logger, "Unknown exception ocurred...");
		return E_FAIL;
	}

	LOG4CXX_INFO(m_logger, "startSubscription exit");
	return S_OK;
}

STDMETHODIMP CATSEventSubscription::stopSubscription(LONG* plVal)
{
	// TODO: Add your implementation code here
	IATSEventsAPI::ATSSubscriptionError subscribeRet;
	LOG4CXX_INFO(m_logger, "stopSubscription entry");

	try
	{
		LOG4CXX_INFO(m_logger, "Before Calling stopSubscription");
		subscribeRet = m_pATSEventSubscription->stopSubscription();
		*plVal = (long)subscribeRet;

		if(subscribeRet != ATS_SUBSCRIPTION_OK)
		{
			LOG4CXX_ERROR(m_logger, "After stopSubscription | failed | Result: " << (int) subscribeRet);
		}
		else
		{
			LOG4CXX_INFO(m_logger, "After Calling stopSubscription | Result: 0 ");

			if (m_pCallbackHandler->getCurrentState() == Thread::THREAD_STATE_RUNNING)
			{
				m_pCallbackHandler->terminateAndWait();
			}
		}
	}
	catch(...)
	{
		LOG4CXX_ERROR(m_logger, "Unknown exception occurred...");
		return E_FAIL;
	}

	LOG4CXX_INFO(m_logger, "stopSubscription exit");
	return S_OK;
}