/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GENAPublicationManager.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This class contains the interface for processing the request.
  */

#ifndef __PUBLICATION_MANAGER_H_INCLUDED__
#define __PUBLICATION_MANAGER_H_INCLUDED__
#pragma once
#include "TopicPublisherFactory.h"
#include "ITopicPublisher.h"
#include "CommonDef.h"
#include "GENATopicData.h"
#include "IGENATopicDataStore.h"
#include "GENAConnectionFactory.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include <string>

namespace TA_Base_Bus
{
	// Forward Declaration
	class IGENATopicDataStore;

	class GENAPublicationManager
	{
	public:
		/*
		* Constructor
		*/
		GENAPublicationManager(ConnectionFactoryPtr pConnectionMgr);

		/*
		* Destructor
		*/
		~GENAPublicationManager(void);

		/*
		* isTopicURIExist
		*/
		bool isTopicURIExist(const std::string& topicURI);

		/*
		* addSubscriber
		*/
		unsigned long addSubscriber(SubscriberInfoPtr subscriber);

		/*
		* removeSubscriber
		*/
		void removeSubscriber(const std::string& topicURI, const std::string& sid);

		/*
		* onNotify
		*/
		void publish(GENATopicDataPtr ptopicUpdate);

		/*
		* registerDataStore
		*/
		void registerDataStore(IGENATopicDataStore* pDataStore);

		/*
		* getDataStore
		*/
		IGENATopicDataStore* getDataStore() {return m_pDataStore; } 

		/*
		* resendBirthMessage
		*/
		void resendBirthMessage(const std::string& topicURI, std::string nSID);
	private:
		TA_Base_Core::NonReEntrantThreadLockable m_threadLock;
		// map containing the topic to publisher data
		TopicPublisherMap_T m_topicPublishers;
		TopicPublisherFactoryPtr m_pTopicPublisherFactory;
		IGENATopicDataStore* m_pDataStore;
	};

} //TA_Base_Bus


#endif //__PUBLICATION_MANAGER_H_INCLUDED__