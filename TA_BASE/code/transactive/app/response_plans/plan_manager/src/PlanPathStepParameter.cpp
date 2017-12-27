/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanPathStepParameter.cpp $
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

#if defined(_MSC_VER)
    #pragma warning(disable:4250)
#endif // defined (_MSC_VER)

#include "core/utilities/src/TAAssert.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "PlanPathStepParameter.h"
#include "PlanAgentAccess.h"
#include "PlanStepParameterEditDlg.h"
#include "PlanStepParameterPlanPathEditDlg.h"
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

PlanPathStepParameter::PlanPathStepParameter(PlanStep& parent,std::string name, TreeNodeId planNodeId) :
    PlanStepParameter(parent,name),
    m_planNodeId(planNodeId)
{
    FUNCTION_ENTRY( "PlanPathStepParameter" );
    FUNCTION_EXIT;
}


PlanPathStepParameter::PlanPathStepParameter(PlanStep& parent, PlanPathStepParameter& parameter) :
    PlanStepParameter(parent, parameter.getName()),
    m_planNodeId(parameter.getPlanNodeId())
{
    FUNCTION_ENTRY( "PlanPathStepParameter" );
    FUNCTION_EXIT;
}


PlanPathStepParameter::PlanPathStepParameter(PlanPathStepParameter& parameter) :
    PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
    m_planNodeId(parameter.getPlanNodeId())
{
    FUNCTION_ENTRY( "PlanPathStepParameter" );
    FUNCTION_EXIT;
}


PlanPathStepParameter::~PlanPathStepParameter()
{
    FUNCTION_ENTRY( "~PlanPathStepParameter" );
    FUNCTION_EXIT;
}


void PlanPathStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    try
    {
        std::string planPath = getPlanPath();
        listCtrl.AddParameter(getName().c_str(), planPath.c_str(), this);
    }
    catch(TA_Base_Core::TransactiveException&)
    {
        // Couldn't retrieve plan path. Try adding the parameter with the plan path set
        // to an error message.
        TA_Base_Bus::TransActiveMessage msg;
        CString param;
        param.LoadString(IDS_RETRIEVE_PARAMETER_VALUE);
        msg << param;
        CString error = msg.constructMessage(IDS_UE_010028);
        listCtrl.AddParameter(getName().c_str(), error, this);
    }

    FUNCTION_EXIT;
}


TA_Base_App::CPlanStepParameterEditDlg* PlanPathStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new TA_Base_App::CPlanStepParameterPlanPathEditDlg(*this,canEdit);
}


void PlanPathStepParameter::setPlanNodeId(TreeNodeId planNodeId)
{
    FUNCTION_ENTRY( "setPlanNodeId" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_planNodeId = planNodeId;

    FUNCTION_EXIT;
}


std::string PlanPathStepParameter::getPlanPath()
{
    FUNCTION_ENTRY( "getPlanPath" );
    FUNCTION_EXIT;
    return PlanAgentAccess::getInstance().getNodePath(m_planNodeId);
}


