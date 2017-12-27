/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/ApprovedPlanTreeView.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/08/03 17:02:56 $
  * Last modified by:  $Author: qi.huang $
  *
  * <description>
  *
  **/

#include "StdAfx.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/TransactiveException.h"
#include "PlanManager.h"
#include "ApprovedPlanTreeView.h"
#include "ApprovedPlanDoc.h"
#include "TreeNode.h"
#include "PlanNode.h"
#include "TreeNodeFactory.h"
#include "MessageBox.h"
#include "PlanManagerCommonDefs.h"
#include "PlanAgentAccess.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"

// CL 15087 (2) ++
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/types/AlarmAudit_MessageTypes.h"
// ++ CL 15087 (2) 

#include "PlanControllerThread.h"
#include "FloatingWindowThreadRegister.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CApprovedPlanTreeView

IMPLEMENT_DYNCREATE(CApprovedPlanTreeView, CPlanTreeView)

CApprovedPlanTreeView::CApprovedPlanTreeView()
{
    FUNCTION_ENTRY( "CApprovedPlanTreeView" );
    FUNCTION_EXIT;
}


CApprovedPlanTreeView::~CApprovedPlanTreeView()
{
    FUNCTION_ENTRY( "~CApprovedPlanTreeView" );

	for(std::size_t i = 0; i < m_vecControlPlan.size(); ++i)
	{
		delete m_vecControlPlan[i];
		m_vecControlPlan[i] = NULL;
	}

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CApprovedPlanTreeView, CPlanTreeView)
    // {{AFX_MSG_MAP(CApprovedPlanTreeView)
    ON_NOTIFY_REFLECT(TVN_GETDISPINFO, OnGetDispInfo)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CApprovedPlanTreeView diagnostics

#ifdef _DEBUG
void CApprovedPlanTreeView::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CTreeView::AssertValid();

    FUNCTION_EXIT;
}


void CApprovedPlanTreeView::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CTreeView::Dump(dc);

    FUNCTION_EXIT;
}


CApprovedPlanDoc* CApprovedPlanTreeView::GetDocument() // non-debug version is inline
{
    FUNCTION_ENTRY( "GetDocument" );

    CPlanTreeDoc *document = CPlanTreeView::GetDocument();
    ASSERT(document->IsKindOf(RUNTIME_CLASS(CApprovedPlanDoc)));

    FUNCTION_EXIT;
    return (CApprovedPlanDoc*)document;
}


#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CApprovedPlanTreeView message handlers

void CApprovedPlanTreeView::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnGetDispInfo" );

    CPlanTreeView::OnGetDispInfo(pNMHDR, pResult);

    TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

    if ((pTVDispInfo->item.mask & TVIF_CHILDREN) == TVIF_CHILDREN)
    {
        TreeNode *treeNode = reinterpret_cast<TreeNode *>(pTVDispInfo->item.lParam);
        TA_ASSERT(treeNode, "Tree node pointer is null");

        pTVDispInfo->item.cChildren = treeNode->getNodeSummary().hasApprovedDescendant? 1 : 0;

        // If the node does not have any approved descendants, it may still need to be shown
        // as having children (i.e. be expandable). This applies when a category lock is
        // active and the node lies on the path to the locked category. The locked category
        // itself should not be flagged as having children if it does not have approved
        // descendants. What this achieves is it enables the locked category to be added to
        // the approved plan tree so that the user can be tied to it.
        if (!pTVDispInfo->item.cChildren)
        {
            CApprovedPlanDoc *planDoc = GetDocument();

            if (planDoc->IsPlanTreeLocked() &&
                planDoc->GetLockedCategoryId() != treeNode->getNodeId() &&
                planDoc->IsNodeOnLockedCategoryPath(treeNode->getNodeId()))
            {
                pTVDispInfo->item.cChildren = 1;
            }
        }
    }

    *pResult = 0;

    FUNCTION_EXIT;
}


