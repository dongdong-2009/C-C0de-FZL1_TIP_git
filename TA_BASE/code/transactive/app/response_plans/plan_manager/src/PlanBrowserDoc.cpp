//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanBrowserDoc.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// This is the base class for all documents that have interest in Plan Agent
// updates. To receive the updates, derived document classes must register
// with PlanAgentUpdateProcessor. Use subscribeForActivePlanUpdates() to
// register for plan execution updates and/or subscribeForPlanConfigUpdates()
// to register for plan configuration change updates.

#include "StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "PlanManager.h"
#include "PlanBrowserDoc.h"
#include "TreeNode.h"
#include "TreeNodeFactory.h"
#include "PlanInstance.h"
#include "PlanStep.h"
#include "PlanAgentAccess.h"
#include "MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const unsigned int MAX_SEND_UPDATE_ATTEMPTS = 5;

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanBrowserDoc

IMPLEMENT_DYNCREATE(CPlanBrowserDoc, CDocument)

BEGIN_MESSAGE_MAP(CPlanBrowserDoc, CDocument)
    // {{AFX_MSG_MAP(CPlanBrowserDoc)
    ON_COMMAND(ID_ACTIVE_PLAN_DETAIL_UPDATE, OnActivePlanDetailUpdate)
    ON_COMMAND(ID_PLAN_CONFIG_DATA_UPDATE, OnPlanConfigDataUpdate)
    ON_COMMAND(ID_PLAN_CUSTOMISATION_UPDATE, OnPlanCustomisationUpdate)
    ON_COMMAND(ID_RPARAM_EXECPLANID_UPDATE, OnRparamExecPlanIdUpdate)
    ON_COMMAND(ID_RPARAM_VIEWPLANID_UPDATE, OnRparamViewPlanIdUpdate)
    ON_COMMAND(ID_RPARAM_ASSOC_ALARM_UPDATE, OnRparamAssocAlarmIdUpdate)
    ON_COMMAND(ID_RPARAM_LOCKCATEGORY_UPDATE, OnRparamLockCategoryUpdate)
    ON_COMMAND(ID_RPARAM_SESSIONID_UPDATE, OnRparamSessionIdUpdate)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanBrowserDoc construction/destruction

CPlanBrowserDoc::CPlanBrowserDoc() :
    m_currentNode(NULL),
    m_parentFrameHWND(NULL)
{
    FUNCTION_ENTRY( "CPlanBrowserDoc" );

    m_lockCategoryPathNodes.clear();

    FUNCTION_EXIT;
}


CPlanBrowserDoc::~CPlanBrowserDoc()
{
    FUNCTION_ENTRY( "~CPlanBrowserDoc" );

    m_currentStepMap.clear();
    m_lockCategoryPathNodes.clear();

    while (!m_activePlanDetailUpdateQueue.empty())
    {
        delete m_activePlanDetailUpdateQueue.front();
        m_activePlanDetailUpdateQueue.pop_front();
    }

    while (!m_planConfigDataUpdateQueue.empty())
    {
        delete m_planConfigDataUpdateQueue.front();
        m_planConfigDataUpdateQueue.pop_front();
    }

    while (!m_planCustomisationUpdateQueue.empty())
    {
        delete m_planCustomisationUpdateQueue.front();
        m_planCustomisationUpdateQueue.pop_front();
    }

    FUNCTION_EXIT;
}


BOOL CPlanBrowserDoc::OnNewDocument()
{
    FUNCTION_ENTRY( "OnNewDocument" );

    if (!CDocument::OnNewDocument())
    {
        FUNCTION_EXIT;
        return FALSE;
    }


    // TODO: Add your specialized code here and/or call the base class
    POSITION pos = GetFirstViewPosition();
    if (pos != NULL)
    {
        CView* view = GetNextView(pos);
        if (view)
        {
            m_parentFrameHWND = view->GetParentFrame()->GetSafeHwnd();
        }
    }

    FUNCTION_EXIT;
    return TRUE;
}


void CPlanBrowserDoc::OnCloseDocument()
{
    FUNCTION_ENTRY( "OnCloseDocument" );

    // TODO: Add your specialized code here and/or call the base class
    CDocument::OnCloseDocument();

    FUNCTION_EXIT;
}


void CPlanBrowserDoc::OnActivePlanDetailUpdate()
{
    FUNCTION_ENTRY( "OnActivePlanDetailUpdate" );

    while (!m_activePlanDetailUpdateQueue.empty())
    {
        ActivePlanDetailUpdate* activePlanDetailUpdate = m_activePlanDetailUpdateQueue.front();
        m_activePlanDetailUpdateQueue.pop_front();

        if (activePlanDetailUpdate)
        {
            UpdateAllViews(NULL, 0, activePlanDetailUpdate);

            delete activePlanDetailUpdate;
            activePlanDetailUpdate = NULL;
        }
    }

    FUNCTION_EXIT;
}


