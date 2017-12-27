//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditBackgroundPromptPage.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2015/01/20 12:26:46 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(_PLANSTEPEDIT_BACKGROUND_PROMPTPAGE_H__)
#define _PLANSTEPEDIT_BACKGROUND_PROMPTPAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_BACKGROUND_PROMPT)

#include "PlanStepEditParameterPage.h"
#include "NumericStepParameter.h"

namespace TA_Base_App
{
    class BackgroundPromptStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditLogPage dialog

    class CPlanStepEditBackgroundPromptPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditBackgroundPromptPage)

        // Construction
    public:
        CPlanStepEditBackgroundPromptPage(BackgroundPromptStep* step);
        ~CPlanStepEditBackgroundPromptPage();


        // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();


        // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditBackgroundPromptPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

        // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditBackgroundPromptPage)
        virtual BOOL OnInitDialog();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        NumericStepParameter m_operatorProfileParameter;

        // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditBackgroundPromptPage)
        enum { IDD = IDD_PLAN_STEP_EDIT_BACKGROUND_PROMPT_DLG };
        CString    m_promptMessage;
        CEdit m_promptMsg;
        CSpinButtonCtrl    m_responseTimeoutSpin;
        UINT    m_responseTimeout;
        CComboBox    m_operatorProfileCombo;
        // }}AFX_DATA
    };

} // end namespace

#endif // defined (STEPTYPE_BACKGROUND_PROMPT)

#endif // !defined(_PLANSTEPEDIT_BACKGROUND_PROMPTPAGE_H__)
