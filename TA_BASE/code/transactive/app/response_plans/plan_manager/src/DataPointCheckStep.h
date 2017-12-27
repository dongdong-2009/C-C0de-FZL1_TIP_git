//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointCheckStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_DATAPOINTCHECKSTEP_H__6FB752F8_A4CB_4F92_B486_C933B5041E39__INCLUDED_)
#define AFX_DATAPOINTCHECKSTEP_H__6FB752F8_A4CB_4F92_B486_C933B5041E39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_DATA_POINT_CHECK)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class DataPointStepParameter;
    class DataPointTestTypeStepParameter;
    class DataPointValueStepParameter;
    class StepNumberStepParameter;


    /////////////////////////////////////////////////////////////////////////////
    // DataPointCheckStep

    class DataPointCheckStep : public PlanStep
    {
    // Operations
    public:
        DataPointCheckStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        DataPointCheckStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~DataPointCheckStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return true; }
        virtual bool canIgnoreFailure() const { return false; }

        DataPointStepParameter*         getDataPointParameter() { return m_dataPoint;};
        DataPointTestTypeStepParameter* getTestTypeParameter() { return m_dataPointTestType;};
        DataPointValueStepParameter*    getValueParameter() { return m_dataPointValue;};
        StepNumberStepParameter*        getSuccessParameter() { return m_successParameter;};
        StepNumberStepParameter*        getFailParameter() { return m_failureParameter;};

    protected:
        DataPointCheckStep(const DataPointCheckStep &other);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                              PlanStepParameterList &stepParameters);

        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        DataPointCheckStep &operator=(const DataPointCheckStep &other);


    // Attributes
    private:
        DataPointStepParameter *m_dataPoint;
        DataPointTestTypeStepParameter *m_dataPointTestType;
        DataPointValueStepParameter *m_dataPointValue;
        StepNumberStepParameter* m_successParameter;
        StepNumberStepParameter* m_failureParameter;
    };
}

#endif // defined (STEPTYPE_DATA_POINT_CHECK)

#endif // !defined(AFX_DATAPOINTCHECKSTEP_H__6FB752F8_A4CB_4F92_B486_C933B5041E39__INCLUDED_)
