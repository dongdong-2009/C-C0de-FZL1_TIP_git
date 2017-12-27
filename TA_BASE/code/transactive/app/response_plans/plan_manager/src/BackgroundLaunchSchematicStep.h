/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Bart Golab
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * <description>
  *
  **/

#if !defined(AFX_BackgroundLaunchSchematicStep_H__163347D0_A533_4c93_B503_5F74B7F70333__INCLUDED_)
#define AFX_BackgroundLaunchSchematicStep_H__163347D0_A533_4c93_B503_5F74B7F70333__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_BACKGROUND_LAUNCH_SCHEMATIC)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanManagerCommonDefs.h"
#include "PlanStep.h"


namespace TA_Base_App
{
    class NumericStepParameter;
    class TextualStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // BackgroundLaunchSchematicStep

    class BackgroundLaunchSchematicStep : public PlanStep
    {
    // Operations
    public:
        BackgroundLaunchSchematicStep(PlanNode* parent, const TA_Base_Core::StepDetail &stepDetail);
        BackgroundLaunchSchematicStep(PlanNode* parent, PlanStepId stepId);  // For new plan steps
        virtual ~BackgroundLaunchSchematicStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }

        NumericStepParameter* getDataPointParameter() { return m_dataPoint;};
        TextualStepParameter* getSchematicNameParameter()  { return m_schematicNameParameter; }
        TextualStepParameter* getCommandLineParameter()    { return m_commandLineParameter; }
        NumericStepParameter* getScreenNumberParameter()   { return m_screenNumberParameter; }
        NumericStepParameter* getActionType() { return m_actionType; };

        TextualStepParameter* getDataPointDisplayParameter() { return m_dataPointDisplay; };
        TextualStepParameter* getActionTypeDisplayParameter() { return m_actionTypeDisplay; };

        NumericStepParameter* getOperatorProfile() { return m_operatorProfileNumber; }
        TextualStepParameter*  getOperatorProfileName()  { return m_operatorProfileDisplay; }
    protected:
        BackgroundLaunchSchematicStep(const BackgroundLaunchSchematicStep &other);

        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                              PlanStepParameterList &stepParameters);

        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        BackgroundLaunchSchematicStep &operator=(const BackgroundLaunchSchematicStep &other);


    // Attributes
    private:
        NumericStepParameter* m_dataPoint;
        TextualStepParameter* m_schematicNameParameter;
        TextualStepParameter* m_commandLineParameter;
        NumericStepParameter* m_screenNumberParameter;
        NumericStepParameter* m_actionType;

        TextualStepParameter* m_dataPointDisplay;
        TextualStepParameter* m_actionTypeDisplay;
        NumericStepParameter* m_operatorProfileNumber;
        TextualStepParameter* m_operatorProfileDisplay;
    };
}

#endif // defined (STEPTYPE_BACKGROUND_LAUNCH_SCHEMATIC)

#endif // !defined(AFX_BackgroundLaunchSchematicStep_H__163347D0_A533_4c93_B503_5F74B7F70333__INCLUDED_)
