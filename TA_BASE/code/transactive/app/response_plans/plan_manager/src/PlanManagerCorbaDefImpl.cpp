//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File$
// @author:  Bart Golab
// @version: $Revision$
//
// Last modification: $DateTime$
// Last modified by:  $Author$
//
// <description>

#include "StdAfx.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/CorbaException.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"
#include "PlanManagerCorbaDefImpl.h"
#include "InteractiveStep.h"
#include "ExecutableDecisionStep.h"
#include "ExecutablePromptStep.h"
#include "ExecutableLogEntryStep.h"
#include "ExecutableRunPlanStep.h"
#include "ExecutableLaunchGuiAppStep.h"
#include "ExecutableLaunchSchematicStep.h"
#include "ExecutableFailureSummaryStep.h"
#include "TreeNodeFactory.h"
#include "TreeNode.h"
#include "PlanNode.h"
#include "PlanAgentAccess.h"
#include "PlanManagerCommonDefs.h"

#include "Utilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanManagerCorbaDefImpl

PlanManagerCorbaDefImpl::PlanManagerCorbaDefImpl()
{
    FUNCTION_ENTRY( "PlanManagerCorbaDefImpl" );
    FUNCTION_EXIT;
}


PlanManagerCorbaDefImpl::~PlanManagerCorbaDefImpl()
{
    FUNCTION_ENTRY( "~PlanManagerCorbaDefImpl" );

    // Launching Issue, Yanrong++
    CloseAllRegisterThread();
    // ++Yanrong
  // release all agent-registration timers

     std::vector<Timer*> timers;
    m_agentRegisterTimerMap.getKeys(timers);

    for (std::vector<Timer*>::iterator iter = timers.begin(); iter != timers.end(); iter++)
    {
        Timer *timer = *iter;

        delete timer;
        timer = NULL;
    }
    timers.clear();
    m_agentRegisterTimerMap.clear();
    deactivateServant();
    FUNCTION_EXIT;
}


int PlanManagerCorbaDefImpl::toActivate()
{
    FUNCTION_ENTRY( "toActivate" );

    activateServant();
    try
    {
        // Get all the locations
        m_locationList = TA_Base_Bus::PlanAgentLocationAccess::getInstance().getAllPlanAgentLocations();

        FUNCTION_EXIT;
        return m_locationList.size();
    }
    catch (const TA_Base_Core::TransactiveException)
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210072);
    }

    FUNCTION_EXIT;
    return 0;
}


void PlanManagerCorbaDefImpl::init(int nIndex, std::string &error)
{
    FUNCTION_ENTRY( "init" );

    // Launching Issue, Yanrong++
    LocationKey lk = m_locationList.at(nIndex);
    unsigned int retryInterval = 30000;
    StartThreadRegisterPM( lk, retryInterval );

    FUNCTION_EXIT;
    return;
    // ++Yanrong

    try
    {
        // Contact the plan agent at each location and register for interactive steps.
        // registerPlanManager() will skip a location if no Plan Agent entity is configured for it.
        LocationKey location = m_locationList.at(nIndex);

        try
        {
            PlanAgentAccess::getInstance().registerPlanManager(_this(), location);
        }
        catch (const TA_Base_Core::TransactiveException&)
        {
            error = TA_Base_Bus::PlanAgentLocationAccess::getInstance().getLocationName(location);
            // start a timer for each agent that could not be registered with initially
            Timer *timer = new Timer();
            timer->subscribe(this);
            timer->start (REREGISTRATION_TIMEOUT_SECONDS * 1000);

            m_agentRegisterTimerMap.insert(timer, location);
        }
    }
    catch (const TA_Base_Core::TransactiveException)
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210072);
    }

    FUNCTION_EXIT;
}


void PlanManagerCorbaDefImpl::timerExpired(Timer &timer)
{
    FUNCTION_ENTRY( "timerExpired" );

    // It is time to retry the regitration with the agent associated with the elapsed timer.
    TA_ASSERT(m_agentRegisterTimerMap.find(&timer), "Expired timer not found in edit lock refresh timer map");

    LocationKey location = m_agentRegisterTimerMap[&timer];

    // only those agents that this manager is not registered with
    // on start up will have active timers
    try
    {
        // register manager
        PlanAgentAccess::getInstance().registerPlanManager(_this(), location);

        // if registration goes well stop the lock refresh timer for this location
        Timer *timer = getAgentRegistrationTimer (location);
        TA_ASSERT(timer != NULL, "Attempt to release an edit lock without an associated refresh timer");

        m_agentRegisterTimerMap.erase(timer);
        timer->stop();
        timer->unsubscribe();
        delete timer;

    }
    catch (const TA_Base_Core::TransactiveException&)
    {
        // do not report failed registration here - just keep trying
        // untill agent becomes available
    }

    FUNCTION_EXIT;
}


