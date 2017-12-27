/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/PropagateActorCentral.cpp $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#include "app/event/event_agent/src/PropagateActorCentral.h"
#include "app/event/event_agent/src/EventCache.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_App
{
	/**
	 * Constructor
	 */
	PropagateActorCentral::PropagateActorCentral()
	{
		FUNCTION_ENTRY("PropagateActorCentral constructor");
		m_receiveCache.start();
		FUNCTION_EXIT;
	}
	
	/**
	 * Destructor
	 */
	PropagateActorCentral::~PropagateActorCentral()
	{
		FUNCTION_ENTRY("PropagateActorCentral destructor");
		m_receiveCache.terminateAndWait();
		FUNCTION_EXIT;
	}

	/**
	 * push
	 */
	void PropagateActorCentral::push(TA_Base_Core::T_EventItemList& pEvents)
	{
		FUNCTION_ENTRY("push");
		
		// Insert into events cache
		m_receiveCache.pushEvents(pEvents);

		FUNCTION_EXIT;
	}

	/**
	 * pushEvent
	 */
	void PropagateActorCentral::pushEvent(TA_Base_Core::EventItem* pEvent)
	{
		FUNCTION_ENTRY("pushEvent");
		TA_ASSERT(0 != pEvent, "pEvents must be a valid pointer.");
		
		// Insert into event cache
		m_receiveCache.pushEvent(pEvent);

		FUNCTION_EXIT;
	}

	/**
	 * StartPropagation
	 */
	void PropagateActorCentral::StartPropagation()
	{
		FUNCTION_ENTRY("StartPropagation");
		//Empty
		FUNCTION_EXIT;
	}

	/**
	 * StopPropagation
	 */
	void PropagateActorCentral::StopPropagation()
	{
		FUNCTION_ENTRY("StopPropagation");
		//Empty
		FUNCTION_EXIT;
	}
}
