/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/WallboardStepParameter.h $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Parameter that stores Wallboard information for the Video Wall step.
  *
  **/

#if !defined(AFX_WALLBOARD_STEP_PARAMETER_H__D7BD9004_A234_4B1F_BDE8_B44B8CEF113C__INCLUDED_)
#define AFX_WALLBOARD_STEP_PARAMETER_H__D7BD9004_A234_4B1F_BDE8_B44B8CEF113C__INCLUDED_

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
    // WallboardStepParameter

    class WallboardStepParameter : public PlanStepParameter
    {
    // Operations
    public:
        WallboardStepParameter(PlanStep& parent, std::string name, unsigned long wallboardKey);
        WallboardStepParameter(PlanStep& parent, const WallboardStepParameter& parameter);
        WallboardStepParameter(const WallboardStepParameter& parameter);
        virtual ~WallboardStepParameter();

        virtual void addToParameterList(CPlanStepParameterListCtrl& listCtrl);

        void setWallboardKey(unsigned long wallboardKey);
        void setWallboardName(const std::string& wallboardName);
        unsigned long getWallboardKey() const { return m_wallboardKey; }
        std::string getWallboardName() const { return m_wallboardName; }

    protected:
        virtual TA_Base_App::CPlanStepParameterEditDlg* getEditDialog(bool canEdit);

    private:
        WallboardStepParameter& operator=(const WallboardStepParameter& parameter);


    // Attributes
    private:
        unsigned long m_wallboardKey;
        std::string m_wallboardName;
    };
}

#endif // defined (STEPTYPE_VIDEOWALL)

#endif // !defined(AFX_WALLBOARD_STEP_PARAMETER_H__D7BD9004_A234_4B1F_BDE8_B44B8CEF113C__INCLUDED_)
