/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event/src/EventUtilities.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __EVENT_UTILITIES_H_INCLUDED__
#define __EVENT_UTILITIES_H_INCLUDED__

#include "core/event/IDL/src/EventDetailCorbaDef.h"
#include "core/event/src/EventItem.h"

namespace TA_Base_Core
{
	class EventUtilities
	{
	public:
		/**
          * EventCorbaToEventItem
          * 
		  * Converts EventDetailCorbadef to EventItem
		  *
          * @param The EventSerializeCorbaDef object to convert
		  *
		  * @return a pointer to EventItem
          */
		static TA_Base_Core::EventItem* EventCorbaToEventItem(const TA_Base_Core::EventDetailCorbaDef& evDetailCorba);

		/**
          * EventItemToEventCorba
          * 
		  * Converts EventItem to EventDetailCorbadef
		  *
		  * @param EventItem data to convert.
          * @param The EventSerializeCorbaDef object for the converted data
		  *
		  * @return none
          */
		static void EventItemToEventCorba(TA_Base_Core::EventItem* eventItem, TA_Base_Core::EventDetailCorbaDef& refRetVal);

		/**
          * EventInfoToCorbaDef
          * 
		  * Converts EventItemInfo to EventInfoCorbaDef
		  *
          * @param The EventItemInfo object to convert
		  * @param The EventInfoCorbaDef object for the converted data
		  *
		  * @return none
          */
		static void EventInfoToCorbaDef(EventItemInfo* event, EventInfoCorbaDef& refCorbaObj);

		/**
          * UnpackPackage
          * 
		  * Converts EventItemInfo to EventInfoCorbaDef
		  *
          * @param The EvDataPkgCorbaDef event package to unpack
		  * @param The T_EventItemList object for the converted data
		  *
		  * @return none
          */
		static void UnpackPackage(const TA_Base_Core::EvDataPkgCorbaDef* pEventPkg, TA_Base_Core::T_EventItemList& eventList);

	private:
		EventUtilities(); //Disable constructor
		EventUtilities(const EventUtilities& rhs); //Disable copy constructor
		EventUtilities& operator=(const EventUtilities &); // Assignment operator

	};
}

#endif //__EVENT_UTILITIES_H_INCLUDED__
