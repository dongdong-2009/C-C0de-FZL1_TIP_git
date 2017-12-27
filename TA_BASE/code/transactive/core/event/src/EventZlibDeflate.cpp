/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event/src/EventZlibDeflate.cpp $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#include "core/event/src/EventZlibDeflate.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "zlib.h"

namespace TA_Base_Core
{
	//Constructor
	EventZlibDeflate::EventZlibDeflate(IEventDataProcessor* _objEvProcessor)
		: m_objEvProcessor(_objEvProcessor)
		, m_pPackage(0)
	{
		TA_ASSERT(0 != m_objEvProcessor, "EventProcessor cannot be NULL");
	}

	EventZlibDeflate::EventZlibDeflate(EvDataPkg* _pPackage)
		: m_objEvProcessor(0)
		, m_pPackage(_pPackage)
	{
	}

	//Destructor
	EventZlibDeflate::~EventZlibDeflate(){}

	//Inherited function
	char* EventZlibDeflate::processEvent()
	{
		char * pszRetData = 0;
		char * pszTempData = 0;
	
		pszTempData = m_objEvProcessor->processEvent();
		unsigned long ulUnCompressedlen = strlen(pszTempData);
		// Required size by Zlib
		unsigned long ulCompressedLen = (ulUnCompressedlen * 1.1) + 12;

		// Allocate buffer size for compressed data
		pszRetData = new char[ulCompressedLen];
		memset(pszRetData, 0, ulCompressedLen);

		int z_result = compress(
			(Bytef*) pszRetData,         // destination buffer,
			&ulCompressedLen,    // pointer to var containing size of compressed buffer
			(Bytef*) pszTempData,           // source data for compression
			ulUnCompressedlen ) ;    // size of source data in bytes

		switch( z_result )
		{
		case Z_OK:
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data Compression SUCCESS!");
			break;

		case Z_MEM_ERROR:
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data Compression Error! Out of Memory");
			return 0;

		case Z_BUF_ERROR:
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data Compression Error! output buffer wasn't large enough");
			return 0;
		}
		
		delete [] pszTempData;

		return pszRetData;
	}

	EvDataPkg* EventZlibDeflate::processEventEx()
	{
		EvDataPkg * pRetData = new EvDataPkg;
		EvDataPkg * pTempData = 0;

		if (0 != m_objEvProcessor)
			pTempData = m_objEvProcessor->processEventEx();
		else
			pTempData = m_pPackage;
		
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "EventZlibDeflate::processEventEx Start"); 

		unsigned long ulUnCompressedlen = pTempData->ulDataLen;
		// Required size by Zlib
		unsigned long ulCompressedLen = (ulUnCompressedlen * 1.1) + 12;

		// Allocate buffer size for compressed data
		pRetData->pszData = new char[ulCompressedLen];
		memset(pRetData->pszData, 0, ulCompressedLen);

		int z_result = compress(
			(Bytef*) pRetData->pszData,         // destination buffer,
			&ulCompressedLen,    // pointer to var containing size of compressed buffer
			(Bytef*) pTempData->pszData,           // source data for compression
			ulUnCompressedlen ) ;    // size of source data in bytes

		switch( z_result )
		{
		case Z_OK:
			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data Compression SUCCESS! Original Size=%lu, Compressed Size=%lu", ulUnCompressedlen, ulCompressedLen);
			break;

		case Z_MEM_ERROR:
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data Compression Error! Out of Memory");
			return 0;

		case Z_BUF_ERROR:
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data Compression Error! output buffer wasn't large enough");
			return 0;
		}

		pRetData->ulDataLen = pTempData->ulDataLen;
		pRetData->ulCompressLen = ulCompressedLen;
		
		delete pTempData;

		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "EventZlibDeflate::processEventEx End"); 

		return pRetData;
	}
}