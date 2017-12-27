/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/dlgbars.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// DlgBars.h : header file
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

/*****************************************************************************

  Purpose:
	Interface for CDlgToolBar, a special type of CToolBar which does not
	expect a parent frame window to be available, and CDlgStatusBar, which
	does the same for CStatusBars.  This allows the control bars
	to be used in applications where the main window is a dialog bar.

  Functions:
	CDlgToolBar::CDlgToolBar()          -- constructor
	CDlgToolBar::~CDlgToolBar()         -- destructor
	CDlgToolBar::OnIdleUpdateCmdUI()    -- WM_IDLEUPDATECMDUI handler

	CDlgStatusBar::CDlgStatusBar()      -- constructor
	CDlgStatusBar::~CDlgStatusBar()     -- destructor
	CDlgStatusBar::OnIdleUpdateCmdUI()  -- WM_IDLEUPDATECMDUI handler

  Development Team:
	Mary Kirtland
  Ported to 32-bit by:
	Mike Hedley
  Created by Microsoft Product Support Services, Premier ISV Support
  Copyright (c) 1998 Microsoft Corporation. All rights reserved.
\****************************************************************************/

#ifndef __DLGBARS_H__
#define __DLGBARS_H__

//#include "bcgdurationctrl.h"

class CDlgToolBar : public CToolBar
{
// Construction
public:
	CDlgToolBar();
	//CStatic m_HistoryTypeText;
	//CComboBox m_HistoryTypeCtrl;
	//CStatic m_RangeText;
	//CBCGDurationCtrl m_RangeCtrl;
	//CStatic m_StartText;
	//CDateTimeCtrl m_StartDateTimeCtrl;

// Implementation
public:
	virtual ~CDlgToolBar();
	//void DisableApply();
	// Setup the bitmaps for the now button
	//void AddNowBitmaps();
	// Set the bitmap for the now button
	//void SetNowBitmap();
    
    //void DisableAll();
    //void EnableAll();

protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgToolBar)
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);
	//afx_msg void EnableApply();
	//afx_msg void OnStartDateTimeChange(NMHDR* pNMHDR, LRESULT* pResult);
	//afx_msg void OnRangeChanged(long lTotalSeconds);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//int m_LedOn;
	//int m_LedOff;
};

class CDlgStatusBar : public CStatusBar
{

public:
	CDlgStatusBar();


public:
	virtual ~CDlgStatusBar();	

    void setLastMsg()
    {
        GetWindowText(m_lastMsgDisplayed);
    }

    CString getLastMsg()
    {
        return m_lastMsgDisplayed;
    }

	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
protected:

	//{{AFX_MSG(CDlgStatusBar)
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

    // Last message displayed in status bar before server went down
    CString m_lastMsgDisplayed;
};



#endif //__DLGBARS_H__




