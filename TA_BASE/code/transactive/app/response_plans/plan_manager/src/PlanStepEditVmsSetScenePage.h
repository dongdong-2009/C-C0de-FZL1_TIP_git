//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditVmsSetScenePage.h $
// @author:  Katherine Thomson
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// This class handles the dialog interaction for setting a scene on a VMS.


#if !defined(AFX_PLANSTEPEDITVMSSETSCENEPAGE_H__1F5AED02_B755_42B9_BE63_F95399165498__INCLUDED_)
#define AFX_PLANSTEPEDITVMSSETSCENEPAGE_H__1F5AED02_B755_42B9_BE63_F95399165498__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_VMS_SET_SCENE)

#include "PlanStepEditParameterPage.h"
#include "VmsStepParameter.h"
#include "VmsSceneStepParameter.h"
#include "core/data_access_interface/entity_access/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_App
{
    class VmsSetSceneStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditVmsSetScenePage dialog

    class CPlanStepEditVmsSetScenePage : public CPlanStepEditParameterPage
    {
           DECLARE_DYNAMIC(CPlanStepEditVmsSetScenePage)

    // Construction
    public:
        CPlanStepEditVmsSetScenePage(VmsSetSceneStep* step);
        ~CPlanStepEditVmsSetScenePage() {};


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

        /**
          * PopulateVmsSceneCombo
          *
          * Populates the scene combo box with all the scenes for the given VMS.
          *
          * @param  unsigned long vmsEntityKey
          *         The key of the VMS to load the scenes for.
          */
        void PopulateVmsSceneCombo( unsigned long vmsEntityKey );


    // Attributes
    private:
           VmsStepParameter m_vmsParameter;
        VmsSceneStepParameter m_vmsSceneParameter;


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditVmsSetScenePage)
        enum { IDD = IDD_PLAN_STEP_EDIT_VMS_SET_SCENE_DLG };
        CComboBox    m_signs;
        CComboBox    m_scenes;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditVmsSetScenePage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditVmsSetScenePage)
        virtual BOOL OnInitDialog();
        afx_msg void OnSelectionChangeSignList();
        afx_msg void OnSelectionChangeSceneList();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // defined (STEPTYPE_VMS_SET_SCENE)

#endif // !defined(AFX_PLANSTEPEDITVMSSETSCENEPAGE_H__1F5AED02_B755_42B9_BE63_F95399165498__INCLUDED_)
