/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GenaConfigHelper.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/08/14 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the implementation for GenaConfigHelper
  */

#include "GenaConfigHelper.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/GenaConfigEntity.h"

#include "core/data_access_interface/Gena/src/GenaClientSourceAccessFactory.h"
#include "core/data_access_interface/Gena/src/IGenaClientSource.h"

#include "core/data_access_interface/Gena/src/GenaTopicAccessFactory.h"
#include "core/data_access_interface/Gena/src/IGenaTopic.h"
#include "core/utilities/src/Hostname.h"

#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"

using namespace TA_Base_Core;
using namespace Poco;

namespace TA_Base_Bus
{
	
	/* 
	* Constructor
	*/
	GenaConfigHelper::GenaConfigHelper(const unsigned long parentEntity)
		: m_parentEntity(parentEntity)
		, m_strConfigFile("")
		, m_bIsLoaded(false)
	{
		FUNCTION_ENTRY("Constructor");
		FUNCTION_EXIT;
	}

	/* 
	* Constructor
	*/
	GenaConfigHelper::GenaConfigHelper(const std::string& configFile)
		: m_strConfigFile(configFile)
		, m_parentEntity(0)
		, m_bIsLoaded(false)
	{
		FUNCTION_ENTRY("Constructor");
		FUNCTION_EXIT;
	}

	/* 
	* getGenaClientConfig
	*/
	GenaClientParamsPtr GenaConfigHelper::getGenaClientConfig()
	{
		FUNCTION_ENTRY("getGenaClientConfig");
		loadCofiguration();
		FUNCTION_EXIT;
		return m_clientParams;
	}

	/* 
	* getGenaServerConfig
	*/
	GenaServerParamsPtr GenaConfigHelper::getGenaServerConfig()
	{
		FUNCTION_ENTRY("getGenaServerConfig");
		loadCofiguration();
		FUNCTION_EXIT;
		return m_serverParams;
	}

	/* 
	* getGenaBridgeConfig
	*/
	GenaBridgeParamsPtr GenaConfigHelper::getGenaBridgeConfig()
	{
		FUNCTION_ENTRY("getGenaBridgeConfig");
		loadCofiguration();
		FUNCTION_EXIT;
		return m_bridgeParams;
	}

	void GenaConfigHelper::loadCofiguration()
	{
		FUNCTION_ENTRY("loadCofiguration");
		if (m_bIsLoaded)
			return;
		
		if (m_strConfigFile.empty())
			loadFromDb();
		else
			loadFromFile();
		
		m_bIsLoaded = true;
		FUNCTION_EXIT;
	}

	void GenaConfigHelper::loadFromDb()
	{
		FUNCTION_ENTRY("loadFromDb");
		IEntityDataList entityList = EntityAccessFactory::getInstance().getChildEntitiesOf(m_parentEntity);
		for (int idx=0; idx<entityList.size(); idx++)
		{
			if (std::string::npos == entityList[idx]->getName().find("Gena"))
				continue;

			GenaConfigEntity* pGenaConfig = dynamic_cast<GenaConfigEntity*>(entityList[idx]);
			if (pGenaConfig->getObjectType() == GenaConfigEntity::GENA_CLIENT_TYPE)
			{
				m_clientParams = loadClientParams(pGenaConfig);
			}
			else if (pGenaConfig->getObjectType() == GenaConfigEntity::GENA_SERVER_TYPE)
			{
				m_serverParams = loadServerParams(pGenaConfig);
			}
			else if (pGenaConfig->getObjectType() == GenaConfigEntity::GENA_BRIDGE_TYPE)
			{
				m_bridgeParams = GenaBridgeParamsPtr(new GenaBridgeParams);
				m_bridgeParams->entityKey = pGenaConfig->getKey();
				m_bridgeParams->clientParams = loadClientParams(pGenaConfig);
				m_bridgeParams->serverParams = loadServerParams(pGenaConfig);
			}
		}

		FUNCTION_EXIT;
	}

