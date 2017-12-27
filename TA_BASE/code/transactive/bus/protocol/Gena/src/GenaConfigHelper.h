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
  * This file contains the interface declaration for GenaConfigHelper 
  * class reads the file configuration and organize it into structures
  */

#ifndef __GENA_CONFIG_HELPER_H_INCLUDED__
#define __GENA_CONFIG_HELPER_H_INCLUDED__

#include "CommonDef.h"
#include "core/data_access_interface/entity_access/src/GenaConfigEntity.h"

#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeIterator.h"
#include "Poco/DOM/NamedNodeMap.h"
#include "Poco/DOM/NodeFilter.h"
#include "Poco/DOM/AutoPtr.h"
#include "Poco/SAX/InputSource.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{
	class GenaConfigHelper
	{
	public: // Methods

		/* 
		* Constructor
		* @purpose: use this constructor if configuration is stored in database
		* @params: parentEntity - Agent entity key 
		*/
		GenaConfigHelper(const unsigned long parentEntity);

		/* 
		* Constructor
		* @purpose: use this constructor if configuration is stored in a file
		* @params: configFile - complete path of the configuration file
		*/
		GenaConfigHelper(const std::string& configFile);

		/* 
		* Destructor
		* @purpose: Destroys this object
		*/
		~GenaConfigHelper() {};

		/* 
		* getGenaClientConfig
		* @purpose: retrieves the client configuration
		* @params:	none
		* @return:	shared pointer to Client configured parameters
		*/
		GenaClientParamsPtr getGenaClientConfig();

		/* 
		* getGenaServerConfig
		* @purpose: retrieves the server configuration
		* @params:	none
		* @return:	shared pointer to Server configured parameters
		*/
		GenaServerParamsPtr getGenaServerConfig();

		/* 
		* getGenaBridgeConfig
		* @purpose: retrieves the bridge configuration
		* @params:	none
		* @return:	shared pointer to bridge configured parameters
		*/
		GenaBridgeParamsPtr getGenaBridgeConfig();

		/* 
		* getClientTopics
		* @purpose: retrieves the bridge configuration
		* @params:	clientEntity - entity name for the client configuration
		* @return:	shared pointer to client topics that we need to subscribe
		*/
		T_ClientTopicList getClientTopics(unsigned long clientEntity);

	private: // Methods
		/* 
		* loadCofiguration
		* @purpose: loads configuration
		* @params:	none
		* @return:	none
		*/
		void loadCofiguration();

		/* 
		* loadFromDb
		* @purpose: reads configuration from database
		* @params:	none
		* @return:	none
		*/
		void loadFromDb();
		
		/* 
		* loadFromFile
		* @purpose: reads configuration from file
		* @params:	none
		* @return:	none
		*/
		void loadFromFile();
	
		/* 
		* loadServerParams
		* @purpose: converts GenaConfigEntity to GenaServerParams
		* @params:	pGenaConfig - pointer to GenaConfigEntity
		* @return:	shared pointer to GenaServerParams
		*/
		GenaServerParamsPtr loadServerParams(GenaConfigEntity* pGenaConfig);

		/* 
		* loadClientParams
		* @purpose: converts GenaConfigEntity to GenaClientParams
		* @params:	pGenaConfig - pointer to GenaConfigEntity
		* @return:	shared pointer to GenaClientParams
		*/
		GenaClientParamsPtr loadClientParams(GenaConfigEntity* pGenaConfig);

		/* 
		* loadServerParams
		* @purpose: converts XML Node configuration to GenaServerParams
		* @params:	pNode - pointer to XML Nodes
		* @return:	shared pointer to GenaServerParams
		*/
		GenaServerParamsPtr loadServerParams(Poco::XML::Node* pNode);
		/* 
		* loadClientParams
		* @purpose: converts GenaConfigEntity to GenaClientParams
		* @params:	pNode - pointer to XML Nodes
		* @return:	shared pointer to GenaClientParams
		*/
		GenaClientParamsPtr loadClientParams(Poco::XML::Node* pNode);

		std::string getMyConfig(const std::string& stringData);
	private: // Variables
		// Path to the configured file
		std::string m_strConfigFile;
		// Agent Entity Key
		unsigned long m_parentEntity;
		// Indicates whether configuration was loaded.
		bool m_bIsLoaded;
		// Shared pointer to client params
		GenaClientParamsPtr m_clientParams;
		// Shared pointer to server params
		GenaServerParamsPtr m_serverParams;
		// Bridge pointer 
		GenaBridgeParamsPtr m_bridgeParams;
		// Topic to client map
		typedef std::map<unsigned long, T_ClientTopicList> T_ClientTopicMap;
		 T_ClientTopicMap m_clientTopicMap;
	};

} //TA_Base_Bus

#endif //__GENA_CONFIG_HELPER_H_INCLUDED__