void CPlanBrowserDoc::OnPlanConfigDataUpdate()
{
    FUNCTION_ENTRY( "OnPlanConfigDataUpdate" );

    while (!m_planConfigDataUpdateQueue.empty())
    {
        PlanConfigDataUpdate* planConfigDataUpdate = m_planConfigDataUpdateQueue.front();
        m_planConfigDataUpdateQueue.pop_front();

        if (planConfigDataUpdate)
        {
            // The path of the node is included as part of the title. When the node is moved the title needs to be updated.
            TA_Base_Core::PlanConfigUpdateData planConfigData = planConfigDataUpdate->GetUpdateData();

            if (planConfigData.configUpdate._d() == TA_Base_Core::PCUT_MOVE_PLAN ||
                planConfigData.configUpdate._d() == TA_Base_Core::PCUT_MOVE_PLAN_TO_ROOT ||
                planConfigData.configUpdate._d() == TA_Base_Core::PCUT_MOVE_PLAN_FROM_ROOT)
            {
                RefreshTitle();
            }

            UpdateAllViews(NULL, 0, planConfigDataUpdate);

            delete planConfigDataUpdate;
            planConfigDataUpdate = NULL;
        }
    }

    FUNCTION_EXIT;
}


void CPlanBrowserDoc::OnPlanCustomisationUpdate()
{
    FUNCTION_ENTRY( "OnPlanCustomisationUpdate" );

    while (!m_planCustomisationUpdateQueue.empty())
    {
        PlanCustomisationUpdate* planCustomisationUpdate = m_planCustomisationUpdateQueue.front();
        m_planCustomisationUpdateQueue.pop_front();

        if (planCustomisationUpdate)
        {
            UpdateAllViews(NULL, 0, planCustomisationUpdate);

            delete planCustomisationUpdate;
            planCustomisationUpdate = NULL;
        }
    }

    FUNCTION_EXIT;
}


void CPlanBrowserDoc::OnRparamExecPlanIdUpdate()
{
    FUNCTION_ENTRY( "OnRparamExecPlanIdUpdate" );

    UpdateAllViews(NULL, ID_RPARAM_EXECPLANID_UPDATE, NULL);

    FUNCTION_EXIT;
}


void CPlanBrowserDoc::OnRparamViewPlanIdUpdate()
{
    FUNCTION_ENTRY( "OnRparamViewPlanIdUpdate" );

    UpdateAllViews(NULL, ID_RPARAM_VIEWPLANID_UPDATE, NULL);

    FUNCTION_EXIT;
}


void CPlanBrowserDoc::OnRparamAssocAlarmIdUpdate()
{
    FUNCTION_ENTRY( "OnRparamAssocAlarmIdUpdate" );

    UpdateAllViews(NULL, ID_RPARAM_ASSOC_ALARM_UPDATE, NULL);

    FUNCTION_EXIT;
}


void CPlanBrowserDoc::OnRparamLockCategoryUpdate()
{
    FUNCTION_ENTRY( "OnRparamLockCategoryUpdate" );

    m_lockCategoryPathNodes.clear();

    UpdateAllViews(NULL, ID_RPARAM_LOCKCATEGORY_UPDATE, NULL);

    FUNCTION_EXIT;
}


void CPlanBrowserDoc::OnRparamSessionIdUpdate()
{
    FUNCTION_ENTRY( "OnRparamSessionIdUpdate" );

    m_lockCategoryPathNodes.clear();

    UpdateAllViews(NULL, ID_RPARAM_SESSIONID_UPDATE, NULL);

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// CPlanBrowserDoc diagnostics

#ifdef _DEBUG
void CPlanBrowserDoc::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CDocument::AssertValid();

    FUNCTION_EXIT;
}


void CPlanBrowserDoc::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CDocument::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CPlanBrowserDoc commands

