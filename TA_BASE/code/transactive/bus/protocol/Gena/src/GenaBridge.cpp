/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GenaBridge.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/08/14 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the implementation for GenaBridge
  */

#include "GenaBridge.h"

#include "GenaServer.h"
#include "GenaClient.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Bus
{
	/*
	* Constructor
	*/
	GenaBridge::GenaBridge(const GenaServerParams& serverParams, 
		const GenaClientParams& clientParams)
		: m_pDataStore(0)
	{
		FUNCTION_ENTRY("GenaBridge");

		// Create DataStore
		m_pDataStore = new GENATopicDataStore();
		m_pDataStore->registerDataChangeObserver(this);

		// Create Server
		m_pGenaServer = boost::shared_ptr<GenaServer>(new GenaServer(serverParams));
		
		// Create Client
		m_pGenaClient = boost::shared_ptr<GenaClient>(new GenaClient(clientParams));

		m_pGenaServer->registerDataStore(m_pDataStore);
		m_pGenaClient->registerDataStoreTopicObserver(m_pDataStore);
		//Register Request Forwarder
		m_pGenaServer->registerForwarder(this);

		FUNCTION_EXIT;
	}

	/*
	* Destructor
	*/
	GenaBridge::~GenaBridge()
	{
		FUNCTION_ENTRY("GenaBridge");
		m_pDataStore->unRegisterDataChangeObserver(this);
		delete m_pDataStore;
		m_pDataStore = 0;

		stopService();
		destroyService();
		FUNCTION_EXIT;
	}

	/*
	* initialize
	*/
	void GenaBridge::startService()
	{
		FUNCTION_ENTRY("GenaBridge");
		m_pGenaClient->startService();
		m_pGenaServer->startService();

		FUNCTION_EXIT;
	}

	void GenaBridge::stopService()
	{
		FUNCTION_ENTRY("stopService");
		m_pGenaClient->stopService();
		m_pGenaServer->stopService();
		FUNCTION_EXIT;
	}

		
	void GenaBridge::destroyService()
	{
		FUNCTION_ENTRY("destroyService");
		if (0 != m_pDataStore)
		{
			delete m_pDataStore;
			m_pDataStore = 0;
		}

		FUNCTION_EXIT;
	}


	/*
	* getServerInstance
	*/
	
	boost::shared_ptr<GenaServer> GenaBridge::getServerInstance()
	{
		FUNCTION_ENTRY("getServerInstance");
		FUNCTION_EXIT;
		return m_pGenaServer;
	}

	/*
	* getClientInstance
	*/
	boost::shared_ptr<GenaClient> GenaBridge::getClientInstance()
	{
		FUNCTION_ENTRY("getClientInstance");
		FUNCTION_EXIT;
		return m_pGenaClient;
	}
	
	/*
	* Virtual subscribe
	*/
	std::string GenaBridge::subscribe(const std::string& topicURL, const std::string& notification)
	{
		FUNCTION_ENTRY("subscribe");
		std::string strSID = "";
		
		T_SubUrlSIDMap::iterator itFind = m_subUrlSIDs.find(topicURL);
		if (m_subUrlSIDs.end() == itFind)
		{
			strSID = m_pGenaClient->subscribe(topicURL, notification);
			m_subUrlSIDs[topicURL] = strSID;

			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Subscription successful with ID=%s", strSID.c_str());
		}
		else
			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Already have a subscription to topic: %s with SID: %s", topicURL.c_str(), itFind->second.c_str());

		FUNCTION_EXIT;
		return strSID;
	}

	/*
	* Virtual unsubscribe
	*/
	void GenaBridge::unsubscribe(const std::string& topicURL)
	{
		FUNCTION_ENTRY("unsubscribe");
		T_SubUrlSIDMap::iterator itFind = m_subUrlSIDs.find(topicURL);
		if (m_subUrlSIDs.end() != itFind)
		{
			std::string ulSID = itFind->second;
			m_pGenaClient->unsubscribe(ulSID);
			m_subUrlSIDs.erase(topicURL);

			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "UnSubscription successful with ID=%s and URL %d", ulSID.c_str(), topicURL.c_str());
		}
		else
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Unable to find topic: %s", topicURL.c_str());
	
		FUNCTION_EXIT;
	}

	/*
	* Virtual onNotifyUpdate
	*/
	/*void GenaBridge::onNotifyUpdate(const TopicUpdate& topicUpdate)
	{
		FUNCTION_ENTRY("onNotifyUpdate");
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Receive update notification for topic %s", topicUpdate.topicURI.c_str());
		m_pGenaServer->publishUpdate(topicUpdate);

		FUNCTION_EXIT;
	}*/

	void GenaBridge::onDataChange(const std::string& topicName,  GENATopicDataPtr topicData)
	{
		FUNCTION_ENTRY("onNotifyUpdate");
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Receive update notification for topic %s", topicData->getTopicURI().c_str());
		m_pGenaServer->publishUpdate(topicData);

		FUNCTION_EXIT;
	}

}
