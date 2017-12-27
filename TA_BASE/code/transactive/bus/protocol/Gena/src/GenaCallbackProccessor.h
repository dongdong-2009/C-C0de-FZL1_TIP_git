/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: IRequestListener.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This class contains the interface for processing the request.
  */


#ifndef __CALLBACK_REQUEST_PROCCESSOR_H_INCLUDED__
#define __CALLBACK_REQUEST_PROCCESSOR_H_INCLUDED__
#pragma once

#include "core/synchronisation/src/Condition.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/QueueProcessor.h"
#include "AbstractGENARequestProccessor.h"
#include "IGENATopicDataStore.h"
#include "GENARequest.h"

#include <set>

namespace TA_Base_Bus
{
	class GenaCallbackProccessor
		: public AbstractGENARequestProccessor
		, private TA_Base_Core::QueueProcessor<GENATopicData>
	{
	public:
		typedef std::set<IGENATopicDataStore* > T_TopicDataStoreList;

		GenaCallbackProccessor();
		~GenaCallbackProccessor();

		void registerNotifyUpdate(IGENATopicDataStore* pTopicDataStore);

		void unRegisterNotifyUpdate(IGENATopicDataStore* pTopicDataStore);

		void resetTopicSync(const std::string& topicUri);
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

		/**
		 * QueueProcessor::processEvent()
		 *
		 * Adds the new event request to the current bundle
		 *
		 * @param newEvent	the request for the new event
		 *
		 */
		virtual void processEvent( boost::shared_ptr<GENATopicData> ptopicUpdate );


		/* 
		 * checkSyncState
		 * check for any synchronization state change
		 *
		 */
		void GenaCallbackProccessor::checkSyncState();

	private:
		class SyncStateMonitor
			: TA_Base_Core::Thread
		{
		public:
			SyncStateMonitor(GenaCallbackProccessor& processor);
			~SyncStateMonitor();

			virtual void run();

			virtual void terminate();

		private:
			bool m_bTerminate;
			TA_Base_Core::Condition m_condition;
			GenaCallbackProccessor& m_processor;
		};

	private:
		TA_Base_Core::ReEntrantThreadLockable m_threadLock;
		std::string extractTopic(const std::string& uri);
		void _processSyncUpdate(const std::string& topicName, const std::string& updateType);

		// Topic listeners
		T_TopicDataStoreList m_topicDataStoreList;
		typedef std::map<std::string, TopicSycnInfoPtr> T_TopicSyncStateMap;
		T_TopicSyncStateMap m_topicInitList;

		SyncStateMonitor* m_pSyncStateMonitor;
	};
}


#endif //__CALLBACK_REQUEST_PROCCESSOR_H_INCLUDED__