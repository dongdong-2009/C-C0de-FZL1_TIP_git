//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterCameraEditDlg.h $
// @author:  Katherine Thomson
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// Dialog for selecting a camera name for the Assign Camera To Monitor and Activate Camera Preset steps.

#if !defined(AFX_PLANSTEPPARAMETERCAMERAEDITDLG_H__77D17040_8037_4BCB_9260_8AFDF8F5FFA2__INCLUDED_)
#define AFX_PLANSTEPPARAMETERCAMERAEDITDLG_H__77D17040_8037_4BCB_9260_8AFDF8F5FFA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR) || defined (STEPTYPE_ACTIVATE_CAMERA_PRESET)

#include "resource.h"
#include "PlanStepParameterEditDlg.h"

namespace TA_Base_App
{
    class CameraStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepParameterCameraEditDlg dialog

    class CPlanStepParameterCameraEditDlg : public CPlanStepParameterEditDlg
    {
    // Construction
    public:
        CPlanStepParameterCameraEditDlg(CameraStepParameter& parameter, bool canEdit);

    // Dialog Data
        // {{AFX_DATA(CPlanStepParameterCameraEditDlg)
        enum { IDD = IDD_PLAN_STEP_PARAMETER_CAMERA_EDIT_DLG };
        CButton    m_okButton;
        CTreeCtrl    m_cameraTree;
        CString    m_cameraName;
        CString    m_parameterName;
        // }}AFX_DATA


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepParameterCameraEditDlg)
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        // {{AFX_MSG(CPlanStepParameterCameraEditDlg)
        afx_msg void onSelectionChangedCameraTree(NMHDR* pNMHDR, LRESULT* pResult);
        virtual BOOL OnInitDialog();
        afx_msg void onOK();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:

        /**
          * loadCameraTree
          *
          * Loads the tree containing the cameras and their groups.  Then
          * selects the camera identified by m_cameraParameter.getCameraKey().
          */

        void loadCameraTree();

        // The parameter that is updated using this dialog.
        // This is stored in the CPlanStepParameterEditDlg class, but we need it
        // as a CameraStepParameter not just a PlanStepParameter.

        CameraStepParameter& m_cameraParameter;

        // Keeps track of the currently selected camera's key.

        unsigned long m_cameraKey;
    };

} // namespace TA_Base_App

#endif // defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR) || defined (STEPTYPE_ACTIVATE_CAMERA_PRESET)

#endif // !defined(AFX_PLANSTEPPARAMETERCAMERAEDITDLG_H__77D17040_8037_4BCB_9260_8AFDF8F5FFA2__INCLUDED_)
