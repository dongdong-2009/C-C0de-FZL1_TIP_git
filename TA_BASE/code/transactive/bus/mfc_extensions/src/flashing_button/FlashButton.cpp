/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/flashing_button/FlashButton.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// FlashButton.cpp : implementation file
//

#include <afxwin.h>         // MFC core and standard components
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include "FlashButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// FlashButton

FlashButton::FlashButton() : m_bFlashing(false),
					m_flashPeriodMillisec(500),  // default to 1/2 second
					m_flashTimerID(123),
					m_currentFlashColour(::GetSysColor(COLOR_BTNFACE)), // use system
					m_flashColour(RGB(255,0,0)), // default to red
					m_isChecked(false) //See class comments as to why I am not using the standard CButton "checked" functionality
{
}

FlashButton::~FlashButton()
{
}


BEGIN_MESSAGE_MAP(FlashButton, CButton)
	//{{AFX_MSG_MAP(FlashButton)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FlashButton message handlers

void FlashButton::PreSubclassWindow() 
{
	CButton::PreSubclassWindow();

	//TODO
	//assert if client has set radio button Window styles
	//See class comments as to why I am not using the standard CButton "checked" functionality

	// set the owner draw flag, in case the client didn't do it for us
	ModifyStyle(0, BS_OWNERDRAW);
}

void FlashButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// get the MFC version of our drawing context
	CDC* pDC   = CDC::FromHandle(lpDrawItemStruct->hDC);
	// same for our bounding rectangle
	CRect rect = lpDrawItemStruct->rcItem;
	// and this is our state
	UINT state = lpDrawItemStruct->itemState;

	// get the text, we'll draw it later
	CString strText;
	GetWindowText(strText);
	
	// if we're currently pushed down or the (radio)button is checked...
	//See class comments as to why I am not using the standard CButton "checked" functionality
	if (state & ODS_SELECTED || isChecked())
	{
		// get the system to draw a pushed down push button border
		pDC->DrawFrameControl(rect, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_PUSHED);
	}
	else
	{
		 // otherwise draw a push btn brder in the "up" position
		pDC->DrawFrameControl(rect, DFC_BUTTON, DFCS_BUTTONPUSH);
	}

	// ok now that we've drawn our edge, let's get ready to draw the inside
	// that is a rectangle = our size minus the size of the edge...
	rect.DeflateRect( CSize(GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE)));

	// draw the inside, in the current colour
	pDC->FillSolidRect(rect, m_currentFlashColour);
	

	// finally, draw the btn caption if it isn't an empty string 
	if (!strText.IsEmpty())
	{
		CSize Extent = pDC->GetTextExtent(strText);
		// Centered text...
		// to be precise, we should use the btn styles and adjust
		// accordingly to handle left/right alignment.
		// It's easy enough, but it's late and I can't be bothered
		CPoint pt( rect.CenterPoint().x - Extent.cx/2, 
					rect.CenterPoint().y - Extent.cy/2 );

		if (state & ODS_SELECTED) // if the button is selected
		{
			pt.Offset(1,1); // offset the text by 1 pixel right, 1 pixel down, 
							// just like standard windows buttons
		}

		int nMode = pDC->SetBkMode(TRANSPARENT);
		
		// paint the btn caption, using the system defined colour
		COLORREF prevColor = pDC->SetTextColor(::GetSysColor(COLOR_BTNTEXT));
		pDC->TextOut(pt.x, pt.y, strText);

		// reset things the way we found them...
		pDC->SetBkMode(nMode);
		pDC->SetTextColor(prevColor);
	}
}

void FlashButton::SetFlashPeriod(unsigned long flashPeriodMillisec)
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


void FlashButton::StartFlashing()
{
	if (! m_bFlashing) // if not already flashing
	{
		// change the color, right now... (this prevents delay introduced by the timer period)
		GetNextFlashColour();
		Invalidate();
		// start the flash timer
		m_flashTimerID = SetTimer(m_flashTimerID, m_flashPeriodMillisec, 0);
		m_bFlashing = true;
	}
}

void FlashButton::StopFlashing()
{
	if (m_bFlashing) // if flashing
	{
		// stop the flash timer
		KillTimer(m_flashTimerID);
		m_bFlashing = false;

		// reset the colour to the default system colour
		m_currentFlashColour = ::GetSysColor(COLOR_BTNFACE);
		// re-draw self
		Invalidate();
	}
}

void FlashButton::SetFlashColour(COLORREF rgbColour)
{
	// store the new color
	m_flashColour = rgbColour;

	// if flashing
	if (m_bFlashing)
	{
		// stop and restart, this will re-create the coloured brush
		StopFlashing();
		StartFlashing();
	}
}

COLORREF FlashButton::GetNextFlashColour()
{
	// if the current background colour is the flash colour
	if (m_currentFlashColour == m_flashColour)
	{
		// then the next colour is that of a button, as defined by the system
		m_currentFlashColour = ::GetSysColor(COLOR_BTNFACE);
	}
	else
	{
		// else use the alternate colour
		m_currentFlashColour = m_flashColour;
	}

	return m_currentFlashColour;
}

void FlashButton::OnTimer(UINT nIDEvent) 
{
	// if it's our flash timer
	if (nIDEvent == m_flashTimerID)
	{
		// get the next flash colour
		GetNextFlashColour();
		// repaint self
		Invalidate();
	}
	
	CButton::OnTimer(nIDEvent);
}

void FlashButton::OnDestroy() 
{
	StopFlashing();

	CButton::OnDestroy();
}

void FlashButton::ToggleFlash()
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

//See class comments as to why I am not using the standard CButton "checked" functionality
bool FlashButton::isChecked()
{
	return m_isChecked;
}
//See class comments as to why I am not using the standard CButton "checked" functionality
void FlashButton::setCheck(bool checkIt)
{
	m_isChecked = checkIt;
	Invalidate();
}
