
#include "GenaCallbackProccessor.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "GENATopicData.h"
#include "CommonDef.h"
#include "GENAEvent.h"
#include <string>
#include <time.h>

namespace TA_Base_Bus
{
	
	GenaCallbackProccessor::GenaCallbackProccessor()
		: m_pSyncStateMonitor(0)
	{
		FUNCTION_ENTRY("Constructor");
		start();
		m_pSyncStateMonitor = new SyncStateMonitor(*this);
		FUNCTION_EXIT;
	}
	GenaCallbackProccessor::~GenaCallbackProccessor()
	{
		FUNCTION_ENTRY("Destructor");
		terminateAndWait();
		if (m_pSyncStateMonitor != 0)
		{
			delete m_pSyncStateMonitor;
			m_pSyncStateMonitor = 0;
		}
		FUNCTION_EXIT;
	}

	void GenaCallbackProccessor::registerNotifyUpdate(IGENATopicDataStore* pTopicDataStore)
	{
		FUNCTION_ENTRY("registerNotifyUpdate");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		T_TopicDataStoreList::iterator itFind = m_topicDataStoreList.find(pTopicDataStore);
		if (itFind != m_topicDataStoreList.end())
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "topic data store is already registered");
			return;
		}

		m_topicDataStoreList.insert(pTopicDataStore);
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Registered new topic data store");
		FUNCTION_EXIT;
	}

	void GenaCallbackProccessor::unRegisterNotifyUpdate(IGENATopicDataStore* pTopicDataStore)
	{
		FUNCTION_ENTRY("unRegisterNotifyUpdate");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		T_TopicDataStoreList::iterator itFind = m_topicDataStoreList.find(pTopicDataStore);
		if (itFind == m_topicDataStoreList.end())
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "topic data store was not found");
			return;
		}

		m_topicDataStoreList.erase(itFind);
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "unRegistered successful for topic data store");
		FUNCTION_EXIT;
	}

