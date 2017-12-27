//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditActivePlanCheckPage.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANSTEPEDITACTIVEPLANCHECKPAGE_H__7EE45DFB_6A62_4646_B92B_681BC7AC402C__INCLUDED_)
#define AFX_PLANSTEPEDITACTIVEPLANCHECKPAGE_H__7EE45DFB_6A62_4646_B92B_681BC7AC402C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_ACTIVE_PLAN_CHECK)

#include "PlanStepEditParameterPage.h"
#include "StepNumberStepParameter.h"
#include "PlanPathStepParameter.h"

namespace TA_Base_App
{
    class ActivePlanCheckStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditActivePlanCheckPage dialog

    class CPlanStepEditActivePlanCheckPage : public CPlanStepEditParameterPage
    {
    // Construction
    public:
        CPlanStepEditActivePlanCheckPage(ActivePlanCheckStep* parameter);   // standard constructor


    // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();


    // Attributes
    private:
        StepNumberStepParameter  m_noStepParameter;
        StepNumberStepParameter  m_yesStepParameter;
        PlanPathStepParameter    m_planPathStepParameter;


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditActivePlanCheckPage)
        enum { IDD = IDD_PLAN_STEP_EDIT_ACTIVE_PLAN_CHECK_DLG };
        CString    m_noStepLabel;
        CString    m_yesStepLabel;
        CString    m_planPathLabel;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditActivePlanCheckPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditActivePlanCheckPage)
        afx_msg void OnPickNoStepButton();
        afx_msg void OnPickYesStepButton();
        afx_msg void OnPickPlanPathButton();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };
}

#endif // defined (STEPTYPE_ACTIVE_PLAN_CHECK)

#endif // !defined(AFX_PLANSTEPEDITACTIVEPLANCHECKPAGE_H__7EE45DFB_6A62_4646_B92B_681BC7AC402C__INCLUDED_)