void CApprovedPlanTreeView::ProcessRparamExecPlanIdUpdate()
{
    FUNCTION_ENTRY( "ProcessRparamExecPlanIdUpdate" );

    // Activate the approved plan view and populate the root node if necessary.
    AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_APPROVEDPLANS, 0), 0);

    // The target plan is in the format of a plan ID or plan path, as per the following:
    // 1. <plan ID>[:<instance #>:<location key>]
    // 2. [/<category 1>/<category N>]/<plan name>
    std::string execPlan(TA_Base_Core::RunParams::getInstance().get(RPARAM_FWDEXECPLANID));
	
	// CL 15087 (2) ++
	// Alarm Manager will send AlarmDescription as --assoc-alarm-id
	// Wrong: this parameter will active another function
	//std::string alarmDetail(TA_Base_Core::RunParams::getInstance().get(RPARAM_ASSOC_ALARMID));
	// ++ CL 15087 (2)

	// CL 15087 (2) ++
	// we use AlarmPlanStateUpdate message
	std::auto_ptr<TA_Base_Core::AuditMessageSender> messageSender(TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(
		TA_Base_Core::AlarmAudit::Context));
	// get entity key
	std::auto_ptr<TA_Base_Core::IEntityData> planManagerEntityData(TA_Base_Core::EntityAccessFactory::getInstance().getEntity("PlanManager"));
	unsigned long entityKey = planManagerEntityData->getKey();
	// get session
	std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
	TA_Base_Core::DescriptionParameters dp;
	std::ostringstream strOut;
	strOut << "DSS Run for " << execPlan;
	TA_Base_Core::NameValuePair alarmDesc("alarmDescription", strOut.str().c_str());
	// ++ CL 15087 (2) 


    TreeNodeId planNodeId = -1;
    PlanInstanceNumber instanceNumber = -1;
    LocationKey location = -1;
    std::string planPath;

    if (execPlan.find_first_not_of(" :0123456789") == std::string::npos)
    {
        std::istringstream execPlanStrm(execPlan);
        char colon;

        execPlanStrm >> planNodeId >> colon >> instanceNumber >> colon >> location;
    }
    else
    {
        planPath = execPlan;
          if (location = -1)
          {
              location = TA_Base_Bus::PlanAgentLocationAccess::getInstance().getDefaultPlanAgentLocation();
          }
    }

    // Verify the plan ID, instance and location key
    if (planPath.empty() && (planNodeId == -1 ||
                             instanceNumber == -1 && location != -1 ||
                             instanceNumber != -1 && location == -1))
    {
        std::ostringstream errorMessage;
        errorMessage << "Error parsing runtime parameter --exec-plan-id." << std::endl
                     << "Invalid plan node ID '" << execPlan << "'." << std::endl
                     << "Plan Controller cannot be launched.";

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.str().c_str());

        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << execPlan;
        userMsg.showMsgBox(IDS_UE_210097);
		// CL 15087 (2) ++
		// if the DSS Run plan controller was launched failed, we send 
		TA_Base_Core::NameValuePair alarmValue("Value", "FAIL");
		dp.clear();
		dp.push_back(&alarmValue);
		dp.push_back(&alarmDesc);	
		// DSS Run for Failed message (without asset)
		messageSender->sendAuditMessage(TA_Base_Core::AlarmAudit::AlarmPlanStateUpdated,
			entityKey, dp, "", session, "", "", "");
		// ++ CL 15087 (2)

        FUNCTION_EXIT;
        return;
    }

    // Locate the node in the tree, expanding branches along the way
    HTREEITEM nodeItem = NULL;

    if (planPath.empty())
    {
        nodeItem = FindNode(planNodeId, true);
        if (!nodeItem)
        {
            std::ostringstream errorMessage;
            errorMessage << "Failed to locate node ID '" << planNodeId << "' in the approved plan tree." << std::endl <<
                            "Plan Controller cannot be launched.";

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.str().c_str());

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << planNodeId;
            userMsg.showMsgBox(IDS_UE_210043);
			// CL 15087 (2) ++
			// if the DSS Run plan controller was launched failed, we send 
			TA_Base_Core::NameValuePair alarmValue("Value", "FAIL");
			dp.clear();
			dp.push_back(&alarmValue);
			dp.push_back(&alarmDesc);	
			// DSS Run for Failed message (without asset)
			messageSender->sendAuditMessage(TA_Base_Core::AlarmAudit::AlarmPlanStateUpdated,
				entityKey, dp, "", session, "", "", "");	
			// ++ CL 15087 (2)
            return;
        }
    }
    else
    {
        nodeItem = FindNode(planPath, true);
        if (!nodeItem)
        {
            std::ostringstream errorMessage;
            errorMessage << "Failed to locate node '" << planPath << "' in the approved plan tree." << std::endl <<
                            "Plan Controller cannot be launched.";

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.str().c_str());

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << planPath;
                userMsg.showMsgBox(IDS_UE_210116);
			// CL 15087 (2) ++
			// if the DSS Run plan controller was launched failed, we send 
			TA_Base_Core::NameValuePair alarmValue("Value", "FAIL");
			dp.clear();
			dp.push_back(&alarmValue);
			dp.push_back(&alarmDesc);	
			// DSS Run for Failed message (without asset)
			messageSender->sendAuditMessage(TA_Base_Core::AlarmAudit::AlarmPlanStateUpdated,
				entityKey, dp, "", session, "", "", "");	
			// ++ CL 15087 (2)
            return;
        }
    }

    // Select the node in the plan tree.
    GetTreeCtrl().SelectItem(nodeItem);

    // Verify the node is in fact a plan node
    TreeNode *treeNode = reinterpret_cast<TreeNode *>(GetTreeCtrl().GetItemData(nodeItem));
    PlanNode *planNode = dynamic_cast<PlanNode *>(treeNode);
    if (!planNode)
    {
        std::ostringstream errorMessage;
        errorMessage << "Node ID " << treeNode->getNodeId() << " (" << treeNode->getNodeName() << ") is not a plan type node." << std::endl
                     << "Plan Controller cannot be launched.";

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.str().c_str());

        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << treeNode->getNodeId() << treeNode->getNodeName();
        userMsg.showMsgBox(IDS_UE_210044);
		// CL 15087 (2) ++
		// if the DSS Run plan controller was launched failed, we send 
		TA_Base_Core::NameValuePair alarmValue("Value", "FAIL");
		dp.clear();
		dp.push_back(&alarmValue);
		dp.push_back(&alarmDesc);	
		// DSS Run for Failed message (without asset)
		messageSender->sendAuditMessage(TA_Base_Core::AlarmAudit::AlarmPlanStateUpdated,
			entityKey, dp, "", session, "", "", "");	
		// ++ CL 15087 (2)
        return;
    }

    try
    {
        CWaitCursor waitCursor;
	
		ControlPlanThread* pControlPlan = 
			new ControlPlanThread(planNode, location, instanceNumber, execPlan, true);
		pControlPlan->start();
		m_vecControlPlan.push_back(pControlPlan);
    }
    catch (...)
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << planNode->getNodeName();
        userMsg.showMsgBox(IDS_UE_210045);

		// CL 15087 (2) ++
		// if the DSS Run plan controller was launched failed, we send 
		TA_Base_Core::NameValuePair alarmValue("Value", "FAIL");
		dp.clear();
		dp.push_back(&alarmValue);
		dp.push_back(&alarmDesc);	
		// DSS Run for Failed message (without asset)
		messageSender->sendAuditMessage(TA_Base_Core::AlarmAudit::AlarmPlanStateUpdated,
			entityKey, dp, "", session, "", "", "");	
		// ++ CL 15087 (2)
    }

    FUNCTION_EXIT;
}


