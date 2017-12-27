//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/PromptDlg.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2015/01/20 12:26:46 $
// Last modified by:  $Author: CM $
//
// <description>

#pragma once

#include "app/response_plans/plan_mft_processor/src/resource.h"
#include "app/response_plans/plan_mft_processor/src/InteractiveStepDlg.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CPromptDlg dialog

    class CPromptDlg : public CInteractiveStepDlg
    {
        DECLARE_DYNAMIC(CPromptDlg)
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
        enum { IDD = IDD_PROMPT_DIALOG };
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
