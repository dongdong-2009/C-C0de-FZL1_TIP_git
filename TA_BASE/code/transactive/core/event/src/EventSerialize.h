/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event/src/EventSerialize.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __EVENT_SERIALIZE_H_INCLUDED__
#define __EVENT_SERIALIZE_H_INCLUDED__

#include "core/event/src/IEventProcessor.h"
#include "core/event/src/EventItem.h"
#include "core/event/src/EventSerializeHelper.h"

namespace TA_Base_Core
{
	class EventSerialize
		:public IEventDataProcessor
	{
	public:
		//Constructor
		EventSerialize(const T_EventItemList& pEventList);
		
		//Destructor
		~EventSerialize();

		//Inherited function
		virtual char* processEvent();

		virtual EvDataPkg* processEventEx();
	private:
		EventSerialize();
		EventSerialize(const EventSerialize& rhs);
		EventSerialize& operator=(const EventSerialize& rhs);
		char* serializeEvent(char* pWPos, EvSzItemInfo& pEvSzItemInfo, EventItem* pEventData);
		unsigned long calculateDataLen(T_EvSzItemInfoList & evIteminfoList);
			
		const T_EventItemList& m_pEventList;
	};
}

#endif //__EVENT_SERIALIZE_H_INCLUDED__