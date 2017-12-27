/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/WallboardLayoutStepParameter.cpp $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Parameter that stores WallboardLayout Layout information for the Video Wall step.
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_VIDEOWALL)

#include "WallboardLayoutStepParameter.h"
#include "PlanStepParameterEditDlg.h"
#include "PlanStep.h"
#include "PlanNode.h"
#include "VideoEnquirer.h"
#include "MessageBox.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TransactiveException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

namespace
{
    const unsigned long DEFAULT_KEY = 0;
    const std::string UNKNOWN_NAME = "";
}

/////////////////////////////////////////////////////////////////////////////
// PlanStepParameter

WallboardLayoutStepParameter::WallboardLayoutStepParameter(PlanStep& parent, std::string name, unsigned long layoutKey )
  : PlanStepParameter(parent,name),
    m_layoutKey(layoutKey),
    m_layoutName("")
{
    FUNCTION_ENTRY( "WallboardLayoutStepParameter" );

    try
    {
        VideoEnquirer enquirer;
        m_layoutName = enquirer.getLayoutName( m_layoutKey );
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        m_layoutName = UNKNOWN_NAME;
    }

    FUNCTION_EXIT;
}


WallboardLayoutStepParameter::WallboardLayoutStepParameter(PlanStep& parent, const WallboardLayoutStepParameter& parameter)
  : PlanStepParameter(parent, parameter.getName()),
    m_layoutKey(parameter.getLayoutKey()),
    m_layoutName("")
{
    FUNCTION_ENTRY( "WallboardLayoutStepParameter" );

    try
    {
        VideoEnquirer enquirer;
        m_layoutName = enquirer.getLayoutName( m_layoutKey );
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        m_layoutName = UNKNOWN_NAME;
    }

    FUNCTION_EXIT;
}


WallboardLayoutStepParameter::WallboardLayoutStepParameter(const WallboardLayoutStepParameter& parameter)
  : PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
    m_layoutKey(parameter.getLayoutKey()),
    m_layoutName("")
{
    FUNCTION_ENTRY( "WallboardLayoutStepParameter" );

    try
    {
        VideoEnquirer enquirer;
        m_layoutName = enquirer.getLayoutName( m_layoutKey );
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        m_layoutName = UNKNOWN_NAME;
    }

    FUNCTION_EXIT;
}


WallboardLayoutStepParameter::~WallboardLayoutStepParameter()
{
    FUNCTION_ENTRY( "~WallboardLayoutStepParameter" );
    FUNCTION_EXIT;
}


void WallboardLayoutStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    listCtrl.AddParameter(getName().c_str(), m_layoutName.c_str(),this);

    FUNCTION_EXIT;
}


void WallboardLayoutStepParameter::setLayoutKey(unsigned long layoutKey)
{
    FUNCTION_ENTRY( "setLayoutKey" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_layoutKey = layoutKey;

    FUNCTION_EXIT;
}


void WallboardLayoutStepParameter::setLayoutName(const std::string& layoutName)
{
    FUNCTION_ENTRY( "setLayoutName" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_layoutName = layoutName;

    FUNCTION_EXIT;
}


TA_Base_App::CPlanStepParameterEditDlg* WallboardLayoutStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new TA_Base_App::CPlanStepParameterEditDlg(*this,canEdit);
}


#endif // defined (STEPTYPE_VIDEOWALL)