	void GenaConfigHelper::loadFromFile()
	{
		FUNCTION_ENTRY("loadFromFile");

		std::ifstream in(m_strConfigFile.c_str());
		Poco::XML::InputSource src(in);

		XML::DOMParser parser;
		Poco::AutoPtr<XML::Document> pDoc = parser.parse(&src);

		XML::NodeIterator it(pDoc, Poco::XML::NodeFilter::SHOW_ATTRIBUTE);
		XML::Node* pNode = it.root()->firstChild(); // Should get first configuration

		std::string nodeName = pNode->nodeName();
		XML::Node* ptempNode = pNode->getNodeByPath(SERVER_ELEM);
		if (ptempNode != NULL && ptempNode->attributes()->getNamedItem(ENABLE_ATTR)->nodeValue()=="1")
		{
			m_serverParams = loadServerParams(ptempNode);
		}

		ptempNode = pNode->getNodeByPath(CLIENT_ELEM);
		if (ptempNode != NULL && ptempNode->attributes()->getNamedItem(ENABLE_ATTR)->nodeValue()=="1")
		{
			m_clientParams = loadClientParams(ptempNode);
		}

		ptempNode = pNode->getNodeByPath(BRIDGE_ELEM);
		if (ptempNode != NULL && ptempNode->attributes()->getNamedItem(ENABLE_ATTR)->nodeValue()=="1")
		{
			m_bridgeParams = GenaBridgeParamsPtr(new GenaBridgeParams);
			m_bridgeParams->entityKey = 0;
			XML::Node* pTemp = ptempNode->getNodeByPath(SERVER_ELEM);
			if (pTemp != NULL)
			{
				m_bridgeParams->serverParams = loadServerParams(pTemp);
			}
			
			pTemp = ptempNode->getNodeByPath(CLIENT_ELEM);
			if (pTemp != NULL)
			{
				m_bridgeParams->clientParams = loadClientParams(pTemp);
			}
		}

		FUNCTION_EXIT;
	}

	GenaServerParamsPtr GenaConfigHelper::loadServerParams(GenaConfigEntity* pGenaConfig)
	{
		FUNCTION_ENTRY("loadServerParams");

		GenaServerParamsPtr serverParams = GenaServerParamsPtr(new GenaServerParams());
		serverParams->entityKey = pGenaConfig->getKey();
		serverParams->subscriptionTimeOut = pGenaConfig->getSubscriptionTimeOut();
		serverParams->serverName = getMyConfig(pGenaConfig->getServerName());
		serverParams->ipAddress = getMyConfig(pGenaConfig->getServerIP());
		serverParams->serverPort = pGenaConfig->getServerPort();
		serverParams->keepAlive = pGenaConfig->getHTTPKeepAlive();
		serverParams->timeout = pGenaConfig->getHTTPTimeout(); //* 1000; // Poco uses microsecond

		FUNCTION_EXIT;
		return serverParams;
	}

	GenaClientParamsPtr GenaConfigHelper::loadClientParams(GenaConfigEntity* pGenaConfig)
	{
		FUNCTION_ENTRY("loadClientParams");
		GenaClientParamsPtr clientParams = GenaClientParamsPtr(new GenaClientParams());
		clientParams->entityKey = pGenaConfig->getKey();
		clientParams->nResubscribeInterval = pGenaConfig->getResubscriptionTimeout();
		clientParams->callbackInfo.serverName = getMyConfig(pGenaConfig->getCbServerName());
		clientParams->callbackInfo.ipAddress = getMyConfig(pGenaConfig->getCbServerIP());
		clientParams->callbackInfo.serverPort = pGenaConfig->getCbServerPort();
		clientParams->callbackInfo.keepAlive = pGenaConfig->getCbHTTPKeepAlive();
		clientParams->callbackInfo.timeout = pGenaConfig->getCbHTTPTimeout(); //* 1000; // Poco uses microsecond
		
		// Load Source servers
		std::vector<IGenaClientSource*> vecSources = GenaClientSourceAccessFactory::getInstance().getClientSourceForEntity(pGenaConfig->getKey());
		for (int srcIdx=0; srcIdx<vecSources.size(); srcIdx++)
		{
			GenaClientSourcePtr objNew(new GenaClientSource());
			objNew->serverURL = vecSources[srcIdx]->getServerURL();
			objNew->maxRetry = vecSources[srcIdx]->getMaxRetry();
			objNew->keepAlive = vecSources[srcIdx]->getKeepAlive();
			objNew->timeout = vecSources[srcIdx]->getTimeOut();

			clientParams->sourceServers.push_back(objNew);
		}

		// Load Topic List
		std::vector<IGenaTopic*> objTopicList = GenaTopicAccessFactory::getInstance().getGenaTopicForEntity(clientParams->entityKey);
		if (objTopicList.size() != 0)
		{
			T_ClientTopicList objTopics;
			for (int idx=0; idx<objTopicList.size(); idx++)
			{
				GenaClientTopic objTemp;
				objTemp.topic = objTopicList[idx]->getTopic();
				objTemp.topicUrl = objTopicList[idx]->getSubscribeURL();
				objTemp.notification = objTopicList[idx]->getNotification();

				objTopics.push_back(objTemp);
			}

			m_clientTopicMap[clientParams->entityKey] = objTopics;
		}

		
		FUNCTION_EXIT;
		return clientParams;
	}	