// 	std::map<unsigned long , ITopicListener* > GenaCallbackProccessor::getTopicDataStores()
// 	{
// 		FUNCTION_ENTRY("getTopicListeners");
// 		TA_Base_Core::ThreadGuard guard(m_threadLock);
// 		FUNCTION_EXIT;
// 		return m_topicDataStoreList;
// 	}
// 
// 	void GenaCallbackProccessor::resetTopicListeners(std::map<unsigned long , ITopicListener* > topicListeners)
// 	{
// 		FUNCTION_ENTRY("resetTopicListeners");
// 		TA_Base_Core::ThreadGuard guard(m_threadLock);
// 		m_topicDataStoreList.swap(topicListeners);
// 		FUNCTION_EXIT;
// 	}

	/*
	* subscribe
	*/
	std::string GenaCallbackProccessor::subscribe(GENARequestPtr objRequest)
	{
		FUNCTION_ENTRY("subscribe");
		//Todo: Throw no implement
		FUNCTION_EXIT;
		return "";
	}

	/*
	* unsubscribe
	*/
	bool GenaCallbackProccessor::unsubscribe(GENARequestPtr objRequest)
	{
		FUNCTION_ENTRY("unsubscribe");
		//Todo: Throw no implement
		FUNCTION_EXIT;
		return false;
	}

	/*
	* notify
	*/
	void GenaCallbackProccessor::notify(GENARequestPtr objRequest)
	{
		FUNCTION_ENTRY("notify");
		// Cast it to the correct data type.
		GENAEventPtr pEvent = boost::static_pointer_cast<GENAEvent>(objRequest);
		
		GENATopicDataPtr pData(new GENATopicData(pEvent->getContent()));
		pData->setTopicURI(pEvent->getTopicURI());
		pData->setUpdateType(pEvent->getNotifyType());

		insert(pData);

// 		if (itFind == m_topicDataStoreList.end())
// 		{
// 			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Listener from subscriber %lu is was not found", ulSID);
// 			return;
// 		}
		
		FUNCTION_EXIT;
	}

	std::string GenaCallbackProccessor::extractTopic(const std::string& uri)
	{
		FUNCTION_ENTRY("extractTopic");
		std::string strRetVal = "";
		if (uri.empty())
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The URI provided from the request is empty.");
			FUNCTION_EXIT;
			return strRetVal;
		}

		std::string::size_type startPos = uri.find(SLASH_DELIMETER);
		if (startPos == std::string::npos)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid URI provided from the request. URI: %s", uri.c_str());
			FUNCTION_EXIT;
			return strRetVal;
		}

		//Now to extract the topic
		std::string::size_type endPos = uri.find(QUESTION_DELIMETER);
		if (endPos == std::string::npos)
		{
			endPos = uri.length() - 1;
		}

		strRetVal = uri.substr(startPos + 1, endPos);
		FUNCTION_EXIT;

		return strRetVal;
	}

	void GenaCallbackProccessor::processEvent( boost::shared_ptr<GENATopicData> ptopicUpdate )
	{	
		FUNCTION_ENTRY("processEvent");
		//2. notify appropriate listener
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		_processSyncUpdate(ptopicUpdate->getTopicName(), ptopicUpdate->getUpdateType());

		T_TopicDataStoreList::iterator itr = m_topicDataStoreList.begin();
		for (itr; itr!=m_topicDataStoreList.end(); itr++)
		{
			IGENATopicDataStore* pDataStore = *itr;
			// goodness we must notify now
			pDataStore->updateData(ptopicUpdate->getTopicName(), ptopicUpdate);
		}

		FUNCTION_EXIT;
	}

	void GenaCallbackProccessor::resetTopicSync(const std::string& topicUri)
	{
		FUNCTION_ENTRY("resetTopicSync");
		std::string topicName=convertToUpperCase(extractTopic(topicUri));
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Syncronization requested for topic %s", topicName.c_str());
		T_TopicSyncStateMap::iterator itFind = m_topicInitList.find(topicName);
		if (itFind != m_topicInitList.end())
		{
			itFind->second->state = SyncReq; // Synchronization requested
			itFind->second->lastBirthMsg = time(0);
		}
		else
		{
			TopicSycnInfoPtr newInfo(new TopicSycnInfo);
			newInfo->topicName = topicName;
			newInfo->state = SyncReq;
			newInfo->lastBirthMsg = time(0);
			m_topicInitList[topicName] = newInfo;
		}

		FUNCTION_EXIT;
	}

	void GenaCallbackProccessor::_processSyncUpdate(const std::string& topicName, const std::string& updateType)
	{
		FUNCTION_ENTRY("_processSyncUpdate");
		T_TopicSyncStateMap::iterator itFind = m_topicInitList.find(topicName);
		if (itFind != m_topicInitList.end())
		{	// Found it
			if (updateType == GENA_BIRTH)
			{	
				if (itFind->second->state == SyncReq)
				{
					// Trigger Sync Begin
					T_TopicDataStoreList::iterator itr = m_topicDataStoreList.begin();
					for (itr; itr!=m_topicDataStoreList.end(); itr++)
					{
						IGENATopicDataStore* pDataStore = *itr;
						pDataStore->synchronizeBegin(topicName);
					}
					// Update to sync begin
					itFind->second->state = SyncBegin;
					itFind->second->lastBirthMsg = time(0);
				}
				else if (itFind->second->state == SyncBegin)
				{	// update the last known birth time
					itFind->second->lastBirthMsg = time(0);
				}
				//What to do if we receive birth while the state is in SyncEnd???
			}
			else if (updateType != GENA_BIRTH && itFind->second->state == SyncBegin)
			{	// Trigger Sync End
				T_TopicDataStoreList::iterator itr = m_topicDataStoreList.begin();
				for (itr; itr!=m_topicDataStoreList.end(); itr++)
				{
					IGENATopicDataStore* pDataStore = *itr;
					pDataStore->synchronizeEnd(topicName);
				}
				// Update to sync begin
				itFind->second->state = SyncEnd;
			}
		}
		FUNCTION_EXIT;
	}

	void GenaCallbackProccessor::checkSyncState()
	{
		FUNCTION_ENTRY("checkSyncState");
		T_TopicSyncStateMap::iterator itr = m_topicInitList.begin();
		for (itr; itr!=m_topicInitList.end(); itr++)
		{
			if (itr->second->state == SyncBegin)
			{	// Check if it exceed the wait time
				time_t now;
				time(&now);
				double seconds = difftime(now, itr->second->lastBirthMsg);
				if (seconds >= 5) // 5 seconds
				{	// Notify Sync End
					T_TopicDataStoreList::iterator itrStore = m_topicDataStoreList.begin();
					for (itrStore; itrStore!=m_topicDataStoreList.end(); itrStore++)
					{
						IGENATopicDataStore* pDataStore = *itrStore;
						pDataStore->synchronizeEnd(itr->second->topicName);
					}
					itr->second->state = SyncEnd;
				}
			}
		}

		FUNCTION_EXIT;
	}

	GenaCallbackProccessor::SyncStateMonitor::SyncStateMonitor(GenaCallbackProccessor& processor)
		: m_processor(processor)
		, m_bTerminate(false)
	{
		start();
	}

	GenaCallbackProccessor::SyncStateMonitor::~SyncStateMonitor()
	{
		terminateAndWait();
	}

	void GenaCallbackProccessor::SyncStateMonitor::run()
	{
		FUNCTION_ENTRY("SyncStateMonitor::run");
		while (!m_bTerminate && !m_condition.timedWait(1000))
		{
			if (m_bTerminate)
				break;

			m_processor.checkSyncState();
		}

		FUNCTION_EXIT;
	}

	void GenaCallbackProccessor::SyncStateMonitor::terminate()
	{
		m_bTerminate = true;
		m_condition.signal();
	}
}
