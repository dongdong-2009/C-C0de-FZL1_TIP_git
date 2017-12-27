/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/banner_framework/src/FlashIconButton.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * FlashIconButton.cpp : implementation file
  *
  */

#include "stdafx.h"

#pragma warning(disable:4786)   // Decorated name too long

#include "bus/banner_framework/src/FlashIconButton.h"
#include "bus/banner_framework/src/BannerFrameworkDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_Bus
{
    FlashIconButton::FlashIconButton(const TA_Base_Bus::BannerIcon& bannerIcon, int buttonId) 
    :
    m_bFlashing(false),
    m_currentFlashStateOn(false),
    m_flashPeriodMillisec(500),  // default to 1/2 second
    m_flashTimerID(114),
    m_buttonId(buttonId),
    m_isChecked(false) //See class comments as to why I am not using the standard CStatic "checked" functionality
    {
	    m_onImage = (HBITMAP)LoadImage(AfxGetInstanceHandle(), 
            MAKEINTRESOURCE(bannerIcon.onIcon), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);	

	    m_offImage = (HBITMAP)LoadImage(AfxGetInstanceHandle(), 
            MAKEINTRESOURCE(bannerIcon.offIcon), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);

	    m_onDownImage = (HBITMAP)LoadImage(AfxGetInstanceHandle(), 
            MAKEINTRESOURCE(bannerIcon.onDownIcon), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);

	    m_offDownImage = (HBITMAP)LoadImage(AfxGetInstanceHandle(), 
            MAKEINTRESOURCE(bannerIcon.offDownIcon), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
    }


    FlashIconButton::~FlashIconButton()
    {
	}

    BEGIN_MESSAGE_MAP(FlashIconButton, CStatic)
	    //{{AFX_MSG_MAP(FlashIconButton)
	    ON_WM_TIMER()
	    ON_WM_DESTROY()
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // FlashIconButton message handlers


    void FlashIconButton::SetFlashPeriod(unsigned long flashPeriodMillisec)
    {
	    // store the new blinking period
	    m_flashPeriodMillisec = flashPeriodMillisec;
	    
	    // if flashing
	    if (m_bFlashing)
	    {
		    // Stop & re-start flashing, this will force the timer to use
		    // the new flash period
		    StopFlashing();
		    StartFlashing();
	    }
    }


    void FlashIconButton::StartFlashing()
    {
	    if ( false == m_bFlashing ) // if not already flashing
	    {
		    // change the flash state right now... (this prevents delay introduced by the timer period)
		    GetNextFlashState();

		    if(m_currentFlashStateOn)
		    {
			    if(m_isChecked)
			    {
				    SetBitmap(m_onDownImage); 	
			    }
			    else
			    {
				    SetBitmap(m_onImage); 
			    }
		    }
		    else
		    {
			    if(m_isChecked)
			    {
				    SetBitmap(m_offDownImage); 	
			    }
			    else
			    {
				    SetBitmap(m_offImage); 
			    }
		    }
		    
		    // start the flash timer
		    m_flashTimerID = SetTimer(m_flashTimerID, m_flashPeriodMillisec, 0);
		    m_bFlashing = true;
		    Invalidate();
	    }
    }

    void FlashIconButton::StopFlashing()
    {
	    if (m_bFlashing) // if flashing
	    {
		    m_bFlashing = false;

		    // stop the flash timer
		    KillTimer(m_flashTimerID);

		    m_currentFlashStateOn = false;

		    if(m_isChecked)
		    {
			    SetBitmap(m_offDownImage); 	
		    }
		    else
		    {
			    SetBitmap(m_offImage); 
		    }

		    Invalidate();
	    }
    }


    bool FlashIconButton::GetNextFlashState()
    {
	    m_currentFlashStateOn = !m_currentFlashStateOn;	    
	    return m_currentFlashStateOn;
    }


    void FlashIconButton::OnTimer(UINT nIDEvent) 
    {
	    // if it's our flash timer
	    if (nIDEvent == m_flashTimerID)
	    {
		    if (m_bFlashing)
		    {
			    // get the next flash state
			    GetNextFlashState();
			    // repaint self
			    if(m_currentFlashStateOn)
			    {
				    if(m_isChecked)
				    {
					    SetBitmap(m_onDownImage); 	
				    }
				    else
				    {
					    SetBitmap(m_onImage); 
				    }
			    }
			    else
			    {
				    if(m_isChecked)
				    {
					    SetBitmap(m_offDownImage); 	
				    }
				    else
				    {
					    SetBitmap(m_offImage); 
				    }
			    }
		    }
	    }
	    
	    CStatic::OnTimer(nIDEvent);
    }


    void FlashIconButton::OnDestroy() 
    {
	    StopFlashing();

	    CStatic::OnDestroy();
    }


    void FlashIconButton::ToggleFlash()
    {
	    // if flashing, stop... if not, start
	    if (m_bFlashing)
	    {
		    StopFlashing();
	    }
	    else
	    {
		    StartFlashing();
	    }
    }


    //See class comments as to why I am not using the standard CStatic "checked" functionality
    bool FlashIconButton::isChecked()
    {
	    return m_isChecked;
    }


    //See class comments as to why I am not using the standard CStatic "checked" functionality
    void FlashIconButton::setCheck(bool checkIt)
    {
	    m_isChecked = checkIt;
	    if(m_isChecked)
	    {
		    SetBitmap(m_offDownImage); 
	    }
	    else
	    {
		    SetBitmap(m_offImage); 
	    }
	    
	    Invalidate();
    }
}
