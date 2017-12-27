/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event/src/EventZlibInflate.cpp $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#include "core/event/src/EventZlibInflate.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "zlib.h"

namespace TA_Base_Core
{
	//Constructor
	EventZlibInflate::EventZlibInflate(IEventDataProcessor* _objEvProcessor)
		: m_objEvProcessor(_objEvProcessor)
		, m_pszData(0)
		, m_ulCompressLen(0)
		, m_ulUnCompressLen(0)
	{
		TA_ASSERT(0 != m_objEvProcessor, "EventProcessor cannot be NULL");
	}

	EventZlibInflate::EventZlibInflate(const char * pszData, unsigned long ulCompressLen, unsigned long ulUnCompressLen)
		: m_objEvProcessor(0)
		, m_pszData(pszData)
		, m_ulCompressLen(ulCompressLen)
		, m_ulUnCompressLen(ulUnCompressLen)
	{
	}

	//Destructor
	EventZlibInflate::~EventZlibInflate()
	{}

	//Inherited function
	char* EventZlibInflate::processEvent()
	{
		char * pszRetData = new char[m_ulUnCompressLen];
		const char * pszTempData = m_pszData;
		memset(pszRetData, 0, m_ulUnCompressLen);
		
		// now uncompress
		int z_result = uncompress(
			(Bytef*)pszRetData,       // destination for the uncompressed
			&m_ulUnCompressLen,  // length of destination (uncompressed) buffer
			(Bytef*)pszTempData,   // source buffer - the compressed data
			m_ulCompressLen );   // length of compressed data in bytes

		switch( z_result )
		{
		case Z_OK:
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data DeCompression SUCCESS!");
			break;

		case Z_MEM_ERROR:
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data DeCompression Error! Out of Memory");
			return 0;

		case Z_BUF_ERROR:
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data DeCompression Error! output buffer wasn't large enough");
			return 0;
		}

		return pszRetData;
	}

}
