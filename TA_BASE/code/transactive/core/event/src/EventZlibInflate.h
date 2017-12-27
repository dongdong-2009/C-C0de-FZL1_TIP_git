/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event/src/EventZlibInflate.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __EVENT_ZLIB_INFLATE_H_INCLUDED__
#define __EVENT_ZLIB_INFLATE_H_INCLUDED__

#include "core/event/src/IEventProcessor.h"
#include "core/event/src/EventItem.h"


namespace TA_Base_Core
{
	class EventZlibInflate
		: public IEventDataProcessor
	{
	public:
		//Constructor
		EventZlibInflate(IEventDataProcessor* _objEvProcessor);
		EventZlibInflate(const char * pszData, unsigned long ulCompressLen, unsigned long ulUnCompressLen);

		//Destructor
		~EventZlibInflate();

		//Inherited function
		virtual char* processEvent();

	private:
		EventZlibInflate();
		EventZlibInflate(const EventZlibInflate& rhs);
		EventZlibInflate& operator=(const EventZlibInflate& rhs);
		
		IEventDataProcessor* m_objEvProcessor;
		const char * m_pszData;
		unsigned long m_ulCompressLen;
		unsigned long m_ulUnCompressLen;
	};
}

#endif //__EVENT_ZLIB_INFLATE_H_INCLUDED__