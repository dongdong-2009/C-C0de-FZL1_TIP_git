/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/toggleButton/toggleButton.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// ToggleButton.cpp : implementation file
//

#include <afxwin.h>         // MFC core and standard components
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include "ToggleButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ToggleButton

ToggleButton::ToggleButton() :
	m_unCheckedColour(::GetSysColor(COLOR_BTNFACE)), // use system
	m_checkedColour(RGB(255,0,0)), // default to red
	m_isChecked(false) //See class comments as to why I am not using the standard CButton "checked" functionality
{
}

ToggleButton::~ToggleButton()
{
}


BEGIN_MESSAGE_MAP(ToggleButton, CButton)
	//{{AFX_MSG_MAP(ToggleButton)
	ON_WM_ENABLE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ToggleButton message handlers

void ToggleButton::PreSubclassWindow() 
{
	CButton::PreSubclassWindow();

	//TODO
	//assert if client has set check box button Window styles
	//See class comments as to why I am not using the standard CButton "checked" functionality

	// set the owner draw flag, in case the client didn't do it for us
	ModifyStyle(0, BS_OWNERDRAW);
}

void ToggleButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
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

		// Now draw the inside of the button
		// that is a rectangle = our size minus the size of the edge...
		rect.DeflateRect( CSize(GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE)));

		// draw the inside, in the current colour
		pDC->FillSolidRect(rect, m_checkedColour);
	}
	else
	{
		 // otherwise draw a push btn brder in the "up" position
		pDC->DrawFrameControl(rect, DFC_BUTTON, DFCS_BUTTONPUSH);

		// Now draw the inside of the button
		// that is a rectangle = our size minus the size of the edge...
		rect.DeflateRect( CSize(GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE)));

		// draw the inside, in the current colour
		pDC->FillSolidRect(rect, m_unCheckedColour);
	}
	

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
		COLORREF prevColor = pDC->SetTextColor(::GetSysColor(IsWindowEnabled()? COLOR_BTNTEXT : COLOR_GRAYTEXT));
		pDC->TextOut(pt.x, pt.y, strText);

		// reset things the way we found them...
		pDC->SetBkMode(nMode);
		pDC->SetTextColor(prevColor);
	}
}

void ToggleButton::SetCheckedColour(COLORREF rgbColour)
{
	// store the new color
	m_checkedColour = rgbColour;

	Invalidate();
}

void ToggleButton::SetUnCheckedColour(COLORREF rgbColour)
{
	// store the new color
	m_unCheckedColour = rgbColour;

	Invalidate();
}

void ToggleButton::SetBackgroundColour(COLORREF rgbColour)
{
	// store the new color
	m_checkedColour = m_unCheckedColour = rgbColour;

	Invalidate();
}

void ToggleButton::OnEnable(BOOL bEnable) 
{
	CButton::OnEnable(bEnable);
	
	Invalidate();
}

void ToggleButton::OnDestroy() 
{
	CButton::OnDestroy();
}

//See class comments as to why I am not using the standard CButton "checked" functionality
bool ToggleButton::isChecked()
{
	return m_isChecked;
}
//See class comments as to why I am not using the standard CButton "checked" functionality
void ToggleButton::setCheck(bool checkIt)
{
	m_isChecked = checkIt;
	Invalidate();
}
