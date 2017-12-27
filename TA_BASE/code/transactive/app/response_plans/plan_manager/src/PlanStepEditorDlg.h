//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditorDlg.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANSTEPEDITORDLG_H__C08A7DFF_F0FD_4409_BDE5_EB600F0B2A96__INCLUDED_)
#define AFX_PLANSTEPEDITORDLG_H__C08A7DFF_F0FD_4409_BDE5_EB600F0B2A96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "resource.h"
#include "PlanStepParameterListCtrl.h"
#include "PlanStepEditorDetailsPage.h"
#include "PlanStepEditDecisionParameterPage.h"
#include "PlanStepEditorPropertySheet.h"


namespace TA_Base_App
{
    class PlanStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditorDlg dialog

    class CPlanStepEditorDlg : public CDialog
    {
    // Construction/destruction
    public:
        CPlanStepEditorDlg(PlanStep* step, bool canEdit);
        CPlanStepEditorDlg(PlanStep* step, TA_Base_Core::StepCustomisableDetail *customStepDetail);
        virtual ~CPlanStepEditorDlg();


    // Attributes
    private:
        PlanStep*                           m_planStep;
        TA_Base_Core::StepCustomisableDetail*    m_customStepDetail;
        bool                                m_canEdit;
        bool                                m_canCustomise;

        CPlanStepEditorPropertySheet  m_stepPropertySheet;
        CPlanStepEditorDetailsPage    m_stepDetailsPage;
        CPlanStepEditParameterPage*   m_parametersPage;


    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditorDlg)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditorDlg)
        virtual BOOL OnInitDialog();
        afx_msg void OnOK();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditorDlg)
        enum { IDD = IDD_PLAN_STEP_EDIT_DLG };
        // }}AFX_DATA
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_Base_App namespace

#endif // !defined(AFX_PLANSTEPEDITORDLG_H__C08A7DFF_F0FD_4409_BDE5_EB600F0B2A96__INCLUDED_)
