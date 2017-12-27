/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/LaunchSchematicStep.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  **/

#if !defined(AFX_LAUNCHSCHEMATICSTEP_H__0A8BB324_82D4_43BE_8670_61A6B2B15048__INCLUDED_)
#define AFX_LAUNCHSCHEMATICSTEP_H__0A8BB324_82D4_43BE_8670_61A6B2B15048__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_LAUNCH_SCHEMATIC)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanManagerCommonDefs.h"
#include "PlanStep.h"


namespace TA_Base_App
{
    class NumericStepParameter;
    class TextualStepParameter;


    /////////////////////////////////////////////////////////////////////////////
    // LaunchSchematicStep

    class LaunchSchematicStep : public PlanStep
    {
    // Operations
    public:
        LaunchSchematicStep(PlanNode* parent, const TA_Base_Core::StepDetail &stepDetail);
        LaunchSchematicStep(PlanNode* parent, PlanStepId stepId);  // For new plan steps
        virtual ~LaunchSchematicStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }

        TextualStepParameter* getSchematicNameParameter()  { return m_schematicNameParameter; }
        TextualStepParameter* getCommandLineParameter()    { return m_commandLineParameter; }
        NumericStepParameter* getScreenNumberParameter()   { return m_screenNumberParameter; }

    protected:
        LaunchSchematicStep(const LaunchSchematicStep &other);

        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                              PlanStepParameterList &stepParameters);

        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        LaunchSchematicStep &operator=(const LaunchSchematicStep &other);


    // Attributes
    private:
        TextualStepParameter* m_schematicNameParameter;
        TextualStepParameter* m_commandLineParameter;
        NumericStepParameter* m_screenNumberParameter;
    };
}

#endif // defined (STEPTYPE_LAUNCH_SCHEMATIC)

#endif // !defined(AFX_LAUNCHSCHEMATICSTEP_H__0A8BB324_82D4_43BE_8670_61A6B2B15048__INCLUDED_)