// Is the specified node at or below the locked category
bool CPlanBrowserDoc::IsNodeAccessible(TreeNodeId nodeId)
{
    FUNCTION_ENTRY( "IsNodeAccessible" );

    if (!IsPlanTreeLocked())
    {
        FUNCTION_EXIT;
        return true;
    }

    TreeNodeId lockCategoryId = GetLockedCategoryId();

    if (nodeId == lockCategoryId)
    {
        FUNCTION_EXIT;
        return true;
    }

    TreeNodeIdList nodeIds;

    try
    {
        TreeNodeFactory::getInstance().getNodeIdsInNodePath(nodeId, nodeIds);
    }
    catch (TA_Base_Core::TransactiveException  &ex)
    {
        MessageBox::error(IDS_DETERMINE_IF_NODE_IS_ACCESSIBLE, ex);

        FUNCTION_EXIT;
        return false;
    }

    for (TreeNodeIdList::iterator iter = nodeIds.begin(); iter != nodeIds.end(); iter++)
    {
        if (*iter == lockCategoryId)
        {
            FUNCTION_EXIT;
            return true;
        }
    }

    FUNCTION_EXIT;
    return false;
}


// Is the specified node at or above the locked category
bool CPlanBrowserDoc::IsNodeOnLockedCategoryPath(TreeNodeId nodeId)
{
    FUNCTION_ENTRY( "IsNodeOnLockedCategoryPath" );

    if (!IsPlanTreeLocked())
    {
        FUNCTION_EXIT;
        return false;
    }

    TA_ASSERT(!m_lockCategoryPathNodes.empty(), "Invalid lock category path node ID list");

    for (TreeNodeIdList::iterator iter = m_lockCategoryPathNodes.begin(); iter != m_lockCategoryPathNodes.end(); iter++)
    {
        if (*iter == nodeId)
        {
            FUNCTION_EXIT;
            return true;
        }
    }

    FUNCTION_EXIT;
    return false;
}


bool CPlanBrowserDoc::IsPlanTreeLocked()
{
    FUNCTION_ENTRY( "IsPlanTreeLocked" );
    FUNCTION_EXIT;
    return (GetLockedCategoryId() != -1);
}


TreeNodeId CPlanBrowserDoc::GetLockedCategoryId()
{
    FUNCTION_ENTRY( "GetLockedCategoryId" );

    std::string lockCategoryPath = TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCKCATEGORY);

    if (lockCategoryPath.empty() || lockCategoryPath == "/")
    {
        m_lockCategoryPathNodes.clear();  // no lock
        FUNCTION_EXIT;
        return -1;
    }
    else if (m_lockCategoryPathNodes.empty())
    {
        try
        {
            TreeNodeId lockCategoryId = PlanAgentAccess::getInstance().getNodeId(lockCategoryPath);
            TreeNodeFactory::getInstance().getNodeIdsInNodePath(lockCategoryId, m_lockCategoryPathNodes);
        }
        catch (TA_Base_Core::TransactiveException  &ex)
        {
            std::ostringstream errorMessage;
            MessageBox::error(IDS_ENFORCE_RESTRICTION_ON_CATEGORY, ex);

            TA_Base_Core::RunParams::getInstance().set(RPARAM_LOCKCATEGORY, ""); // reset lock category
            FUNCTION_EXIT;
            return -1;
        }
    }

    FUNCTION_EXIT;
    return m_lockCategoryPathNodes[m_lockCategoryPathNodes.size() - 1];
}


void CPlanBrowserDoc::SetCurrentNode(TreeNode *node)
{
    FUNCTION_ENTRY( "SetCurrentNode" );

    m_currentNode = node;

    RefreshTitle();

    FUNCTION_EXIT;
}


TreeNode *CPlanBrowserDoc::GetCurrentNode() const
{
    FUNCTION_ENTRY( "GetCurrentNode" );
    FUNCTION_EXIT;
    return m_currentNode;
}


void CPlanBrowserDoc::SetCurrentInstance(PlanInstanceSharedPtr& instance)
{
    FUNCTION_ENTRY( "SetCurrentInstance" );

    m_currentInstance = instance;

    FUNCTION_EXIT;
}


PlanInstanceSharedPtr& CPlanBrowserDoc::GetCurrentInstance()
{
    FUNCTION_ENTRY( "GetCurrentInstance" );
    FUNCTION_EXIT;
    return m_currentInstance;
}


PlanInstanceFilter *CPlanBrowserDoc::GetPlanInstanceFilter()
{
    FUNCTION_ENTRY( "GetPlanInstanceFilter" );
    FUNCTION_EXIT;
    return &m_planInstanceFilter;
}


void CPlanBrowserDoc::AddToCurrentSteps(PlanStep *planStep)
{
    FUNCTION_ENTRY( "AddToCurrentSteps" );

    TA_ASSERT(planStep, "Plan step is null");

    CurrentStepMap::iterator iter = m_currentStepMap.find(planStep->getStepId());

    if (iter == m_currentStepMap.end())
    {
        m_currentStepMap.insert(std::make_pair(planStep->getStepId(), planStep));
    }

    FUNCTION_EXIT;
}