Timer *PlanManagerCorbaDefImpl::getAgentRegistrationTimer(LocationKey location)
{
    FUNCTION_ENTRY( "getAgentRegistrationTimer" );

    std::vector<Timer*> timers;
    m_agentRegisterTimerMap.getKeys(timers);

    for (std::vector<Timer*>::iterator iter = timers.begin(); iter != timers.end(); iter++)
    {
        Timer *timer = *iter;

        if (location == m_agentRegisterTimerMap[timer])
        {
            FUNCTION_EXIT;
            return timer;
        }
    }

    FUNCTION_EXIT;
    return NULL;
}


void PlanManagerCorbaDefImpl::executeInteractiveStep(const TA_Base_Core::CurrentPlanStepDetail& stepDetail)
{
    FUNCTION_ENTRY( "executeInteractiveStep" );

    std::auto_ptr<InteractiveStep> interactiveStep;

    switch (stepDetail.data._d())
    {
        case TA_Base_Core::PROMPT_STEP:
            interactiveStep = std::auto_ptr<InteractiveStep>(new ExecutablePromptStep(stepDetail));
            break;

        case TA_Base_Core::DECISION_STEP:
            interactiveStep = std::auto_ptr<InteractiveStep>(new ExecutableDecisionStep(stepDetail));
            break;

        case TA_Base_Core::LOG_STEP:
            interactiveStep = std::auto_ptr<InteractiveStep>(new ExecutableLogEntryStep(stepDetail));
            break;

        case TA_Base_Core::RUN_PLAN_STEP:
            interactiveStep = std::auto_ptr<InteractiveStep>(new ExecutableRunPlanStep(stepDetail));
            break;

        case TA_Base_Core::LAUNCH_GUI_APP_STEP:
            interactiveStep = std::auto_ptr<InteractiveStep>(new ExecutableLaunchGuiAppStep(stepDetail));
            break;

        case TA_Base_Core::LAUNCH_SCHEMATIC_STEP:
            interactiveStep = std::auto_ptr<InteractiveStep>(new ExecutableLaunchSchematicStep(stepDetail));
            break;
        // lizettejl++
        case TA_Base_Core::FAILURE_SUMMARY_STEP:
            interactiveStep = std::auto_ptr<InteractiveStep>(new ExecutableFailureSummaryStep(stepDetail));
            break;
        // ++lizettejl

        default:
            TA_ASSERT(false, "Invalid interactive step type");
    }

    try
    {
        executeStep(*interactiveStep.get());
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        LOG(SourceInfo,    TA_Base_Core::DebugUtil::ExceptionConstruction,
            "TA_Base_Bus::IPlanManagerCorbaDef::InteractiveStepExecuteError", ex.what());

        throw TA_Base_Bus::IPlanManagerCorbaDef::InteractiveStepExecuteError(ex.what());
    }
    catch (...)
    {
        LOG(SourceInfo,    TA_Base_Core::DebugUtil::ExceptionConstruction,
            "TA_Base_Bus::IPlanManagerCorbaDef::InteractiveStepExecuteError", "Unknown exception");

        throw TA_Base_Bus::IPlanManagerCorbaDef::InteractiveStepExecuteError("Failed to launch interactive step.");
    }

    FUNCTION_EXIT;
}


void PlanManagerCorbaDefImpl::executeStep(InteractiveStep& interactiveStep)
{
    FUNCTION_ENTRY( "executeStep" );

    TreeNodeId planId = interactiveStep.getStepDetail().activePlan.plan;
    TreeNode *treeNode = TreeNodeFactory::getInstance().getTreeNode(planId);
    if (!treeNode)
    {
        treeNode = TreeNodeFactory::getInstance().createTreeNode(planId);
        TA_ASSERT(treeNode, "Tree node produced by factory is null");
    }

    PlanNode *planNode = static_cast<PlanNode *>(treeNode);
    TA_ASSERT(planNode, "Tree node produced by factory is not a plan");

    planNode->executeInteractiveStep(interactiveStep);

    FUNCTION_EXIT;
}


