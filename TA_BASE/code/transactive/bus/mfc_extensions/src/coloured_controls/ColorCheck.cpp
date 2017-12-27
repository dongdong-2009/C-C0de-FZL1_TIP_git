/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/coloured_controls/ColorCheck.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2016/01/08 19:01:10 $
  * Last modified by:  $Author: hongzhi.zhang $
  *
  */
// ColorCheck.cpp : implementation file
//
// Copyright (c) 2002 Radu Naiu
//
// Distribute and use freely, except:
// a) Don't alter or remove this notice.
// b) Mark the changes you make.
// This file is provided "as is" with no expressed or implied warranty.
// Use at your own risk. 
//
// History:
//
//      v1.0  24.01.2002  Initial release
//		v1.1  28.01.2002  BUG corrected: default checked state afeter initialization handled  

#include "bus/mfc_extensions/src/stdafx.h"
#include "bus/mfc_extensions/src/coloured_controls/ColorCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorCheck
IMPLEMENT_DYNAMIC(CColorCheck, CButton)

CColorCheck::CColorCheck()
{
	checkFlag = FALSE;
	oldState = 0;
	oldAction = 0;
	drawFocus = FALSE;
	newColor = RGB(230,0,0);
	newArrowColor = RGB(0,0,0);
	newTextColor = RGB(0,0,0);
}

CColorCheck::~CColorCheck()
{
}


BEGIN_MESSAGE_MAP(CColorCheck, CButton)
	//{{AFX_MSG_MAP(CColorCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorCheck message handlers

