/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/FloatingWindowThreadRegister.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Activates and stores Plan Controller and Plan Editor threads.
  *
  **/

#include "StdAfx.h"

#if defined(_MSC_VER)
    #pragma warning(disable:4250)
#endif // defined (_MSC_VER)

#include "core/utilities/src/TAAssert.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "Resource.h"
#include "FloatingWindowThreadRegister.h"
#include "FloatingWindowThread.h"
#include "PlanControllerThread.h"
#include "PlanEditorThread.h"
#include "PlanAgentAccess.h"
#include "TreeNodeFactory.h"
#include "TreeNode.h"
#include "MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const unsigned int PLAN_CONTROLLER_LIMIT = 10;  // Up to 10 Plan Controllers can be open at a time
static const unsigned int PLAN_EDITOR_LIMIT = 3;       // Up to 3 Plan Editors can be open at a time

TA_Base_App::FloatingWindowThreadRegister *TA_Base_App::FloatingWindowThreadRegister::s_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable TA_Base_App::FloatingWindowThreadRegister::s_singletonLock;

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// FloatingWindowThreadRegister

FloatingWindowThreadRegister::FloatingWindowThreadRegister() :
    m_deregisterEvent(FALSE, FALSE)
{
    FUNCTION_ENTRY( "FloatingWindowThreadRegister" );
    FUNCTION_EXIT;
}


FloatingWindowThreadRegister::~FloatingWindowThreadRegister()
{
    FUNCTION_ENTRY( "~FloatingWindowThreadRegister" );

    terminateAllThreads();

    m_deregisterEvent.Unlock();

    s_instance = NULL;

    FUNCTION_EXIT;
}


FloatingWindowThreadRegister &FloatingWindowThreadRegister::getInstance()
{
    FUNCTION_ENTRY( "getInstance" );

    if (!s_instance)
    {
        TA_Base_Core::ThreadGuard guard(s_singletonLock);

        if (!s_instance)
        {
            s_instance = new FloatingWindowThreadRegister();
        }
    }

    FUNCTION_EXIT;
    return *s_instance;
}


void FloatingWindowThreadRegister::destroyInstance()
{
    FUNCTION_ENTRY( "destroyInstance" );

    TA_Base_Core::ThreadGuard guard(s_singletonLock);

    delete s_instance;

    FUNCTION_EXIT;
}


CPlanControllerThread *FloatingWindowThreadRegister::createPlanControllerThread(TreeNodeId planId, bool ignoreActivePlanControllerLimit)
{
    FUNCTION_ENTRY( "createPlanControllerThread" );

    // See if a Plan Controller is already running for the specified plan. If not, create a new one.
    CPlanControllerThread *planControllerThread = getPlanControllerThread(planId);

    if (!planControllerThread)
    {
        // Limit the number of Plan Controllers that can be active at any one time.
        // The caller may request to ignore the limit, e.g. when Plan Controller is being launched
        // for the purpose of executing an interactive step.
        if (m_planControllerThreadMap.size() >= PLAN_CONTROLLER_LIMIT && !ignoreActivePlanControllerLimit)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UW_210014);

            FUNCTION_EXIT;
            return NULL;
        }

        planControllerThread = new CPlanControllerThread(planId);
        planControllerThread->CreateThread();

        m_planControllerThreadMap.insert(planId, planControllerThread);
    }
    else
    {
        // Bring existing Plan Controller to the foreground
        planControllerThread->ActivateWindow();
    }

    FUNCTION_EXIT;
    return planControllerThread;
}


CPlanEditorThread *FloatingWindowThreadRegister::createPlanEditorThread(TreeNodeId planId)
{
    FUNCTION_ENTRY( "createPlanEditorThread" );

    // See if a Plan Editor is already running for the specified plan. If not, create a new one.
    CPlanEditorThread *planEditorThread = getPlanEditorThread(planId);

    if (!planEditorThread)
    {
        // Limit the number of Plan Editors that can be active at any one time.
        if (m_planEditorThreadMap.size() >= PLAN_EDITOR_LIMIT)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UW_210021);

            FUNCTION_EXIT;
            return NULL;
        }

        try
        {
            // Before the Plan Editor is launched we have to obtain an editing lock.
            PlanAgentAccess::getInstance().getPlanEditingLock(planId);
        }
        catch (const TA_Base_Core::TransactiveException &ex)
        {
            MessageBox::error(IDS_LAUNCH_PLAN_EDITOR, ex);

            FUNCTION_EXIT;
            return NULL;
        }

        planEditorThread = new CPlanEditorThread(planId);
        planEditorThread->CreateThread();

        m_planEditorThreadMap.insert(planId, planEditorThread);
    }
    else
    {
        // Bring existing Plan Editor to the foreground
        planEditorThread->ActivateWindow();
    }

    FUNCTION_EXIT;
    return planEditorThread;
}


CPlanControllerThread *FloatingWindowThreadRegister::getPlanControllerThread(TreeNodeId planId)
{
    FUNCTION_ENTRY( "getPlanControllerThread" );

    if (m_planControllerThreadMap.find(planId))
    {
        FUNCTION_EXIT;
        return m_planControllerThreadMap[planId];
    }

    FUNCTION_EXIT;
    return NULL;
}


