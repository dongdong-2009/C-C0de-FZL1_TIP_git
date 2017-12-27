/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanViewerDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanViewerDlg.h : header file
//

#if !defined(AFX_PLANVIEWERDLG_H__C75EC6C6_B3BD_11D5_B644_0050BAB1D92B__INCLUDED_)
#define AFX_PLANVIEWERDLG_H__C75EC6C6_B3BD_11D5_B644_0050BAB1D92B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPlanViewerDlg dialog

class CPlanViewerDlg : public CDialog
{
// Construction
public:
    int DoModal();
    CPlanViewerDlg(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
    // {{AFX_DATA(CPlanViewerDlg)
    enum { IDD = IDD_PLANVIEWER_DIALOG };
        // NOTE: the ClassWizard will add data members here
    // }}AFX_DATA

    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CPlanViewerDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    // }}AFX_VIRTUAL

// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    // {{AFX_MSG(CPlanViewerDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnDestroy();
    // }}AFX_MSG
    afx_msg LRESULT OnStartBusiness(WPARAM, LPARAM);

    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANVIEWERDLG_H__C75EC6C6_B3BD_11D5_B644_0050BAB1D92B__INCLUDED_)
