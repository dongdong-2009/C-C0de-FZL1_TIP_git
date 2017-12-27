/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/WallboardLayoutStepParameter.h $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Parameter that stores WallboardLayout Layout information for the Video Wall step.
  *
  **/

#if !defined(AFX_WALLBOARD_LAYOUT_STEP_PARAMETER_H__D7BD9004_A234_4B1F_BDE8_B44B8CEF113C__INCLUDED_)
#define AFX_WALLBOARD_LAYOUT_STEP_PARAMETER_H__D7BD9004_A234_4B1F_BDE8_B44B8CEF113C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_VIDEOWALL)

#include <string>
#include "core/data_access_interface/entity_access/src/PlanAgentDataCorbaDef.h"
#include "PlanStepParameter.h"
#include "PlanStepParameterListCtrl.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // WallboardLayoutStepParameter

    class WallboardLayoutStepParameter : public PlanStepParameter
    {
    // Operations
    public:
        WallboardLayoutStepParameter(PlanStep& parent, std::string name, unsigned long layoutKey);
        WallboardLayoutStepParameter(PlanStep& parent, const WallboardLayoutStepParameter& parameter);
        WallboardLayoutStepParameter(const WallboardLayoutStepParameter& parameter);
        virtual ~WallboardLayoutStepParameter();

        virtual void addToParameterList(CPlanStepParameterListCtrl& listCtrl);

        void setLayoutKey(unsigned long layoutKey);
        void setLayoutName(const std::string& layoutName);
        unsigned long getLayoutKey() const { return m_layoutKey; }
        std::string getLayoutName() const { return m_layoutName; }

    protected:
        virtual TA_Base_App::CPlanStepParameterEditDlg* getEditDialog(bool canEdit);

    private:
        WallboardLayoutStepParameter& operator=(const WallboardLayoutStepParameter& parameter);


    // Attributes
    private:
        unsigned long m_layoutKey;
        std::string m_layoutName;
    };
}

#endif // defined (STEPTYPE_VIDEOWALL)

#endif // !defined(AFX_WALLBOARD_LAYOUT_STEP_PARAMETER_H__D7BD9004_A234_4B1F_BDE8_B44B8CEF113C__INCLUDED_)
