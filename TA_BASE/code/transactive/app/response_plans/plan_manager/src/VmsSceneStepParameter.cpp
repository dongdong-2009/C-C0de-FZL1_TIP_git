/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/VmsSceneStepParameter.cpp $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_VMS_SET_SCENE)

#include "VmsSceneStepParameter.h"
#include "PlanStepParameterEditDlg.h"
#include "PlanStep.h"
#include "PlanNode.h"
#include "VmsEnquirer.h"
#include "MessageBox.h"
#include "core/utilities/src/TAAssert.h"

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
// VmsSceneStepParameter

VmsSceneStepParameter::VmsSceneStepParameter(PlanStep& parent, std::string name, unsigned long vmsSceneKey )
  : PlanStepParameter(parent,name),
    m_vmsSceneKey(vmsSceneKey),
    m_vmsSceneName( UNKNOWN_NAME )
{
    FUNCTION_ENTRY( "VmsSceneStepParameter" );

    try
    {
        m_vmsSceneName = TA_Signs::VmsEnquirer::getInstance().getVmsSceneName( m_vmsSceneKey );
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        m_vmsSceneName = UNKNOWN_NAME;
    }

    FUNCTION_EXIT;
}


VmsSceneStepParameter::VmsSceneStepParameter(PlanStep& parent, const VmsSceneStepParameter& parameter)
  : PlanStepParameter(parent, parameter.getName()),
    m_vmsSceneKey(parameter.getVmsSceneKey()),
    m_vmsSceneName( UNKNOWN_NAME )
{
    FUNCTION_ENTRY( "VmsSceneStepParameter" );

    try
    {
        m_vmsSceneName = TA_Signs::VmsEnquirer::getInstance().getVmsSceneName( m_vmsSceneKey );
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        m_vmsSceneName = UNKNOWN_NAME;
    }

    FUNCTION_EXIT;
}


VmsSceneStepParameter::VmsSceneStepParameter(const VmsSceneStepParameter& parameter)
  : PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
    m_vmsSceneKey(parameter.getVmsSceneKey()),
    m_vmsSceneName( UNKNOWN_NAME )
{
    FUNCTION_ENTRY( "VmsSceneStepParameter" );

    try
    {
        m_vmsSceneName = TA_Signs::VmsEnquirer::getInstance().getVmsSceneName( m_vmsSceneKey );
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        m_vmsSceneName = UNKNOWN_NAME;
    }

    FUNCTION_EXIT;
}


VmsSceneStepParameter::~VmsSceneStepParameter()
{
    FUNCTION_ENTRY( "~VmsSceneStepParameter" );
    FUNCTION_EXIT;
}


void VmsSceneStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    listCtrl.AddParameter(getName().c_str(), m_vmsSceneName.c_str(),this);

    FUNCTION_EXIT;
}


void VmsSceneStepParameter::setVmsSceneKey(unsigned long vmsSceneKey)
{
    FUNCTION_ENTRY( "setVmsSceneKey" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_vmsSceneKey = vmsSceneKey;

    FUNCTION_EXIT;
}


void VmsSceneStepParameter::setVmsSceneName(const std::string& vmsSceneName)
{
    FUNCTION_ENTRY( "setVmsSceneName" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_vmsSceneName = vmsSceneName;

    FUNCTION_EXIT;
}


TA_Base_App::CPlanStepParameterEditDlg* VmsSceneStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new TA_Base_App::CPlanStepParameterEditDlg(*this,canEdit);
}


#endif // defined (STEPTYPE_VMS_SET_SCENE)
