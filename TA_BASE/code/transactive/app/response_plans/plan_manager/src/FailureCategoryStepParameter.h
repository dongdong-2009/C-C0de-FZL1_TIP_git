/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/FailureCategoryStepParameter.h $
  * @author:  Lizette Lingo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  **/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_FAILURE_SUMMARY_STEP)

#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStepParameter.h"
#include "PlanStepParameterListCtrl.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // FailureCategoryStepParameter

    class FailureCategoryStepParameter : public PlanStepParameter
    {
    // Operations
    public:
        FailureCategoryStepParameter(PlanStep& parent, std::string name, TA_Base_Core::EStepFailureCategory category);
        FailureCategoryStepParameter(PlanStep& parent, const FailureCategoryStepParameter& parameter);
        FailureCategoryStepParameter(const FailureCategoryStepParameter& parameter);
        virtual ~FailureCategoryStepParameter() {}

        virtual void addToParameterList(CPlanStepParameterListCtrl& listCtrl);

        void setCategory(TA_Base_Core::EStepFailureCategory category);
        TA_Base_Core::EStepFailureCategory getCategory() const { return m_category; }


    protected:
        virtual CPlanStepParameterEditDlg* getEditDialog(bool canEdit);

    private:
        FailureCategoryStepParameter& operator=(const FailureCategoryStepParameter& parameter);

    // Attributes
    private:
        TA_Base_Core::EStepFailureCategory m_category;

    };
}

#endif // defined (STEPTYPE_FAILURE_SUMMARY_STEP)


