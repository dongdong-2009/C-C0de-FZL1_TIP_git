/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/PropagateActorCentral.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __PROPAGATE_ACTOR_CENTRAL_H_INCLUDED__
#define __PROPAGATE_ACTOR_CENTRAL_H_INCLUDED__

#include "app/event/event_agent/src/PropagateActor.h"
#include "app/event/event_agent/src/ReceiveEventCache.h"

namespace TA_Base_App
{
	class PropagateActorCentral
		: public PropagateActor
	{
	public:
		/**
		 * Constructor
		 */
		PropagateActorCentral();
		
		/**
		 * Destructor
		 */
		~PropagateActorCentral();

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
		 * verrides the pure virtual push method declared in PropagateActor 
		 */
		virtual void StartPropagation();

		/**
		 * StopPropagation
		 *
		 * verrides the pure virtual push method declared in PropagateActor 
		 */
		virtual void StopPropagation();

	private:
		ReceiveEventCache m_receiveCache;
	};
}


#endif //__PROPAGATE_ACTOR_CENTRAL_H_INCLUDED__