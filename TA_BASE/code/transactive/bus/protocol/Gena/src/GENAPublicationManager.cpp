
#include "GENAPublicationManager.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"

namespace TA_Base_Bus
{
	GENAPublicationManager::GENAPublicationManager(ConnectionFactoryPtr pConnectionMgr)
		:m_pDataStore(0)
	{
		FUNCTION_ENTRY("PublicationManager::Constructor");
		m_pTopicPublisherFactory.reset(new TopicPublisherFactory(pConnectionMgr));

		FUNCTION_EXIT;
	}

	GENAPublicationManager::~GENAPublicationManager(void)
	{
		FUNCTION_ENTRY("PublicationManager::destructor");
		FUNCTION_EXIT;
	}

	/*
	* isTopicURIExist
	*/
	bool GENAPublicationManager::isTopicURIExist(const std::string& topicURI)
	{
		FUNCTION_ENTRY("isTopicURIExist");
		TA_Base_Core::ThreadGuard guard(m_threadLock);

		bool bRetVal = false;

		TopicPublisherMap_T::iterator itFind = m_topicPublishers.find(topicURI);
		if (itFind != m_topicPublishers.end())
		{
			bRetVal = true;
		}

		FUNCTION_EXIT;
		return bRetVal;
	}

	/*
	* addSubscriber
	*/
	unsigned long GENAPublicationManager::addSubscriber(SubscriberInfoPtr subscriber)
	{
		FUNCTION_ENTRY("addSubscriber");
		TA_Base_Core::ThreadGuard guard(m_threadLock);

		unsigned long lRetVal = 0;
		ITopicPublisher* pTopicPub = 0;
		TopicPublisherMap_T::iterator itFind = m_topicPublishers.find(subscriber->topicURI);
		if (itFind == m_topicPublishers.end())
		{	// No publisher has been found for this topic, create a new publisher
			pTopicPub = m_pTopicPublisherFactory->createPublisher(this, subscriber->topicURI);
			m_topicPublishers[subscriber->topicURI] = pTopicPub;
		}
		else
		{
			pTopicPub = itFind->second;
		}

		// Do we have a valid pointer
		if (0 != pTopicPub)
		{
			pTopicPub->subscribe(subscriber);
			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Add subscriber successfuly with SID: %s and url %s", subscriber->subID.c_str(), subscriber->topicURI.c_str());
		}
		else
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Publisher not found for topic: %s", subscriber->topicURI.c_str());
		}
		
		FUNCTION_EXIT;
		return lRetVal;
	}

	/*
	* removeSubscriber
	*/
	void GENAPublicationManager::removeSubscriber(const std::string& topicURI, const std::string& sid)
	{
		FUNCTION_ENTRY("removeSubscriber");
		TA_Base_Core::ThreadGuard guard(m_threadLock);

		ITopicPublisher* pTopicPub = 0;	
		TopicPublisherMap_T::iterator itFind = m_topicPublishers.find(topicURI);
		if (itFind == m_topicPublishers.end())
		{	
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Publisher not found for topicURI: %s", topicURI.c_str());
			//Todo: Throw not found exception
			return;
		}

		pTopicPub = itFind->second;
		if (0 == pTopicPub)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Publisher not found for topicURI: %s", topicURI.c_str());
			//Todo: Throw not found exception
			return;
		}

		pTopicPub->unSubscribe(sid);
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Unsubscribe successfuly with SID: %s", sid.c_str());
		
		FUNCTION_EXIT;
	}

	/*
	* onNotifyUpdate
	*/
	void GENAPublicationManager::publish(GENATopicDataPtr ptopicUpdate)
	{
		FUNCTION_ENTRY("onNotifyUpdate");
		TA_Base_Core::ThreadGuard guard(m_threadLock);

		ITopicPublisher* pTopicPub = 0;
		TopicPublisherMap_T::iterator itFind = m_topicPublishers.find(ptopicUpdate->getTopicURI());
		if (itFind == m_topicPublishers.end())
		{	
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Publisher not found for topic: %s, we will now create a publisher", ptopicUpdate->getTopicURI().c_str());
			pTopicPub = m_pTopicPublisherFactory->createPublisher(this, ptopicUpdate->getTopicURI());
			m_topicPublishers[ptopicUpdate->getTopicURI()] = pTopicPub;
		}
		else
			pTopicPub = itFind->second;

		if (0 == pTopicPub)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Publisher not found for topic: %s", ptopicUpdate->getTopicURI().c_str());
			//Todo: Throw not found exception
			return;
		}

		pTopicPub->publish(ptopicUpdate);
		
		FUNCTION_EXIT;
	}

	void GENAPublicationManager::registerDataStore(IGENATopicDataStore* pDataStore)
	{
		FUNCTION_ENTRY("registerDataStore");
		m_pDataStore = pDataStore;
		FUNCTION_EXIT;
	}
	
	void GENAPublicationManager::resendBirthMessage(const std::string& topicURI, std::string nSID)
	{
		FUNCTION_ENTRY("resendBirthMessage");
		TopicPublisherMap_T::iterator itFind = m_topicPublishers.find(topicURI);
		if (itFind != m_topicPublishers.end())
		{
			ITopicPublisher* pTopicPub = 0;
			pTopicPub = itFind->second;

			pTopicPub->resendBirthMessage(nSID);
		}

		FUNCTION_EXIT;
	}

} //TA_Base_Bus
