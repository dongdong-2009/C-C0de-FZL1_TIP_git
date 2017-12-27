/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/CameraPresetStepParameter.h $
  * @author:  Bart Golab/Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * The preset information for the Activate Camera Preset step.
  *
  **/

#if !defined(AFX_CAMERAPRESETSTEPPARAMETER_H__A0430EFE_D3A5_43DE_A46B_0E0E8A3C167B__INCLUDED_)
#define AFX_CAMERAPRESETSTEPPARAMETER_H__A0430EFE_D3A5_43DE_A46B_0E0E8A3C167B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_ACTIVATE_CAMERA_PRESET)

#include <string>
#include "core/data_access_interface/entity_access/src/PlanAgentDataCorbaDef.h"
#include "PlanStepParameter.h"
#include "PlanStepParameterListCtrl.h"


namespace TA_Base_App
{
    class CPlanStepParameterEditDlg;


    /////////////////////////////////////////////////////////////////////////////
    // CameraPresetStepParameter

    class CameraPresetStepParameter : public PlanStepParameter
    {
    // Operations
    public:
        CameraPresetStepParameter(PlanStep& parent, std::string name, unsigned long cameraKey, unsigned long presetKey);
        CameraPresetStepParameter(PlanStep& parent, const CameraPresetStepParameter& parameter);
        CameraPresetStepParameter(const CameraPresetStepParameter& parameter);
        virtual ~CameraPresetStepParameter();

        virtual void addToParameterList(CPlanStepParameterListCtrl& listCtrl);

        // You have to know the camera key to get the preset name.

        void setCameraKey(unsigned long cameraKey);
        void setPresetKey(unsigned long presetKey);
        void setPresetName(const std::string& presetName);
        unsigned long getCameraKey() const { return m_cameraKey; }
        unsigned long getPresetKey() const { return m_presetKey; }
        std::string getPresetName() const { return m_presetName; }

    protected:
        virtual TA_Base_App::CPlanStepParameterEditDlg* getEditDialog(bool canEdit);

    private:
        CameraPresetStepParameter& operator=(const CameraPresetStepParameter& parameter);


    // Attributes
    private:
        unsigned long m_cameraKey;
        unsigned long m_presetKey;
        std::string m_presetName;
    };
}

#endif // defined(STEPTYPE_ACTIVATE_CAMERA_PRESET)

#endif // !defined(AFX_CAMERAPRESETSTEPPARAMETER_H__A0430EFE_D3A5_43DE_A46B_0E0E8A3C167B__INCLUDED_)