void CApprovedPlanTreeView::ProcessRparamViewPlanIdUpdate()
{
    FUNCTION_ENTRY( "ProcessRparamViewPlanIdUpdate" );

    // Activate the approved plan view and populate the root node if necessary.
    AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_APPROVEDPLANS, 0), 0);

    // The target plan is in the format of a plan ID or plan path, as per the following:
    // 1. <plan ID>[:<instance #>:<location key>]
    // 2. [/<category 1>/<category N>]/<plan name>
    std::string viewPlan(TA_Base_Core::RunParams::getInstance().get(RPARAM_FWDVIEWPLANID));

    TreeNodeId planNodeId = -1;
    PlanInstanceNumber instanceNumber = -1;
    LocationKey location = -1;
    std::string planPath;

    if (viewPlan.find_first_not_of(" :0123456789") == std::string::npos)
    {
        std::istringstream viewPlanStrm(viewPlan);
        char colon;

        viewPlanStrm >> planNodeId >> colon >> instanceNumber >> colon >> location;
    }
    else
    {
        planPath = viewPlan;
          if (location = -1)
          {
              location = TA_Base_Bus::PlanAgentLocationAccess::getInstance().getDefaultPlanAgentLocation();
          }
    }

    // Verify the plan ID, instance and location key
    if (planPath.empty() && (planNodeId == -1 ||
                             instanceNumber == -1 && location != -1 ||
                             instanceNumber != -1 && location == -1))
    {
        std::ostringstream errorMessage;
        errorMessage << "Error parsing runtime parameter --view-plan-id." << std::endl
                     << "Invalid plan node ID '" << viewPlan << "'." << std::endl
                     << "Plan Controller cannot be launched.";

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.str().c_str());

        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << viewPlan;
        userMsg.showMsgBox(IDS_UE_210097);

        FUNCTION_EXIT;
        return;
    }

    // Locate the node in the tree, expanding branches along the way
    HTREEITEM nodeItem = NULL;

    if (planPath.empty())
    {
        nodeItem = FindNode(planNodeId, true);
        if (!nodeItem)
        {
            std::ostringstream errorMessage;
            errorMessage << "Failed to locate node ID '" << planNodeId << "' in the approved plan tree." << std::endl <<
                            "Plan Controller cannot be launched.";

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.str().c_str());

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << planNodeId;
            userMsg.showMsgBox(IDS_UE_210043);

            return;
        }
    }
    else
    {
        nodeItem = FindNode(planPath, true);
        if (!nodeItem)
        {
            std::ostringstream errorMessage;
            errorMessage << "Failed to locate node '" << planPath << "' in the approved plan tree." << std::endl <<
                            "Plan Controller cannot be launched.";

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.str().c_str());

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << planPath;
                userMsg.showMsgBox(IDS_UE_210116);

            return;
        }
    }

    // Select the node in the plan tree.
    GetTreeCtrl().SelectItem(nodeItem);

    // Verify the node is in fact a plan node
    TreeNode *treeNode = reinterpret_cast<TreeNode *>(GetTreeCtrl().GetItemData(nodeItem));
    PlanNode *planNode = dynamic_cast<PlanNode *>(treeNode);
    if (!planNode)
    {
        std::ostringstream errorMessage;
        errorMessage << "Node ID " << treeNode->getNodeId() << " (" << treeNode->getNodeName() << ") is not a plan type node." << std::endl
                     << "Plan Controller cannot be launched.";

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.str().c_str());

        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << treeNode->getNodeId() << treeNode->getNodeName();
        userMsg.showMsgBox(IDS_UE_210044);

        return;
    }

    try
    {
        CWaitCursor waitCursor;
	
		ControlPlanThread* pControlPlan = 
			new ControlPlanThread(planNode, location, instanceNumber, viewPlan, false);
		pControlPlan->start();
		m_vecControlPlan.push_back(pControlPlan);
    }
    catch (...)
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << planNode->getNodeName();
        userMsg.showMsgBox(IDS_UE_210045);
    }

    FUNCTION_EXIT;
}


