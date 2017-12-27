
#include "GenaServerImp.h"
#include "GENAMethod.h"
#include "GENAPublicationManager.h"
#include "IGENAClientStatusForwarder.h"
#include "ServerModeException.h"
#include "SIDNotFoundException.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include <time.h>

using namespace TA_Base_Core;

namespace TA_Base_Bus
{
	GenaServerImp::GenaServerImp(ConnectionFactoryPtr pConnectionMgr,GenaServerParams serverParameter)
		: m_serverState(Active)		
	{
		FUNCTION_ENTRY("GenaServerImp Constructor");
		m_publicationMgr.reset(new GENAPublicationManager(pConnectionMgr));
		m_serverParameter=serverParameter;
		FUNCTION_EXIT;
	}

	GenaServerImp::~GenaServerImp(void)
	{
		FUNCTION_ENTRY("GenaServerImp Destructor");
		terminateAndWait();

		FUNCTION_EXIT;
	}

	void GenaServerImp::startService()
	{
		FUNCTION_ENTRY("startService");
		m_serverState = Active;
		start();
		FUNCTION_EXIT;
	}
	
	void GenaServerImp::stopService()
	{
		FUNCTION_ENTRY("stopService");
		m_serverState = Passive;
		terminateAndWait();
		_cleanUp();
		FUNCTION_EXIT;
	}


	void GenaServerImp::publishUpdate(GENATopicDataPtr ptopicUpdate)
	{
		FUNCTION_ENTRY("GenaServerImp Destructor");
		if (m_serverState != Active)
			throw ServerModeException("Server received a request from client while not in active mode");

		m_publicationMgr->publish(ptopicUpdate);

		FUNCTION_EXIT;
	}

	std::string GenaServerImp::subscribe(GENARequestPtr objRequest)
	{
		FUNCTION_ENTRY("subscribe");
		if (m_serverState != Active)
			throw ServerModeException("Server received a request from client while not in active mode");

		TA_Base_Core::ThreadGuard guard(m_threadLock);
	
		unsigned long lRetVal=0;
		std::string sid="";

		if (!objRequest->getHeaderValue(HEAD_SID).empty())
		{   // This is a re-subscription
			GENARenewSubscribeMethodPtr objTemp = boost::dynamic_pointer_cast<GENARenewSubscribeMethod>(objRequest);
			sid = objTemp->getSID();
			T_SubscriptionInfoMap::iterator itFind = m_subscriptions.find(sid);
			if (m_subscriptions.end() == itFind)
			{
				LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to find SID: %s from client: %s", objRequest->getHeaderValue(HEAD_SID).c_str(), objRequest->getHeaderValue(HEAD_HOST).c_str())
				throw SIDNotFoundException("SID not found");
			}
			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "renew subscriber to find SID: %s from client: %s", sid.c_str(), objRequest->getHeaderValue(HEAD_HOST).c_str())
			// Determine the expiry time
			time_t timeNow;
			time(&timeNow);
			time_t expiryTime = timeNow + (m_serverParameter.subscriptionTimeOut/1000) ; //Convert to seconds

			m_subUpdateList[sid] = expiryTime; //Update time			
		}
		else // This is a new subscriber
		{
			GENASubscribeMethodPtr objTemp = boost::dynamic_pointer_cast<GENASubscribeMethod>(objRequest);
			//Todo: Verify that the subscriber has no duplicate
			T_SubscriptionInfoMap::iterator itr = m_subscriptions.begin();
			for (itr; itr!=m_subscriptions.end(); itr++)
			{
				SubscriberInfoPtr pSubTemp = itr->second;
				if (pSubTemp->callback == objTemp->getCallBackAddress() 
					&& pSubTemp->topicURI == objTemp->getTopicURI())
				{	// Already have existing subscriber return the existing SID instead
					
					// Determine the expiry time
					time_t timeNow;
					time(&timeNow);
					time_t expiryTime = timeNow + DEFAULT_SUBSCRIPTION_TIMEOUT;

					m_subUpdateList[pSubTemp->subID] = expiryTime; // set to current time
					LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Client: %s is already subscribed with SID %s", objRequest->getHeaderValue(HEAD_HOST).c_str(), pSubTemp->subID.c_str());
					return pSubTemp->subID;
				}
			}
			
			SubscriberInfoPtr subInfo(new SubscriberInfo());
			sid=m_sidMgr.getNextSID(); // Acquire new SID
			subInfo->subID = sid;
			
			extractTopic(objTemp->getTopicURI(), *subInfo);
			subInfo->host = objTemp->getHost();
			subInfo->notification = objTemp->getNotifyType(); // objRequest.msgHeader.get(HEAD_NT);
			subInfo->timeout = objTemp->getHeaderValue(HEAD_TIMEOUT); //objRequest.msgHeader.get(HEAD_TIMEOUT);
			subInfo->callback = objTemp->getCallBackAddress(); //objRequest.msgHeader.get(HEAD_CALLBACK);

			m_publicationMgr->addSubscriber(subInfo);

			m_subscriptions[subInfo->subID] = subInfo; //Add to the master list
			
			// Determine the expiry time
			time_t timeNow;
			time(&timeNow);
			time_t expiryTime = timeNow + (m_serverParameter.subscriptionTimeOut/1000);
			
			m_subUpdateList[subInfo->subID] = expiryTime; // set to current time
			//sid = subInfo->subID;
			LOG3(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "new subscriber  SID: %s from client: %s, url:%s", sid.c_str(), objRequest->getHeaderValue(HEAD_HOST).c_str(),objTemp->getTopicURI().c_str() );
			doForwardSubscribeRequest(objTemp->getTopicURI(), subInfo->notification);
		}

