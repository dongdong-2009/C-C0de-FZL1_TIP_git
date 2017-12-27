//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/LaunchGuiAppStep.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_LaunchGuiAppStepSTEP_H__ED768651_B2BC_415D_8DEF_B66C76BE0231__INCLUDED_)
#define AFX_LaunchGuiAppStepSTEP_H__ED768651_B2BC_415D_8DEF_B66C76BE0231__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_LAUNCH_GUI_APP)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanManagerCommonDefs.h"
#include "PlanStep.h"


namespace TA_Base_App
{
    class GuiAppTypeStepParameter;
    class NumericStepParameter;
    class TextualStepParameter;


    /////////////////////////////////////////////////////////////////////////////
    // LaunchGuiAppStep

    class LaunchGuiAppStep : public PlanStep
    {
    // Operations
    public:
        LaunchGuiAppStep(PlanNode* parent, const TA_Base_Core::StepDetail &stepDetail);
        LaunchGuiAppStep(PlanNode* parent, PlanStepId stepId);  // For new plan steps
        virtual ~LaunchGuiAppStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }

        GuiAppTypeStepParameter* getAppTypeParameter()  { return m_appTypeParameter; }
        TextualStepParameter* getCommandLineParameter() { return m_commandLineParameter; }
        NumericStepParameter* getXPositionParameter()   { return m_xPositionParameter; }
        NumericStepParameter* getYPositionParameter()   { return m_yPositionParameter; }
        NumericStepParameter* getWidthParameter()       { return m_widthParameter; }
        NumericStepParameter* getHeightParameter()      { return m_heightParameter; }

    protected:
        LaunchGuiAppStep(const LaunchGuiAppStep &other);

        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                              PlanStepParameterList &stepParameters);

        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        LaunchGuiAppStep &operator=(const LaunchGuiAppStep &other);


    // Attributes
    public:
        static const unsigned long DEFAULT_X_POSITION;
        static const unsigned long DEFAULT_Y_POSITION;
        static const unsigned long DEFAULT_WIDTH;
        static const unsigned long DEFAULT_HEIGHT;

    private:
        GuiAppTypeStepParameter* m_appTypeParameter;
        TextualStepParameter* m_commandLineParameter;
        NumericStepParameter* m_xPositionParameter;
        NumericStepParameter* m_yPositionParameter;
        NumericStepParameter* m_widthParameter;
        NumericStepParameter* m_heightParameter;
    };
}

#endif // defined (STEPTYPE_LAUNCH_GUI_APP)

#endif // !defined(AFX_LaunchGuiAppStepSTEP_H__ED768651_B2BC_415D_8DEF_B66C76BE0231__INCLUDED_)
