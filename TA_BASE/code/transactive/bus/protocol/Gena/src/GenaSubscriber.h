/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GenaSubscriber.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the class declaration for GenaSubscriber
  */

#ifndef __GENA_SUBSCRIBER_H_INCLUDED__
#define __GENA_SUBSCRIBER_H_INCLUDED__

#include "CommonDef.h"
#include "ServerProxy.h"
#include "GENAConnectionFactory.h"
#include "boost/shared_ptr.hpp"
#include <map>

namespace TA_Base_Bus
{
	// Forward Declaration
	class GenaCallbackProccessor;

	class GenaSubscriber
	{
	public: 
		GenaSubscriber(const std::string& strTopic, const std::string& uri,
			const std::string& notification, const T_TopicFilterSet& filterSet, 
			GenaClientParams clientParams, GenaCallbackProccessor* pCallBackProcessor, ConnectionFactoryPtr pConnectionMgr);

		GenaSubscriber(const std::string& uri,
			const std::string& notification, GenaClientParams clientParams, GenaCallbackProccessor* pCallBackProcessor, ConnectionFactoryPtr pConnectionMgr);

		~GenaSubscriber();

		void subscribe();

		void resubscribe();

		void unsubscribe();

		void resetSubscribe();

		std::string getSID();

		std::string getTopic();

		std::string getUri();

		T_TopicFilterSet getFilterSet();

		void processStateCheck();

		SubscriberStatus getState() { return m_currentStatus;};
		
		void setState(SubscriberStatus status) { m_requestedStatus = status;}; 	

	private:
		std::string m_strSID;
		std::string m_strTopic;
		std::string m_subscribedUri;
		std::string m_notification;
		T_TopicFilterSet m_filterSet;
		GenaClientParams m_clientParammeter;
		GenaCallbackProccessor* m_pCallbackProc;
		ServerProxy* m_pServer;

		SubscriberStatus m_currentStatus;
		SubscriberStatus m_requestedStatus;
		time_t m_lastSubscribeTime;
	};

	typedef boost::shared_ptr<GenaSubscriber> GenaSubscriberPtr;

	//typedef std::map<unsigned long, GenaSubscriber> T_subscribersMap;
}

#endif // __GENA_SUBSCRIBER_H_INCLUDED__