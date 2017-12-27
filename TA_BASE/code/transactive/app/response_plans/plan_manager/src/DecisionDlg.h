//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File$
// @author:  Bart Golab
// @version: $Revision$
//
// Last modification: $DateTime$
// Last modified by:  $Author$
//
// <description>

#if !defined(AFX_DECISIONDLG_H__DC4B862B_1AC1_46D9_9D10_351E483E273B__INCLUDED_)
#define AFX_DECISIONDLG_H__DC4B862B_1AC1_46D9_9D10_351E483E273B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InteractiveStepDlg.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CDecisionDlg dialog

    class CDecisionDlg : public CInteractiveStepDlg
    {
    // Construction/destruction
    public:
        CDecisionDlg(const TA_Base_Core::CurrentPlanStepDetail& stepDetail);
        virtual ~CDecisionDlg();

    // Operations
    protected:
        virtual CString getTitlePrefix() const;
        virtual void reportCompletionStatus() const;


    // Dialog Data
    private:
        // {{AFX_DATA(CDecisionDlg)
        enum { IDD = IDD_DECISION_DLG };
        CString    m_query;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CDecisionDlg)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual BOOL OnInitDialog(); //DP-changes
        // }}AFX_VIRTUAL

        // Generated message map functions
        // {{AFX_MSG(CDecisionDlg)
        afx_msg void OnYes();
        afx_msg void OnNo();
        afx_msg void OnDestroy();
        afx_msg void OnPause(); //DP-changes
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_DECISIONDLG_H__DC4B862B_1AC1_46D9_9D10_351E483E273B__INCLUDED_)
