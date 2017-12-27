/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/sound/sound_client/src/SoundManagerProxy.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/02/26 16:16:02 $
  * Last modified by:  $Author: Noel $
  *
  * This class acts as a wrapper to the Sound Manager
  * COM interface.
  *
  */

#include "stdafx.h"
#include "bus/sound/sound_client/src/SoundManagerProxy.h"
#include "bus/sound/sound_client/src/SoundInterfaceSink.h"

#include "core/exceptions/src/SoundPlayerException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include <objbase.h>
#include <ole2.h>
#include <afxole.h>

#include <string>


namespace TA_Base_Bus
{

	SoundManagerProxy::SoundManagerProxy(ISoundObserver* observer)
	: m_soundInterface(NULL), m_soundInterfaceSink(NULL), m_isOleInitialised(false)
	{
        HRESULT oleInit = OleInitialize(NULL);
        if (oleInit == S_OK || oleInit == S_FALSE)
        {
            m_isOleInitialised = true;
        }

        // Initialise COM interface
		HRESULT hresult = CoCreateInstance(CLSID_SoundInterface, NULL, CLSCTX_LOCAL_SERVER,
								IID_ISoundInterface, (void**)&m_soundInterface);

		if (FAILED(hresult))
		{
			TA_THROW(TA_Base_Core::SoundPlayerException("Unable to initialise COM interface"));
		}

		// To avoid creation of EventSink while there is no observer -  CL-BannerHangup-Sound Issue
		if(observer != NULL)
		{
			m_soundInterfaceSink = new SoundInterfaceSink(m_soundInterface, observer);
		}

	}


	SoundManagerProxy::~SoundManagerProxy()
	{
		//TD17838 Haipeng added
		if (m_soundInterface != NULL)
		{
			m_soundInterface->Release();
		}
		
		if(m_soundInterfaceSink != NULL)
		{
			delete m_soundInterfaceSink;
			m_soundInterfaceSink = NULL;
		}
		//TD17838 Haipeng added


        if (m_isOleInitialised)
        {
            OleUninitialize();
        }
	}


	void SoundManagerProxy::setPriority(std::string category, int priority)
	{
		// Make the call on the Sound Manager.
		HRESULT hresult = m_soundInterface->setPriority(_bstr_t(category.c_str()), priority);
		
		// Check the return result.
		checkReturnValue(hresult);
	}


	void SoundManagerProxy::setSilence(std::string category, bool isSilenced)
	{
		// Convert the bool to a VARIANT_BOOL.
		VARIANT_BOOL isSilencedVariant = isSilenced ? VARIANT_TRUE : VARIANT_FALSE;

		// Make the call on the Sound Manager.
		HRESULT hresult = m_soundInterface->setSilence(_bstr_t(category.c_str()), isSilencedVariant);
		
		// Check the return result.
		checkReturnValue(hresult);
	}


	void SoundManagerProxy::setSilenceAll(bool isSilenced)
	{
		// Convert the bool to a VARIANT_BOOL.
		VARIANT_BOOL isSilencedVariant = isSilenced ? VARIANT_TRUE : VARIANT_FALSE;

		// Make the call on the Sound Manager.
		HRESULT hresult = m_soundInterface->setSilenceAll(isSilencedVariant);
		
		// Check the return result.
		checkReturnValue(hresult);
	}


	void SoundManagerProxy::joinCategories(std::string primaryCategory, std::string secondaryCategory)
	{
		// Make the call on the Sound Manager.
		HRESULT hresult = m_soundInterface->joinCategories( _bstr_t(primaryCategory.c_str()), 
															_bstr_t(secondaryCategory.c_str()) );

		// Check the return result.
		checkReturnValue(hresult);
	}


	bool SoundManagerProxy::isSilenced(std::string category) const
	{
		VARIANT_BOOL* returnValue = new VARIANT_BOOL;

		// Make the call on the Sound Manager.
		HRESULT hresult = m_soundInterface->isSilenced(_bstr_t(category.c_str()), returnValue);
		
		bool isSilencedBool = (*returnValue == VARIANT_TRUE);
		delete returnValue;

		// Check the return result.
		checkReturnValue(hresult);

		return isSilencedBool;
	}


	bool SoundManagerProxy::isAllSilenced() const
	{
		VARIANT_BOOL* returnValue = new VARIANT_BOOL;

		// Make the call on the Sound Manager.
		HRESULT hresult = m_soundInterface->isAllSilenced(returnValue);
		
		bool isSilencedBool = (*returnValue == VARIANT_TRUE);
		delete returnValue;

		// Check the return result.
		checkReturnValue(hresult);

		return isSilencedBool;
	}

	void SoundManagerProxy::stopAllSound()
	{
		//We set priority to zero to stop all sound
		// Make the call on the Sound Manager.
		HRESULT hresult = m_soundInterface->setPriority(_bstr_t("ALL"), 0); 

		// Check the return result.
		checkReturnValue(hresult);
	}

	void SoundManagerProxy::checkReturnValue(HRESULT hresult) const
	{
		if(FAILED(hresult))
		{
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "HRESULT is %d", hresult);

			// Get the error info.
			IErrorInfo* errorInfo = 0;
			GetErrorInfo(NULL, &errorInfo);

			// Retrieve the error message.
			CComBSTR errorMsgBSTR;
			if (errorInfo != NULL)
			{
				errorInfo->GetDescription(&errorMsgBSTR);
				char* errorMsg = _com_util::ConvertBSTRToString(errorMsgBSTR);
				errorInfo->Release();
				TA_THROW(TA_Base_Core::SoundPlayerException(errorMsg));
			}
			else
			{
				TA_THROW(TA_Base_Core::SoundPlayerException("Unknown error occurred"));
			}
		}
	}

} // namespace TA_Base_Bus