	T_ClientTopicList GenaConfigHelper::getClientTopics(unsigned long clientEntity)
	{
		FUNCTION_ENTRY("getClientTopics");
		T_ClientTopicList objRetVal;

		T_ClientTopicMap::iterator itFind = m_clientTopicMap.find(clientEntity);
		if (itFind != m_clientTopicMap.end())
		{
			objRetVal = itFind->second;
		}

		FUNCTION_EXIT;
		return objRetVal;
	}

	GenaServerParamsPtr GenaConfigHelper::loadServerParams(XML::Node* pNode)
	{
		FUNCTION_ENTRY("loadServerParams");
		GenaServerParamsPtr serverParams;

		serverParams.reset(new GenaServerParams());
		serverParams->entityKey = strtoul (pNode->attributes()->getNamedItem(ID_ATTR)->nodeValue().c_str(), NULL, 0);

		XML::Node* pTempNode = pNode->getNodeByPath(NAME_ELEM);
		if (pTempNode != NULL)
		{
			serverParams->serverName = pTempNode->firstChild()->nodeValue();
		}

		pTempNode = pNode->getNodeByPath(IPADDRESS_ELEM);
		if (pTempNode != NULL)
		{
			serverParams->ipAddress = pTempNode->firstChild()->nodeValue();
		}

		pTempNode = pNode->getNodeByPath(PORT_ELEM);
		if (pTempNode != NULL)
		{
			serverParams->serverPort = atoi(pTempNode->firstChild()->nodeValue().c_str());
		}

		pTempNode = pNode->getNodeByPath(HTTP_KEEP_ALIVE_ELEM);
		if (pTempNode != NULL)
		{
			serverParams->keepAlive = pTempNode->firstChild()->nodeValue()=="1";
		}
		
		pTempNode = pNode->getNodeByPath(HTTP_TIMEOUT_ELEM);
		if (pTempNode != NULL)
		{
			serverParams->timeout = strtoul (pTempNode->firstChild()->nodeValue().c_str(), NULL, 0); // * 1000; // Poco uses microsecond
		}

		pTempNode = pNode->getNodeByPath(SUBSCRIPTION_TO_ELEM);
		if (pTempNode != NULL)
		{
			serverParams->subscriptionTimeOut = strtoul (pTempNode->firstChild()->nodeValue().c_str(), NULL, 0);
		}
		
		FUNCTION_EXIT;
		return serverParams;
	}

