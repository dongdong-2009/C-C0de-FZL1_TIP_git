//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditAssignCameraToMonitorPage.h $
// @author:  Katherine Thomson
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
//

#if !defined(AFX_PLANSTEPEDITASSIGNCAMERATOMONITORPAGE_H__F0D8AAFE_F0D2_4526_8B1C_6A950A5ECC7E__INCLUDED_)
#define AFX_PLANSTEPEDITASSIGNCAMERATOMONITORPAGE_H__F0D8AAFE_F0D2_4526_8B1C_6A950A5ECC7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)

#include "PlanStepEditParameterPage.h"
#include "CameraStepParameter.h"
#include "MonitorStepParameter.h"

namespace TA_Base_App
{
    class AssignCameraToMonitorStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditAssignCameraToMonitorPage dialog

    class CPlanStepEditAssignCameraToMonitorPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditAssignCameraToMonitorPage)

    // Construction
    public:
        CPlanStepEditAssignCameraToMonitorPage(AssignCameraToMonitorStep* step);
        ~CPlanStepEditAssignCameraToMonitorPage();


    // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();


    // Attributes
    private:
        CameraStepParameter m_cameraParameter;
        MonitorStepParameter m_monitorParameter;


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditAssignCameraToMonitorPage)
        enum { IDD = IDD_PLAN_STEP_EDIT_ASSIGN_CAMERA_TO_MONITOR_DLG };
        CString    m_camera;
        CString    m_monitor;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditAssignCameraToMonitorPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditAssignCameraToMonitorPage)
        afx_msg void OnPickCameraButton();
        afx_msg void OnPickMonitorButton();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)

#endif // !defined(AFX_PLANSTEPEDITASSIGNCAMERATOMONITORPAGE_H__F0D8AAFE_F0D2_4526_8B1C_6A950A5ECC7E__INCLUDED_)
