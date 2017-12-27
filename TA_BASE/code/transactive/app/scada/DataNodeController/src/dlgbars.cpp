/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/DataNodeController/src/dlgbars.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// DLGBARS.cpp : Defines the class behaviors for the dialog tool bar
//               and status bar.
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include <afxpriv.h>

#include "dlgbars.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgToolBar

BEGIN_MESSAGE_MAP(CDlgToolBar, CToolBar)
	//{{AFX_MSG_MAP(CDlgToolBar)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
//	ON_CBN_SELCHANGE(ID_DATA_TYPE_HOLDER, EnableApply)
//	ON_NOTIFY(DTN_DATETIMECHANGE, ID_START_PLACEHOLDER, OnStartDateTimeChange)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDlgToolBar, CToolBar)
    //{{AFX_EVENTSINK_MAP(CDlgToolBar)
//	ON_EVENT(CDlgToolBar, ID_DURATION_PLACEHOLDER, 1 /* OnDurationChanged */, OnRangeChanged, VTS_I4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgToolBar Construction/Destruction

CDlgToolBar::CDlgToolBar()
{
//	m_LedOn  = -1;
//	m_LedOff = -1;
}

CDlgToolBar::~CDlgToolBar()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDlgToolBar::OnIdleUpdateCmdUI
//      OnIdleUpdateCmdUI handles the WM_IDLEUPDATECMDUI message, which is
//      used to update the status of user-interface elements within the MFC
//      framework.
//
//      We have to get a little tricky here: CToolBar::OnUpdateCmdUI
//      expects a CFrameWnd pointer as its first parameter.  However, it
//      doesn't do anything but pass the parameter on to another function
//      which only requires a CCmdTarget pointer.  We can get a CWnd pointer
//      to the parent window, which is a CCmdTarget, but may not be a
//      CFrameWnd.  So, to make CToolBar::OnUpdateCmdUI happy, we will call
//      our CWnd pointer a CFrameWnd pointer temporarily.

LRESULT CDlgToolBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM)
{
	if (IsWindowVisible())
	{
		CFrameWnd *pParent = (CFrameWnd *)GetParent();
		if (pParent)
			OnUpdateCmdUI(pParent, (BOOL)wParam);
	}
	return 0L;
}
/*
// Enable the apply button
void CDlgToolBar::EnableApply() 
{
//	GetToolBarCtrl().EnableButton( ID_APPLY_NOW, TRUE );
}

// Disable the apply button
void CDlgToolBar::DisableApply() 
{
//	GetToolBarCtrl().EnableButton( ID_APPLY_NOW, FALSE );
}


void CDlgToolBar::DisableAll() 
{
//    GetToolBarCtrl().EnableButton( ID_DATAPOINT_ADD, FALSE );
//	GetToolBarCtrl().EnableButton( ID_DATAPOINT_DELETE, FALSE );
//	GetToolBarCtrl().EnableButton( ID_EXPORT, FALSE );
//	GetToolBarCtrl().EnableButton( ID_PRINT_GRAPH, FALSE );
//	GetToolBarCtrl().EnableButton( ID_APPLY_NOW, FALSE );
//	GetToolBarCtrl().EnableButton( ID_NOW, FALSE );
//	GetToolBarCtrl().EnableButton( ID_HELP, FALSE );

  //  m_HistoryTypeCtrl.EnableWindow( FALSE );
    //m_RangeCtrl.EnableWindow( FALSE );
    //m_StartDateTimeCtrl.EnableWindow( FALSE );
}


void CDlgToolBar::EnableAll() 
{
//    GetToolBarCtrl().EnableButton( ID_DATAPOINT_ADD, TRUE );
//	GetToolBarCtrl().EnableButton( ID_DATAPOINT_DELETE, TRUE );
//	GetToolBarCtrl().EnableButton( ID_EXPORT, TRUE );
//	GetToolBarCtrl().EnableButton( ID_PRINT_GRAPH, TRUE );
//	GetToolBarCtrl().EnableButton( ID_APPLY_NOW, TRUE );
//	GetToolBarCtrl().EnableButton( ID_NOW, TRUE );
//	GetToolBarCtrl().EnableButton( ID_HELP, TRUE );

 //   m_HistoryTypeCtrl.EnableWindow( TRUE );
  //  m_RangeCtrl.EnableWindow( TRUE );
    //m_StartDateTimeCtrl.EnableWindow( TRUE );
}


//void CDlgToolBar::OnStartDateTimeChange(NMHDR* pNMHDR, LRESULT* pResult) 
//{
	// Set apply icon to show that apply is required
//	EnableApply();
	
	*pResult = 0;
//}

//void CDlgToolBar::OnRangeChanged(long lTotalSeconds) 
//{
	// Set apply icon to show that apply is required
//	EnableApply();
//}

// Setup the bitmaps for the now button
//void CDlgToolBar::AddNowBitmaps() 
//{
//	m_LedOn = GetToolBarCtrl().AddBitmap( 1, IDB_LEDON );
//	m_LedOff = GetToolBarCtrl().AddBitmap( 1, IDB_LEDOFF );
//}

// Set the bitmap for the now button
void CDlgToolBar::SetNowBitmap()
{
	// Check that the bitmaps have been loaded
	if( m_LedOn <= 0 )
	{
		return;
	}

	TBBUTTONINFO ptbbi;
	ptbbi.cbSize = sizeof( ptbbi );
	ptbbi.dwMask = TBIF_IMAGE;
	BOOL result;
	if( GetToolBarCtrl().GetButtonInfo( ID_NOW, &ptbbi ) )
	{
		ptbbi.dwMask = TBIF_IMAGE;
		if( GetToolBarCtrl().IsButtonChecked( ID_NOW ) )
		{
			ptbbi.iImage = m_LedOn;
		}
		else
		{
			ptbbi.iImage = m_LedOff;
		}
		result = GetToolBarCtrl().SetButtonInfo( ID_NOW, &ptbbi );
	}
}
*/
/////////////////////////////////////////////////////////////////////////////
// CDlgStatusBar

