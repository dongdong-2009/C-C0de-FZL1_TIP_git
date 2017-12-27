/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/NumericStepParameter.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  **/

#include "StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "NumericStepParameter.h"
#include "PlanStepParameterNumericEditDlg.h"
#include "PlanStep.h"
#include "PlanNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStepParameter

NumericStepParameter::NumericStepParameter(PlanStep& parent, std::string name, unsigned long number, bool isHidden) :
    PlanStepParameter(parent, name),
    m_number(number),
    m_isHiddenValue(isHidden)
{
    FUNCTION_ENTRY( "NumericStepParameter" );
    FUNCTION_EXIT;
}


NumericStepParameter::NumericStepParameter(PlanStep& parent, const NumericStepParameter& parameter) :
    PlanStepParameter(parent, parameter.getName()),
    m_number(parameter.getNumericValue()),
    m_isHiddenValue(parameter.isHiddenValue())
{
    FUNCTION_ENTRY( "NumericStepParameter" );
    FUNCTION_EXIT;
}


NumericStepParameter::NumericStepParameter(const NumericStepParameter& parameter) :
    PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
    m_number(parameter.getNumericValue()),
    m_isHiddenValue(parameter.isHiddenValue())
{
    FUNCTION_ENTRY( "NumericStepParameter" );
    FUNCTION_EXIT;
}


NumericStepParameter::~NumericStepParameter()
{
    FUNCTION_ENTRY( "~NumericStepParameter" );
    FUNCTION_EXIT;
}


void NumericStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    listCtrl.AddParameter(getName().c_str(), (isHiddenValue()? "" : TA_Base_Bus::StringUtilities::convertToString(m_number)), this);

    FUNCTION_EXIT;
}


TA_Base_App::CPlanStepParameterEditDlg* NumericStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new TA_Base_App::CPlanStepParameterNumericEditDlg(*this, canEdit);
}


void NumericStepParameter::setValue(unsigned long number)
{
    FUNCTION_ENTRY( "setValue" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_number = number;

    FUNCTION_EXIT;
}


void NumericStepParameter::hideValue(bool isHidden)
{
    FUNCTION_ENTRY( "hideValue" );

    m_isHiddenValue = isHidden;

    FUNCTION_EXIT;
}