void CApprovedPlanTreeView::ProcessRparamAssocAlarmIdUpdate ()
{
    FUNCTION_ENTRY( "ProcessRparamAssocAlarmIdUpdate" );

    // Activate the approved plan view and populate the root node if necessary.
    AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_APPROVEDPLANS, 0), 0);

    // get associated alarm id parameter
    std::string assocAlarmID(TA_Base_Core::RunParams::getInstance().get(RPARAM_ASSOC_ALARMID));

    // Verify the plan ID, instance and location key
    if (assocAlarmID.empty())
    {
        std::ostringstream errorMessage;
        errorMessage << "Error parsing runtime parameter --assoc-alarm-id." << std::endl
            << "Invalid associated alarm ID '" << assocAlarmID << "'." << std::endl
            << "Plan Controller cannot be launched.";

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.str().c_str());

        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << assocAlarmID;
        userMsg.showMsgBox(IDS_UE_210097);

        FUNCTION_EXIT;
        return;
    }

    // Get alarm id and location key
    const char cDelimiter = '@';
    std::stringstream stmAlarmIdAtLoc;
    stmAlarmIdAtLoc << assocAlarmID;
    std::string strAlarmId = "";
    std::getline( stmAlarmIdAtLoc, strAlarmId, cDelimiter );
    std::stringstream stmConvertor;
    std::string strLocKey = "";
    std::getline( stmAlarmIdAtLoc, strLocKey, cDelimiter );
    stmConvertor << strLocKey;
    unsigned long ulLocationKey = 0u;
    stmConvertor >> ulLocationKey;

    // The target plan must be obtained from associated alarm id (done by plan agent)
    TA_Base_Core::ActivePlanDetail_var activePlanDetailVar;
    try
    {
        activePlanDetailVar = PlanAgentAccess::getInstance().getAssocAlarmPlanDetail ( strAlarmId, ulLocationKey );
    }
    catch (PlanNotFoundException &ex)
    {
        AfxMessageBox( ex.what() );

        FUNCTION_EXIT;
        return;
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_RETRIEVE_PLAN, ex);

        FUNCTION_EXIT;
        return;
    }

    TreeNodeId planNodeId = activePlanDetailVar->plan.plan;
    PlanInstanceNumber instanceNumber = activePlanDetailVar->plan.instance;
    LocationKey location = activePlanDetailVar->plan.location;

    // once we have these the rest is pretty much as per ProcessRparamExecPlanIdUpdate
        // Locate the node in the tree, expanding branches along the way
    HTREEITEM nodeItem = NULL;

    nodeItem = FindNode(planNodeId, true);
    if (!nodeItem)
    {
        std::ostringstream errorMessage;
        errorMessage << "Failed to locate node ID '" << planNodeId << "' in the approved plan tree." << std::endl <<
            "Plan Controller cannot be launched.";

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.str().c_str());

        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << planNodeId;
        userMsg.showMsgBox(IDS_UE_210043);

        FUNCTION_EXIT;
        return;
    }

    // Select the node in the plan tree.
    GetTreeCtrl().SelectItem(nodeItem);

    // Verify the node is in fact a plan node
    TreeNode *treeNode = reinterpret_cast<TreeNode *>(GetTreeCtrl().GetItemData(nodeItem));
    PlanNode *planNode = dynamic_cast<PlanNode *>(treeNode);
    if (!planNode)
    {
        std::ostringstream errorMessage;
        errorMessage << "Node ID " << treeNode->getNodeId() << " (" << treeNode->getNodeName() << ") is not a plan type node." << std::endl
            << "Plan Controller cannot be launched.";

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.str().c_str());

        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << treeNode->getNodeId() << treeNode->getNodeName();
        userMsg.showMsgBox(IDS_UE_210044);

        FUNCTION_EXIT;
        return;
    }

    try
    {
        CWaitCursor waitCursor;

        // Start the Plan Controller for the plan
        planNode->controlPlan(PlanInstanceId(instanceNumber, location));
    }
    catch (...)
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << planNode->getNodeName();
        userMsg.showMsgBox(IDS_UE_210045);
    }

    FUNCTION_EXIT;
}


