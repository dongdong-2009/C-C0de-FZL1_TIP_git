/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/VmsStepParameter.cpp $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_VMS_SET_SCENE) || defined (STEPTYPE_VMS_RESTORE_SCHEDULE) || defined (STEPTYPE_VMS_BLANKOUT)

#include "VmsStepParameter.h"
#include "PlanStepParameterEditDlg.h"
#include "PlanStep.h"
#include "PlanNode.h"
#include "VmsEnquirer.h"
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

VmsStepParameter::VmsStepParameter(PlanStep& parent, std::string name, unsigned long vmsKey )
  : PlanStepParameter(parent,name),
    m_vmsKey(vmsKey),
    m_vmsName( "" )
{
    FUNCTION_ENTRY( "VmsStepParameter" );

    try
    {
        m_vmsName = TA_Signs::VmsEnquirer::getInstance().getVmsName( m_vmsKey );
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        m_vmsName = UNKNOWN_NAME;
    }

    FUNCTION_EXIT;
}


VmsStepParameter::VmsStepParameter(PlanStep& parent, const VmsStepParameter& parameter)
  : PlanStepParameter(parent, parameter.getName()),
    m_vmsKey(parameter.getVmsKey()),
    m_vmsName( "" )
{
    FUNCTION_ENTRY( "VmsStepParameter" );

    try
    {
        m_vmsName = TA_Signs::VmsEnquirer::getInstance().getVmsName( m_vmsKey );
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        m_vmsName = UNKNOWN_NAME;
    }

    FUNCTION_EXIT;
}


VmsStepParameter::VmsStepParameter(const VmsStepParameter& parameter)
  : PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
    m_vmsKey(parameter.getVmsKey()),
    m_vmsName( "" )
{
    FUNCTION_ENTRY( "VmsStepParameter" );

    try
    {
        m_vmsName = TA_Signs::VmsEnquirer::getInstance().getVmsName( m_vmsKey );
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        m_vmsName = UNKNOWN_NAME;
    }

    FUNCTION_EXIT;
}


VmsStepParameter::~VmsStepParameter()
{
    FUNCTION_ENTRY( "~VmsStepParameter" );
    FUNCTION_EXIT;
}


void VmsStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    listCtrl.AddParameter(getName().c_str(), m_vmsName.c_str(),this);

    FUNCTION_EXIT;
}


void VmsStepParameter::setVmsKey(unsigned long vmsKey)
{
    FUNCTION_ENTRY( "setVmsKey" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_vmsKey = vmsKey;

    FUNCTION_EXIT;
}


void VmsStepParameter::setVmsName(const std::string& vmsName)
{
    FUNCTION_ENTRY( "setVmsName" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_vmsName = vmsName;

    FUNCTION_EXIT;
}


TA_Base_App::CPlanStepParameterEditDlg* VmsStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new TA_Base_App::CPlanStepParameterEditDlg(*this,canEdit);
}


#endif // defined (STEPTYPE_VMS_SET_SCENE) || defined (STEPTYPE_VMS_RESTORE_SCHEDULE) || defined (STEPTYPE_VMS_BLANKOUT)


