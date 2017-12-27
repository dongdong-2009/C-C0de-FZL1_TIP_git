/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GenaServerImp.h  $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the class declaration for GenaServerImp
  */

#ifndef __GENA_CLIENT_H_INCLUDED__
#define __GENA_CLIENT_H_INCLUDED__

#include "CommonDef.h"
#include "ITopicListener.h"
#include "core/threads/src/Thread.h"
#include "boost/shared_ptr.hpp"
#include "GENAConnectionFactory.h"
#include "IServerStatusObserver.h"
#include "IGENATopicDataStore.h"

#pragma once

namespace TA_Base_Bus
{
	//Forward Declaration
	class GENAHttpServer;
	class GenaCallbackProccessor;
	class SubscriptionManager;

	class GenaClient
		: private TA_Base_Core::Thread
	{
	public:
		GenaClient(const GenaClientParams& pClientParams);
		~GenaClient();
		
		virtual void run();

		virtual void terminate();

		/*
		* subscribe
		*/
		std::string subscribe(const std::string& strTopicName, const std::string& notification,
			const T_TopicFilterSet& filterSet);

		/*
		* subscribe
		*/
		std::string subscribe(const std::string& topicURL, const std::string& notification);
		
		/*
		* addSubscribeTopic
		*/
		void addSubscribeTopic(const std::string& topicURL, const std::string& notification);

		/*
		* unsubscribe
		*/
		void unsubscribe(const std::string& sid);

		/*
		* resetSubscribeTopic
		*/
		void resetSubscribeTopic(const std::string& sid);

		/*
		* unSubscribeAllTopic
		*/
		void unSubscribeAllTopic();
		/*
		* resetSubscribeAllTopic
		*/
		void resetSubscribeAllTopic();
		/*
		* startService
		*/
		void startService();
		/*
		* stopService
		*/
		void stopService();
		/*
		* destroyService
		*/
		void destroyService();
		/*
		* registerServerStatusObserver
		*/
		void registerServerStatusObserver(IServerStatusObserver* pObserver);

		void registerConnStatusObserver(IConnStateListener* pObserver);
		void deRegisterConnStatusObserver(IConnStateListener* pObserver);
		/*
		* registerDataStoreTopicObserver
		*/
		void registerDataStoreTopicObserver(IGENATopicDataStore* pGenaDataStore);
		void deRegisterDataStoreTopicObserver(IGENATopicDataStore* pGenaDataStore);
		/*
		* registerMessageObserver
		*/
		//void registerMessageObserver();

		/*
		* getConnectionMgr
		*/
		ConnectionFactoryPtr getConnectionMgr() { return m_pConnectionMgr; };

	protected:
	private:
		GenaCallbackProccessor* m_pCallBackProc;
		SubscriptionManager* m_pSubscriptionMgr;
		GENAHttpServer* m_pHttpServer;
		ConnectionFactoryPtr m_pConnectionMgr;
	};

	typedef boost::shared_ptr<GenaClient> GenaClientPtr;

} //TA_Base_Bus

#endif //__GENA_CLIENT_H_INCLUDED__
