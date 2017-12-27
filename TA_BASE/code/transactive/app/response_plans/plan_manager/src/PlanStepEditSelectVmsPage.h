//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditSelectVmsPage.h $
// @author:  Katherine Thomson
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// This class handles the dialog interaction for selecting a sign (VMS) for
// both the VmsBlankout and VmsRestoreSchedule step types.

#if !defined(AFX_PLANSTEPEDITSELECTVMSDLG_H__64CF1D42_CED3_4D3E_92A5_FD2AB5FC6E0D__INCLUDED_)
#define AFX_PLANSTEPEDITSELECTVMSDLG_H__64CF1D42_CED3_4D3E_92A5_FD2AB5FC6E0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_VMS_BLANKOUT) || defined (STEPTYPE_VMS_RESTORE_SCHEDULE)

#include "PlanStepEditParameterPage.h"
#include "VmsStepParameter.h"
#include "core/data_access_interface/entity_access/src/PlanAgentDataCorbaDef.h"


namespace TA_Base_App
{
    // Both these steps are handled through this class.

    class VmsBlankoutStep;
    class VmsRestoreScheduleStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditSelectVmsPage dialog

    class CPlanStepEditSelectVmsPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditSelectVmsPage)

    // Construction
    public:
        CPlanStepEditSelectVmsPage(VmsBlankoutStep* step);
        CPlanStepEditSelectVmsPage(VmsRestoreScheduleStep* step);
        ~CPlanStepEditSelectVmsPage() {};


    // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();

    private:
        /**
          * PopulateVmsCombo
          *
          * Populates the combo box with all the signs.
          */
        void PopulateVmsCombo();


    // Attributes
    private:
        VmsStepParameter m_vmsParameter;
        TA_Base_Core::EStep m_stepType;


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditSelectVmsPage)
        enum { IDD = IDD_PLAN_STEP_EDIT_SELECT_VMS_DLG };
        CComboBox    m_signs;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditSelectVmsPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditSelectVmsPage)
        virtual BOOL OnInitDialog();
        afx_msg void OnSelectionChangeSignList();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // defined (STEPTYPE_VMS_BLANKOUT) || defined (STEPTYPE_VMS_RESTORE_SCHEDULE)

#endif // !defined(AFX_PLANSTEPEDITSELECTVMSDLG_H__64CF1D42_CED3_4D3E_92A5_FD2AB5FC6E0D__INCLUDED_)