		FUNCTION_EXIT;
		return sid;
	}

	void GenaServerImp::extractTopic(const std::string& uri, SubscriberInfo& subInfo)
	{
		FUNCTION_ENTRY("extractTopic");
		if (uri.empty())
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The URI provided from the request is empty.");
			FUNCTION_EXIT;
			return;
		}

		std::string::size_type startPos = uri.find(SLASH_DELIMETER);
		if (startPos == std::string::npos)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid URI provided from the request. URI: %s", uri.c_str());
			FUNCTION_EXIT;
			return;
		}

		//Now to extract the topic
		std::string::size_type endPos = uri.find(QUESTION_DELIMETER);
		if (endPos != std::string::npos)
		{	// we have a filter
			subInfo.filters = uri.substr(endPos+1, uri.length()-1);
		}
		else
		{
			endPos = uri.length() - 1;
		}

		subInfo.topic = uri.substr(startPos + 1, endPos);
		subInfo.topicURI = uri;
		FUNCTION_EXIT;
	}

	bool GenaServerImp::unsubscribe(GENARequestPtr objRequest)
	{
		FUNCTION_ENTRY("unSubscribe");
		if (m_serverState != Active)
			throw ServerModeException("Server received a request from client while not in active mode");

		TA_Base_Core::ThreadGuard guard(m_threadLock);

		if (objRequest->getHeaderValue(HEAD_SID).empty())
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Request from client: %s does not have any SID.", objRequest->getHeaderValue(HEAD_HOST).c_str());
			//Todo: Throw Invalid request exception 
			return false;
		}
		GENAUnsubscribeMethodPtr objTemp = boost::dynamic_pointer_cast<GENAUnsubscribeMethod>(objRequest);
		LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Unsubscribe Request received for Topic: %s, SID: %s", objTemp->getTopicURI().c_str(), objTemp->getSID().c_str());

		std::string sid = objTemp->getSID();
		T_SubscriptionInfoMap::iterator itFind = m_subscriptions.find(sid);
		if (m_subscriptions.end() != itFind)
		{
			SubscriberInfoPtr pInfo = itFind->second;
			
			m_publicationMgr->removeSubscriber(pInfo->topicURI, pInfo->subID);
			m_subscriptions.erase(sid);
			m_subUpdateList.erase(sid);

			doForwardUnSubscribeRequest(pInfo->topicURI);
		}
		else
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unable to find subscriber with ID %s", objTemp->getSID().c_str());
			//Todo: Throw not found exception
			return false;
		}
		
		FUNCTION_EXIT;
		return true;
	}

	void GenaServerImp::notify(GENARequestPtr objRequest)
	{
		//Not implemented
	}

	void GenaServerImp::run()
	{
		FUNCTION_ENTRY("run");
		
		while (!m_condition.timedWait(2*1000)) // Check every 2 seconds
		{
			TA_Base_Core::ThreadGuard guard(m_threadLock);

			unsigned long curTime = time(0); // Get current time
			T_SubscribeUpdateList::iterator itr = m_subUpdateList.begin();
			while (itr!=m_subUpdateList.end())
			{
				time_t timeNow;
				time(&timeNow);

				if (itr->second < timeNow)
				{
					std::string sid = itr->first;
					LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Subscriber has failed to resubscribe and expired: SID=%s", itr->first.c_str());
					++itr;
					try
					{						
						m_publicationMgr->removeSubscriber(m_subscriptions[sid]->topicURI, sid);
						doForwardUnSubscribeRequest(m_subscriptions[sid]->topicURI);
					}
					catch(...)
					{
						LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception occurred while trying to remove subscriber");
					}

					m_subscriptions.erase(sid);
					m_subUpdateList.erase(sid);
					
				}
				else
					++itr;
			}
		}

		FUNCTION_EXIT;
	}

	void GenaServerImp::terminate()
	{
		FUNCTION_ENTRY("terminate");
		m_condition.signal();
		FUNCTION_EXIT;
	}

	void GenaServerImp::subscribeToRequest(IGENAClientStatusForwarder* pReqFwdr)
	{
		FUNCTION_ENTRY("subscribeToRequest");
		m_requestFwdr.push_back(pReqFwdr);
		FUNCTION_EXIT;
	}

	void GenaServerImp::doForwardSubscribeRequest(const std::string& topicUri, const std::string& notification)
	{
		FUNCTION_ENTRY("doForwardSubscribeRequest");
		if (m_requestFwdr.size() != 0)
		{
			for (int idx=0; idx<m_requestFwdr.size(); idx++)
			{	
				m_requestFwdr[idx]->subscribe(topicUri, notification);
			}
		}
		FUNCTION_EXIT;	
	}

	void GenaServerImp::doForwardUnSubscribeRequest(const std::string& topicUri)
	{
		FUNCTION_ENTRY("doForwardUnSubscribeRequest");
		if (m_requestFwdr.size() != 0)
		{
			for (int idx=0; idx<m_requestFwdr.size(); idx++)
			{	
				m_requestFwdr[idx]->unsubscribe(topicUri);
			}
		}
		FUNCTION_EXIT;	
	}

	void GenaServerImp::registerDataStore(IGENATopicDataStore* pDataStore)
	{
		FUNCTION_ENTRY("registerDataStore");
		m_publicationMgr->registerDataStore(pDataStore);
		FUNCTION_EXIT;
	}

	void GenaServerImp::requestBirthMessage(const std::string& topicURI, std::string nSID)
	{
		FUNCTION_ENTRY("registerDataStore");
		m_publicationMgr->resendBirthMessage(topicURI, nSID);
		FUNCTION_EXIT;
	}

	void GenaServerImp::_cleanUp()
	{
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		T_SubscriptionInfoMap::iterator itr = m_subscriptions.begin();
		for (itr; itr!=m_subscriptions.end(); itr++)
		{
			SubscriberInfoPtr pInfo = itr->second;
			m_publicationMgr->removeSubscriber(pInfo->topicURI, pInfo->subID);
		}

		m_subUpdateList.clear();
		m_subscriptions.clear(); // remove all subscribers
	}
}//TA_Base_Bus
