//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditActivateCameraPresetPage.h $
// @author:  Katherine Thomson
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
//

#if !defined(AFX_PLANSTEPEDITACTIVATECAMERAPRESETPAGE_H__B84BF5E3_9F3E_41A3_BF63_5FAF61F92E82__INCLUDED_)
#define AFX_PLANSTEPEDITACTIVATECAMERAPRESETPAGE_H__B84BF5E3_9F3E_41A3_BF63_5FAF61F92E82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_ACTIVATE_CAMERA_PRESET)

#include "PlanStepEditParameterPage.h"
#include "CameraStepParameter.h"
#include "CameraPresetStepParameter.h"

namespace TA_Base_App
{
    class ActivateCameraPresetStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditActivateCameraPresetPage dialog

    class CPlanStepEditActivateCameraPresetPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditActivateCameraPresetPage)

    // Construction
    public:
        CPlanStepEditActivateCameraPresetPage(ActivateCameraPresetStep* step);
        ~CPlanStepEditActivateCameraPresetPage();


    // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();

    private:
        /**
          * PopulatePresetsCombo
          *
          * Populates the preset combo and selects the current preset.
          */
        void PopulatePresetsCombo();


    // Attributes
    private:
        CameraStepParameter m_cameraParameter;
        CameraPresetStepParameter m_presetParameter;


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditActivateCameraPresetPage)
        enum { IDD = IDD_PLAN_STEP_EDIT_ACTIVATE_CAMERA_PRESET_DLG };
        CComboBox    m_presets;
        CString    m_cameraName;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditActivateCameraPresetPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditActivateCameraPresetPage)
        afx_msg void OnPickCameraButton();
        afx_msg void OnSelectionChangePresetCombo();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // defined (STEPTYPE_ACTIVATE_CAMERA_PRESET)

#endif // !defined(AFX_PLANSTEPEDITACTIVATECAMERAPRESETPAGE_H__B84BF5E3_9F3E_41A3_BF63_5FAF61F92E82__INCLUDED_)
