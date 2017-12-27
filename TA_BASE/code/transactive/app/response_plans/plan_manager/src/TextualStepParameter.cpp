/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/TextualStepParameter.cpp $
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
#include "TextualStepParameter.h"
#include "PlanStepParameterTextualEditDlg.h"
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

TextualStepParameter::TextualStepParameter(PlanStep& parent,std::string name, std::string text) :
    PlanStepParameter(parent,name),
    m_text(text)
{
    FUNCTION_ENTRY( "TextualStepParameter" );
    FUNCTION_EXIT;
}


TextualStepParameter::TextualStepParameter(PlanStep& parent, const TextualStepParameter& parameter) :
    PlanStepParameter(parent, parameter.getName()),
    m_text(parameter.getMessageText())
{
    FUNCTION_ENTRY( "TextualStepParameter" );
    FUNCTION_EXIT;
}


TextualStepParameter::TextualStepParameter(const TextualStepParameter& parameter) :
    PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
    m_text(parameter.getMessageText())
{
    FUNCTION_ENTRY( "TextualStepParameter" );
    FUNCTION_EXIT;
}


TextualStepParameter::~TextualStepParameter()
{
    FUNCTION_ENTRY( "~TextualStepParameter" );
    FUNCTION_EXIT;
}


void TextualStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    listCtrl.AddParameter(getName().c_str(), m_text.c_str(),this);

    FUNCTION_EXIT;
}


TA_Base_App::CPlanStepParameterEditDlg* TextualStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new TA_Base_App::CPlanStepParameterTextualEditDlg(*this,canEdit);
}


void TextualStepParameter::setText(std::string text)
{
    FUNCTION_ENTRY( "setText" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_text = text;

    FUNCTION_EXIT;
}


