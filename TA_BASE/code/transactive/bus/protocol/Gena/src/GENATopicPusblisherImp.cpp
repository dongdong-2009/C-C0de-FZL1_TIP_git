
#include "GENATopicPusblisherImp.h"
#include "GENAConnectionFactory.h"
#include "GENAPublicationManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"


namespace TA_Base_Bus
{
	GENATopicPusblisherImp::GENATopicPusblisherImp(GENAPublicationManager* pubMgr, const std::string& topicURI, ConnectionFactoryPtr pConnectionMgr)
		: m_topicURI(topicURI)
		, m_pConnectionMgr(pConnectionMgr)
		, m_pubMgr(pubMgr)
		, m_bTerminate(false)
		, m_semaphore(0)
	{
		FUNCTION_ENTRY("Constructor");
		start();
		FUNCTION_EXIT;
	}

	/*
	* Destructor
	*/
	GENATopicPusblisherImp::~GENATopicPusblisherImp()
	{
		FUNCTION_ENTRY("Destructor");
		terminateAndWait();
		FUNCTION_EXIT;
	}

	/*
	* Virtual subscribe
	*/
	void GENATopicPusblisherImp::subscribe(SubscriberInfoPtr objSubInfo)
	{
		FUNCTION_ENTRY("subscribe");
		TA_Base_Core::ThreadGuard   Guard(m_publishLock);
		T_SubClientMap::iterator itFind = m_subcribedClients.find(objSubInfo->subID);
		if (itFind == m_subcribedClients.end())
		{
			GENAClientProxy* pNewClient = new GENAClientProxy(objSubInfo, m_pConnectionMgr);
			m_subcribedClients[objSubInfo->subID] = pNewClient;
			//LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "subscribe topic %s SID %n", objSubInfo->topic.c_str(),objSubInfo->subID );
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"subscribe topic %s SID %s", objSubInfo->topic.c_str(),objSubInfo->subID.c_str());
		}

		FUNCTION_EXIT;
	}

	/*
	* Virtual unSubscribe
	*/
	void GENATopicPusblisherImp::unSubscribe(const std::string& nSID)
	{
		FUNCTION_ENTRY("unSubscribe");
		TA_Base_Core::ThreadGuard   Guard(m_publishLock);
		T_SubClientMap::iterator itFind = m_subcribedClients.find(nSID);
		if (itFind != m_subcribedClients.end())
		{
			GENAClientProxy* pClient = itFind->second;
			delete pClient;
			pClient = 0;

			m_subcribedClients.erase(itFind);	
			//LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "unSubscribe SID %n", nSID);
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"unSubscribe SID %s", nSID.c_str());
		}

		FUNCTION_EXIT;
	}

	/*
	* Virtual publish
	*/
	void GENATopicPusblisherImp::publish(GENATopicDataPtr ptopicUpdate, std::string nSID /*= 0*/)
	{
		FUNCTION_ENTRY("publish");

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Publish topic %s SID %s", ptopicUpdate->getTopicName().c_str(),nSID.c_str());

		//LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Publish topic %s, SID %n", ptopicUpdate->getTopicName().c_str(),nSID);
		TA_Base_Core::ThreadGuard   Guard(m_publishLock);
		if (nSID == "")
		{
			
			T_SubClientMap::iterator itr = m_subcribedClients.begin();
			int nSize=m_subcribedClients.size();
			for (itr; itr != m_subcribedClients.end(); itr++)
			{
				GENAClientProxy* pClient = itr->second;
				pClient->insert(ptopicUpdate);
			}
		}
		else
		{
			T_SubClientMap::iterator itr = m_subcribedClients.find(nSID);
			if (itr != m_subcribedClients.end())
			{
				GENAClientProxy* pClient = itr->second;
				pClient->insert(ptopicUpdate);
			}
			else
				LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to find client proxy for SID: %s", nSID.c_str());
				
		}

		FUNCTION_EXIT;
	}

	void GENATopicPusblisherImp::run()
	{
		FUNCTION_ENTRY("run");
		while (!m_bTerminate)
		{
			m_semaphore.wait();
			if (m_bTerminate)
				break;

			GENAClientProxy* pClient = NULL;
			{
				TA_Base_Core::ThreadGuard guard(m_threadLock);
				pClient = m_queueClients.front();
				m_queueClients.pop();
			}

			if (0 != pClient)
			{	// Then we can send out birth messages to this client
				IGENATopicDataStore* pDataStore = m_pubMgr->getDataStore();
				if (0 != pDataStore)
				{
					// Sending initial birth data from datasource
					T_GENATopicDataList  objDataList = pDataStore->getAllData(m_topicURI);
					pClient->clearQueue();//remove all queued messages
					LOG3(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Sending birth messages for subscriber with SID: %s and url %s data Count %d", pClient->getSID().c_str(), m_topicURI.c_str(), objDataList.size());

					for (int idx=0; idx<objDataList.size() && !m_bTerminate; idx++)
					{
						GENATopicDataPtr pData = objDataList[idx];

						GENATopicDataPtr pNewData(new GENATopicData(pData->getObjectID(), pData->getTopicName()));
						pNewData->setTopicURI(pData->getTopicURI());
						pNewData->setUpdateType(GENA_BIRTH);
						pNewData->setAttributes(pData->getAttributes());

						pClient->insert(pNewData);
					}
				}
				else
					LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "No Data Store registered!, will not send birth message");
			}
		}
		FUNCTION_EXIT
	}
	
	void GENATopicPusblisherImp::terminate()
	{
		FUNCTION_ENTRY("terminate");
		m_bTerminate = true;
		m_semaphore.post();

		FUNCTION_EXIT
	}

	void GENATopicPusblisherImp::resendBirthMessage(std::string nSID)
	{
		FUNCTION_ENTRY("terminate");
		TA_Base_Core::ThreadGuard guardPublish(m_publishLock);
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		T_SubClientMap::iterator itr = m_subcribedClients.find(nSID);
		if (itr != m_subcribedClients.end())
		{
			GENAClientProxy* pClient = itr->second;
			m_queueClients.push(pClient);
			m_semaphore.post();
		}		

		FUNCTION_EXIT
	}
} //TA_Base_Bus
