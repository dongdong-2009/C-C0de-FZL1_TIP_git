//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditParameterPage.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANSTEPEDITPARAMETERPAGE_H__45479BA5_8ED8_4A8F_965E_250A6A0FAEAB__INCLUDED_)
#define AFX_PLANSTEPEDITPARAMETERPAGE_H__45479BA5_8ED8_4A8F_965E_250A6A0FAEAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_App
{
    class PlanStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditParameterPage dialog

    class CPlanStepEditParameterPage : public CPropertyPage
    {
        DECLARE_DYNAMIC(CPlanStepEditParameterPage)

    // Construction
    public:
        CPlanStepEditParameterPage(UINT dialogIDD, PlanStep* step); // For derived dialogs
        CPlanStepEditParameterPage(PlanStep* step);
        ~CPlanStepEditParameterPage();


    // Operations
    public:
        PlanStep* GetPlanStep()            { return m_step; }
        void EnableEditing(bool canEdit)   { m_canEdit = canEdit; }
        bool IsEditingEnabled()            { return m_canEdit; }
        void SaveParameters();

    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();


    // Attributes
    private:
        PlanStep* m_step;
        bool m_canEdit;


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditParameterPage)
        enum { IDD = IDD_PLAN_STEP_EDIT_PARAMETERS_DLG };
            // NOTE - ClassWizard will add data members here.
            // DO NOT EDIT what you see in these blocks of generated code !
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditParameterPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditParameterPage)
        virtual BOOL OnInitDialog();
        afx_msg void OnDisplayParameters();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

}  // end namespace

#endif // !defined(AFX_PLANSTEPEDITPARAMETERPAGE_H__45479BA5_8ED8_4A8F_965E_250A6A0FAEAB__INCLUDED_)
