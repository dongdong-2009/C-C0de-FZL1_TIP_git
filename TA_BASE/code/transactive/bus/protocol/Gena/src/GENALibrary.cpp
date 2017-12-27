/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GENALibrary.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/08/14 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the interface declaration for ISCSGenaHelper
  */

#include "GENALibrary.h"
#include "GenaConfigHelper.h"
#include "GenaConfigException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

namespace TA_Base_Bus
{

	 
	void HttpServicelog::debug(std::string& log)
	{
		LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"%s %s","HttpServicelog", log.c_str());
	}

	void HttpServicelog::info(std::string& log)
	{
		LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"%s %s","HttpServicelog", log.c_str());
	}

	void HttpServicelog::error(std::string& log)
	{
		LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"%s %s","HttpServicelog", log.c_str());
	}
	 


	GENALibrary* GENALibrary::m_pInstance = 0;
	/*
	* Constructor
	*/
	GENALibrary::GENALibrary()
	{
		FUNCTION_ENTRY("Constructor");
		
		FUNCTION_EXIT;
	}
		
	/*
	* Destructor
	*/
	GENALibrary::~GENALibrary()
	{
		FUNCTION_ENTRY("Destructor");
		stopService();
		FUNCTION_EXIT;
	}

	/*
	* getInstance
	*/
	GENALibrary& GENALibrary::getInstance()
	{
		FUNCTION_ENTRY("getInstance");
		if (m_pInstance == 0)
		{
			m_pInstance = new GENALibrary();
			 HttpServicelog* log=new  HttpServicelog();
			 LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "register HttpServicelog");
			 Poco::POCOLogBridge::registerlogCallBack(log);
		}
		FUNCTION_EXIT;
		return *m_pInstance;
	}

	/*
	* releaseInstance
	*/
	void GENALibrary::releaseInstance()
	{
		FUNCTION_ENTRY("releaseInstance");
		if (m_pInstance != 0)
		{
			delete m_pInstance;
			m_pInstance = 0;
		}
		FUNCTION_EXIT;
	}

	/*
	* initialize
	*/
	void GENALibrary::initialize()
	{
		FUNCTION_ENTRY("initialize");

		if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_CONFIG_FILE))
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Loading configuration from xml file");
			m_objConfigHelper = boost::shared_ptr<GenaConfigHelper>(new GenaConfigHelper(TA_Base_Core::RunParams::getInstance().get(RPARAM_CONFIG_FILE)));
		}
		else
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Loading configuration from Entity");
			IEntityData * pTemp = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME));
			if (NULL != pTemp)
			{
				m_objConfigHelper = boost::shared_ptr<GenaConfigHelper>(new GenaConfigHelper(pTemp->getKey()));

				delete pTemp;
			}
		}

		_initialize();
		FUNCTION_EXIT;
	}


	void GENALibrary::startService()
	{
		FUNCTION_ENTRY("startService");
		if (m_clientInstance.get() != 0)
			_startClient();

		if (m_serverInstance.get() != 0)
			_startServer();

		if (m_bridgeInstance.get() != 0)
			_startBridge();

		FUNCTION_EXIT;
	}

	void GENALibrary::stopService()
	{
		FUNCTION_ENTRY("stopService");
		if (m_clientInstance.get() != 0)
			m_clientInstance->stopService();

		if (m_serverInstance.get() != 0)
			m_serverInstance->stopService();

		if (m_bridgeInstance.get() != 0)
			m_bridgeInstance->stopService();

		FUNCTION_EXIT;
	}

	void GENALibrary::publish(GENATopicDataPtr pTopicData)
	{
		FUNCTION_ENTRY("publish");
		if (NULL != m_serverInstance)
		{
			m_serverInstance->publishUpdate(pTopicData);
		}
		FUNCTION_EXIT;
	}

	void GENALibrary::register_TopicObserver(IGENATopicDataStore* pGENADataStore)
	{
		FUNCTION_ENTRY("register_TopicObserver");
		if (m_clientInstance.get() != 0)
			m_clientInstance->registerDataStoreTopicObserver(pGENADataStore);


		FUNCTION_EXIT;
	}

	void GENALibrary::register_TopicSource(IGENATopicDataStore* pGENADataStore)
	{
		FUNCTION_ENTRY("register_TopicSource");
		if (m_serverInstance.get() != 0)
			m_serverInstance->registerDataStore(pGENADataStore);

		FUNCTION_EXIT;
	}

	void GENALibrary::register_ServStatObserver(IServerStatusObserver* pServerStatusObserver)
	{
		FUNCTION_ENTRY("registerGENAClient_ServerStatusObserver");
		if (m_clientInstance.get() != 0)
			m_clientInstance->registerServerStatusObserver(pServerStatusObserver);

		FUNCTION_EXIT;
	}

	//////////////////////////////////////////////////////////////////////////
	// Private Functions
	//////////////////////////////////////////////////////////////////////////

	/*
	* _initialize
	*/
	void GENALibrary::_initialize()
	{
		GenaClientParamsPtr objClientParams = m_objConfigHelper->getGenaClientConfig();
		if (objClientParams.get() != 0) //NULL
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Client Service is configured. Initializing");
			m_clientInstance = GenaClientPtr(new GenaClient(*objClientParams.get()));
		}

		GenaServerParamsPtr objServerParams = m_objConfigHelper->getGenaServerConfig();
		if (objServerParams.get() != 0) //NULL
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Server Service is configured. Initializing");
			m_serverInstance = GenaServerPtr(new GenaServer(*objServerParams.get()));
		}

		GenaBridgeParamsPtr objBridgeParams = m_objConfigHelper->getGenaBridgeConfig(); 
		if (objBridgeParams.get() != 0) //NULL
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Bridge Service is configured. Initializing");
			m_bridgeInstance = GenaBridgePtr(new GenaBridge(*(objBridgeParams->serverParams), *(objBridgeParams->clientParams.get())));
		}
	}

	
	/*
	* startClient
	*/
	void GENALibrary::_startClient()
	{
		FUNCTION_ENTRY("startClient");
		if (m_clientInstance.get() == 0)
		{
			throw GenaConfigException("Client not configured.");
		}

		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Starting Client Service.");
		m_clientInstance->startService();
		
		// Start Subscription immediately data is configured
		GenaClientParamsPtr objClientParams = m_objConfigHelper->getGenaClientConfig();
		T_ClientTopicList topicList = m_objConfigHelper->getClientTopics(objClientParams->entityKey);
		if (topicList.size() != 0)
		{	// Start Subscribing
			//TA_ASSERT(pTopicListener != 0, "topic listener should not be null");
			for (int idx=0; idx<topicList.size(); idx++)
			{
				GenaClientTopic objTopic = topicList[idx];

				m_clientInstance->addSubscribeTopic(objTopic.topicUrl, objTopic.notification);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Add Subscribed to topic: %s", objTopic.topic.c_str());
			}
		}
		
		FUNCTION_EXIT;
	}

	/*
	* startServer
	*/
	void GENALibrary::_startServer()
	{
		FUNCTION_ENTRY("startServer");
		if (m_serverInstance.get() == 0)
		{
			throw GenaConfigException("Server not configured.");
		}
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Starting Server Service.");
		m_serverInstance->startService();
		FUNCTION_EXIT;
	}

	/*
	* startBridge
	*/
	void GENALibrary::_startBridge()
	{
		FUNCTION_ENTRY("startBridge");
		if (m_bridgeInstance.get() == 0)
		{
			throw GenaConfigException("Bridge not configured.");
		}
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Starting Bridge Service.");
		m_bridgeInstance->startService();
		FUNCTION_EXIT;
	}
	
	/*
	* getClientInstance
	*/
	GenaClientPtr GENALibrary::getClientInstance()
	{
		FUNCTION_ENTRY("getClientInstance");
		if (m_clientInstance.get() == 0)
		{
			throw GenaConfigException("Client not configured.");
		}

		FUNCTION_EXIT;
		return m_clientInstance;
	}

	/*
	* getServerInstance
	*/
	GenaServerPtr GENALibrary::getServerInstance()
	{
		FUNCTION_ENTRY("getServerInstance");
		if (m_serverInstance.get() == 0)
		{
			throw GenaConfigException("Server not configured.");
		}

		FUNCTION_EXIT;
		return m_serverInstance;
	}

	/*
	* getBridgeInstance
	*/
	GenaBridgePtr GENALibrary::getBridgeInstance()
	{
		FUNCTION_ENTRY("getBridgeInstance");
		if (m_bridgeInstance.get() == 0)
		{
			throw GenaConfigException("Bridge not configured.");
		}

		FUNCTION_EXIT;
		return m_bridgeInstance;
	}

} //TA_Base_Bus
