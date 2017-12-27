/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event/src/EventDeSerialize.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __EVENT_DE_SERIALIZE_H_INCLUDED__
#define __EVENT_DE_SERIALIZE_H_INCLUDED__

#include "core/event/src/IEventProcessor.h"
#include "core/event/src/EventItem.h"
#include "core/event/src/EventSerializeHelper.h"

namespace TA_Base_Core
{
	class EventDeSerialize
		: public IEventDataProcessor
	{
	public:
		//Constructor
		EventDeSerialize(char* pszData, T_EventItemList& _pEventList);
		EventDeSerialize(IEventDataProcessor* _pObjEvProcessor, T_EventItemList& _pEventList);

		//Destructor
		~EventDeSerialize();

		//Inherited function
		virtual char* processEvent();

	private:
		EventDeSerialize();
		EventDeSerialize(const EventDeSerialize& rhs);
		EventDeSerialize& operator=(const EventDeSerialize& rhs);
		const char* deSerializeEvent(const char* pszData, EventItem* pEventData);

		T_EventItemList& m_pEventList;
		IEventDataProcessor* m_pObjEvProcessor;
		char* m_pszData;
	};
}

#endif //__EVENT_DE_SERIALIZE_H_INCLUDED__