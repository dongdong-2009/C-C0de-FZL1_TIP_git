//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File$
// @author:  Andy Parker
// @version: $Revision$
//
// Last modification: $DateTime$
// Last modified by:  $Author$
//
// <description>

#if !defined(AFX_PLANSTEPPARAMETEREDITDLG_H__29F92798_A7CD_4156_A709_06CC773CD9D3__INCLUDED_)
#define AFX_PLANSTEPPARAMETEREDITDLG_H__29F92798_A7CD_4156_A709_06CC773CD9D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"


namespace TA_Base_App
{
    class PlanStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepParameterEditDlg dialog

    class CPlanStepParameterEditDlg : public CDialog
    {
    // Construction
    public:
        CPlanStepParameterEditDlg(PlanStepParameter& parameter,bool canEdit);   // standard constructor
        CPlanStepParameterEditDlg(PlanStepParameter& parameter,UINT templateID,bool canEdit);   // called by derived classes

        PlanStepParameter* GetStepParameter() const { return &m_parameter;};

    // Dialog Data
        // {{AFX_DATA(CPlanStepParameterEditDlg)
    enum { IDD = IDD_PLAN_STEP_PARAMETER_EDIT_DLG };
    CString    m_parameterName;
    // }}AFX_DATA


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepParameterEditDlg)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual BOOL OnInitDialog();
        virtual afx_msg void OnAdjustFont();
        // }}AFX_VIRTUAL

    // Implementation
    protected:

        bool                 m_canEdit;
        PlanStepParameter&   m_parameter;

        // Generated message map functions
        // {{AFX_MSG(CPlanStepParameterEditDlg)
    // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}  // End namespace

#endif // !defined(AFX_PLANSTEPPARAMETEREDITDLG_H__29F92798_A7CD_4156_A709_06CC773CD9D3__INCLUDED_)
