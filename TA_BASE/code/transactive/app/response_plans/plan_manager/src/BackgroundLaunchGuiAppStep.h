/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/BackgroundLaunchGuiAppStep.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  **/

#if !defined(AFX_BackgroundLaunchGuiAppStep_H__163347D0_A533_4c93_B503_5F74B7F70333__INCLUDED_)
#define AFX_BackgroundLaunchGuiAppStep_H__163347D0_A533_4c93_B503_5F74B7F70333__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_BACKGROUND_LAUNCH_GUI_APP)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanManagerCommonDefs.h"
#include "PlanStep.h"


namespace TA_Base_App
{
    class GuiAppTypeStepParameter;
    class NumericStepParameter;
    class TextualStepParameter;
    class YesNoStepParameter;
    /////////////////////////////////////////////////////////////////////////////
    // BackgroundLaunchGuiAppStep

    class BackgroundLaunchGuiAppStep : public PlanStep
    {
    // Operations
    public:
        BackgroundLaunchGuiAppStep(PlanNode* parent, const TA_Base_Core::StepDetail &stepDetail);
        BackgroundLaunchGuiAppStep(PlanNode* parent, PlanStepId stepId);  // For new plan steps
        virtual ~BackgroundLaunchGuiAppStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }

        GuiAppTypeStepParameter* getAppTypeParameter()  { return m_appTypeParameter; }
        TextualStepParameter* getCommandLineParameter() { return m_commandLineParameter; }
        NumericStepParameter* getXPositionParameter()   { return m_xPositionParameter; }
        NumericStepParameter* getYPositionParameter()   { return m_yPositionParameter; }
        NumericStepParameter* getWidthParameter()       { return m_widthParameter; }
        NumericStepParameter* getHeightParameter()      { return m_heightParameter; }
        YesNoStepParameter* getAddTriggerEntityParameter() {return m_addTriggerEntityParameter;}

        NumericStepParameter* getActionType() { return m_actionType; }
        TextualStepParameter* getActionTypeDisplayParameter() { return m_actionTypeDisplay; }

        NumericStepParameter* getDataPointParameter() { return m_dataPoint;}
        TextualStepParameter* getDataPointDisplayParameter() { return m_dataPointDisplay; }

        YesNoStepParameter* getTriggerEntityCheckActionParameter() {return m_TriggerEntityCheckAction;}
        NumericStepParameter* getOperatorProfile() { return m_operatorProfileNumber; }
        TextualStepParameter*  getOperatorProfileName()  { return m_operatorProfileDisplay; }

    protected:
        BackgroundLaunchGuiAppStep(const BackgroundLaunchGuiAppStep &other);

        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                              PlanStepParameterList &stepParameters);

        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        BackgroundLaunchGuiAppStep &operator=(const BackgroundLaunchGuiAppStep &other);

    // Attributes
    public:
        static const unsigned long DEFAULT_X_POSITION;
        static const unsigned long DEFAULT_Y_POSITION;
        static const unsigned long DEFAULT_WIDTH;
        static const unsigned long DEFAULT_HEIGHT;

    // Attributes
    private:
        GuiAppTypeStepParameter* m_appTypeParameter;
        TextualStepParameter* m_commandLineParameter;
        NumericStepParameter* m_xPositionParameter;
        NumericStepParameter* m_yPositionParameter;
        NumericStepParameter* m_widthParameter;
        NumericStepParameter* m_heightParameter;
        YesNoStepParameter*   m_addTriggerEntityParameter;

        NumericStepParameter* m_actionType;
        TextualStepParameter* m_actionTypeDisplay;

        NumericStepParameter* m_dataPoint;
        TextualStepParameter* m_dataPointDisplay;

        YesNoStepParameter*   m_TriggerEntityCheckAction;
        NumericStepParameter* m_operatorProfileNumber;
        TextualStepParameter* m_operatorProfileDisplay;
        
    };
}

#endif // defined (STEPTYPE_BACKGROUND_LAUNCH_GUI_APP)

#endif // !defined(AFX_BackgroundLaunchGuiAppStep_H__163347D0_A533_4c93_B503_5F74B7F70333__INCLUDED_)
