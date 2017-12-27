/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/StepNumberStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  **/

#if !defined(AFX_STEPNUMBERSTEPPARAMETER_H__5A56B69A_6A40_48BF_93B7_4FF23CCA70E8__INCLUDED_)
#define AFX_STEPNUMBERSTEPPARAMETER_H__5A56B69A_6A40_48BF_93B7_4FF23CCA70E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStepParameter.h"
#include "PlanStepParameterListCtrl.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // StepNumberStepParameter

    class StepNumberStepParameter : public PlanStepParameter
    {
    // Operations
    public:
        StepNumberStepParameter(PlanStep& parent, std::string name, PlanStepId stepId);
        StepNumberStepParameter(PlanStep& parent, StepNumberStepParameter& parameter);
        StepNumberStepParameter(StepNumberStepParameter& parameter);
        virtual ~StepNumberStepParameter();

        virtual void addToParameterList(CPlanStepParameterListCtrl& listCtrl);

        void setStepId(PlanStepId stepId);
        PlanStepId getStepId() const { return m_stepId; }
        std::string getDestinationStepDescription();

    protected:
        virtual CPlanStepParameterEditDlg* getEditDialog(bool canEdit);

    private:
        StepNumberStepParameter& operator=(const StepNumberStepParameter& parameter);


    // Attributes
    private:
        PlanStepId m_stepId;

    };
}

#endif // !defined(AFX_STEPNUMBERSTEPPARAMETER_H__5A56B69A_6A40_48BF_93B7_4FF23CCA70E8__INCLUDED_)
