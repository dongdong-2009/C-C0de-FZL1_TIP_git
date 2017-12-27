/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/CameraStepParameter.h $
  * @author:  Bart Golab/Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  **/

#if !defined(AFX_CAMERASTEPPARAMETER_H__D7BD9004_A234_4B1F_BDE8_B44B8CEF113C__INCLUDED_)
#define AFX_CAMERASTEPPARAMETER_H__D7BD9004_A234_4B1F_BDE8_B44B8CEF113C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR) || defined (STEPTYPE_ACTIVATE_CAMERA_PRESET)

#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStepParameter.h"
#include "PlanStepParameterListCtrl.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CameraStepParameter

    class CameraStepParameter : public PlanStepParameter
    {
    // Operations
    public:
        CameraStepParameter(PlanStep& parent, std::string name, unsigned long cameraKey);
        CameraStepParameter(PlanStep& parent, const CameraStepParameter& parameter);
        CameraStepParameter(const CameraStepParameter& parameter);
        virtual ~CameraStepParameter();

        virtual void addToParameterList(CPlanStepParameterListCtrl& listCtrl);

        void setCameraKey(unsigned long cameraKey);
        void setCameraName(const std::string& cameraName);
        unsigned long getCameraKey() const { return m_cameraKey; }
        std::string getCameraName() const { return m_cameraName; }

          // this method relies on the exceptions thrown
          // by VideoSwitchAgentFactory to determine availability of
          // any cameras in the system
          bool isAnyCameraAvailable ();

    protected:
        virtual TA_Base_App::CPlanStepParameterEditDlg* getEditDialog(bool canEdit);

    private:
        CameraStepParameter& operator=(const CameraStepParameter& parameter);


    // Attributes
    private:
        unsigned long m_cameraKey;
        std::string m_cameraName;
    };
}

#endif // defined(STEPTYPE_ASSIGN_CAMERA_TO_MONITOR) || defined(STEPTYPE_ACTIVATE_CAMERA_PRESET)

#endif // !defined(AFX_CAMERASTEPPARAMETER_H__D7BD9004_A234_4B1F_BDE8_B44B8CEF113C__INCLUDED_)
