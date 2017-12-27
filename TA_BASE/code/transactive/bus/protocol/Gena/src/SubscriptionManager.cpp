
#include "SubscriptionManager.h"
#include "GenaCallbackProccessor.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "SIDNotFoundException.h"

using namespace TA_Base_Core;
namespace TA_Base_Bus
{
	
	/*
	* Constructor
	*/
	SubscriptionManager::SubscriptionManager(GenaClientParams clientParams, GenaCallbackProccessor* pCallbackProc, ConnectionFactoryPtr pConnectionMgr)
		: m_pCallbackProc(pCallbackProc)
		, m_GenaClientParams(clientParams)
		, m_bTerminate(false)
		, m_pConnectionMgr(pConnectionMgr)
	{
		FUNCTION_ENTRY("Constructor");
		// Process the initial status immediately
		//m_condition.signal();
		FUNCTION_EXIT;
	}

	/*
	* Destructor
	*/
	SubscriptionManager::~SubscriptionManager(void)
	{
		FUNCTION_ENTRY("Destructor");
		terminateAndWait();
		FUNCTION_EXIT;
	}

	/*
	* subscribe
	*/
	std::string SubscriptionManager::subscribe(const std::string& strTopic, const std::string& notification,
		const T_TopicFilterSet& filterSet)
	{
		FUNCTION_ENTRY("subscribe");
		TA_Base_Core::ThreadGuard guard(m_threadLock);

		std::string uri = SLASH_DELIMETER + strTopic;
		if (filterSet.size() > 0)
		{
			uri += QUESTION_DELIMETER;
			bool bFirst = true;
			T_TopicFilterSet::const_iterator itr = filterSet.begin();
			for (itr; itr!= filterSet.end(); itr++)
			{
				if (bFirst)
				{
					uri += itr->first + ASSIGN_DELIMETER + itr->second; 
					bFirst = false;
				}
				else
				{
					uri += AND_DELIMETER + itr->first + ASSIGN_DELIMETER + itr->second;
				}
			}
		}

		// Should we allow multiple subscription of the same uri???
		// Todo: Answer the question

		FUNCTION_EXIT;
		return subscribe(uri, notification);
	}

