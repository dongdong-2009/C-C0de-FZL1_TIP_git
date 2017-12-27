    // 
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/CategoryNode.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// This is a representation of a category node within the plan tree.

#if !defined(AFX_CATEGORYNODE_H__5D70BCA1_A160_4D0A_8772_2B9EF0128C84__INCLUDED_)
#define AFX_CATEGORYNODE_H__5D70BCA1_A160_4D0A_8772_2B9EF0128C84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "SynchronisedMap.h"
#include "TreeNode.h"
#include "PlanNode.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CategoryNode

    class CategoryNode : public TreeNode
    {
    // Operations
    public:
        // Constructors and destructors
        CategoryNode(const TA_Base_Core::NodeSummary &nodeSummary);
        virtual ~CategoryNode();

        // Summary info getters
        virtual const std::string &getNodeType();   // Returned by reference to enable setting CategoryNodeView item labels by callback
        virtual const std::string &getNodeStatus(); // Returned by reference to enable setting CategoryNodeView item labels by callback

        // Action methods
        virtual void importPlan (TA_Base_Core::AuxPlanDetail& plan);
        virtual void newPlan();
        virtual void copyPlan(PlanNode &planNode, bool overwrite);
        virtual void movePlan(PlanNode &planNode, bool overwrite);
        virtual void newCategory();
        virtual void deleteCategory();

        // Return runtime class of the view used to display tree node details
        virtual CRuntimeClass *getNodeViewClass();

        // Enable actions that can be performed on this category
        virtual void updateCmdUI(CCmdUI &cmdUI);

        // Return an image type for the corresponding tree item
        virtual ETreeNodeImageType getNodeImageType(bool isExpanded);

        // Window controls modifiers
        virtual void addChildrenToTree(CTreeCtrl &treeCtrl, HTREEITEM parent, ITreeNodeFilter *filter = NULL);
        virtual void addChildrenToList(CListCtrl &listCtrl, ITreeNodeFilter *filter = NULL);

        // Facilitate in-tree renaming of nodes.
        virtual void renameNode(std::string nodeName);

        // Plan config data update handler
        virtual void updateChildNodeList(const TA_Base_Core::PlanConfigUpdateData &planConfigData);

          virtual void getChildren(TreeNodeList &childNodes);
    protected:
        virtual void invalidateDescendantInfo(); // Reset summary information about descendants. Recurses up the ancestry line.
        virtual bool hasPrecedence(TreeNode &siblingNode);
        virtual void createChildren(TreeNodeList &childNodes);
        virtual void addToChildNodeList(const TA_Base_Core::NodeSummary &nodeSummary);
        virtual void removeFromChildNodeList(TreeNodeId nodeId);
        virtual TA_Base_Core::CategoryDetail &getCategoryDetail();

    private:
        CategoryNode(const CategoryNode &other);
        CategoryNode &operator=(const CategoryNode &other);


    // Attributes
    private:
           SynchronisedMap<TreeNodeId, TreeNode *> *m_childNodeMap;

    };

} // namespace TA_Base_App

#endif // !defined(AFX_CATEGORYNODE_H__5D70BCA1_A160_4D0A_8772_2B9EF0128C84__INCLUDED_)
