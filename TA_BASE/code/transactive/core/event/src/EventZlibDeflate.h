/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event/src/EventZlibDeflate.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __EVENT_ZLIB_DEFLATE_H_INCLUDED__
#define __EVENT_ZLIB_DEFLATE_H_INCLUDED__

#include "core/event/src/IEventProcessor.h"
#include "core/event/src/EventItem.h"


namespace TA_Base_Core
{
	class EventZlibDeflate
		:public IEventDataProcessor
	{
	public:
		//Constructor
		EventZlibDeflate(IEventDataProcessor* _objEvProcessor);
		EventZlibDeflate(EvDataPkg* _pPackage);

		//Destructor
		~EventZlibDeflate();

		//Inherited function
		virtual char* processEvent();

		virtual EvDataPkg* processEventEx();
	private:
		EventZlibDeflate();
		EventZlibDeflate(const EventZlibDeflate& rhs);
		EventZlibDeflate& operator=(const EventZlibDeflate& rhs);
		
		IEventDataProcessor* m_objEvProcessor;
		EvDataPkg * m_pPackage;
	};
}

#endif //__EVENT_ZLIB_DEFLATE_H_INCLUDED__