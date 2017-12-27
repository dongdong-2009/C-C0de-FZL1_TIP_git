/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/PropagateActorLocal.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __PROPAGATE_ACTOR_LOCAL_H_INCLUDED__
#define __PROPAGATE_ACTOR_LOCAL_H_INCLUDED__

#include "app/event/event_agent/src/PropagateActor.h"
#include "app/event/event_agent/src/ReceiveEventCache.h"

#include "core/event/IDL/src/EventAgentCorbaDef.h"

#include "core/naming/src/NamedObject.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Condition.h"
#include "boost/shared_ptr.hpp"

#include <deque>
#include <vector>

namespace TA_Base_App
{
	class PropagateActorLocal
		: public PropagateActor, public TA_Base_Core::Thread
	{
	public: // Methods
		typedef std::deque< TA_Base_Core::EventItem* > T_EventQueue;
		typedef std::vector< TA_Base_Core::EventItem* > T_EventList;

		/**
		 * Constructor
		 */
		PropagateActorLocal();
		
		/**
		 * Destructor
		 */
		~PropagateActorLocal();

        /**
          * run
          *
          * Overrides the pure virtual run method declared in Thread
          */
        virtual void run();

        /**
          * terminate
          *
          * Overrides the pure virtual terminate method declared in Thread
          */
        virtual void terminate();

		/**
		 * push
		 *
		 * Overrides the pure virtual push method declared in PropagateActor 
		 * to insert data to cache and propagate data.
		 */
		virtual void push(TA_Base_Core::T_EventItemList& pEvents);

		/**
		 * pushEvent
		 *
		 * Overrides the pure virtual push method declared in PropagateActor 
		 * to insert data to cache and propagate data.
		 */
		virtual void pushEvent(TA_Base_Core::EventItem* pEvent);

		/**
		 * StartPropagation
		 *
		 * Overrides the pure virtual push method declared in PropagateActor 
		 */
		virtual void StartPropagation();

		/**
		 * StopPropagation
		 *
		 * Overrides the pure virtual push method declared in PropagateActor 
		 */
		virtual void StopPropagation();
		
	private: // Methods
		// Disable copy constructor and assignment operator
		PropagateActorLocal( const PropagateActorLocal& theActor);
		PropagateActorLocal& operator=(const PropagateActorLocal &);

		/**
          * _getCachedData
          *
          * returns the cached events for propagation
          */
        void _getCachedData(TA_Base_Core::T_EventItemList& cachedData);

		/**
          * _pushBackToCache
          *
          * insert the events back to the queue when propagation fails.
          */
        void _pushBackToCache(TA_Base_Core::T_EventItemList& cachedData);

	private: // Variables
		
		typedef TA_Base_Core::NamedObject<TA_Base_Core::EventAgentCorbaDef,
			TA_Base_Core::EventAgentCorbaDef_ptr,
			TA_Base_Core::EventAgentCorbaDef_var> EventAgentNameObj;
		// OCC event agent for propagation
		EventAgentNameObj m_eventAgentNameObj;

		// Flag to indicate thread needs to terminate
		bool m_bTerminate;

		// Thread lock for access synchronization on the event queue
		TA_Base_Core::ReEntrantThreadLockable m_threadLock;
		
		// Container for events to be submitted
		T_EventQueue m_eventQueue;

		// Submit event to central time interval
		unsigned long m_ulEvSubmitInterval;

		// Receive cache for temporary storage
		ReceiveEventCache m_receiveCache;

		// My Agent location key
		unsigned long m_ulMyLocation;

		//Maximum limit for number of events in the cache
		unsigned long m_ulMaxCacheSize;

		//Condition lock for propagating events
		TA_Base_Core::Condition m_conditionLock;

		//Event Propagation size
		unsigned long m_ulEventBatchSize;
	};
}

#endif //__PROPAGATE_ACTOR_LOCAL_H_INCLUDED__