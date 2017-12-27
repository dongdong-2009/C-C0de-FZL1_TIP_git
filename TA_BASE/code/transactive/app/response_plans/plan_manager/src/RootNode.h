//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/RootNode.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// This is a representation of the root node within the plan tree.

#if !defined(AFX_ROOTNODE_H__77682CB1_EC32_46C6_BC7E_76C098AED938__INCLUDED_)
#define AFX_ROOTNODE_H__77682CB1_EC32_46C6_BC7E_76C098AED938__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "CategoryNode.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // RootNode

    class RootNode : public CategoryNode
    {
    // Operations
    public:
           // Constructors and destructors
        RootNode(const TA_Base_Core::NodeSummary &nodeSummary);
        virtual ~RootNode();

        // Summary info getters
        virtual const std::string &getNodeType();   // Returned by reference to enable setting CategoryNodeView item labels by callback

        // Action methods
        virtual void newPlan();
        virtual void copyPlan(PlanNode &planNode, bool overwrite);
        virtual void movePlan(PlanNode &planNode, bool overwrite);
        virtual void newCategory();

        // Enable actions that can be performed on the root category
        virtual void updateCmdUI(CCmdUI &cmdUI);

        // Return an image type for the corresponding tree item
        virtual ETreeNodeImageType getNodeImageType(bool isExpanded);

        // Facilitate in-tree renaming of nodes.
        virtual void renameNode(std::string nodeName);

        // Plan config data update handler
        virtual void updateChildNodeList(const TA_Base_Core::PlanConfigUpdateData &planConfigData);

    protected:
        virtual void invalidateNodeName();
        virtual void invalidateNodeSummary();
        virtual void invalidateNodeDetail();
        virtual void invalidateDescendantInfo();
        virtual void createChildren(TreeNodeList &childNodes);
        virtual TA_Base_Core::NodeDetail &getNodeDetail();

    private:
        RootNode(const RootNode &other);
        RootNode &operator=(const RootNode &other);
    };

} // namespace TA_Base_App

#endif // !defined(AFX_ROOTNODE_H__77682CB1_EC32_46C6_BC7E_76C098AED938__INCLUDED_)
