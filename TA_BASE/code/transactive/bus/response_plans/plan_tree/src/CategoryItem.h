/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/plan_tree/src/CategoryItem.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// CategoryItem.h : header file
//

#if !defined(AFX_CATEGORYITEM_H__1F396BAD_CF8E_46C7_88EA_7AEC93A947A2__INCLUDED_)
#define AFX_CATEGORYITEM_H__1F396BAD_CF8E_46C7_88EA_7AEC93A947A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/mfc_extensions/src/object_tree_control/ObjectTreeItem.h"
#include "bus/response_plans/plan_tree/src/IPlanAccess.h"


namespace TA_Base_Bus
{
    /////////////////////////////////////////////////////////////////////////////
    // CCategoryItem

    class CCategoryItem : public CObjectTreeItem
    {
    // Construction/destruction
    public:
	    CCategoryItem(const TA_Base_Core::NodeSummary& nodeSummary, IPlanAccess& planAccess);
	    virtual ~CCategoryItem();

    private:
        CCategoryItem(const CCategoryItem& categoryItem);


    // Operations
    public:
        // CObjectTreeItem overrides
        virtual bool createChildItems();
        virtual const char* getItemLabelText();
        virtual bool hasChildren();
        virtual const std::string getKey();
        virtual HICON getItemImage(bool itemExpanded);

    protected:
        IPlanAccess& getPlanAccess();
        virtual IPlanAccess::PlanNodeList getChildren();

    private:
        CCategoryItem& operator=(const CCategoryItem& categoryItem);


    // Attributes
    private:
        TA_Base_Core::NodeSummary m_nodeSummary;
        IPlanAccess& m_planAccess;
    };

} // namespace TA_Base_Bus

#endif // !defined(AFX_CATEGORYITEM_H__1F396BAD_CF8E_46C7_88EA_7AEC93A947A2__INCLUDED_)
