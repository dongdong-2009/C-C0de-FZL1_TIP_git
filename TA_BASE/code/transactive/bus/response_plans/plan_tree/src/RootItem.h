/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/plan_tree/src/RootItem.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// RootItem.h : header file
//

#if !defined(AFX_ROOTITEM_H__0FE056AE_5B41_4085_9526_DE85DD5CA37F__INCLUDED_)
#define AFX_ROOTITEM_H__0FE056AE_5B41_4085_9526_DE85DD5CA37F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/response_plans/plan_tree/src/PlanTreeCtrl.h"
#include "bus/response_plans/plan_tree/src/CategoryItem.h"
#include "bus/response_plans/plan_tree/src/IPlanAccess.h"


namespace TA_Base_Bus
{
    /////////////////////////////////////////////////////////////////////////////
    // CRootItem

    class CRootItem : public CCategoryItem
    {
    // Construction/destruction
    public:
	    CRootItem(const TA_Base_Core::NodeSummary& nodeSummary, IPlanAccess& planAccess);
	    virtual ~CRootItem();

    private:
        CRootItem(const CRootItem& rootItem);


    // Operations
    public:
        // CObjectTreeItem overrides
        virtual HICON getItemImage(bool itemExpanded);

    protected:
        virtual IPlanAccess::PlanNodeList getChildren();

    private:
        CRootItem& operator=(const CRootItem& rootItem);


    // Attributes
    private:
    };

} // namespace TA_Base_Bus

#endif // !defined(AFX_ROOTITEM_H__0FE056AE_5B41_4085_9526_DE85DD5CA37F__INCLUDED_)
