/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/WallboardStepParameter.cpp $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Parameter that stores Wallboard information for the Video Wall step.
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_VIDEOWALL)

#include "WallboardStepParameter.h"
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

WallboardStepParameter::WallboardStepParameter(PlanStep& parent, std::string name, unsigned long wallboardKey )
  : PlanStepParameter(parent,name),
    m_wallboardKey(wallboardKey),
    m_wallboardName("")
{
    FUNCTION_ENTRY( "WallboardStepParameter" );

    try
    {
        VideoEnquirer enquirer;
        m_wallboardName = enquirer.getWallboardName( m_wallboardKey );
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        m_wallboardName = UNKNOWN_NAME;
    }

    FUNCTION_EXIT;
}


WallboardStepParameter::WallboardStepParameter(PlanStep& parent, const WallboardStepParameter& parameter)
  : PlanStepParameter(parent, parameter.getName()),
    m_wallboardKey(parameter.getWallboardKey()),
    m_wallboardName("")
{
    FUNCTION_ENTRY( "WallboardStepParameter" );

    try
    {
        VideoEnquirer enquirer;
        m_wallboardName = enquirer.getWallboardName( m_wallboardKey );
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        m_wallboardName = UNKNOWN_NAME;
    }

    FUNCTION_EXIT;
}


WallboardStepParameter::WallboardStepParameter(const WallboardStepParameter& parameter)
  : PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
    m_wallboardKey(parameter.getWallboardKey()),
    m_wallboardName("")
{
    FUNCTION_ENTRY( "WallboardStepParameter" );

    try
    {
        VideoEnquirer enquirer;
        m_wallboardName = enquirer.getWallboardName( m_wallboardKey );
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        m_wallboardName = UNKNOWN_NAME;
    }

    FUNCTION_EXIT;
}


WallboardStepParameter::~WallboardStepParameter()
{
    FUNCTION_ENTRY( "~WallboardStepParameter" );
    FUNCTION_EXIT;
}


void WallboardStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    listCtrl.AddParameter(getName().c_str(), m_wallboardName.c_str(),this);

    FUNCTION_EXIT;
}


void WallboardStepParameter::setWallboardKey(unsigned long wallboardKey)
{
    FUNCTION_ENTRY( "setWallboardKey" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_wallboardKey = wallboardKey;

    FUNCTION_EXIT;
}


void WallboardStepParameter::setWallboardName(const std::string& wallboardName)
{
    FUNCTION_ENTRY( "setWallboardName" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_wallboardName = wallboardName;

    FUNCTION_EXIT;
}


TA_Base_App::CPlanStepParameterEditDlg* WallboardStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new TA_Base_App::CPlanStepParameterEditDlg(*this,canEdit);
}


#endif // defined (STEPTYPE_VIDEOWALL)
