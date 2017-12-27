/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/plan_tree/src/CategoryItem.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// CategoryItem.cpp : implementation file
//

#include "StdAfx.h"
#include <sstream>
#include <algorithm>
#include <vector>
#include "core\exceptions\src\TransactiveException.h"
#include "core\utilities\src\TAAssert.h"
#include "bus\generic_gui\src\TransActiveMessage.h"
#include "bus\mfc_extensions\src\object_tree_control\ObjectTreeItem.h"
#include "bus\response_plans\plan_tree\src\PlanTree_resource.h"
#include "bus\response_plans\plan_tree\src\CategoryItem.h"
#include "bus\response_plans\plan_tree\src\PlanItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_Base_Bus
{
    struct CaseInsensitiveCharCompare
    {
        bool operator()(const char& ch1, const char& ch2) const
        {
            return ::tolower(ch1) < ::tolower(ch2);
        }
    };


    struct PlanTreeNodeCompare
    {
        // Determines item precedence in the plan tree.
        // Categories always precede plans. Where two items are of the same type (i.e. both are categories,
        // or both are plans) their labels are compared. The comparison ignores character case.
        bool operator()(TA_Base_Core::NodeSummary& nodeSummary1, TA_Base_Core::NodeSummary& nodeSummary2)
        {
            if (nodeSummary1.type == TA_Base_Core::CATEGORY_NODE && nodeSummary2.type != TA_Base_Core::CATEGORY_NODE)
            {
                return true;
            }

            if (nodeSummary1.type != TA_Base_Core::CATEGORY_NODE && nodeSummary2.type == TA_Base_Core::CATEGORY_NODE)
            {
                return false;
            }
       
            std::string item1Label = nodeSummary1.name;
            std::string item2Label = nodeSummary2.name;

            return std::lexicographical_compare(item1Label.begin(), item1Label.end(),
                                                item2Label.begin(), item2Label.end(), 
                                                CaseInsensitiveCharCompare());        
        }

    };
}

using namespace TA_Base_Bus;


/////////////////////////////////////////////////////////////////////////////
// CCategoryItem

CCategoryItem::CCategoryItem(const TA_Base_Core::NodeSummary& nodeSummary, IPlanAccess& planAccess) : 
    m_nodeSummary(nodeSummary),
    m_planAccess(planAccess)
{
}


CCategoryItem::~CCategoryItem()
{
}


bool CCategoryItem::createChildItems()
{
    try
    {
        // Extract the list of children nodes.
        IPlanAccess::PlanNodeList children = getChildren();

        // Sort the nodes so that they appear in the correct order within the plan tree.
        std::sort(children.begin(), children.end(), PlanTreeNodeCompare());

        // Insert the nodes into the plan tree
        for (IPlanAccess::PlanNodeList::iterator iter = children.begin(); iter != children.end(); iter++)
        {
            TA_Base_Core::NodeSummary nodeSummary = *iter;
            CObjectTreeItem *treeItem = NULL;

            switch (nodeSummary.type)
            {
                case TA_Base_Core::PLAN_NODE:
                    treeItem = new CPlanItem(nodeSummary, m_planAccess);
                    break;

                case TA_Base_Core::CATEGORY_NODE:
                    treeItem = new CCategoryItem(nodeSummary, m_planAccess);
                    break;

                default:
                    TA_ASSERT(false, "Invalid node type");
            }

            AddChildItem(*treeItem, false, true);
        }

        return (children.size() > 0);
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << getItemLabelText();
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210091);
    }
    catch (...)
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << getItemLabelText();
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210091);
    }

    return false;
}


const char* CCategoryItem::getItemLabelText()
{
    return m_nodeSummary.name.in();
}


bool CCategoryItem::hasChildren()
{
    return m_nodeSummary.hasChildren;
}


const std::string CCategoryItem::getKey()
{
    std::ostringstream key;
    key << m_nodeSummary.node;

    return key.str();
}


HICON CCategoryItem::getItemImage(bool itemExpanded)
{
    static HICON collapsedCategoryIcon = LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_COLLAPSED_CATEGORY_ICON));
    static HICON expandedCategoryIcon = LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_EXPANDED_CATEGORY_ICON));

    return (itemExpanded? expandedCategoryIcon : collapsedCategoryIcon);
}


IPlanAccess& CCategoryItem::getPlanAccess()
{
    return m_planAccess;
}


IPlanAccess::PlanNodeList CCategoryItem::getChildren()
{
    return getPlanAccess().getChildren(m_nodeSummary.node);
}

