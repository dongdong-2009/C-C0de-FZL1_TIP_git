/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event/src/IEventProcessor.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __IEVENT_PROCESSOR_H_INCLUDED__
#define __IEVENT_PROCESSOR_H_INCLUDED__

//#include <vector>

namespace TA_Base_Core
{
	//typedef std::vector< char > T_VecCharArray;
	struct EvDataPkg 
	{
		unsigned long ulDataLen;

		unsigned long ulCompressLen;

		char* pszData;
		EvDataPkg():ulDataLen(0), ulCompressLen(0), pszData(0)
		{}

		~EvDataPkg(){delete [] pszData; };
	};

	class IEventDataProcessor
	{
	public: 
		virtual char* processEvent() = 0;

		virtual EvDataPkg* processEventEx(){return 0;};
	};
}

#endif //__IEVENT_PROCESSOR_H_INCLUDED__