void CApprovedPlanTreeView::ProcessPlanApprovalStateUpdate(const TA_Base_Core::ApprovalStateUpdate &approvalStateUpdate)
{
    FUNCTION_ENTRY( "ProcessPlanApprovalStateUpdate" );

    try
    {
        CTreeCtrl &treeCtrl = GetTreeCtrl();
        HTREEITEM ancestorItem = NULL;

        if (approvalStateUpdate.approvalState == TA_Base_Core::APPROVED_APPROVAL_STATE) // plan has been approved
        {
            // Add the plan to the tree. We need to find the nearest ancestor and update its child items
            // in order to open up a route to the newly approved plan. Note that this ancestor may not
            // necessarily be the parent, but a category further up in the tree.
            ancestorItem = FindClosestAncestor(approvalStateUpdate.plan);

            if (ancestorItem)
            {
                TreeNode *ancestorNode = reinterpret_cast<TreeNode *>(treeCtrl.GetItemData(ancestorItem));
                TA_ASSERT(ancestorNode, "Nearest ancestor node pointer is null");

                ancestorNode->addChildrenToTree(treeCtrl, ancestorItem, GetDocument());
            }
        }
        else // plan has been unapproved
        {
            // Remove the plan from the tree. First get rid of the corresponding tree item, if found.
            // Then walk back up the tree purging all categories w/o approved descendants.
            TreeNode *treeNode = TreeNodeFactory::getInstance().getTreeNode(approvalStateUpdate.plan);
            if (treeNode)
            {
                HTREEITEM treeNodeItem = FindNode(treeNode->getNodeId());
                if (treeNodeItem)
                {
                    ancestorItem = treeCtrl.GetParentItem(treeNodeItem);
                    treeCtrl.DeleteItem(treeNodeItem);
                }
            }

            if (!ancestorItem)
            {
                ancestorItem = FindClosestAncestor(approvalStateUpdate.plan);
            }

            while (ancestorItem && !treeCtrl.ItemHasChildren(ancestorItem))
            {
                // If a category lock is active, then make sure that the locked category does not get
                // removed from the tree (even though it may not have any approved descendants) so that
                // the lock can continue to be enforced.
                if (GetDocument()->IsPlanTreeLocked())
                {
                    TreeNode *ancestorNode = reinterpret_cast<TreeNode *>(treeCtrl.GetItemData(ancestorItem));
                    TA_ASSERT(ancestorNode, "Ancestor node pointer is null");

                    if (ancestorNode->getNodeId() == GetDocument()->GetLockedCategoryId())
                    {
                        break;
                    }
                }

                HTREEITEM ancestorParent = treeCtrl.GetParentItem(ancestorItem);
                treeCtrl.DeleteItem(ancestorItem);

                ancestorItem = ancestorParent;
            }
        }
    }
    catch (const TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_PROCESS_APPROVAL_STATE_UPDATE, ex);
    }
    catch (...)
    {
        MessageBox::error(IDS_PROCESS_APPROVAL_STATE_UPDATE);
    }

    FUNCTION_EXIT;
}


