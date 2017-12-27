/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/VmsSceneStepParameter.h $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  **/

#if !defined(AFX_VMS_SCENE_STEP_PARAMETER_H__D7BD9004_A234_4B1F_BDE8_B44B8CEF113C__INCLUDED_)
#define AFX_VMS_SCENE_STEP_PARAMETER_H__D7BD9004_A234_4B1F_BDE8_B44B8CEF113C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_VMS_SET_SCENE)

#include <string>
#include "core/data_access_interface/entity_access/src/PlanAgentDataCorbaDef.h"
#include "PlanStepParameter.h"
#include "PlanStepParameterListCtrl.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // VmsSceneStepParameter

    class VmsSceneStepParameter : public PlanStepParameter
    {
    // Operations
    public:
        VmsSceneStepParameter(PlanStep& parent, std::string name, unsigned long vmsSceneKey);
        VmsSceneStepParameter(PlanStep& parent, const VmsSceneStepParameter& parameter);
        VmsSceneStepParameter(const VmsSceneStepParameter& parameter);
        virtual ~VmsSceneStepParameter();

        virtual void addToParameterList(CPlanStepParameterListCtrl& listCtrl);

        void setVmsSceneKey(unsigned long vmsSceneKey);
        void setVmsSceneName(const std::string& vmsSceneName);
        unsigned long getVmsSceneKey() const { return m_vmsSceneKey; }
        std::string getVmsSceneName() const { return m_vmsSceneName; }

    protected:
        virtual TA_Base_App::CPlanStepParameterEditDlg* getEditDialog(bool canEdit);

    private:
        VmsSceneStepParameter& operator=(const VmsSceneStepParameter& parameter);


    // Attributes
    private:
        unsigned long m_vmsSceneKey;
        std::string m_vmsSceneName;
    };

} // namspace TA_Base_App

#endif // defined (STEPTYPE_VMS_SET_SCENE)

#endif // !defined(AFX_VMS_SCENE_STEP_PARAMETER_H__D7BD9004_A234_4B1F_BDE8_B44B8CEF113C__INCLUDED_)
