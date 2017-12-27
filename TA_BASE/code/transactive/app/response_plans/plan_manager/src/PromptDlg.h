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

#if !defined(AFX_PROMPTDLG_H__C36A3A03_FE79_4CCF_8AD8_A40239D5EB24__INCLUDED_)
#define AFX_PROMPTDLG_H__C36A3A03_FE79_4CCF_8AD8_A40239D5EB24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InteractiveStepDlg.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CPromptDlg dialog

    class CPromptDlg : public CInteractiveStepDlg
    {
    // Construction/destruction
    public:
        CPromptDlg(const TA_Base_Core::CurrentPlanStepDetail& stepDetail);
        virtual ~CPromptDlg();

    // Operations
    protected:
        virtual CString getTitlePrefix() const;
        virtual void reportCompletionStatus() const;


    // Dialog Data
    private:
        // {{AFX_DATA(CPromptDlg)
        enum { IDD = IDD_PROMPT_DLG };
        CString    m_prompt;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPromptDlg)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

        // Generated message map functions
        // {{AFX_MSG(CPromptDlg)
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_PROMPTDLG_H__C36A3A03_FE79_4CCF_8AD8_A40239D5EB24__INCLUDED_)
