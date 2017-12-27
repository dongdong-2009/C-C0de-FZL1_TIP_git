/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/PropagateActor.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __PROPAGATE_ACTOR_H_INCLUDED__
#define __PROPAGATE_ACTOR_H_INCLUDED__

#include "core/event/src/EventItem.h"

namespace TA_Base_App
{
	class PropagateActor 
	{
	public: // functions
		
		/**
		 * virtual Destructor
		 */
		virtual ~PropagateActor() {};

		/**
		 * StartPropagation
		 *
		 * starts propagation to occ event agent
		 */
		virtual void StartPropagation() = 0;

		/**
		 * StopPropagation
		 *
		 * stops propagation to occ event agent
		 */
		virtual void StopPropagation() = 0;
		
		/**
		 * push
		 *
		 * inserts events into cache and propagate it if necessary
		 */
		virtual void push(TA_Base_Core::T_EventItemList& pEvents) = 0;

		/**
		 * pushEvent
		 *
		 * inserts events into cache and propagate it if necessary
		 */
		virtual void pushEvent(TA_Base_Core::EventItem* pEvent) = 0;


	private: // variables

	};
}

#endif //__PROPAGATE_ACTOR_H_INCLUDED__