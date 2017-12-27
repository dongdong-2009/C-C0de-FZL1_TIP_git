/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: SubscriptionManager.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This class contains the interface for subscription manager.
  */

#ifndef __SUBSCRIPTION_MANAGER_H_INCLUDED__
#define __SUBSCRIPTION_MANAGER_H_INCLUDED__
#pragma once

#include "GenaSubscriber.h"
#include "ITopicListener.h"
#include "GENAConnectionFactory.h"
#include "CommonDef.h"
#include <string>

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Condition.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Bus
{
	// Forward Declaration
	class GenaCallbackProccessor;

	class SubscriptionManager
		: public TA_Base_Core::Thread

	{
	public:
		/*
		* Constructor
		*/
		SubscriptionManager(GenaClientParams clientParams, GenaCallbackProccessor* pCallbackProc, ConnectionFactoryPtr pConnectionMgr);

		/*
		* Destructor
		*/
		~SubscriptionManager(void);

		/*
		* subscribe
		*/
		std::string subscribe(const std::string& strTopic, const std::string& notification,
			const T_TopicFilterSet& filterSet);

		/*
		* subscribe
		*/
		std::string subscribe(const std::string& strTopicURL, const std::string& notification);

		/*
		* addSubscribeTopic
		*/
		void addSubscribeTopic(const std::string& strTopicURL, const std::string& notification);

		/*
		* unsubscribe
		*/
		void unsubscribe(const std::string& sid);

		void resetSubscribeTopic(const std::string& sid);

		/*
		* run
		*/
		virtual void run();

		/*
		* terminate
		*/
		virtual void terminate();

		//void resubscribeAllTopic();
		void unsubscribeAllTopic();
		void stopSubscribeAllTopic();
		void resetSubscribeAllToic();

		void registerDataStore();

	private: // Functions
		void _checkSubscriberState();
		void _cleanUpSubscribers();
		std::string _isTopicSubscribed(const std::string& strTopicURL);		

	private: // Variables
		//typedef std::map<std::string, GenaSubscriber*> T_subscribersMap;
		typedef std::vector<GenaSubscriberPtr> T_subscriberVec;
		// map containing the subscription information
		T_subscriberVec m_subscribers;

		GenaCallbackProccessor* m_pCallbackProc;
		GenaClientParams m_GenaClientParams;
		ConnectionFactoryPtr m_pConnectionMgr;

		// Thread related 
		bool m_bTerminate;
		TA_Base_Core::Condition m_condition;
		TA_Base_Core::ReEntrantThreadLockable m_threadLock;

		// Resubscribe flag
		//bool m_bResubscribe;
		// Identify Subscriber 
	};

} //TA_Base_Bus

#endif //__SUBSCRIPTION_MANAGER_H_INCLUDED__