void CPlanBrowserDoc::RemoveFromCurrentSteps(PlanStep *planStep)
{
    FUNCTION_ENTRY( "RemoveFromCurrentSteps" );

    TA_ASSERT(planStep, "Plan step is null");

    CurrentStepMap::iterator iter = m_currentStepMap.find(planStep->getStepId());

    if (iter != m_currentStepMap.end())
    {
        m_currentStepMap.erase(iter);
    }

    FUNCTION_EXIT;
}


void CPlanBrowserDoc::PurgeCurrentSteps()
{
    FUNCTION_ENTRY( "PurgeCurrentSteps" );

    m_currentStepMap.clear();

    FUNCTION_EXIT;
}


PlanStep* CPlanBrowserDoc::GetCurrentStep()
{
    FUNCTION_ENTRY( "GetCurrentStep" );

    TA_ASSERT(m_currentStepMap.size() <= 1, "Multiple steps selected, cannot return a single current step");

    CurrentStepMap::iterator iter = m_currentStepMap.begin();

    if (iter != m_currentStepMap.end())
    {
        FUNCTION_EXIT;
        return iter->second;
    }

    FUNCTION_EXIT;
    return NULL;
}


PlanStepList CPlanBrowserDoc::GetCurrentSteps()
{
    FUNCTION_ENTRY( "GetCurrentSteps" );

    PlanStepList currentSteps;
    currentSteps.reserve(m_currentStepMap.size());

    for (CurrentStepMap::iterator iter = m_currentStepMap.begin(); iter != m_currentStepMap.end(); iter++)
    {
        currentSteps.push_back(iter->second);
    }

    FUNCTION_EXIT;
    return currentSteps;
}


bool CPlanBrowserDoc::EditingEnabled() const
{
    FUNCTION_ENTRY( "EditingEnabled" );
    FUNCTION_EXIT;
    return false;
}


bool CPlanBrowserDoc::ControlEnabled() const
{
    FUNCTION_ENTRY( "ControlEnabled" );
    FUNCTION_EXIT;
    return false;
}


void CPlanBrowserDoc::RefreshTitle()
{
    FUNCTION_ENTRY( "RefreshTitle" );

    std::string nodePath = "";
    if (GetCurrentNode())
    {
        try
        {
            nodePath = PlanAgentAccess::getInstance().getNodePath(GetCurrentNode()->getNodeId());
        }
        catch (...)
        {
            nodePath = m_currentNode->getNodeName();
        }
    }

    SetTitle(_T(nodePath.c_str()));

    FUNCTION_EXIT;
}


// IActivePlanObserver interface
void CPlanBrowserDoc::activeStateChanged(const TA_Base_Core::ActivePlanDetail &activePlanDetail)
{
    FUNCTION_ENTRY( "activeStateChanged" );

    if (m_parentFrameHWND)
    {
        m_activePlanDetailUpdateQueue.push_back(new ActivePlanDetailUpdate(activePlanDetail));

        for (unsigned int attempts = 1; attempts <= MAX_SEND_UPDATE_ATTEMPTS; attempts++)
        {
            // The frame does not handle this command. It will be routed back to the document in a thread safe manner.
            if (::PostMessage(m_parentFrameHWND, WM_COMMAND, MAKEWPARAM(ID_ACTIVE_PLAN_DETAIL_UPDATE, 0), 0))
            {
                FUNCTION_EXIT;
                return;
            }

            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
                "Failed to post active plan detail update, retrying...");
        }

        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
            "Unable to post active plan detail update after %u attempts. Update remains queued.",
            MAX_SEND_UPDATE_ATTEMPTS);
    }

    FUNCTION_EXIT;
}


// IPlanConfigObserver interface
void CPlanBrowserDoc::planConfigChanged(const TA_Base_Core::PlanConfigUpdateData &planConfigData)
{
    FUNCTION_ENTRY( "planConfigChanged" );

    if (m_parentFrameHWND)
    {
        m_planConfigDataUpdateQueue.push_back(new PlanConfigDataUpdate(planConfigData));

        for (unsigned int attempts = 1; attempts <= MAX_SEND_UPDATE_ATTEMPTS; attempts++)
        {
            // The frame does not handle this command. It will be routed back to the document in a thread safe manner.
            if (::PostMessage(m_parentFrameHWND, WM_COMMAND, MAKEWPARAM(ID_PLAN_CONFIG_DATA_UPDATE, 0), 0))
            {
                FUNCTION_EXIT;
                return;
            }

            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
                "Failed to post plan config data update, retrying...");
        }

        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
            "Unable to post plan config data update after %u attempts. Update remains queued.",
            MAX_SEND_UPDATE_ATTEMPTS);
    }

    FUNCTION_EXIT;
}


