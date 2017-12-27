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

#if !defined(AFX_RUNPLANDLG_H__4FA01EE9_9C54_4E61_BF7B_877E4007909E__INCLUDED_)
#define AFX_RUNPLANDLG_H__4FA01EE9_9C54_4E61_BF7B_877E4007909E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InteractiveStepDlg.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CRunPlanDlg dialog

    class CRunPlanDlg : public CInteractiveStepDlg
    {
    // Construction
    public:
        CRunPlanDlg(const TA_Base_Core::CurrentPlanStepDetail& stepDetail);


    // Operations
    protected:
        virtual CString getTitlePrefix() const;
        virtual void reportCompletionStatus() const;


    // Dialog Data
    private:
        // {{AFX_DATA(CRunPlanDlg)
        enum { IDD = IDD_RUN_PLAN_DLG };
        CString    m_message;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CRunPlanDlg)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual BOOL OnInitDialog();
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CRunPlanDlg)
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

} // namespace TA_App

#endif // !defined(AFX_RUNPLANDLG_H__4FA01EE9_9C54_4E61_BF7B_877E4007909E__INCLUDED_)