void CColorCheck::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rc;
	CRect rect(13,1,11,11);
	CRect focusRect, btnRect;
	focusRect.CopyRect(&lpDrawItemStruct->rcItem); 
	disabled = RGB(100,100,100);         // dark gray disabled text
	CSize captionSize;

 
	// Retrieve the button's caption
	//
    const int bufSize = 512;
    TCHAR buffer[bufSize];
    GetWindowText(buffer, bufSize);

	//
	// Set the Text Position more to the left
	//
	captionSize = pDC->GetTextExtent(buffer, strlen(buffer));
	btnRect.SetRect(16,1,16 + captionSize.cx + 4,15);

	//
	// Set the focus rectangle to just past the border decoration
	//
	focusRect.SetRect(18,1,16 + captionSize.cx +8, 15);

	//Get control's ID
	int id = GetDlgCtrlID();

	UINT state = lpDrawItemStruct->itemState;
	UINT action = lpDrawItemStruct->itemAction;

	// Don't redraw whole control if only focus has changed  
	if (action == ODA_FOCUS)
		drawFocus = TRUE;
	else
		drawFocus = FALSE;

	if (!drawFocus)
	{
		//Start drawing the check box
		CPen normRect(PS_SOLID, 1,NULL_PEN);
		CPen* m_normRect = pDC->SelectObject(&normRect);

		pDC->SelectStockObject(NULL_BRUSH);

		//draw the caption
		pDC->DrawText(buffer, strlen(buffer), btnRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);	
		DrawCheckCaption(pDC, btnRect, buffer, newTextColor);
		
		//draw the check rectangle
		pDC->FillSolidRect(2,2,11,11,newColor);
		normRect.DeleteObject();

		//draw the borders
		CPen normRect1(PS_SOLID, 1,RGB(80,80,80));
		CPen* m_normRect1 = pDC->SelectObject(&normRect1);

		CPoint startPt = pDC->MoveTo(1,13);

		pDC->LineTo(1,1);
		pDC->LineTo(13,1);
		normRect1.DeleteObject();							//release

		CPen normRect2(PS_SOLID, 1,RGB(100,100,100));
		CPen* m_normRect2 = pDC->SelectObject(&normRect2);

		CPoint startPt1 = pDC->MoveTo(0,14);

		pDC->LineTo(0,0);
		pDC->LineTo(14,0);
		normRect2.DeleteObject();							//release

		CPen normRect3(PS_SOLID, 1,RGB(255,255,255));
		CPen* m_normRect3 = pDC->SelectObject(&normRect3);

		CPoint startPt2 = pDC->MoveTo(14,0);

		pDC->LineTo(14,14);
		pDC->LineTo(0,14);
		normRect3.DeleteObject();							//release

		CPen normRect4(PS_SOLID, 1,RGB(180,180,180));
		CPen* m_normRect4 = pDC->SelectObject(&normRect4);

		CPoint startPt3 = pDC->MoveTo(13,1);

		pDC->LineTo(13,13);
		pDC->LineTo(13,1);
		normRect4.DeleteObject();							//release

		if (checkFlag == 1)
		{
			pDC->FillSolidRect(2,2,11,11,newColor);

			// draw the arrow
			CPen normRect5(PS_SOLID, 1, newArrowColor);//BLACK_PEN);
			CPen* m_normRect5 = pDC->SelectObject(&normRect5);

			CPoint startPt2 = pDC->MoveTo(4,7);

			pDC->LineTo(7,11);
			pDC->LineTo(10,3);

			CPoint startPt3 = pDC->MoveTo(4,8);

			pDC->LineTo(7,9);
			pDC->LineTo(9,4);
			normRect5.DeleteObject();	//release
		}

	}

	//draw "checked" - arrow depending on state

	if (action & ODA_SELECT && checkFlag == 1)

		pDC->FillSolidRect(2,2,11,11,newColor);

	else if (action & ODA_SELECT && checkFlag == 0)
	{

		pDC->FillSolidRect(2,2,11,11,newColor);

		// draw the arrow
		CPen normRect5(PS_SOLID, 1, newArrowColor);//BLACK_PEN);
		CPen* m_normRect5 = pDC->SelectObject(&normRect5);

		CPoint startPt2 = pDC->MoveTo(4,7);

		pDC->LineTo(7,11);
		pDC->LineTo(10,3);

		CPoint startPt3 = pDC->MoveTo(4,8);

		pDC->LineTo(7,9);
		pDC->LineTo(9,4);
		normRect5.DeleteObject();	//release
	}

	if( ( state & ODS_FOCUS ) ^ ( oldState & ODS_FOCUS ) ||( !drawFocus && ( state & ODS_FOCUS ) ) )
			
		DrawFocusRect(lpDrawItemStruct->hDC, (LPRECT)&focusRect);

	else if (state & ODS_DISABLED)
	{
		pDC->FillSolidRect(2,2,11,12,RGB(192,192,192));  //grayed
		DrawCheckCaption(pDC, btnRect, buffer, GetDisabledColor());
	}

	oldState = state;
	oldAction = action;
}

void CColorCheck::DrawCheckCaption(CDC *pDC, CRect R, const char *Buf, COLORREF TextColor)
{
    COLORREF prevColor = pDC->SetTextColor(TextColor);
    pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(Buf, strlen(Buf), R, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	pDC->SetTextColor(prevColor);
}


void CColorCheck::SetCheck(int nCheck)
{
//	nCheck = GetCheck();
	if (nCheck == 1)
		checkFlag = 1;
	else
		checkFlag = 0;
}

BOOL CColorCheck::GetCheck()
{	//hongzhi, fix bug of CheckColor Control
	if(CButton::GetCheck()==BST_CHECKED)
		return 1;
	else
		return 0;
	/*
	if (checkFlag == 1)
		return 1;	
	else
		return 0;*/
}

COLORREF CColorCheck::SetBkColor(COLORREF boxColor)
{
	newColor = boxColor;
	return newColor;
}

COLORREF CColorCheck::SetArrowColor(COLORREF arrowColor)
{
	newArrowColor = arrowColor;
	return newArrowColor;
}

COLORREF CColorCheck::SetMyTextColor(COLORREF textColor)
{
	newTextColor = textColor;
	return newTextColor;
}