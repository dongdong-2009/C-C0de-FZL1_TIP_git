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

#ifndef __GENA_SERVER_IMPLEMENT_H_INCLUDED__
#define __GENA_SERVER_IMPLEMENT_H_INCLUDED__

#include "AbstractGENARequestProccessor.h"
#include "SIDManager.h"
#include "CommonDef.h"
#include "GENATopicData.h"
#include "GENAConnectionFactory.h"
#include "IGENATopicDataStore.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Condition.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#pragma once

namespace TA_Base_Bus
{
	//Forward Declaration
	class GENAPublicationManager;
	class IGENAClientStatusForwarder;

	class GenaServerImp
		: public AbstractGENARequestProccessor
		, private TA_Base_Core::Thread
	{
	public:
		/*
		* Constructor
		*/
		GenaServerImp(ConnectionFactoryPtr pConnectionMgr, GenaServerParams  serverParameter);

		/*
		* Destructor
		*/
		~GenaServerImp(void);

		/*
		* startService
		*/
		void startService();
		
		/*
		* stopService
		*/
		void stopService();


		/*
		* publishUpdate
		*/
		void publishUpdate(GENATopicDataPtr ptopicUpdate);

		/*
		* subscribeToRequest
		*/
		void subscribeToRequest(IGENAClientStatusForwarder* pReqFwdr);

		/*
		* registerDataStore
		*/
		void registerDataStore(IGENATopicDataStore* pDataStore);

		/*
		* requestBirthMessage
		* @purpose: used to request for birth messages. 
		* @param:	topicURI - topic URI to identify the correct publisher
					SID - subscriber identification 
		* @return: none
		*/
		virtual void requestBirthMessage(const std::string& topicURI, std::string nSID);
	protected:
		/*
		* subscribe
		*/
		virtual std::string subscribe(GENARequestPtr objRequest);

		/*
		* unsubscribe
		*/
		virtual bool unsubscribe(GENARequestPtr objRequest);

		/*
		* notify
		*/
		virtual void notify(GENARequestPtr objRequest);

		/*
		* run
		*/
		virtual void run();

		/*
		* terminate
		*/
		virtual void terminate();
	private:

		SIDManager m_sidMgr; //SIDMgr

		unsigned long addSubscriber();

		void updateSubscriber();

		void extractTopic(const std::string& uri, SubscriberInfo& subInfo);

		void doForwardSubscribeRequest(const std::string& topicUri, const std::string& notification);

		void doForwardUnSubscribeRequest(const std::string& topicUri);
	
		void _cleanUp();
	private:
		typedef std::map<std::string, time_t> T_SubscribeUpdateList;

		TA_Base_Core::Condition m_condition;
		TA_Base_Core::NonReEntrantThreadLockable m_threadLock;
		T_SubscriptionInfoMap m_subscriptions;
		T_SubscribeUpdateList m_subUpdateList;
        GenaServerParams   m_serverParameter;
		boost::shared_ptr<GENAPublicationManager> m_publicationMgr;
		std::vector<IGENAClientStatusForwarder*> m_requestFwdr;
		ServerState m_serverState;
	}; //GenaServerImp

} //TA_Base_Bus


#endif //__GENA_SERVER_IMPLEMENT_H_INCLUDED__
