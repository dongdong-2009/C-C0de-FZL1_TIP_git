//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterYesNoEditDlg.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANSTEPPARAMETERYESNOEDITDLG_H__2A3064BF_4A94_4469_B133_DCE3A1937466__INCLUDED_)
#define AFX_PLANSTEPPARAMETERYESNOEDITDLG_H__2A3064BF_4A94_4469_B133_DCE3A1937466__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlanStepParameterEditDlg.h"


namespace TA_Base_App
{
    class YesNoStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepParameterYesNoEditDlg dialog

    class CPlanStepParameterYesNoEditDlg : public CPlanStepParameterEditDlg
    {
    // Construction
    public:
        CPlanStepParameterYesNoEditDlg(YesNoStepParameter& parameter,bool canEdit);   // standard constructor

    // Dialog Data
        // {{AFX_DATA(CPlanStepParameterYesNoEditDlg)
        enum { IDD = IDD_PLAN_STEP_PARAMETER_YES_NO_EDIT_DLG };
        CButton    m_valueCheck;
        // }}AFX_DATA


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepParameterYesNoEditDlg)
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        // {{AFX_MSG(CPlanStepParameterYesNoEditDlg)
        afx_msg void onOK();
        virtual BOOL OnInitDialog();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
} // namespace

#endif // !defined(AFX_PLANSTEPPARAMETERYESNOEDITDLG_H__2A3064BF_4A94_4469_B133_DCE3A1937466__INCLUDED_)
