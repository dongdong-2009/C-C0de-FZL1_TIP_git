/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GENATopicPusblisherImp.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the class declaration for TopicPusblisherImp
  */

#ifndef __TOPIC_PUBLISHER_IMPLEMENT_H_INCLUDED__
#define __TOPIC_PUBLISHER_IMPLEMENT_H_INCLUDED__

#include "ITopicPublisher.h"
#include "GENAClientProxy.h"
#include "CommonDef.h"
#include "GENAConnectionFactory.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include <string>
#include <map>
#include <queue>

#pragma once

namespace TA_Base_Bus
{
	// Forward Declaration
	class GENAPublicationManager;
	typedef std::map<std::string, GENAClientProxy* > T_SubClientMap;

	class GENATopicPusblisherImp
		: public ITopicPublisher
		, private TA_Base_Core::Thread
	{
	public:
		/*
		* Constructor
		*/
		GENATopicPusblisherImp(GENAPublicationManager* pubMgr, const std::string& topicURI, ConnectionFactoryPtr pConnectionMgr);
		
		/*
		* Destructor
		*/
		~GENATopicPusblisherImp();

		/*
		* subscribe
		*/
		virtual void subscribe(SubscriberInfoPtr objSubInfo);

		/*
		* unSubscribe
		*/
		virtual void unSubscribe(const std::string& nSID);

		/*
		* publish
		*/
		virtual void publish(GENATopicDataPtr ptopicUpdate, std::string nSID = "");	

		/*
		* resendBirthMessage
		*/
		virtual void resendBirthMessage(std::string nSID);

	protected:
		/*
		* Virtual run
		*/
		virtual void run();
		
		/*
		* Virtual terminate
		*/
		virtual void terminate();

	private:
		std::string m_topicURI; //Topic to publish
		T_SubClientMap m_subcribedClients;
		ConnectionFactoryPtr m_pConnectionMgr;
		
		bool m_bTerminate;
		TA_Base_Core::Semaphore m_semaphore;
		TA_Base_Core::ReEntrantThreadLockable m_threadLock;
		TA_Base_Core::ReEntrantThreadLockable m_publishLock;
		std::queue<GENAClientProxy*> m_queueClients;
		GENAPublicationManager* m_pubMgr;
	}; //TopicPusblisherImp

} //TA_Base_Bus


#endif //__TOPIC_PUBLISHER_IMPLEMENT_H_INCLUDED__