	GenaClientParamsPtr GenaConfigHelper::loadClientParams(XML::Node* pNode)
	{
		FUNCTION_ENTRY("loadClientParams");
		GenaClientParamsPtr clientParams;

		clientParams.reset(new GenaClientParams());
		clientParams->entityKey = strtoul (pNode->attributes()->getNamedItem(ID_ATTR)->nodeValue().c_str(), NULL, 0);

		XML::Node* pTempNode = pNode->getNodeByPath(RESUBSCRIPTION_ELEM);
		if (pTempNode != NULL)
		{
			clientParams->nResubscribeInterval = strtoul (pTempNode->firstChild()->nodeValue().c_str(), NULL, 0);
		}

		XML::Node* pCallbackNode = pNode->getNodeByPath(CALLBACK_ELEM);
		if (pCallbackNode != NULL)
		{
			pTempNode = pCallbackNode->getNodeByPath(NAME_ELEM);
			if (pTempNode != NULL)
			{
				clientParams->callbackInfo.serverName = pTempNode->firstChild()->nodeValue();
			}

			pTempNode = pCallbackNode->getNodeByPath(IPADDRESS_ELEM);
			if (pTempNode != NULL)
			{
				clientParams->callbackInfo.ipAddress = pTempNode->firstChild()->nodeValue();
			}

			pTempNode = pCallbackNode->getNodeByPath(PORT_ELEM);
			if (pTempNode != NULL)
			{
				clientParams->callbackInfo.serverPort = atoi(pTempNode->firstChild()->nodeValue().c_str());
			}

			pTempNode = pCallbackNode->getNodeByPath(HTTP_KEEP_ALIVE_ELEM);
			if (pTempNode != NULL)
			{
				clientParams->callbackInfo.keepAlive = pTempNode->firstChild()->nodeValue()=="1";
			}

			pTempNode = pCallbackNode->getNodeByPath(HTTP_TIMEOUT_ELEM);
			if (pTempNode != NULL)
			{
				clientParams->callbackInfo.timeout = strtoul (pTempNode->firstChild()->nodeValue().c_str(), NULL, 0); // * 1000; // Poco uses microsecond
			}
		}

		// Load Source Servers
		XML::Node* pSourceServerNode = pNode->getNodeByPath(SOURCE_SERVER_ELEM);
		pTempNode = pSourceServerNode->firstChild();
		while (pTempNode)
		{
			std::string name = pTempNode->nodeName();
			if (name == SERVER_URL_ELEM)
			{
				GenaClientSourcePtr objNew(new GenaClientSource());

				objNew->serverURL = pTempNode->firstChild()->nodeValue().c_str();
				objNew->maxRetry = atoi(pTempNode->attributes()->getNamedItem(MAX_RETRY_ATTR)->nodeValue().c_str());
				objNew->keepAlive = pTempNode->attributes()->getNamedItem(KEEPALIVE_ATTR)->nodeValue()=="1";
				objNew->timeout = strtoul(pTempNode->attributes()->getNamedItem(TIMEOUT_ATTR)->nodeValue().c_str(), NULL, 0);

				clientParams->sourceServers.push_back(objNew);
			}

			pTempNode = pTempNode->nextSibling();
		}
		
		// Load Client Topic
		XML::Node* pClientTopic = pNode->getNodeByPath(SUBSCRIBE_TOPIC_ELEM);
		if (pClientTopic != NULL)
		{
			T_ClientTopicList objTopicList;
			pTempNode = pClientTopic->firstChild();
			while (pTempNode)
			{
				std::string name = pTempNode->nodeName();
				if (name == TOPIC_ELEM)
				{
					GenaClientTopic objTemp;

					objTemp.topic = pTempNode->attributes()->getNamedItem(NAME_ATTR)->nodeValue();
					objTemp.topicUrl = pTempNode->attributes()->getNamedItem(URL_ATTR)->nodeValue();
						// + "?attr=TrainLabel&attr=FrontPos&attr=currentDir&attr=TravelTimeExceeded&attr=DriveMode";
					objTemp.notification = pTempNode->attributes()->getNamedItem(NOTIFICATION_ATTR)->nodeValue();

					objTopicList.push_back(objTemp);				
				}

				pTempNode = pTempNode->nextSibling();
			}

			m_clientTopicMap[clientParams->entityKey] = objTopicList;
		}

		FUNCTION_EXIT;	
		return clientParams;
	}

	std::string GenaConfigHelper::getMyConfig(const std::string& stringData)
	{
		FUNCTION_ENTRY("getMyConfig");
		std::string strRetVal;
		std::vector<std::string> strVector;
		std::string myHostName = TA_Base_Core::Hostname::getHostname();
		bool bFound = false;
		boost::split(strVector,stringData,boost::is_any_of(";"));
		for (int idx=0; idx<strVector.size(); idx++)
		{
			std::vector<std::string> strHostVecTemp;
			boost::split(strHostVecTemp,strVector[idx],boost::is_any_of("@"));
			if (strHostVecTemp.size() > 1)
			{
				strRetVal = strHostVecTemp[1];
				if (strHostVecTemp[0] == myHostName)
				{	
					bFound = true;
					break;
				}
			}
			else
				strRetVal = strHostVecTemp[0]; // record the last known config
		}
		
		FUNCTION_EXIT;
		return strRetVal;
	}
} //TA_Base_Bus
