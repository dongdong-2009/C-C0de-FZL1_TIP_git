/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/plan_tree/src/PlanTreeCtrl.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/22 18:34:29 $
  * Last modified by:  $Author: qi.huang $
  *
  */
// PlanTreeCtrl.cpp : implementation file
//

#include "StdAfx.h"
#include "core\exceptions\src\TransactiveException.h"
#include "core\utilities\src\TAAssert.h"
#include "bus\generic_gui\src\TransActiveMessage.h"
#include "bus\mfc_extensions\src\object_tree_control\ObjectTreeItem.h"
#include "bus\response_plans\plan_tree\src\PlanTreeCtrl.h"
#include "bus\response_plans\plan_tree\src\RootItem.h"
#include "bus\response_plans\plan_tree\src\PlanItem.h"
#include "bus\response_plans\plan_tree\src\IPlanAccess.h"
#include "bus\response_plans\plan_tree\src\DatabasePlanAccess.h"
#include "bus\response_plans\plan_tree\src\AgentPlanAccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_Bus
{

enum PlanTreeUserMessage
{
    WM_USER_INITIALISE = WM_USER + 500,
    WM_USER_SELECT_PLAN
};

std::string CPlanTreeCtrl::RootItemName = "Plans";

/////////////////////////////////////////////////////////////////////////////
// CPlanTreeCtrl

CPlanTreeCtrl::CPlanTreeCtrl(PlanFilter::PlanFilterType planFilterType, bool directDbAccess)
{
    // Create appropriate plan data accessor object
    if (directDbAccess)
    {
        m_planAccess = new DatabasePlanAccess(planFilterType);
    }
    else
    {
        m_planAccess = new AgentPlanAccess(planFilterType);
    }
}


CPlanTreeCtrl::CPlanTreeCtrl(PlanFilter::PlanFilterType planFilterType, const std::vector<TA_Base_Core::NodeId>& nonFilterablePlans, bool directDbAccess)
{
    // Create appropriate plan data accessor object
    if (directDbAccess)
    {
        m_planAccess = new DatabasePlanAccess(planFilterType, nonFilterablePlans);
    }
    else
    {
        m_planAccess = new AgentPlanAccess(planFilterType, nonFilterablePlans);
    }
}


CPlanTreeCtrl::~CPlanTreeCtrl()
{
    delete m_planAccess;
}


void CPlanTreeCtrl::PreSubclassWindow() 
{
	CObjectTreeCtrl::PreSubclassWindow();

    PostMessage(WM_USER_INITIALISE, 0, 0);
}


void CPlanTreeCtrl::setSelectedPlan(TA_Base_Core::NodeId planId)
{
    if (::IsWindow(GetSafeHwnd()))
    {
        PostMessage(WM_USER_SELECT_PLAN, static_cast<WPARAM>(planId), 0);
    }
}


void CPlanTreeCtrl::setSelectedPlan(const std::string& planPath)
{
    TA_ASSERT(m_planAccess, "Plan access pointer is null");

    setSelectedPlan(m_planAccess->getPlanId(planPath));
}


CPlanItem* CPlanTreeCtrl::getSelectedPlan()
{
    CPlanItem* planItem = dynamic_cast<CPlanItem*>(getSelectedItem());

    return planItem;
}


BEGIN_MESSAGE_MAP(CPlanTreeCtrl, CObjectTreeCtrl)
	//{{AFX_MSG_MAP(CPlanTreeCtrl)
	ON_MESSAGE(WM_USER_INITIALISE, OnInitialise)
    ON_MESSAGE(WM_USER_SELECT_PLAN, OnSetSelectedPlan)
    ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemExpanding)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanTreeCtrl message handlers

LRESULT CPlanTreeCtrl::OnInitialise(WPARAM wParam, LPARAM lParam)
{
    // Customise the appearance of the plan tree
    ModifyStyle(0, TVS_HASLINES | TVS_HASBUTTONS | TVS_SHOWSELALWAYS);

    // Create a root item for the tree
    TA_Base_Core::NodeSummary rootSummary;
    rootSummary.name = RootItemName.c_str();
    rootSummary.node = 1;
    rootSummary.type = TA_Base_Core::CATEGORY_NODE;
    rootSummary.approvalState = TA_Base_Core::UNDEFINED_APPROVAL_STATE;
    rootSummary.hasChildren = true;
    rootSummary.hasApprovedDescendant = true;
    rootSummary.hasUnapprovedDescendant = true;

    m_rootItem = new CRootItem(rootSummary, *m_planAccess);

    // Set the root of the plan tree and auto expand it.
    AddRootItem(*m_rootItem, true);
    m_rootItem->expandItem();

    return 0;
}


LRESULT CPlanTreeCtrl::OnSetSelectedPlan(WPARAM wParam, LPARAM lParam)
{
    TA_Base_Core::NodeId planId = static_cast<TA_Base_Core::NodeId>(wParam);

    try
    {
        TA_ASSERT(m_planAccess, "Plan access pointer is null");
        IPlanAccess::PlanIdList nodeIdsInPath = m_planAccess->getNodeIdsInPlanPath(planId);

        // The first ID in the list denotes the root of the tree. The last ID in the list should match the specified plan ID.    
        CObjectTreeItem* treeItem = m_rootItem;
        for (IPlanAccess::PlanIdList::const_iterator iter = nodeIdsInPath.begin() + 1; iter != nodeIdsInPath.end(); iter++)
        {
            TA_Base_Core::NodeId nodeId = *iter;

            std::ostringstream searchKey;
            searchKey << nodeId;

		    treeItem->expandItem();
		    treeItem = treeItem->findSelectedItem(searchKey.str());

            if (!treeItem)
            {
                std::ostringstream errorMessage;
                errorMessage << "Node ID " << nodeId << " missing from the plan path.";
 
                TA_THROW(TA_Base_Core::TransactiveException(errorMessage.str()));
            }
        }

        treeItem->selectItem();
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Transactive Exception", ex.what());
        TA_Base_Bus::TransActiveMessage userMsg;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210093);
    }
    catch (...)
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210093);
    }
 
    return 0;
}


void CPlanTreeCtrl::OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = reinterpret_cast<NM_TREEVIEW*>(pNMHDR);

    // Prevent the root item being collapsed
    if (pNMTreeView->action == TVE_COLLAPSE && pNMTreeView->itemNew.hItem == GetRootItem())
    {
        *pResult = 1;
        return;
    }

    CObjectTreeCtrl::onItemExpanding(pNMHDR, pResult);
}

}