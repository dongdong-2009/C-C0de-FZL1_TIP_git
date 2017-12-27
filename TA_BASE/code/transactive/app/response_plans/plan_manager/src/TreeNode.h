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
// This is a representation of a node within the plan tree.

#if !defined(AFX_TREENODE_H__7D5CB743_CFCD_406D_AE34_E1C7F44685A5__INCLUDED_)
#define AFX_TREENODE_H__7D5CB743_CFCD_406D_AE34_E1C7F44685A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "MainToolBarManager.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class ITreeNodeFilter;


    /////////////////////////////////////////////////////////////////////////////
    // TreeNode

    class TreeNode
    {
    // Operations
    public:
        // Constructors and destructors
        TreeNode(const TA_Base_Core::NodeSummary &nodeSummary);
        virtual ~TreeNode();

        // Summary info getters
        virtual TreeNode *getParentNode();
        virtual TreeNodeId getNodeId() const;
        virtual TreeNodeId getParentId() const;
        virtual TA_Base_Core::NodeSummary getNodeSummary();
        virtual const std::string &getNodeName();       // Returned by reference to enable setting PlanTreeView/CategoryNodeView item labels by callback
        virtual const std::string &getNodeType() = 0;   // Returned by reference to enable setting CategoryNodeView item labels by callback
        virtual const std::string &getNodeStatus() = 0; // Returned by reference to enable setting CategoryNodeView item labels by callback
        virtual bool isApproved();

        // Return runtime class of the view used to display tree node details
        virtual CRuntimeClass *getNodeViewClass();

        // Enable actions that can be performed on this node
        virtual void updateCmdUI(CCmdUI &cmdUI);

        // Return an image type for the corresponding tree item
        virtual ETreeNodeImageType getNodeImageType(bool isExpanded) = 0;

        // Window controls modifiers
        virtual void addToTree(CTreeCtrl &treeCtrl, HTREEITEM parent = NULL, ITreeNodeFilter *filter = NULL);
        virtual void addToList(CListCtrl &listCtrl, ITreeNodeFilter *filter = NULL);
        virtual void addChildrenToTree(CTreeCtrl &treeCtrl, HTREEITEM parent, ITreeNodeFilter *filter = NULL) = 0;
        virtual void addChildrenToList(CListCtrl &listCtrl, ITreeNodeFilter *filter = NULL) = 0;

        // Facilitate in-tree renaming of nodes.
        virtual void renameNode(std::string nodeName) = 0;

        // Plan config data update handlers
        virtual void updateNode(const TA_Base_Core::NodeSummary &nodeSummary);
        virtual void updateNode(const TA_Base_Core::PlanConfigUpdateData &planConfigData);
        virtual void updateChildNodeList(const TA_Base_Core::PlanConfigUpdateData &planConfigData) = 0;

        // Tree node comparison callback used in sorting
        static int CALLBACK compareSiblingNodes(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
        bool isVisible();
        void setVisible(bool bVisible, bool writeToDb = true);

    protected:
        virtual TA_Base_Core::NodeDetail &getNodeDetail();
        virtual void invalidateNodeName();
        virtual void invalidateNodeSummary();
        virtual void invalidateNodeDetail();
        virtual bool hasPrecedence(TreeNode &siblingNode) = 0;

    private:
        TreeNode(const TreeNode &other);
        TreeNode &operator=(const TreeNode &other);


    // Attributes
    private:
        TreeNodeId m_nodeId;
        TreeNodeId m_parentId;
        std::string *m_nodeName;
        TA_Base_Core::NodeSummary *m_nodeSummary;
        TA_Base_Core::NodeDetail *m_nodeDetail;
        bool m_nodeVisible;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_TREENODE_H__7D5CB743_CFCD_406D_AE34_E1C7F44685A5__INCLUDED_)
