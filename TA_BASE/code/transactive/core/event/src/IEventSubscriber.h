/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event/src/IEventSubscriber.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __IEVENT_SUBSCRIBER_H_INCLUDED__
#define __IEVENT_SUBSCRIBER_H_INCLUDED__

#include "core/event/src/EventItem.h"
#include <list>

namespace TA_Base_Core  
{
	typedef std::list< EventItem* > T_eventList;

	class IEventSubscriber
	{
	public:
		virtual IEventSubscriber();
		
		virtual ~IEventSubscriber(){};

		virtual void onReceiveEvents(const T_eventList& refEventList) = 0;

	private:
	};

	typedef std::vector< IEventSubscriber* > T_EventSubscribers;
}

#endif //__IEVENT_SUBSCRIBER_H_INCLUDED__
