//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointSetStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_DATAPOINTSETSTEP_H__E1BCCBDF_F2D1_4A63_8C98_0DA66344B8BB__INCLUDED_)
#define AFX_DATAPOINTSETSTEP_H__E1BCCBDF_F2D1_4A63_8C98_0DA66344B8BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_DATA_POINT_SET)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class DataPointStepParameter;
    class DataPointValueStepParameter;


    /////////////////////////////////////////////////////////////////////////////
    // DataPointSetStep

    class DataPointSetStep : public PlanStep
    {
    // Operations
    public:
        DataPointSetStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        DataPointSetStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~DataPointSetStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }
		virtual bool canEnableNoWait() const {return true;}

        DataPointStepParameter*             getDataPointParameter() { return m_dataPoint;};
        DataPointValueStepParameter*        getValueParameter() { return m_dataPointValue;};

    protected:
        DataPointSetStep(const DataPointSetStep &other);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                              PlanStepParameterList &stepParameters);
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        DataPointSetStep &operator=(const DataPointSetStep &other);


    // Attributes
    private:
        DataPointStepParameter *m_dataPoint;
        DataPointValueStepParameter *m_dataPointValue;
    };
}

#endif // defined (STEPTYPE_DATA_POINT_SET)

#endif // !defined(AFX_DATAPOINTSETSTEP_H__E1BCCBDF_F2D1_4A63_8C98_0DA66344B8BB__INCLUDED_)
