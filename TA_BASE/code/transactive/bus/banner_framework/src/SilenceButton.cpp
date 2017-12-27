/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/banner_framework/src/SilenceButton.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class represents a silence button that can be added
  * to any banner page.
  *
  */

#include "stdafx.h"

#include "bus/banner_framework/src/SilenceButton.h"
#include "bus/resources/resource.h"
#include "bus/sound/sound_client/src/SoundManagerProxy.h"

#include "core/exceptions/src/SoundPlayerException.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_Base_Bus
{
    BEGIN_MESSAGE_MAP(SilenceButton, CButton)
	//{{AFX_MSG_MAP(SilenceButton)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    SilenceButton::SilenceButton(const std::string& soundCategory)
    :
    m_isSilenced(false),
    m_soundCategory(soundCategory),
    m_soundInterface(NULL)
    {
        try
	    {
		    m_soundInterface = new TA_Base_Bus::SoundManagerProxy(this);
	    }
	    catch(TA_Base_Core::SoundPlayerException& ex)
	    {
		    std::string error = "Unable to initialise SoundManager: ";
		    error += ex.what();
		    LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", error.c_str());
	    }
    }


    SilenceButton::~SilenceButton()
    {
		// Delete the SoundManagerProxy
		if(m_soundInterface != NULL)
		{
			delete m_soundInterface;
			m_soundInterface = NULL;
		}
	}
  

    void SilenceButton::initialise(bool isSilenced)
    {
        m_isSilenced = isSilenced;
	    handleCategorySilenced(m_soundCategory, isSilenced);
    }


    void SilenceButton::toggleState()
    {
	    handleCategorySilenced(m_soundCategory, !m_isSilenced);

	    try
	    {
            if (m_soundInterface != NULL)
            {
		        m_soundInterface->setSilence(m_soundCategory, m_isSilenced);
            }
	    }
	    catch(TA_Base_Core::SoundPlayerException& ex)
	    {
		    std::string error = "Error setting silence state of ";
            error += m_soundCategory;
            error += " sound category: ";
		    error += ex.what();
		    LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", error.c_str());
	    }
    }


    void SilenceButton::unsilence()
    {
        if (m_isSilenced)
        {
            try
            {
                handleCategorySilenced(m_soundCategory, !m_isSilenced);
                
                if (m_soundInterface != NULL)
                {
                    m_soundInterface->setSilence(m_soundCategory, false);
                }
            }
            catch(TA_Base_Core::SoundPlayerException& ex)
            {
                std::string error = "Error unsilencing ";
                error += m_soundCategory;
                error += " sound category: ";
		        error += ex.what();
		        LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", error.c_str());
	        }
	    }
    }


    void SilenceButton::handleCategorySilenced(const std::string& category, bool isSilenced)
    {
	    if (category == m_soundCategory)
	    {
		    // Our sound category was silenced or unsilenced.
		    m_isSilenced = isSilenced;

		    if (isSilenced)
		    {
			    SetIcon(AfxGetApp()->LoadIcon(IDI_SILENCE_ICON));
			    SetCheck(true);
		    }
		    else
		    {
			    SetIcon(AfxGetApp()->LoadIcon(IDI_UNSILENCE_ICON));
			    SetCheck(false);
		    }
	    }
    }


    void SilenceButton::handleAllSilenced(bool isSilenced)
    {
	    handleCategorySilenced(m_soundCategory, isSilenced);
    }
} // namespace TA_Base_Bus
