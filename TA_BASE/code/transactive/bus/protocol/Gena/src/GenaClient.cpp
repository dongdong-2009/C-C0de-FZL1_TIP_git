

#include "GenaClient.h"
#include "SubscriptionManager.h"
#include "GenaCallbackProccessor.h"
#include "GENAHttpServer.h"
#include "Poco/Net/HTTPServerParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "GENAConnectionFactory.h"

using Poco::Net::HTTPServerParams;

namespace TA_Base_Bus
{
	
	GenaClient::GenaClient(const GenaClientParams& pClientParams)
		: m_pCallBackProc(0) 
		, m_pSubscriptionMgr(0)
		, m_pHttpServer(0)
	{
		FUNCTION_ENTRY("Constructor");

		// create callback processor
		m_pCallBackProc = new GenaCallbackProccessor();

		// Set up callback server parameters
		HTTPServerParams::Ptr pParams = new HTTPServerParams();
		pParams->setServerName(pClientParams.callbackInfo.serverName);
		pParams->setKeepAlive(pClientParams.callbackInfo.keepAlive);	

		Poco::Timespan timeout(0,pClientParams.callbackInfo.timeout*1000);  //milliSeconds to microseconds
		pParams->setKeepAliveTimeout(timeout);
		pParams->setTimeout(timeout);

		pParams->setMaxKeepAliveRequests(0);



		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Creating callback serverName: %s, port: %d, KeepAliveTimout(Ms):%d, Timeout(Ms):%d",
			   pClientParams.callbackInfo.serverName.c_str(), pClientParams.callbackInfo.serverPort, pClientParams.callbackInfo.timeout, pClientParams.callbackInfo.timeout);
		
		// Instantiate callback server
		m_pHttpServer = new GENAHttpServer(pClientParams.callbackInfo.ipAddress, 
			pClientParams.callbackInfo.serverPort, pParams, m_pCallBackProc);

		// Set Server connections
		m_pConnectionMgr.reset(new GENAConnectionFactory());
		m_pConnectionMgr->setServerConnections(pClientParams.sourceServers);

		// create subscription manager
		m_pSubscriptionMgr = new SubscriptionManager(pClientParams, m_pCallBackProc, m_pConnectionMgr);
	
		start();
		FUNCTION_EXIT;
	}

	GenaClient::~GenaClient()
	{
		FUNCTION_ENTRY("Destructor");
		stopService();
		destroyService();
		FUNCTION_EXIT;
	}
	
	void GenaClient::run()
	{
		FUNCTION_ENTRY("run");

		// Start HTTP Server
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Starting Client Callback Server.");
		m_pHttpServer->start();

		FUNCTION_EXIT;
	}

	void GenaClient::terminate()
	{
		FUNCTION_ENTRY("terminate");
		FUNCTION_EXIT;
	}

	/*
	* subscribe
	*/
	std::string GenaClient::subscribe(const std::string& strTopicName, const std::string& notification,
		const T_TopicFilterSet& filterSet)
	{
		FUNCTION_ENTRY("subscribe");
		std::string ulSID = m_pSubscriptionMgr->subscribe(strTopicName, notification, filterSet);
		
		FUNCTION_EXIT;
		return ulSID;
	}

	/*
	* addSubscribeTopic
	*/
	void GenaClient::addSubscribeTopic(const std::string& topicURL, const std::string& notification)
	{
		FUNCTION_ENTRY("addSubscribeTopic");
		m_pSubscriptionMgr->addSubscribeTopic(topicURL, notification);
		FUNCTION_EXIT;
	}

	/*
	* subscribe
	*/
	std::string GenaClient::subscribe(const std::string& topicURL, const std::string& notification)
	{
		FUNCTION_ENTRY("subscribe");
		std::string ulSID = m_pSubscriptionMgr->subscribe(topicURL, notification);

		FUNCTION_EXIT;
		return ulSID;
	}

	/*
	* unsubscribe
	*/
	void GenaClient::unsubscribe(const std::string& sid)
	{
		FUNCTION_ENTRY("unsubscribe");
		m_pSubscriptionMgr->unsubscribe(sid);

		FUNCTION_EXIT;
	}

	void GenaClient::resetSubscribeTopic(const std::string& sid)
	{
		FUNCTION_ENTRY("resetSubscribeTopic");
		m_pSubscriptionMgr->resetSubscribeTopic(sid);
		FUNCTION_EXIT;
	}

	void GenaClient::unSubscribeAllTopic()
	{
		FUNCTION_ENTRY("unSubscribeAllTopic");
		m_pSubscriptionMgr->unsubscribeAllTopic();
		FUNCTION_EXIT;
	}

	void GenaClient::resetSubscribeAllTopic()
	{
		FUNCTION_ENTRY("resetSubscribeAllTopic");
		m_pSubscriptionMgr->resetSubscribeAllToic();
		FUNCTION_EXIT;
	}

	void GenaClient::startService()
	{
		FUNCTION_ENTRY("startService");
		m_pConnectionMgr->start();
		m_pSubscriptionMgr->start();		
		FUNCTION_EXIT;
	}

	void GenaClient::stopService()
	{
		FUNCTION_ENTRY("stopService");
		m_pSubscriptionMgr->terminateAndWait();
		m_pConnectionMgr->terminateAndWait();
		FUNCTION_EXIT;
	}

	void GenaClient::destroyService()
	{
		FUNCTION_ENTRY("destroyService");
		terminateAndWait();
		if (0 != m_pHttpServer)
		{
			m_pHttpServer->stop();
			delete m_pHttpServer;
			m_pHttpServer=0;
		}
		FUNCTION_EXIT;
	}

	void GenaClient::registerServerStatusObserver(IServerStatusObserver* pObserver)
	{
		FUNCTION_ENTRY("registerServerStatusObserver");

		FUNCTION_EXIT;
	}

	void GenaClient::registerConnStatusObserver(IConnStateListener* pObserver)
	{
		FUNCTION_ENTRY("registerServerStatusObserver");
		m_pConnectionMgr->registerStateObserver(pObserver);

		FUNCTION_EXIT;
	}

	void GenaClient::deRegisterConnStatusObserver(IConnStateListener* pObserver)
	{
		FUNCTION_ENTRY("registerServerStatusObserver");
		m_pConnectionMgr->unregisterStateObserver(pObserver);

		FUNCTION_EXIT;
	}


	void GenaClient::registerDataStoreTopicObserver(IGENATopicDataStore* pGenaDataStore)
	{
		FUNCTION_ENTRY("registerServerStatusObserver");
		m_pCallBackProc->registerNotifyUpdate(pGenaDataStore);
		FUNCTION_EXIT;
	}

	void GenaClient::deRegisterDataStoreTopicObserver(IGENATopicDataStore* pGenaDataStore)
	{
		FUNCTION_ENTRY("registerServerStatusObserver");
		m_pCallBackProc->unRegisterNotifyUpdate(pGenaDataStore);
		FUNCTION_EXIT;
	}

// 	void GenaClient::registerMessageObserver()
// 	{
// 		FUNCTION_ENTRY("registerServerStatusObserver");
// 		FUNCTION_EXIT;
// 	}

} //TA_Base_Bus
