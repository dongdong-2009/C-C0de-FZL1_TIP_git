/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/coloured_controls/ColorComboBox.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// ColorComboBox.cpp : implementation file
//
// Copyright (C) 2002-2004  William Braynen
// Contact information can be found at www.samdurak.com
//
// The CColorComboBox MFC class consists of two files:
// ColorComboBox.cpp and ColorComboBox.h.  It is a subclass of
// CComboBox and allows to change the background and text colors of a
// Windows combo box control.  However, it changes the text color of
// all the text and does not implement multi-colored text or
// multi-colored backgrounds.
//
// Use this class just as you would a regular CComboBox class.
// Just replace 'CComboBox' in the declaration by 'CColorComboBox'.
// And don't forget to #include ColorComboBox.h
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "../stdafx.h"

#include <yvals.h>              // warning numbers get enabled in yvals.h 
#pragma warning(disable: 4786)  // identifier was truncated to 'number' characters in the debug information

#include "bus/mfc_extensions/src/coloured_controls/ColorComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RGB_DEFAULT_BACKGROUND RGB(255,255,255)
#define RGB_DEFAULT_TEXT   RGB(0,0,0)    

/////////////////////////////////////////////////////////////////////////////
// CColorComboBox

CColorComboBox::CColorComboBox (COLORREF backgroundColor, COLORREF textColor)
{
  if (0 == backgroundColor && 0 == textColor)
  {
    m_backgroundColor = RGB_DEFAULT_BACKGROUND;
    m_textColor       = RGB_DEFAULT_TEXT;
  }

  m_brush.CreateSolidBrush (m_backgroundColor);
}

CColorComboBox::~CColorComboBox()
{
  m_brush.DeleteObject();
}


BEGIN_MESSAGE_MAP(CColorComboBox, CComboBox)
	//{{AFX_MSG_MAP(CColorComboBox)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorComboBox message handlers

BOOL CColorComboBox::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
  if (WM_CTLCOLOR != message && WM_CTLCOLOREDIT != message)
  {
    return CComboBox::OnChildNotify(message, wParam, lParam, pLResult);
  }
    
  // Set the text foreground to red:
  HDC hdcChild = (HDC)wParam;
  ::SetTextColor(hdcChild, m_textColor);

  // Set the text background to green:
  ::SetBkColor(hdcChild, m_backgroundColor);

  // Send what would have been the return value of OnCtlColor() - the
  // brush handle - back in pLResult:
  *pLResult = (LRESULT)(m_brush.GetSafeHandle());

  // Return TRUE to indicate that the message was handled:
  return TRUE;
}

void CColorComboBox::SetBackgroundColor(COLORREF color)
{
  // Create a green brush for the background for the class of controls:
  m_brush.DeleteObject();
  m_brush.CreateSolidBrush (color);

  m_backgroundColor = color;
}

void CColorComboBox::SetTextColor(COLORREF color)
{
  m_textColor = color;
}


////
//
// Changes the background color of an OPEN drop-down list
//
HBRUSH CColorComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here

  if( nCtlColor == CTLCOLOR_LISTBOX || nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_MSGBOX )
  {
    hbr = m_brush;
	pDC->SelectStockObject(NULL_BRUSH);
    pDC->SetTextColor (m_textColor);
    pDC->SetBkColor (m_backgroundColor);
  }

	// TODO: Return a different brush if the default is not desired
	return hbr; //m_brush;
}


void CColorComboBox::MapIndexToColour(unsigned int index, COLORREF color)
{
	m_comboIndexToColourMap[index] = color;
}


void CColorComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	//TRACE("DrawItem()\n");
	if( lpDrawItemStruct->itemID == -1 )
		return;

	CDC			pDC;	
	pDC.Attach(lpDrawItemStruct->hDC);
	CBrush pBrush;
	CString		sItem;

    // Note: In this MFC extension, if the background colour of the combo box is white then 
	// the highlight mechanism mimics the windows highlight scheme. If the background color
	// is not white, the highlight mechanism will be realised in the form of different text
	// colours. The whole purpose of this is to allow the combo box to retain the 
	// background colour information to the user. Depending on the background colour, 
	// the text colour would need to change to suit the contrast etc.
	if(m_comboIndexToColourMap.size() == 0)
	{
		for(int i=0; i<GetCount(); ++i)
		{
			m_comboIndexToColourMap[i] = RGB(255, 255, 255);
		}
	}

	if(lpDrawItemStruct->itemState & ODS_FOCUS)
	{
		if(m_comboIndexToColourMap[lpDrawItemStruct->itemID] != RGB_DEFAULT_BACKGROUND)
		{
			pBrush.CreateSolidBrush(m_comboIndexToColourMap[lpDrawItemStruct->itemID]);
			pDC.FillRect(&lpDrawItemStruct->rcItem, &pBrush);
			pDC.SetTextColor(m_textColor);
		}
		else 
		{
			// If the background colour is white.. 
			pBrush.CreateSolidBrush(::GetSysColor(COLOR_HIGHLIGHT));
			pDC.FillRect(&lpDrawItemStruct->rcItem, &pBrush);
			// The colour of the text when highlighted.
			pDC.SetTextColor(RGB(255, 255, 255));
		}
	}
	else
	{
		pBrush.CreateSolidBrush(m_comboIndexToColourMap[lpDrawItemStruct->itemID]);
		pDC.FillRect(&lpDrawItemStruct->rcItem, &pBrush);
		// If the background is already white then we want the text
		// black.
		if(m_comboIndexToColourMap[lpDrawItemStruct->itemID] == RGB_DEFAULT_BACKGROUND)
		{

			pDC.SetTextColor(m_textColor); 
		}
		else // Otherwise, the text is going to be white.
		{
			pDC.SetTextColor(RGB(255, 255, 255));
		}
	}

	// Copy the text of the item to a string
	GetLBText(lpDrawItemStruct->itemID, sItem);
	pDC.SetBkMode(TRANSPARENT);

	// Draw the text after the images left postion
	//lpDrawItemStruct->rcItem.left = nListBoxTextLeft;
	pDC.SelectObject(GetFont());
	pDC.DrawText(sItem, &lpDrawItemStruct->rcItem, DT_VCENTER | DT_SINGLELINE);

	pDC.Detach();
}
