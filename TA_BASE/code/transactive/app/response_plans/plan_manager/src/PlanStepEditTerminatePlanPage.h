//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditTerminatePlanPage.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANSTEPEDITTERMINATEPLANPAGE_H__7EE45DFB_6A62_4646_B92B_681BC7AC402C__INCLUDED_)
#define AFX_PLANSTEPEDITTERMINATEPLANPAGE_H__7EE45DFB_6A62_4646_B92B_681BC7AC402C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_TERMINATE_PLAN)

#include "PlanStepEditParameterPage.h"
#include "PlanPathStepParameter.h"


namespace TA_Base_App
{
    class TerminatePlanStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditTerminatePlanPage dialog

    class CPlanStepEditTerminatePlanPage : public CPlanStepEditParameterPage
    {
    // Construction
    public:
        CPlanStepEditTerminatePlanPage(TerminatePlanStep* parameter);   // standard constructor


    // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();


    // Attributes
    private:
        PlanPathStepParameter    m_planPathStepParameter;


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditTerminatePlanPage)
        enum { IDD = IDD_PLAN_STEP_EDIT_TERMINATE_PLAN_DLG };
        CString    m_planPathLabel;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditTerminatePlanPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditTerminatePlanPage)
        afx_msg void OnPickPlanPathButton();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };
}

#endif // defined (STEPTYPE_TERMINATE_PLAN)

#endif // !defined(AFX_PLANSTEPEDITTERMINATEPLANPAGE_H__7EE45DFB_6A62_4646_B92B_681BC7AC402C__INCLUDED_)
