//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterNumericEditDlg.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANSTEPPARAMETERNUMERICEDITDLG_H__9C869E79_120E_4BB9_9BC1_CF8AA37E10E6__INCLUDED_)
#define AFX_PLANSTEPPARAMETERNUMERICEDITDLG_H__9C869E79_120E_4BB9_9BC1_CF8AA37E10E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "PlanStepParameterEditDlg.h"


namespace TA_Base_App
{
    class NumericStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepParameterNumericEditDlg dialog

    class CPlanStepParameterNumericEditDlg : public CPlanStepParameterEditDlg
    {
    // Construction
    public:
        CPlanStepParameterNumericEditDlg(NumericStepParameter& parameter,bool canEdit);   // standard constructor

    // Dialog Data
        // {{AFX_DATA(CPlanStepParameterNumericEditDlg)
    enum { IDD = IDD_PLAN_STEP_NUMERIC_PARAMETER_EDIT_DLG };
        CString    m_units;
    CString    m_value;
    // }}AFX_DATA


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepParameterNumericEditDlg)
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        // {{AFX_MSG(CPlanStepParameterNumericEditDlg)
    afx_msg void onOK();
    virtual BOOL OnInitDialog();
    // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // End namespace

#endif // !defined(AFX_PLANSTEPPARAMETERNUMERICEDITDLG_H__9C869E79_120E_4BB9_9BC1_CF8AA37E10E6__INCLUDED_)
