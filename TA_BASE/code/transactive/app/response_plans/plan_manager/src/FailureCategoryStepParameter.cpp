/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/FailureCategoryStepParameter.cpp $
  * @author:  Lizette Lingo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_FAILURE_SUMMARY_STEP)

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TransactiveException.h"
#include "FailureCategoryStepParameter.h"
#include "PlanStepParameterEditDlg.h"
#include "PlanStep.h"
#include "PlanNode.h"
#include "Utilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

/////////////////////////////////////////////////////////////////////////////
// PlanStepParameter

FailureCategoryStepParameter::FailureCategoryStepParameter(PlanStep& parent,std::string name, TA_Base_Core::EStepFailureCategory category)
    : PlanStepParameter(parent,name), m_category(category)
{
    FUNCTION_ENTRY( "FailureCategoryStepParameter" );
    FUNCTION_EXIT;
}


FailureCategoryStepParameter::FailureCategoryStepParameter(PlanStep& parent, const FailureCategoryStepParameter& parameter)
    : PlanStepParameter(parent, parameter.getName()), m_category(parameter.getCategory())
{
    FUNCTION_ENTRY( "FailureCategoryStepParameter" );
    FUNCTION_EXIT;
}


FailureCategoryStepParameter::FailureCategoryStepParameter(const FailureCategoryStepParameter& parameter)
    : PlanStepParameter(*parameter.getParentStep(), parameter.getName()), m_category(parameter.getCategory())
{
    FUNCTION_ENTRY( "FailureCategoryStepParameter" );
    FUNCTION_EXIT;
}


void FailureCategoryStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    listCtrl.AddParameter(getName().c_str(), Utilities::convertToString(m_category).c_str(),this);

    FUNCTION_EXIT;
}


void FailureCategoryStepParameter::setCategory(TA_Base_Core::EStepFailureCategory category)
{
    FUNCTION_ENTRY( "setCategory" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_category = category;

    FUNCTION_EXIT;
}


TA_Base_App::CPlanStepParameterEditDlg* FailureCategoryStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new TA_Base_App::CPlanStepParameterEditDlg(*this,canEdit);
}


#endif // defined (STEPTYPE_FAILURE_SUMMARY_STEP)
