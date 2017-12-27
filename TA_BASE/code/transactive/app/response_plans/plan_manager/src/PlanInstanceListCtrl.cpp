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

#include "stdafx.h"

#if defined(_MSC_VER)
    #pragma warning(disable:4250)
#endif // defined (_MSC_VER)

#include "core/utilities/src/TAAssert.h"
#include "app/response_plans/plan_manager/src/PlanManager.h"
#include "app/response_plans/plan_manager/src/PlanAgentAccess.h"
#include "app/response_plans/plan_manager/src/PlanInstanceListCtrl.h"
#include "app/response_plans/plan_manager/src/TreeNodeFactory.h"
#include "app/response_plans/plan_manager/src/TreeNode.h"
#include "app/response_plans/plan_manager/src/PlanNode.h"
#include "app/response_plans/plan_manager/src/PlanManagerCommonDefs.h"
#include "app/response_plans/plan_manager/src/MessageBox.h"
#include "app/response_plans/plan_manager/src/Utilities.h"
#include "app/response_plans/plan_manager/src/ApplicationFont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;
using namespace TA_Base_Bus;


/////////////////////////////////////////////////////////////////////////////
// CPlanInstanceListCtrl

CPlanInstanceListCtrl::CPlanInstanceListCtrl(bool planPathVisible) :
    CPlanActiveInstanceListCtrl(planPathVisible)
{
    FUNCTION_ENTRY( "CPlanInstanceListCtrl" );
    FUNCTION_EXIT;
}


CPlanInstanceListCtrl::~CPlanInstanceListCtrl()
{
    FUNCTION_ENTRY( "~CPlanInstanceListCtrl" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanInstanceListCtrl, CPlanActiveInstanceListCtrl)
    // {{AFX_MSG_MAP(CPlanInstanceListCtrl)
    // }}AFX_MSG_MAP
    ON_MESSAGE(WM_ACTIVE_PLAN_DETAIL_UPDATE, OnActivePlanDetailUpdate)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanInstanceListCtrl message handlers
BOOL CPlanInstanceListCtrl::PreCreateWindow( CREATESTRUCT& cs )
{
    BOOL bRes = TA_Base_Bus::CPlanActiveInstanceListCtrl::PreCreateWindow( cs );

    ApplicationFont::getInstance().adjustChildrenFont( GetSafeHwnd() );

    return bRes;
}


LRESULT CPlanInstanceListCtrl::OnActivePlanDetailUpdate(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnActivePlanDetailUpdate" );

    // Automatically select every new instance activated by this session
    TA_Base_Core::ActivePlanDetail* activePlanDetail = reinterpret_cast<TA_Base_Core::ActivePlanDetail *>(wParam);
    bool newInstance = (FindInstance(activePlanDetail->plan) == -1);

    CPlanActiveInstanceListCtrl::OnActivePlanDetailUpdate(wParam, lParam); // add or update instance

    if (newInstance)
    {
        int item = FindInstance(activePlanDetail->plan);

        if (item != -1)
        {
            try
            {
                if (PlanAgentAccess::getInstance().isOwner(activePlanDetail->plan))
                {
                    SelectItem(item);
                }
            }
            catch (...) {}
        }
    }

    FUNCTION_EXIT;
    return 0;
}


void CPlanInstanceListCtrl::SetSelectedInstance(PlanInstanceSharedPtr planInstance)
{
    FUNCTION_ENTRY( "SetSelectedInstance" );

    int selectedItem = GetNextItem(-1, LVNI_SELECTED);
    if (selectedItem != -1)
    {
        SelectItem(selectedItem, false);
    }

    if (!planInstance)
    {
        FUNCTION_EXIT;
        return;
    }

    if (planInstance != GetSelectedInstance())
    {
        for (int item = 0; item < GetItemCount(); item++)
        {
            TA_Base_Core::ActivePlanId activePlanId = GetPlanInstanceId(item);

            PlanNode *planNode = dynamic_cast<PlanNode *>(TreeNodeFactory::getInstance().getTreeNode(activePlanId.plan));

            if (planNode)
            {
                PlanInstanceSharedPtr tmpInstance = planNode->getPlanInstance(PlanInstanceId(activePlanId.instance, activePlanId.location));

                if (tmpInstance.get() == planInstance.get())
                {
                    SelectItem(item);

                    FUNCTION_EXIT;
                    return;
                }
            }
        }
    }

    FUNCTION_EXIT;
}


PlanInstanceSharedPtr CPlanInstanceListCtrl::GetSelectedInstance()
{
    FUNCTION_ENTRY( "GetSelectedInstance" );

    PlanNode *planNode = GetSelectedPlan();

    if (planNode)
    {
        TA_Base_Core::ActivePlanId selectedActivePlanId = GetSelectedPlanInstanceId();

        FUNCTION_EXIT;
        return planNode->getPlanInstance(PlanInstanceId(selectedActivePlanId.instance, selectedActivePlanId.location));
    }

    FUNCTION_EXIT;
    return PlanInstanceSharedPtr();
}


PlanNode *CPlanInstanceListCtrl::GetSelectedPlan()
{
    FUNCTION_ENTRY( "GetSelectedPlan" );

    TA_Base_Core::ActivePlanId selectedActivePlanId = GetSelectedPlanInstanceId();

    if (selectedActivePlanId.plan != (TreeNodeId) -1)
    {
        try
        {
            TreeNode *treeNode = TreeNodeFactory::getInstance().getTreeNode(selectedActivePlanId.plan);

            if (!treeNode)
            {
                treeNode = TreeNodeFactory::getInstance().createTreeNode(selectedActivePlanId.plan);
            }

            FUNCTION_EXIT;
            return dynamic_cast<PlanNode *>(treeNode);
        }
        catch (TA_Base_Core::TransactiveException &ex)
        {
            MessageBox::error(IDS_RETRIEVE_PLAN, ex);
        }
    }

    FUNCTION_EXIT;
    return NULL;
}


void CPlanInstanceListCtrl::SelectItem(int item, bool selected)
{
    FUNCTION_ENTRY( "SelectItem" );

    TA_ASSERT(item >= 0 && item < GetItemCount(), "Invalid item");

    int selectionState = selected? (LVIS_SELECTED | LVIS_FOCUSED) : ~(LVIS_SELECTED | LVIS_FOCUSED);

    SetItemState(item, selectionState, LVIS_SELECTED | LVIS_FOCUSED);

    if (selected)
    {
        EnsureVisible(item, FALSE);
    }

    FUNCTION_EXIT;
}