void CApprovedPlanTreeView::ProcessInsertNodeUpdate(const TA_Base_Core::NodeSummary &nodeSummary, std::string sessionId)
{
    FUNCTION_ENTRY( "ProcessInsertNodeUpdate" );

    // Inserted nodes are either unapproved plans or empty categories. No need to process the update.

    FUNCTION_EXIT;
}


void CApprovedPlanTreeView::ProcessInsertNodeIntoRootUpdate(const TA_Base_Core::NodeSummary &nodeSummary, std::string sessionId)
{
    FUNCTION_ENTRY( "ProcessInsertNodeIntoRootUpdate" );

    // Inserted nodes are either unapproved plans or empty categories. No need to process the update.

    FUNCTION_EXIT;
}


void CApprovedPlanTreeView::ProcessDeleteNodeUpdate(const TA_Base_Core::DeleteNodeUpdate &deleteNodeUpdate)
{
    FUNCTION_ENTRY( "ProcessDeleteNodeUpdate" );

    // Node deletion does not ordinarily affect approved plans, however, if a category lock is active
    // and the locked category is removed, the corresponding node and its ancestors (with the exception
    // of root) will need to be deleted from the active plan tree.
    CPlanTreeView::ProcessDeleteNodeUpdate(deleteNodeUpdate);

    if (GetDocument()->IsPlanTreeLocked())
    {
        // The deleted node has not affected the category lock, nothing changes as far as approved plans.
        FUNCTION_EXIT;
        return;
    }

    CTreeCtrl &treeCtrl = GetTreeCtrl();
    HTREEITEM ancestorItem = FindNodeOrClosestAncestor(deleteNodeUpdate.parent);

    while (ancestorItem && !treeCtrl.ItemHasChildren(ancestorItem))
    {
        HTREEITEM ancestorParent = treeCtrl.GetParentItem(ancestorItem);
        treeCtrl.DeleteItem(ancestorItem);

        ancestorItem = ancestorParent;
    }

    FUNCTION_EXIT;
}