void CPlanBrowserDoc::planTreeNodeDeleted(const TA_Base_Core::PlanConfigUpdateData &planConfigData)
{
    FUNCTION_ENTRY( "planTreeNodeDeleted" );

    if (m_parentFrameHWND)
    {
        // After this function returns, the node is going to be retired from the TreeNodeFactory,
        // so it is important that the update be processed synchronously. Use SendMessage() rather
        // than PostMessage().
        PlanConfigDataUpdate* planConfigDataUpdate = new PlanConfigDataUpdate(planConfigData);
        m_planConfigDataUpdateQueue.push_back(planConfigDataUpdate);

        for (unsigned int attempts = 1; attempts <= MAX_SEND_UPDATE_ATTEMPTS; attempts++)
        {
            // The frame does not handle this command. It will be routed back to the document in a thread safe manner.
            if (::SendMessage(m_parentFrameHWND, WM_COMMAND, MAKEWPARAM(ID_PLAN_CONFIG_DATA_UPDATE, 0), 0))
            {
                FUNCTION_EXIT;
                return;
            }

            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
                "Failed to send plan node deletion update, retrying...");
        }

        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
            "Unable to send plan node deletion update after %u attempts. Update discarded.",
            MAX_SEND_UPDATE_ATTEMPTS);

        // Update not processed. Remove it from the queue and destroy.
        m_planConfigDataUpdateQueue.erase(planConfigDataUpdate);

        delete planConfigDataUpdate;
        planConfigDataUpdate = NULL;
    }

    FUNCTION_EXIT;
}


// IPlanCustomisationObserver interface
void CPlanBrowserDoc::planCustomised(const TA_Base_Core::PlanCustomisationUpdate &planCustomDetail)
{
    FUNCTION_ENTRY( "planCustomised" );

    if (m_parentFrameHWND)
    {
        m_planCustomisationUpdateQueue.push_back(new PlanCustomisationUpdate(planCustomDetail));

        for (unsigned int attempts = 1; attempts <= MAX_SEND_UPDATE_ATTEMPTS; attempts++)
        {
            // The frame does not handle this command. It will be routed back to the document in a thread safe manner.
            if (::PostMessage(m_parentFrameHWND, WM_COMMAND, MAKEWPARAM(ID_PLAN_CUSTOMISATION_UPDATE, 0), 0))
            {
                FUNCTION_EXIT;
                return;
            }

            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
                "Failed to post plan customisation update, retrying...");
        }

        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
            "Unable to post plan customisation update after %u attempts. Update remains queued.",
            MAX_SEND_UPDATE_ATTEMPTS);
    }

    FUNCTION_EXIT;
}


// RunParamUser interface
void CPlanBrowserDoc::onRunParamChange(const std::string& name, const std::string& value)
{
    FUNCTION_ENTRY( "onRunParamChange" );

    if (m_parentFrameHWND)
    {
        if (name == RPARAM_FWDEXECPLANID)
        {
            // The frame does not handle this command. It will be routed back to the document in a thread safe manner.
            ::PostMessage(m_parentFrameHWND, WM_COMMAND, MAKEWPARAM(ID_RPARAM_EXECPLANID_UPDATE, 0), 0);
        }
        else if (name == RPARAM_FWDVIEWPLANID)
        {
            // The frame does not handle this command. It will be routed back to the document in a thread safe manner.
            ::PostMessage(m_parentFrameHWND, WM_COMMAND, MAKEWPARAM(ID_RPARAM_VIEWPLANID_UPDATE, 0), 0);
        }
        else if (name == RPARAM_LOCKCATEGORY)
        {
            // The frame does not handle this command. It will be routed back to the document in a thread safe manner.
            ::PostMessage(m_parentFrameHWND, WM_COMMAND, MAKEWPARAM(ID_RPARAM_LOCKCATEGORY_UPDATE, 0), 0);
        }
        else if (name == RPARAM_SESSIONID)
        {
            // The frame does not handle this command. It will be routed back to the document in a thread safe manner.
            ::PostMessage(m_parentFrameHWND, WM_COMMAND, MAKEWPARAM(ID_RPARAM_SESSIONID_UPDATE, 0), 0);
        }
          else if (name == RPARAM_FWDASSOCALARMID)
          {
              // The frame does not handle this command. It will be routed back to the document in a thread safe manner.
              ::PostMessage(m_parentFrameHWND, WM_COMMAND, MAKEWPARAM(ID_RPARAM_ASSOC_ALARM_UPDATE, 0), 0);
          }
    }

    FUNCTION_EXIT;
}


