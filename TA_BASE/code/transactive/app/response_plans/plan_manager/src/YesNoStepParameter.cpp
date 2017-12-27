/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/YesNoStepParameter.cpp $
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
#include "YesNoStepParameter.h"
#include "LabelMaker.h"
#include "PlanStepParameterYesNoEditDlg.h"
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

YesNoStepParameter::YesNoStepParameter(PlanStep& parent,std::string name, bool affirmative) :
    PlanStepParameter(parent,name),
    m_affirmative(affirmative)
{
    FUNCTION_ENTRY( "YesNoStepParameter" );
    FUNCTION_EXIT;
}


YesNoStepParameter::YesNoStepParameter(PlanStep& parent, const YesNoStepParameter& parameter) :
    PlanStepParameter(parent, parameter.getName()),
    m_affirmative(parameter.getAffirmative())
{
    FUNCTION_ENTRY( "YesNoStepParameter" );
    FUNCTION_EXIT;
}


YesNoStepParameter::YesNoStepParameter(const YesNoStepParameter& parameter) :
    PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
    m_affirmative(parameter.getAffirmative())
{
    FUNCTION_ENTRY( "YesNoStepParameter" );
    FUNCTION_EXIT;
}


YesNoStepParameter::~YesNoStepParameter()
{
    FUNCTION_ENTRY( "~YesNoStepParameter" );
    FUNCTION_EXIT;
}


void YesNoStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    listCtrl.AddParameter(getName().c_str(), LabelMaker::getYesNoLabel(m_affirmative),this);

    FUNCTION_EXIT;
}


TA_Base_App::CPlanStepParameterEditDlg* YesNoStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new TA_Base_App::CPlanStepParameterYesNoEditDlg(*this,canEdit);
}


void YesNoStepParameter::setAffirmative(bool affirmative)
{
    FUNCTION_ENTRY( "setAffirmative" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_affirmative = affirmative;

    FUNCTION_EXIT;
}


