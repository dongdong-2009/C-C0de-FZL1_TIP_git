
//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File$
// @author:  Lizette Lingo
// @version: $Revision$
//
// Last modification: $DateTime$
// Last modified by:  $Author$
//
// <description>


#if !defined(AFX_FAILURESUMMARYDLG_H__B1C6D463_D910_417F_B687_E49413F9F4DB__INCLUDED_)
#define AFX_FAILURESUMMARYDLG_H__B1C6D463_D910_417F_B687_E49413F9F4DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FailureSummaryDlg.h : header file
//
#include "InteractiveStepDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CFailureSummaryDlg dialog
namespace TA_Base_App
{

    class CFailureSummaryDlg : public CInteractiveStepDlg
    {
        // Construction
    public:
        CFailureSummaryDlg(const TA_Base_Core::CurrentPlanStepDetail& stepDetail);
        virtual ~CFailureSummaryDlg();

        // Operations
    protected:
        virtual CString getTitlePrefix() const;
        virtual void reportCompletionStatus() const;

        // Dialog Data
        // {{AFX_DATA(CFailureSummaryDlg)
    enum { IDD = IDD_FAILURE_SUMMARY_DLG };
    CListCtrl    m_failedStepListCtrl;
    // }}AFX_DATA


        // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CFailureSummaryDlg)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

        // Implementation
    protected:

        // Generated message map functions
        // {{AFX_MSG(CFailureSummaryDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FAILURESUMMARYDLG_H__B1C6D463_D910_417F_B687_E49413F9F4DB__INCLUDED_)
