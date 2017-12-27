/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/plan_tree/src/PlanItem.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// PlanItem.cpp : implementation file
//

#include "StdAfx.h"
#include <sstream>
#include "core\exceptions\src\TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus\generic_gui\src\TransActiveMessage.h"
#include "bus\response_plans\plan_tree\src\PlanTree_resource.h"
#include "bus\response_plans\plan_tree\src\PlanItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_Bus;


/////////////////////////////////////////////////////////////////////////////
// CPlanItem

CPlanItem::CPlanItem(const TA_Base_Core::NodeSummary& nodeSummary, IPlanAccess& planAccess) : 
    m_nodeSummary(nodeSummary),
    m_planAccess(planAccess)
{
}


CPlanItem::~CPlanItem()
{
}


TA_Base_Core::NodeId CPlanItem::getPlanId()
{
    return m_nodeSummary.node;
}


std::string CPlanItem::getPlanPath()
{
    try
    {
        return m_planAccess.getPlanPath(m_nodeSummary.node);
    }
    catch (const TA_Base_Core::TransactiveException &ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << m_nodeSummary.name.in();
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210092);
    }
    catch (...)
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << m_nodeSummary.name.in();
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210092);
    }

    return m_nodeSummary.name.in();  // return the name of the plan only
}


bool CPlanItem::createChildItems()
{
    return false;
}


const char* CPlanItem::getItemLabelText()
{
    return m_nodeSummary.name.in();
}


bool CPlanItem::hasChildren()
{
    return false;
}


const std::string CPlanItem::getKey()
{
    std::ostringstream key;
    key << m_nodeSummary.node;

    return key.str();
}


HICON CPlanItem::getItemImage(bool itemExpanded)
{
    static HICON approvedPlanIcon = LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_APPROVED_PLAN_ICON));
    static HICON unapprovedPlanIcon = LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_UNAPPROVED_PLAN_ICON));

    return (m_nodeSummary.approvalState == TA_Base_Core::APPROVED_APPROVAL_STATE? approvedPlanIcon : unapprovedPlanIcon);
}
