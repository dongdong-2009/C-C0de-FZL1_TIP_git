//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditBackgroundLogPage.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2015/01/20 12:26:46 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(_PLANSTEPEDIT_BACKGROUND_LOGPAGE_H_)
#define _PLANSTEPEDIT_BACKGROUND_LOGPAGE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_BACKGROUND_LOG)

#include "PlanStepEditParameterPage.h"
#include "NumericStepParameter.h"

namespace TA_Base_App
{
    class BackgroundLogStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditBackgroundLogPage dialog

    class CPlanStepEditBackgroundLogPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditBackgroundLogPage)

        // Construction
    public:
        CPlanStepEditBackgroundLogPage(BackgroundLogStep* step);
        ~CPlanStepEditBackgroundLogPage();


        // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();


        // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditBackgroundLogPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

        // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditBackgroundLogPage)
        virtual BOOL OnInitDialog();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    
    private:
        NumericStepParameter m_operatorProfileParameter;

        // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditBackgroundLogPage)
        enum { IDD = IDD_PLAN_STEP_EDIT_BACKGROUND_LOG_DLG };
        CString    m_promptMessage;
        CEdit m_promptMsgTxt;
        CSpinButtonCtrl    m_responseTimeoutSpin;
        UINT    m_responseTimeout;
        BOOL    m_mandatoryLogEntry;
        CComboBox    m_operatorProfileCombo;
        // }}AFX_DATA
    };

} // end namespace

#endif // defined (STEPTYPE_BACKGROUND_LOG)

#endif // !defined(_PLANSTEPEDIT_BACKGROUND_LOGPAGE_H_)
