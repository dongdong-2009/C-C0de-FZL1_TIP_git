/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/MainFrm.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__24FA27D8_B3B3_11D5_B644_0050BAB1D92B__INCLUDED_)
#define AFX_MAINFRM_H__24FA27D8_B3B3_11D5_B644_0050BAB1D92B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "PlansTreeView.h"

class CPlanViewerDlg;

class CMainFrame : public CFrameWnd
{

protected: // create from serialization only
    CMainFrame();
    DECLARE_DYNCREATE(CMainFrame)

// Attributes
protected:
    CSplitterWnd m_wndSplitter;
public:
    CPlanViewerDlg*    m_pSplashScreen;


// Operations
public:
    CPlansTreeView* GetTree();


// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CMainFrame)
    public:
    virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    // }}AFX_VIRTUAL

// Implementation
public:
    virtual ~CMainFrame();
    CWnd* GetRightPane();

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

  // control bar embedded members
public:
    CToolBar    m_wndToolBar;
protected:
    CStatusBar  m_wndStatusBar;
    CImageList    m_toolBarImageListHot;

// Generated message map functions
protected:
    // {{AFX_MSG(CMainFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    // }}AFX_MSG
    // afx_msg LRESULT OnStartBusiness(WPARAM, LPARAM);
    DECLARE_MESSAGE_MAP()

    bool CreateToolBars();
    bool PopulateToolBars();
    friend class CPlansTreeView;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__24FA27D8_B3B3_11D5_B644_0050BAB1D92B__INCLUDED_)