	/*
	* subscribe
	*/
	std::string SubscriptionManager::subscribe(const std::string& strTopicURL, const std::string& notification)
	{
		FUNCTION_ENTRY("subscribe");
		std::string sid = "";
		
		try
		{	
			// Check for duplicate
			sid = _isTopicSubscribed(strTopicURL);
			if (sid.empty() == false)
			{
				LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Topic %s is already subscribed with SID=%s", strTopicURL.c_str(), sid.c_str());
				return sid;
			}

			GenaSubscriberPtr pNewSubscriber( new GenaSubscriber(strTopicURL, notification, m_GenaClientParams, m_pCallbackProc, m_pConnectionMgr));
			TA_Base_Core::ThreadGuard guard(m_threadLock);
			m_subscribers.push_back(pNewSubscriber);
			pNewSubscriber->subscribe();

			sid = pNewSubscriber->getSID();
			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Successfuly subscribe for URI %s,  SID=%s", strTopicURL.c_str(),sid.c_str());
		}
		catch(...)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to subscribe for URI %s", strTopicURL.c_str());
			return "";
		}
		FUNCTION_EXIT;
		return sid;
	}

	/*
	* subscribe
	*/
	void SubscriptionManager::addSubscribeTopic(const std::string& strTopicURL, const std::string& notification)
	{
		FUNCTION_ENTRY("addSubscribeTopic");
			
		// Check for duplicate
		std::string sid = _isTopicSubscribed(strTopicURL);
		if (sid.empty() == false)
		{
			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Topic %s is already subscribed with SID=%s", strTopicURL.c_str(), sid.c_str());
			return;
		}

		GenaSubscriberPtr pNewSubscriber( new GenaSubscriber(strTopicURL, notification, m_GenaClientParams, m_pCallbackProc, m_pConnectionMgr));
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		m_subscribers.push_back(pNewSubscriber);
		
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Add subscribe topic for URI %s", strTopicURL.c_str());
		
		FUNCTION_EXIT;
	}

	/*
	* unsubscribe
	*/
	void SubscriptionManager::unsubscribe(const std::string& sid)
	{
		FUNCTION_ENTRY("unsubscribe");
		TA_Base_Core::ThreadGuard guard(m_threadLock);

		T_subscriberVec::iterator itr = m_subscribers.begin();
		for (itr; itr!= m_subscribers.end(); itr++)
		{
			GenaSubscriberPtr pTemp = *itr;
			if (sid == pTemp->getSID())
			{
				pTemp->unsubscribe();
				m_subscribers.erase(itr);
				break;
			}
		}

		FUNCTION_EXIT;
	}

	void SubscriptionManager::resetSubscribeTopic(const std::string& sid)
	{
		FUNCTION_ENTRY("resetSubscribeTopic");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		for (int idx=0; idx<m_subscribers.size(); idx++)
		{
			GenaSubscriberPtr pTemp = m_subscribers[idx];
			if (sid == pTemp->getSID())
			{
				pTemp->resetSubscribe();
				break;
			}
		}

		FUNCTION_EXIT;
	}

	/*
	* run
	*/
	void SubscriptionManager::run()
	{
		while (!m_bTerminate && !m_condition.timedWait(2000))
		{
			if (m_bTerminate)
				break;

			_checkSubscriberState();
		}
	}

	/*
	* terminate
	*/
	void SubscriptionManager::terminate()
	{
		m_bTerminate = true;
		m_condition.signal();

		_cleanUpSubscribers();
	}

	void SubscriptionManager::resetSubscribeAllToic()
	{
		FUNCTION_ENTRY("resubscribeAllTopic");
		for (int idx=0; idx<m_subscribers.size(); idx++)
		{
			m_subscribers[idx]->setState(ResetSubscribe);
		}

		m_condition.signal();
		FUNCTION_EXIT;
	}

	void SubscriptionManager::unsubscribeAllTopic()
	{
		FUNCTION_ENTRY("unsubscribeAllTopic");
		for (int idx=0; idx<m_subscribers.size(); idx++)
		{
			m_subscribers[idx]->setState(UnSubscribe);
		}
		m_condition.signal();
		FUNCTION_EXIT;
	}

	void SubscriptionManager::stopSubscribeAllTopic()
	{
		FUNCTION_ENTRY("stopSubscribeAllTopic");
		for (int idx=0; idx<m_subscribers.size(); idx++)
		{
			m_subscribers[idx]->setState(UnSubscribe);
		}
		m_condition.signal();
		FUNCTION_EXIT;
	}

	void SubscriptionManager::_checkSubscriberState()
	{
		FUNCTION_ENTRY("_checkSubscriberState");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		for (int idx=0; idx<m_subscribers.size(); idx++)
		{
			m_subscribers[idx]->processStateCheck();
		}

		FUNCTION_EXIT;
	}

	std::string SubscriptionManager::_isTopicSubscribed(const std::string& strTopicURL)
	{
		FUNCTION_ENTRY("_isTopicSubscribed");
		std::string sid;
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		for (int idx=0; idx<m_subscribers.size(); idx++)
		{
			if (m_subscribers[idx]->getUri() == strTopicURL)
			{
				sid = m_subscribers[idx]->getSID();
			}
		}

		FUNCTION_EXIT;
		return sid;
	}

	void SubscriptionManager::_cleanUpSubscribers()
	{
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		for (int idx=0; idx<m_subscribers.size(); idx++)
		{
			m_subscribers[idx]->unsubscribe();
		}

		m_subscribers.clear();
	}

} //TA_Base_Bus