BEGIN_MESSAGE_MAP(CDlgStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(CDlgStatusBar)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgStatusBar Construction/Destruction

CDlgStatusBar::CDlgStatusBar()
: m_lastMsgDisplayed("")
{
}


CDlgStatusBar::~CDlgStatusBar()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDlgStatusBar::OnIdleUpdateCmdUI
//      OnIdleUpdateCmdUI handles the WM_IDLEUPDATECMDUI message, which is
//      used to update the status of user-interface elements within the MFC
//      framework.
//
//      We have to get a little tricky here: CStatusBar::OnUpdateCmdUI
//      expects a CFrameWnd pointer as its first parameter.  However, it
//      doesn't do anything but pass the parameter on to another function
//      which only requires a CCmdTarget pointer.  We can get a CWnd pointer
//      to the parent window, which is a CCmdTarget, but may not be a
//      CFrameWnd.  So, to make CStatusBar::OnUpdateCmdUI happy, we will call
//      our CWnd pointer a CFrameWnd pointer temporarily.

LRESULT CDlgStatusBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM)
{
	if (IsWindowVisible())
	{
		CFrameWnd *pParent = (CFrameWnd *)GetParent();
		if (pParent)
			OnUpdateCmdUI(pParent, (BOOL)wParam);
	}
	return 0L;
}

void CDlgStatusBar::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{	
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	dc.SetBkMode(TRANSPARENT);
	
	CRect rect(&lpDrawItemStruct->rcItem);

	switch(lpDrawItemStruct->itemID) {

		case 0:
			dc.SetTextColor(RGB(0, 0, 255));
			dc.TextOut(rect.left+2, rect.top, "Blue");
			break;

		case 1:
			dc.SetTextColor(RGB(255, 0, 0));
			dc.TextOut(rect.left+2, rect.top, "Red");
			break;
	}

	// Detach from the CDC object, otherwise the hDC will be
	// destroyed when the CDC object goes out of scope
	dc.Detach();
}

