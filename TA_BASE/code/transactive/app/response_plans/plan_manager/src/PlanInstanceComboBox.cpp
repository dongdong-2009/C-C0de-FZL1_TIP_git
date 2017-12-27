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
#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"
#include "PlanManager.h"
#include "PlanInstanceComboBox.h"
#include "PlanBrowserDoc.h"
#include "PlanNode.h"
#include "PlanInstance.h"
#include "PlanInstanceNone.h"
#include "PlanInstanceFilter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanInstanceComboBox

static const PlanInstanceId InvalidPlanInstance(0u, 0u);

CPlanInstanceComboBox::CPlanInstanceComboBox() : m_idCurSelection(InvalidPlanInstance), m_planInstanceFilter(NULL)
{
    FUNCTION_ENTRY( "CPlanInstanceComboBox" );
    FUNCTION_EXIT;
}


CPlanInstanceComboBox::~CPlanInstanceComboBox()
{
    FUNCTION_ENTRY( "~CPlanInstanceComboBox" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanInstanceComboBox, CComboBox)
    // {{AFX_MSG_MAP(CPlanInstanceComboBox)
    ON_CONTROL_REFLECT_EX(CBN_SELCHANGE, OnItemSelected)
    ON_WM_DESTROY()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanInstanceComboBox message handlers

BOOL CPlanInstanceComboBox::OnItemSelected()
{
    FUNCTION_ENTRY( "OnItemSelected" );

    // Ignore same-instance selections
    PlanInstanceId *newSelection = static_cast<PlanInstanceId *>(GetItemDataPtr(GetCurSel()));
    if (*newSelection != m_idCurSelection)
    {
        m_idCurSelection = *newSelection;

        FUNCTION_EXIT;
        return FALSE;
    }

    FUNCTION_EXIT;
    return TRUE; // Prevent the parent handling the notification
}


void CPlanInstanceComboBox::SetPlanInstanceFilter(PlanInstanceFilter *planInstanceFilter)
{
    FUNCTION_ENTRY( "SetPlanInstanceFilter" );

    m_planInstanceFilter = planInstanceFilter;

    FUNCTION_EXIT;
}


bool CPlanInstanceComboBox::IsNewInstance(const PlanInstanceId& planInstance)
{
    FUNCTION_ENTRY( "IsNewInstance" );
    FUNCTION_EXIT;
    return (FindStringExact(0, GetPlanInstanceLabel(planInstance)) == CB_ERR);
}


void CPlanInstanceComboBox::AddInstance(const int& nActivePlanState, const TA_Base_Core::NodeId& planId, const PlanInstanceId& instanceId)
{
    FUNCTION_ENTRY( "AddInstance" );

    // TD10997: Ignore any updates for a "running" plan where the current step state is not valid.

    if ((( nActivePlanState == TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE ) || 
         ( nActivePlanState == TA_Base_Core::OPERATOR_PAUSED_ACTIVE_PLAN_STATE )) &&
         ( nActivePlanState == TA_Base_Core::UNDEFINED_ACTIVE_STEP_STATE ))
    {
        FUNCTION_EXIT;
        return;
    }

    // OK to add the specified instance.
    int insertPos = -1;

    for (int item = 0; item < GetCount(); item++)
    {
        PlanInstanceId *tmpInstance = static_cast<PlanInstanceId*>(GetItemDataPtr(item));

        if (instanceId == *tmpInstance)
        {
            FUNCTION_EXIT;
            return;
        }

        if (*tmpInstance < instanceId)
        {
            insertPos = item + 1;
        }
    }

    // Before the instance is added, make sure it meets the filtering criteria
    if (m_planInstanceFilter && !m_planInstanceFilter->isRelevant(planId, instanceId))
    {
        FUNCTION_EXIT;
        return;
    }

    insertPos = InsertString(insertPos, GetPlanInstanceLabel(instanceId));
    
    PlanInstanceId *planInstanceIdPtr = new PlanInstanceId(instanceId);
    SetItemDataPtr(insertPos, planInstanceIdPtr);

    m_currentPlanNodeId = planId;
    FUNCTION_EXIT;
}


void CPlanInstanceComboBox::RepositionInstance(const PlanInstanceSharedPtr& planInstance)
{
    FUNCTION_ENTRY( "RepositionInstance" );

    TA_ASSERT(planInstance, "Plan instance is null");

    CString instanceLabel = GetPlanInstanceLabel(planInstance->getInstanceId());
    int item = FindStringExact(0, instanceLabel);

    if (item != CB_ERR)
    {
        // Assume no repositioning will be required
        bool currentPositionOK = true;

        // If there's a preceding instance in the list compare the two
        if (item > 0)
        {
            PlanInstanceId *instanceBefore = static_cast<PlanInstanceId *>(GetItemDataPtr(item - 1));

            currentPositionOK = (*instanceBefore < planInstance->getInstanceId());
        }

        // If OK wrt to preceding instance, compare with the one that follows (if any)
        if (currentPositionOK && item < (GetCount() - 1))
        {
            PlanInstanceId *instanceAfter = static_cast<PlanInstanceId *>(GetItemDataPtr(item + 1));

            currentPositionOK = (planInstance->getInstanceId() < *instanceAfter);
        }

        // If the order has changed
        if (!currentPositionOK)
        {
            // Remove the instance and add it back in at the correct position
            DeleteString(item);
            AddInstance(planInstance->getActivePlanState(), planInstance->getActivePlanId().plan, planInstance->getInstanceId());

            // If the instance was selected before moving then re-select. Don't call SelectItem()
            // as this selection should be transparent to the parent PlanStepDetailPage.
            if (planInstance->getInstanceId() == m_idCurSelection)
            {
                item = FindStringExact(0, GetPlanInstanceLabel(planInstance->getInstanceId()));
                if (item != CB_ERR)
                {
                    SetCurSel(item);
                }
            }
        }
    }

    FUNCTION_EXIT;
}


void CPlanInstanceComboBox::RemoveInstance(const PlanInstanceId &planInstanceId)
{
    FUNCTION_ENTRY( "RemoveInstance" );

    int item = FindStringExact(0, GetPlanInstanceLabel(planInstanceId));

    if (item != CB_ERR)
    {
        PlanInstanceId *removedInstance = static_cast<PlanInstanceId*>(GetItemDataPtr(item));

        DeleteString(item);

        // The removed PlanInstance object may have been deleted so it shouldn't be accessed.
        // It's still ok to compare pointers.
        if (*removedInstance == m_idCurSelection)
        {
            SelectItem(__min(item, GetCount() - 1));
        }
        
        delete removedInstance;
    }

    FUNCTION_EXIT;
}


void CPlanInstanceComboBox::Purge()
{
    FUNCTION_ENTRY( "Purge" );

    bool bSelectionChanged = false;

    if (m_planInstanceFilter)
    {
        for (int item = GetCount() - 1; item >= 0; item--)
        {
            PlanInstanceId *planInstance = static_cast<PlanInstanceId *>(GetItemDataPtr(item));

            if (!m_planInstanceFilter->isRelevant(m_currentPlanNodeId, *planInstance))
            {
                bool isSelected = (item == GetCurSel());

                DeleteString(item);

                if (isSelected)
                {
                    bSelectionChanged = true;

                    // Change the selection to the next closest item. SetCurSel() will not
                    // notify the parent PlanStepDetailPage of the change, so doing this
                    // will not result in unnecessary updates of the plan step list.
                    SetCurSel(__min(item, GetCount() - 1));
                }
            }
        }

        // If the instance that was selected before the purge is no longer in the list, the selection
        // would have been transferred to another item. Now is the time to let the parent PlanStepDetailPage
        // know to update the plan step list based on the new selection.
        if (bSelectionChanged)
        {
            SelectItem(GetCurSel());
        }
    }

    FUNCTION_EXIT;
}


void CPlanInstanceComboBox::SetSelectedInstance(PlanInstanceSharedPtr planInstance)
{
    FUNCTION_ENTRY( "SetSelectedInstance" );

    if (NULL == planInstance.get())
    {
        SelectItem(0); // instance "None"
        FUNCTION_EXIT;
        return;
    }

    if (planInstance->getInstanceId() != m_idCurSelection)
    {
        int item = FindStringExact(0, GetPlanInstanceLabel(planInstance->getInstanceId()));

        if (item != CB_ERR)
        {
            SelectItem(item);
        }
    }

    FUNCTION_EXIT;
}


PlanInstanceId& CPlanInstanceComboBox::GetSelectedInstance()
{
    FUNCTION_ENTRY( "GetSelectedInstance" );
    FUNCTION_EXIT;
    return m_idCurSelection;
}


CString CPlanInstanceComboBox::GetPlanInstanceLabel(const PlanInstanceId &planInstanceId)
{
    FUNCTION_ENTRY( "GetPlanInstanceLabel" );

    if (0 == planInstanceId.m_location && 0 == planInstanceId.m_instance)
    {
        FUNCTION_EXIT;
        return "нч";
    }

    CString planInstanceLabel;
    planInstanceLabel.Format("%d / %s",
                             planInstanceId.m_instance,
                             TA_Base_Bus::PlanAgentLocationAccess::getInstance().getLocationName(planInstanceId.m_location).c_str());

    FUNCTION_EXIT;
    return planInstanceLabel;
}


void CPlanInstanceComboBox::SelectItem(int item)
{
    FUNCTION_ENTRY( "SelectItem" );

    if (SetCurSel(item) != CB_ERR)
    {
        GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CBN_SELCHANGE), (LPARAM) GetSafeHwnd());
    }

    FUNCTION_EXIT;
}



void TA_Base_App::CPlanInstanceComboBox::OnDestroy()
{
    m_idCurSelection = InvalidPlanInstance;

    for (int item = GetCount() - 1; item >= 0; item--)
    {
        PlanInstanceId *planInstance = static_cast<PlanInstanceId *>(GetItemDataPtr(item));
        if (NULL != planInstance)
        {
            delete planInstance;
            planInstance = NULL;
        }
        DeleteString(item);
    }
}
