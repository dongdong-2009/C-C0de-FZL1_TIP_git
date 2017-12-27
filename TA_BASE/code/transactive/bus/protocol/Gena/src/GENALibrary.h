/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: ISCSGenaHelper.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/08/14 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the interface declaration for ISCSGenaLibrary
  */

#ifndef __ISCS_GENA_HELPER_H_INCLUDED__
#define __ISCS_GENA_HELPER_H_INCLUDED__

#include "GenaConfigHelper.h"
#include "GenaClient.h"
#include "GenaServer.h"
#include "GenaBridge.h"
#include "CommonDef.h"
#include "Poco/Logger.h"

namespace TA_Base_Bus
{
	
	class HttpServicelog: public Poco::IPOCOLogCallback
	{
	public:
		virtual void info(std::string& log);
		virtual void debug(std::string& log);
		virtual void error(std::string& log);
	};
	
	class GENALibrary
	{
	public: // Methods
		/* 
		* destructor
		*/
		~GENALibrary();

		/* 
		* getInstance
		* @purpose: retrieves a reference to the singleton instance of this class
		* @params:	none
		* @return:	reference to the singleton instance
		*/
		static GENALibrary& getInstance();

		/* 
		* releaseInstance
		* @purpose: deletes the singleton instance of this class
		* @params:	none
		* @return:	none
		*/
		static void releaseInstance();

		/* 
		* initialize
		* @purpose: initializes the GENA Library, load configuration from database and instantiate 
					all objects that needs to be created based on the configured data.
		* @params:	none
		* @return:	none
		*/
		void initialize();

		/* 
		* startService
		* @purpose: starts the services based on the configured data.
		* @params:	none
		* @return:	none
		*/
		void startService();

		/* 
		* stopService
		* @purpose: stops all services
		* @params:	none
		* @return:	none
		*/
		void stopService();

		/* 
		* publish
		* @purpose: publishes GENA topic data
		* @params:	pTopicData - data to publish
		* @return:	none
		*/
		void publish(GENATopicDataPtr pTopicData);

		/* 
		* register_TopicObserver
		* @purpose: used to register a topic data store observer 
		* @params:	pGENADataStore - pointer to the GENADataStore interface
		* @return:	none
		*/
		void register_TopicObserver(IGENATopicDataStore* pGENADataStore);

		/* 
		* register_TopicSource
		* @purpose: used to register for topic data source
		* @params:	pGENADataStore - pointer to GENADataStore interface
		* @return:	none
		*/
		void register_TopicSource(IGENATopicDataStore* pGENADataStore);

		/* 
		* register_ServStatObserver
		* @purpose: used to register for server status updates
		* @params:	none
		* @return:	shared pointer to Client configured parameters
		*/
		void register_ServStatObserver(IServerStatusObserver* pServerStatusObserver);

		/* 
		* getGenaClientConfig
		* @purpose: retrieves the client configuration
		* @params:	none
		* @return:	shared pointer to Client configured parameters
		*/
		GenaClientPtr getClientInstance();

		/* 
		* getGenaClientConfig
		* @purpose: retrieves the client configuration
		* @params:	none
		* @return:	shared pointer to Client configured parameters
		*/
		GenaServerPtr getServerInstance();

		/* 
		* getGenaClientConfig
		* @purpose: retrieves the client configuration
		* @params:	none
		* @return:	shared pointer to Client configured parameters
		*/
		GenaBridgePtr getBridgeInstance();

	private: // Methods
		/*
		* Constructor
		*/
		GENALibrary();
			
		/**
		* disable default copy constructor
		*/
		GENALibrary( const GENALibrary& rhs);
		GENALibrary& operator = ( const GENALibrary& rhs);

		/* 
		* getGenaClientConfig
		* @purpose: retrieves the client configuration
		* @params:	none
		* @return:	shared pointer to Client configured parameters
		*/
		void _initialize();

		/* 
		* getGenaClientConfig
		* @purpose: retrieves the client configuration
		* @params:	none
		* @return:	shared pointer to Client configured parameters
		*/
		void _startClient();

		/* 
		* getGenaClientConfig
		* @purpose: retrieves the client configuration
		* @params:	none
		* @return:	shared pointer to Client configured parameters
		*/
		void _startServer();

		/* 
		* getGenaClientConfig
		* @purpose: retrieves the client configuration
		* @params:	none
		* @return:	shared pointer to Client configured parameters
		*/
		void _startBridge();

	private: // Variables
		static GENALibrary* m_pInstance;
		boost::shared_ptr<GenaConfigHelper> m_objConfigHelper;
		GenaClientPtr m_clientInstance;
		GenaServerPtr m_serverInstance;
		GenaBridgePtr m_bridgeInstance;
	};

} //TA_Base_Bus

#endif //__GENA_CONFIG_HELPER_H_INCLUDED__