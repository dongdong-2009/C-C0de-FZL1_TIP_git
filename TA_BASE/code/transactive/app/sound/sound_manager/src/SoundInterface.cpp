/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/sound/sound_manager/src/SoundInterface.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// SoundInterface.cpp : Implementation of SoundInterface

#include "stdafx.h"

#include "bus/sound/sound_client/src/SoundManager.h"
#include "SoundInterface.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/SoundPlayerException.h"

#include <iostream>
#include <comdef.h>
#include <afxole.h>

using TA_Base_Core::DebugUtil;


STDMETHODIMP SoundInterface::setPriority(BSTR category, int priority)
{
	std::string categoryStr = _bstr_t(category);

	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Setting highest priority of %s category to %d",
		categoryStr.c_str(), priority);

	try
	{
		m_categoryManager.setPriority(categoryStr, priority);
	}
	catch(TA_Base_Core::SoundPlayerException& ex)
	{
		std::string errorMsg(ex.what());
		LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", errorMsg.c_str());

		return reportError(0, const_cast<char*>(errorMsg.c_str()));
	}
	catch(...)
	{
		std::string errorMsg("Caught unknown exception while setting the priority");
		LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", errorMsg.c_str());
		
		return reportError(0, const_cast<char*>(errorMsg.c_str()));
	}

	return S_OK;
}


STDMETHODIMP SoundInterface::setSilence(BSTR category, VARIANT_BOOL isSilenced)
{
	std::string categoryStr = _bstr_t(category);

	bool isSilencedBool = (isSilenced == VARIANT_TRUE ? true : false);

	LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Setting silence state of %s category to %s",
		categoryStr.c_str(), (isSilenced == VARIANT_TRUE ? "true" : "false") );

	try
	{
		m_categoryManager.setSilence(categoryStr, isSilencedBool);
	}
	catch(TA_Base_Core::SoundPlayerException& ex)
	{
		std::string errorMsg(ex.what());
		LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", errorMsg.c_str());

		return reportError(0, const_cast<char*>(errorMsg.c_str()));
	}
	catch(...)
	{
		std::string errorMsg("Caught unknown exception while setting the silence state");
		LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", errorMsg.c_str());
		
		return reportError(0, const_cast<char*>(errorMsg.c_str()));
	}

	Fire_categorySilenced(category, isSilenced);

    // If this category is linked to another category, also fire an event that says the linked
    // category has changed.
    std::string linkedCategoryStr = m_categoryManager.getJoinedCategory(categoryStr);
    if (linkedCategoryStr != "")
    {
        BSTR linkedCategory( _bstr_t(linkedCategoryStr.c_str()) );
        Fire_categorySilenced(linkedCategory, isSilenced);
    }

	return S_OK;
}


STDMETHODIMP SoundInterface::setSilenceAll(VARIANT_BOOL isSilenced)
{
	bool isSilencedBool = (isSilenced == VARIANT_TRUE ? true : false);

	LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Setting silence state of all categories to %s",
		(isSilencedBool ? "true" : "false") );

	try
	{
		m_categoryManager.setSilenceAll(isSilencedBool);
	}
	catch(TA_Base_Core::SoundPlayerException& ex)
	{
		std::string errorMsg(ex.what());
		LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", errorMsg.c_str());

		return reportError(0, const_cast<char*>(errorMsg.c_str()));
	}
	catch(...)
	{
		std::string errorMsg("Caught unknown exception while setting the silence state");
		LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", errorMsg.c_str());
		
		return reportError(0, const_cast<char*>(errorMsg.c_str()));
	}

	Fire_allSilenced(isSilenced);
	return S_OK;
}


STDMETHODIMP SoundInterface::joinCategories(BSTR primaryCategory, BSTR secondaryCategory)
{
	std::string primaryCategoryStr = _bstr_t(primaryCategory);
	std::string secondaryCategoryStr = _bstr_t(secondaryCategory);

	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Joining categories %s and %s",
		primaryCategoryStr.c_str(), secondaryCategoryStr.c_str());

	try
	{
		m_categoryManager.joinCategories(primaryCategoryStr, secondaryCategoryStr);
	}
	catch(TA_Base_Core::SoundPlayerException& ex)
	{
		std::string errorMsg(ex.what());
		LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", errorMsg.c_str());

		return reportError(0, const_cast<char*>(errorMsg.c_str()));
	}
	catch(...)
	{
		std::string errorMsg("Caught unknown exception while joining categories");
		LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", errorMsg.c_str());
		
		return reportError(0, const_cast<char*>(errorMsg.c_str()));
	}
	
	return S_OK;
}


STDMETHODIMP SoundInterface::isSilenced(BSTR category, VARIANT_BOOL* isSilenced)
{
	std::string categoryStr = _bstr_t(category);

	try
	{
		bool isSilencedBool = m_categoryManager.isSilenced(categoryStr);

		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Silence state of %s category is %s",
			categoryStr.c_str(), (isSilencedBool ? "true" : "false") );

		*isSilenced = (isSilencedBool ? VARIANT_TRUE : VARIANT_FALSE);
	}
	catch(TA_Base_Core::SoundPlayerException& ex)
	{
		std::string errorMsg(ex.what());
		LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", errorMsg.c_str());

		return reportError(0, const_cast<char*>(errorMsg.c_str()));
	}
	catch(...)
	{
		std::string errorMsg("Caught unknown exception while retrieving the silence state");
		LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", errorMsg.c_str());
		
		return reportError(0, const_cast<char*>(errorMsg.c_str()));
	}

	return S_OK;
}


STDMETHODIMP SoundInterface::isAllSilenced(VARIANT_BOOL* isSilenced)
{
	try
	{
		bool isSilencedBool = m_categoryManager.isAllSilenced();

		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Silence state of all categories is %s",
			(isSilencedBool ? "true" : "false") );

		*isSilenced = (isSilencedBool ? VARIANT_TRUE : VARIANT_FALSE);
	}
	catch(TA_Base_Core::SoundPlayerException& ex)
	{
		std::string errorMsg(ex.what());
		LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", errorMsg.c_str());

		return reportError(0, const_cast<char*>(errorMsg.c_str()));
	}
	catch(...)
	{
		std::string errorMsg("Caught unknown exception while retrieving the silence state");
		LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", errorMsg.c_str());
		
		return reportError(0, const_cast<char*>(errorMsg.c_str()));
	}

	return S_OK;
}


STDMETHODIMP SoundInterface::reportError(long errorCode, LPSTR errorString)
{
	ICreateErrorInfo *pCrErrInfo = NULL;
	IErrorInfo *pErrInfo = NULL;

	if (SUCCEEDED(CreateErrorInfo(&pCrErrInfo)))
	{
		
		pCrErrInfo->SetDescription(_com_util::ConvertStringToBSTR(errorString));
		pCrErrInfo->SetGUID(IID_ISoundInterface);
		errorString = NULL;
	}

	if (SUCCEEDED(pCrErrInfo->QueryInterface(IID_IErrorInfo, 
             reinterpret_cast<void**>(&pErrInfo))))
    {
        //  Set error information for current thread.
        SetErrorInfo(0, pErrInfo);
        pErrInfo->Release();
    }

	pCrErrInfo->Release();

	return MAKE_HRESULT(1, FACILITY_ITF, errorCode);
}


STDMETHODIMP SoundInterface::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = { &IID_ISoundInterface };
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}
