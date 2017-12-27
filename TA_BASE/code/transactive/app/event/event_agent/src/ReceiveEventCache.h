/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/ReceiveEventCache.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __RECEIVE_EVENT_CACHE_H_INCLUDED__
#define __RECEIVE_EVENT_CACHE_H_INCLUDED__

#include "core/event/IDL/src/EventDetailCorbaDef.h"
#include "core/event/src/EventItem.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include <queue>
#include <vector>

namespace TA_Base_App
{
	typedef std::queue< TA_Base_Core::EventItem* > T_ReceiveEventsQueue;
	typedef std::vector< TA_Base_Core::EventItem* > T_vecEventData;
	class ReceiveEventCache
		: public TA_Base_Core::Thread

	{
	public:
		ReceiveEventCache();
		~ReceiveEventCache();

		virtual void run();
		virtual void terminate();

		void pushEvent(TA_Base_Core::EventItem* p_eventData);
		void pushEvents(TA_Base_Core::T_EventItemList& pEvents);

	private: // functions
		unsigned long _getQueueSize();
		void _getQueueItem(T_vecEventData& pEvents);

		/**
          * _corbaDefToEventData
		  * 
		  * data conversion from corba define to event data.
		  */
		TA_Base_Core::EventItem* _corbaDefToEventData(const TA_Base_Core::EventDetailCorbaDef& p_Event);
	private:
		bool m_bTerminate;
		TA_Base_Core::NonReEntrantThreadLockable m_threadLock;
		T_ReceiveEventsQueue m_recEventsQueue;
		unsigned long m_ulMaxCacheSize;
	};
}

#endif //__RECEIVE_EVENT_CACHE_H_INCLUDED__
