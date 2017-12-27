//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditorDetailsPage.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANSTEPEDITORDETAILSPAGE_H__E7E8CF69_B2EA_46F6_8211_1CB9F62EDF00__INCLUDED_)
#define AFX_PLANSTEPEDITORDETAILSPAGE_H__E7E8CF69_B2EA_46F6_8211_1CB9F62EDF00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/mfc_extensions/src/numeric_edit_control/numericedit.h"


namespace TA_Base_App
{
    class PlanStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditorDetailsPage dialog

    class CPlanStepEditorDetailsPage : public CPropertyPage
    {
        DECLARE_DYNAMIC(CPlanStepEditorDetailsPage)

    // Construction
    public:
        CPlanStepEditorDetailsPage(PlanStep* step, bool canEdit);
        CPlanStepEditorDetailsPage(PlanStep* step, TA_Base_Core::StepCustomisableDetail* customStepDetail);
        ~CPlanStepEditorDetailsPage();


    // Operations
    public:
        void UpdateStepDetails();
        void UpdateCustomDetails();

    protected:
        void EnableControls();


    // Attributes
    private:
        PlanStep* m_planStep;
        TA_Base_Core::StepCustomisableDetail* m_customStepDetail;
        bool m_canEdit;
        bool m_canCustomise;


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditorDetailsPage)
        enum { IDD = IDD_PLAN_STEP_EDIT_DETAILS_DLG };
		CButton	m_nowaitCheck;
        CDateTimeCtrl    m_delayPicker;
        CButton    m_ignoreFailureCheck;
        CButton    m_skippableCheck;
        CButton    m_skipCheck;
        CEdit    m_descriptionEdit;
        CEdit    m_nameEdit;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditorDetailsPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditorDetailsPage)
        virtual BOOL OnInitDialog();
        // virtual void OnOK();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // end namespace

#endif // !defined(AFX_PLANSTEPEDITORDETAILSPAGE_H__E7E8CF69_B2EA_46F6_8211_1CB9F62EDF00__INCLUDED_)
