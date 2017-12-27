/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/MonitorStepParameter.cpp $
  * @author:  Bart Golab/Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TransactiveException.h"
#include "MonitorStepParameter.h"
#include "PlanStepParameterEditDlg.h"
#include "PlanStepParameterMonitorEditDlg.h"
#include "PlanStep.h"
#include "PlanNode.h"
#include "VideoEnquirer.h"
#include "MessageBox.h"

#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"

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

MonitorStepParameter::MonitorStepParameter(PlanStep& parent,std::string name, unsigned long monitorKey) :
    PlanStepParameter(parent,name),
    m_monitorKey(monitorKey),
    m_monitorName("")
{
    FUNCTION_ENTRY( "MonitorStepParameter" );

    try
    {
        VideoEnquirer enquirer;
        m_monitorName = enquirer.getMonitorName( m_monitorKey );
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        m_monitorName = UNKNOWN_NAME;
    }

    FUNCTION_EXIT;
}


MonitorStepParameter::MonitorStepParameter(PlanStep& parent, const MonitorStepParameter& parameter) :
    PlanStepParameter(parent, parameter.getName()),
    m_monitorKey(parameter.getMonitorKey()),
    m_monitorName("")
{
    FUNCTION_ENTRY( "MonitorStepParameter" );

    try
    {
        VideoEnquirer enquirer;
        m_monitorName = enquirer.getMonitorName( m_monitorKey );
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        m_monitorName = UNKNOWN_NAME;
    }

    FUNCTION_EXIT;
}


MonitorStepParameter::MonitorStepParameter(const MonitorStepParameter& parameter) :
    PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
    m_monitorKey(parameter.getMonitorKey()),
    m_monitorName("")
{
    FUNCTION_ENTRY( "MonitorStepParameter" );

    try
    {
        VideoEnquirer enquirer;
        m_monitorName = enquirer.getMonitorName( m_monitorKey );
    }
    catch( TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        m_monitorName = UNKNOWN_NAME;
    }

    FUNCTION_EXIT;
}


MonitorStepParameter::~MonitorStepParameter()
{
    FUNCTION_ENTRY( "~MonitorStepParameter" );
    FUNCTION_EXIT;
}


void MonitorStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    listCtrl.AddParameter(getName().c_str(), m_monitorName.c_str(),this);

    FUNCTION_EXIT;
}


void MonitorStepParameter::setMonitorKey(unsigned long monitorKey)
{
    FUNCTION_ENTRY( "setMonitorKey" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_monitorKey = monitorKey;

    FUNCTION_EXIT;
}


void MonitorStepParameter::setMonitorName(const std::string& monitorName)
{
    FUNCTION_ENTRY( "setMonitorName" );

    TA_ASSERT(getParentStep()->getParentPlan()->isCopiedPlan(),"Attempt to update the factory plan.");
    m_monitorName = monitorName;

    FUNCTION_EXIT;
}


TA_Base_App::CPlanStepParameterEditDlg* MonitorStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new TA_Base_App::CPlanStepParameterMonitorEditDlg(*this,canEdit);
}


bool MonitorStepParameter::isAnyMonitorAvailable ()
{
    FUNCTION_ENTRY( "isAnyMonitorAvailable" );

	//[[ GF-155 shiva
	TA_Base_Bus::VideoSwitchAgentFactory::VideoMonitorVector monitors;
    try
    {
		monitors = TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getVideoMonitors();
		if(monitors.size() != 0 )
		{				
			FUNCTION_EXIT;
			return true;
		}
    }//]] GF-155 shiva
    catch( const TA_Base_Core::ObjectResolutionException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", e.what() );
        CString actionName, agent;
        actionName.LoadString(IDS_RETRIEVE_MONITORS);
        agent.LoadString(IDS_SCHEDULING_AGENT);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName << agent;
        userMsg.showMsgBox(IDS_UE_030064);
    }
    catch( const TA_Base_Bus::VideoSwitchAgentException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", static_cast< const char* >( e.what ) );
        CString actionName, agent;
        actionName.LoadString(IDS_RETRIEVE_MONITORS);
        agent.LoadString(IDS_SCHEDULING_AGENT);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName << agent;
        userMsg.showMsgBox(IDS_UE_030064);
    }
    catch( const CORBA::Exception& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
        MessageBox::error(IDS_RETRIEVE_MONITORS);
    }
    catch( const TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        MessageBox::error(IDS_RETRIEVE_MONITORS, e);
    }
    catch( ... )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Caught unknown exception" );
        MessageBox::error(IDS_RETRIEVE_MONITORS);
    }

    FUNCTION_EXIT;
    return false;
}


#endif // defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)