CPlanEditorThread *FloatingWindowThreadRegister::getPlanEditorThread(TreeNodeId planId)
{
    FUNCTION_ENTRY( "getPlanEditorThread" );

    if (m_planEditorThreadMap.find(planId))
    {
        FUNCTION_EXIT;
        return m_planEditorThreadMap[planId];
    }

    FUNCTION_EXIT;
    return NULL;
}


std::vector<CPlanControllerThread *> FloatingWindowThreadRegister::getRegisteredPlanControllerThreads()
{
    FUNCTION_ENTRY( "getRegisteredPlanControllerThreads" );

    std::vector<CPlanControllerThread *> planControllerThreads;

    m_planControllerThreadMap.getItems(planControllerThreads);

    FUNCTION_EXIT;
    return planControllerThreads;
}


std::vector<CPlanEditorThread *> FloatingWindowThreadRegister::getRegisteredPlanEditorThreads()
{
    FUNCTION_ENTRY( "getRegisteredPlanEditorThreads" );

    std::vector<CPlanEditorThread *> planEditorThreads;

    m_planEditorThreadMap.getItems(planEditorThreads);

    FUNCTION_EXIT;
    return planEditorThreads;
}


void FloatingWindowThreadRegister::terminateAllThreads()
{
    FUNCTION_ENTRY( "terminateAllThreads" );

    // Ask all registered Plan Controllers to exit.
    typedef std::vector<CPlanControllerThread *> PlanControllerThreadList;
    PlanControllerThreadList planControllerThreads = getRegisteredPlanControllerThreads();

    for (PlanControllerThreadList::iterator pcIter = planControllerThreads.begin(); pcIter != planControllerThreads.end(); pcIter++)
    {
        CPlanControllerThread *planControllerThread = *pcIter;

        planControllerThread->Terminate();
    }

    // Ask all registered Plan Editors to exit.
    typedef std::vector<CPlanEditorThread *> PlanEditorThreadList;
    PlanEditorThreadList planEditorThreads = getRegisteredPlanEditorThreads();

    for (PlanEditorThreadList::iterator peIter = planEditorThreads.begin(); peIter != planEditorThreads.end(); peIter++)
    {
        CPlanEditorThread *planEditorThread = *peIter;

        // In case changes have been made to the plan, bring up the Plan Editor so that the
        // "Save changes?" message is visible.
        planEditorThread->ActivateWindow();
        planEditorThread->Terminate();
    }

    // Wait for all threads to deregister.
    while (!m_planControllerThreadMap.empty() || !m_planEditorThreadMap.empty())
    {
        CSingleLock deregisterEventLock(&m_deregisterEvent);
        deregisterEventLock.Lock();
    }

    FUNCTION_EXIT;
}


void FloatingWindowThreadRegister::deregisterThread(CFloatingWindowThread *floatingWindowThread)
{
    FUNCTION_ENTRY( "deregisterThread" );

    TA_ASSERT(floatingWindowThread != NULL, "Floating window thread is NULL");

    // First see whether the thread is a registered Plan Controller thread.
    typedef std::vector<CPlanControllerThread *> PlanControllerThreadList;
    PlanControllerThreadList planControllerThreads = getRegisteredPlanControllerThreads();

    for (PlanControllerThreadList::iterator pcIter = planControllerThreads.begin(); pcIter != planControllerThreads.end(); pcIter++)
    {
        CPlanControllerThread *planControllerThread = *pcIter;

        if (planControllerThread == floatingWindowThread)
        {
            // Remove the thread from the relevant map.
            m_planControllerThreadMap.erase(planControllerThread->GetNodeId());

            // Indicate that the thread has deregistered.
            m_deregisterEvent.SetEvent();

            FUNCTION_EXIT;
            return;
        }
    }

    // Not a Plan Controller thread. Should be a Plan Editor thread.
    typedef std::vector<CPlanEditorThread *> PlanEditorThreadList;
    PlanEditorThreadList planEditorThreads = getRegisteredPlanEditorThreads();

    for (PlanEditorThreadList::iterator peIter = planEditorThreads.begin(); peIter != planEditorThreads.end(); peIter++)
    {
        CPlanEditorThread *planEditorThread = *peIter;

        if (planEditorThread == floatingWindowThread)
        {
            try
            {
                // Release the editing lock on the plan
                PlanAgentAccess::getInstance().releasePlanEditingLock(planEditorThread->GetNodeId());
            }
            catch (const TA_Base_Core::TransactiveException &ex)
            {
                MessageBox::error(IDS_RELEASE_LOCK, ex);
            }

            // Remove the thread from the relevant map.
            m_planEditorThreadMap.erase(planEditorThread->GetNodeId());

            // Indicate that the thread has deregistered.
            m_deregisterEvent.SetEvent();

            FUNCTION_EXIT;
            return;
        }
    }

    TA_ASSERT(false, "Specified floating window thread is not registered");

    FUNCTION_EXIT;
}