// CL16116++
void TA_Base_App::ControlPlanThread::run()
{
	// CL 15087 (2) ++
	std::auto_ptr<TA_Base_Core::AuditMessageSender> messageSender(TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(
		TA_Base_Core::AlarmAudit::Context));
	// get entity key
	std::auto_ptr<TA_Base_Core::IEntityData> planManagerEntityData(TA_Base_Core::EntityAccessFactory::getInstance().getEntity("PlanManager"));
	unsigned long entityKey = planManagerEntityData->getKey();
	// get session
	std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
	TA_Base_Core::DescriptionParameters dp;
	std::ostringstream strOut;
	strOut << "DSS Run for " << m_planName;
	TA_Base_Core::NameValuePair alarmDesc("alarmDescription", strOut.str().c_str());
	// ++ CL 15087 (2) 

	m_bStop = false;
	if(m_pPlanNode)
	{
		int nTry = 10;
		while(!PlanAgentAccess::getInstance().isRegisteredWithAgent(m_Location)
			&& nTry > 0)
		{
			--nTry;
			::Sleep(3000); // sleep 3 seconds
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				"[ControlPlanThread]Waiting Plan Manager register to Plan Agent, nTry:%d", 
				nTry);
		}
		
		try
		{
			if(!(nTry >0))
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
					"[ControlPlanThread] PlanManager can not register to PlanAgnet[%u] in 30 seconds", 
					m_Location);
				
				TA_Base_Bus::TransActiveMessage userMsg;
				std::ostringstream osTemp;
				osTemp << "register to Plan Agent.\nFailed to launch Plan:" << m_pPlanNode->getNodeName();
				userMsg << osTemp.str();
				userMsg.showMsgBox(IDS_UW_632006); // Unable to {XXX}
			}
			else
			{
				// We will start a new instance for --exec-plan-id
				// If no plan instance specialized and we call PlanNode.controlPlan(PlanInstanceId)
				// it will retrieve plan instances from all plan agent for this node, it is very slow>.
				// start a new instance do not need this at all, unless instance was assigned
				if(m_InstanceId != -1)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
						"[ControlPlanThread]Call controlPlan() with plan instance:%u", 
						m_InstanceId);
					m_pPlanNode->controlPlan(PlanInstanceId(m_InstanceId, m_Location));
				}
				else
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
						"[ControlPlanThread]Call controlPlan() without parameter");
					m_pPlanNode->controlPlan();
				}

				// get control plan dialog thread
				CPlanControllerThread* pControlThreadDlg = 
					FloatingWindowThreadRegister::getInstance().getPlanControllerThread(m_pPlanNode->getNodeId());
				if (!pControlThreadDlg)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
						"[ControlPlanThread] Can not get controlPlan Dialog Thread for PlanNode:%s", 
						m_pPlanNode->getNodeName().c_str());				
                    throw TA_Base_Core::TransactiveException("");
				}

                if (m_execPlan)
                {
				    // send start new instance message for auto run a plan
				    pControlThreadDlg->GetMainWnd()->PostMessage(WM_COMMAND, ID_PLAN_EXECUTE);

				    // send audit message for DSS Run success
				    // CL 15087 (2) ++
				    TA_Base_Core::NameValuePair alarmValue("Value", "SUCCESSFUL");	
				    dp.clear();
				    dp.push_back(&alarmValue);
				    dp.push_back(&alarmDesc);	
				    // DSS Run for Successful message (without asset)
				    messageSender->sendAuditMessage(TA_Base_Core::AlarmAudit::AlarmPlanStateUpdated,
					    entityKey, dp, "", session, "", "", "");	
				    // ++ CL 15087 (2)
                }
			}
		}
		catch(...)
		{
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << m_pPlanNode->getNodeName();
			userMsg.showMsgBox(IDS_UE_210045);

            if (m_execPlan)
            {
			    // send audit message for DSS Run Failed
			    // CL 15087 (2) ++
			    TA_Base_Core::NameValuePair alarmValue("Value", "FAIL");
			    dp.clear();
			    dp.push_back(&alarmValue);
			    dp.push_back(&alarmDesc);	
			    // DSS Run for Successful message (without asset)
			    messageSender->sendAuditMessage(TA_Base_Core::AlarmAudit::AlarmPlanStateUpdated,
				    entityKey, dp, "", session, "", "", "");	
			    // ++ CL 15087 (2)
            }
		}

	}
}