// Launching Issue, Yanrong++
bool PlanManagerCorbaDefImpl::ThreadRegisterPlanManager( unsigned long location )
{
    FUNCTION_ENTRY( "ThreadRegisterPlanManager" );

    try
    {
        // register manager
        PlanAgentAccess::getInstance().registerPlanManager(_this(), location);

        FUNCTION_EXIT;
        return true;
    }
    catch (const TA_Base_Core::TransactiveException&)
    {
        // do not report failed registration here - just keep trying
        // untill agent becomes available
    }

    FUNCTION_EXIT;
    return false;
}


void PlanManagerCorbaDefImpl::StartThreadRegisterPM( unsigned long location, unsigned long interval )
{
    FUNCTION_ENTRY( "StartThreadRegisterPM" );

    std::map<unsigned long, PlanRegisterThread*>::iterator itFound = m_registerThreads.find( location );
    if ( itFound == m_registerThreads.end() )
    {
        PlanManagerCorbaDefImpl::PlanRegisterThread* pThread = new PlanManagerCorbaDefImpl::PlanRegisterThread( this, location, interval );
        m_registerThreads.insert( std::pair<unsigned long, PlanRegisterThread*>(location, pThread) ).second;
        pThread->start();
    }
    else
    {
        itFound->second->reRegister();
    }

    FUNCTION_EXIT;
}


void PlanManagerCorbaDefImpl::CloseAllRegisterThread()
{
    FUNCTION_ENTRY( "CloseAllRegisterThread" );

    std::map< unsigned long, PlanRegisterThread* >::iterator iter = m_registerThreads.begin();
	long  nIndex=0;
    while( iter != m_registerThreads.end() )
    {
        PlanRegisterThread* thread = iter->second;
        //thread->terminateAndWait();
        //delete thread;
		thread->terminate();
        ++iter;
		LOG_GENERIC(SourceInfo,    TA_Base_Core::DebugUtil::DebugDebug,"PlanManager_Init Thread closethread nIndex = %d",nIndex++);
    }
	Sleep(5000);
    m_registerThreads.clear();

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////
// PlanRegisterThread
PlanManagerCorbaDefImpl::PlanRegisterThread::PlanRegisterThread( PlanManagerCorbaDefImpl* parent,
    unsigned long location, unsigned long interval ) : 
m_location( location ), 
m_interval( interval ), 
m_parent( parent ), 
m_terminated( false ), 
m_bRegistered( false ), 
m_waitObject()
{
    FUNCTION_ENTRY( "Constructor PlanRegisterThread" );

    TA_ASSERT( m_parent != NULL, "m_parent should not NULL" );

    FUNCTION_EXIT;
}


PlanManagerCorbaDefImpl::PlanRegisterThread::~PlanRegisterThread()
{
    FUNCTION_ENTRY( "Destructor PlanRegisterThread" );

    m_parent = NULL;

    FUNCTION_EXIT;
}


void PlanManagerCorbaDefImpl::PlanRegisterThread::run()
{
    FUNCTION_ENTRY( "run" );

    m_terminated = false;
    m_bRegistered = false;
    while( !m_terminated )
    {
		if(!m_bRegistered)
		{
			m_waitObject.timedWait(2000);
			m_bRegistered = m_parent->ThreadRegisterPlanManager(m_location);
			LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugDebug,"PlanManager_Init Thread continue Register failed, Location  = %d",m_location);
		}
		else
		{
			LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugDebug,"PlanManager_Init Thread Register success, Location  = %d",m_location);
			m_waitObject.wait();
		}
    }

	if(m_terminated && m_bRegistered)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "PlanManager_Init Thread  terminate Location =  %d ",m_location);
        try
        {
            PlanAgentAccess::getInstance().deregisterPlanManager(m_location);
        }
        catch (...)
        {
        	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "PlanManager_Init Thread  terminate Location =  %d fail",m_location);
        }
		
	}
    FUNCTION_EXIT;
}


void PlanManagerCorbaDefImpl::PlanRegisterThread::terminate()
{
    m_terminated = true;
	m_waitObject.signal();
}

void TA_Base_App::PlanManagerCorbaDefImpl::PlanRegisterThread::reRegister()
{
    m_bRegistered = false;
    m_terminated = false;
    m_waitObject.signal();
}