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

#include "StdAfx.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataReader.h"
#include "Resource.h"
#include "TreeNodeFactory.h"
#include "TreeNode.h"
#include "RootNode.h"
#include "CategoryNode.h"
#include "PlanNode.h"
#include "PlanAgentAccess.h"
#include "Utilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TA_Base_App::TreeNodeFactory *TA_Base_App::TreeNodeFactory::s_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable TA_Base_App::TreeNodeFactory::s_singletonLock;

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// TreeNodeFactory

TreeNodeFactory::TreeNodeFactory()
{
    FUNCTION_ENTRY( "TreeNodeFactory" );

    try
    {
        m_planTypeMap = TA_Base_Bus::PlanDataReader::instance()->getPlanTypeMap();
    }
    catch ( ... )
    {
    }

    FUNCTION_EXIT;
}


TreeNodeFactory::~TreeNodeFactory()
{
    FUNCTION_ENTRY( "~TreeNodeFactory" );

    while (!m_treeNodeMap.empty())
    {
        TreeNode *treeNode = m_treeNodeMap.front();
        m_treeNodeMap.erase(treeNode->getNodeId());

        delete treeNode;
    }

    s_instance = NULL;

    FUNCTION_EXIT;
}


TreeNodeFactory &TreeNodeFactory::getInstance()
{
    FUNCTION_ENTRY( "getInstance" );

    if (!s_instance)
    {
        TA_Base_Core::ThreadGuard guard(s_singletonLock);

        if (!s_instance)
        {
            s_instance = new TreeNodeFactory();
        }
    }

    FUNCTION_EXIT;
    return *s_instance;
}


void TreeNodeFactory::destroyInstance()
{
    FUNCTION_ENTRY( "destroyInstance" );

    TA_Base_Core::ThreadGuard guard(s_singletonLock);

    delete s_instance;

    FUNCTION_EXIT;
}


TreeNodeId TreeNodeFactory::getRootNodeId()
{
    FUNCTION_ENTRY( "getRootNodeId" );
    FUNCTION_EXIT;
    return TA_Base_Core::ROOT_NODE_ID;  // defined in PlanAgentDataCorbaDef.idl
}


RootNode *TreeNodeFactory::createRootNode()
{
    FUNCTION_ENTRY( "createRootNode" );

    RootNode *rootNode = getRootNode();

    if (rootNode)
    {
        FUNCTION_EXIT;
        return rootNode;
    }

    TA_Base_Core::NodeSummary rootNodeSummary;
    rootNodeSummary.name = CORBA::string_dup("Plans");
    rootNodeSummary.node = getRootNodeId();
    rootNodeSummary.parent = -1;
    rootNodeSummary.type = TA_Base_Core::CATEGORY_NODE;
    rootNodeSummary.approvalState = TA_Base_Core::UNDEFINED_APPROVAL_STATE;
    rootNodeSummary.hasChildren = true;
    rootNodeSummary.hasApprovedDescendant = true;
    rootNodeSummary.hasUnapprovedDescendant = true;

    rootNode = new RootNode(rootNodeSummary);
    m_treeNodeMap.insert(rootNode->getNodeId(), rootNode);

    FUNCTION_EXIT;
    return rootNode;
}


TreeNode *TreeNodeFactory::createTreeNode(TreeNodeId nodeId)
{
    FUNCTION_ENTRY( "createTreeNode" );

    TA_Base_Core::NodeSummary_var nodeSummaryVar = PlanAgentAccess::getInstance().getNodeSummary(nodeId);

    FUNCTION_EXIT;
    return createTreeNode(nodeSummaryVar);
}


TreeNode *TreeNodeFactory::createTreeNode(const TA_Base_Core::NodeSummary &nodeSummary)
{
    FUNCTION_ENTRY( "createTreeNode" );

    TreeNode *treeNode = getTreeNode(nodeSummary.node);

    if (treeNode)
    {
        treeNode->updateNode(nodeSummary);
    }
    else
    {
        switch (nodeSummary.type)
        {
            case TA_Base_Core::PLAN_NODE:
                treeNode = new PlanNode(nodeSummary);
                break;

            case TA_Base_Core::CATEGORY_NODE:
                treeNode = new CategoryNode(nodeSummary);
                break;

            default:
                TA_ASSERT(false, "Invalid node type");
        }

        m_treeNodeMap.insert(treeNode->getNodeId(), treeNode);
    }

    FUNCTION_EXIT;
    return treeNode;
}


void TreeNodeFactory::createChildren(TreeNodeId nodeId, TreeNodeList &children)
{
    FUNCTION_ENTRY( "createChildren" );

    TA_Base_Core::SiblingNodes_var nodesVar = PlanAgentAccess::getInstance().getChildNodes(nodeId);

    createTreeNodes(nodesVar, children);

    FUNCTION_EXIT;
}


void TreeNodeFactory::createRootChildren(TreeNodeList &children)
{
    FUNCTION_ENTRY( "createRootChildren" );

    TA_Base_Core::SiblingNodes_var nodesVar = PlanAgentAccess::getInstance().getRootChildNodes();

    createTreeNodes(nodesVar, children);

    FUNCTION_EXIT;
}


void TreeNodeFactory::retireTreeNode(TreeNodeId nodeId)
{
    FUNCTION_ENTRY( "retireTreeNode" );

    if (m_treeNodeMap.find(nodeId))
    {
        TreeNode *treeNode = m_treeNodeMap[nodeId];
        m_treeNodeMap.erase(nodeId);

        delete treeNode;
    }

    FUNCTION_EXIT;
}


RootNode *TreeNodeFactory::getRootNode()
{
    FUNCTION_ENTRY( "getRootNode" );
    FUNCTION_EXIT;
    return static_cast<RootNode *>(getTreeNode(getRootNodeId()));
}


TreeNode* TreeNodeFactory::getTreeNode(TreeNodeId nodeId)
{
    FUNCTION_ENTRY( "getTreeNode" );

    TreeNode* pNode = NULL;

    if (m_treeNodeMap.find(nodeId, pNode))
    {
        FUNCTION_EXIT;
        return pNode;
    }

    FUNCTION_EXIT;
    return NULL;
}


TreeNode *TreeNodeFactory::getClosestAncestor(TreeNodeId nodeId)
{
    FUNCTION_ENTRY( "getClosestAncestor" );

    // See if we have a parent for the specified node. That will be the closest ancestor.
    TreeNode *treeNode = getTreeNode(nodeId);

    if (treeNode)
    {
        TreeNode *parentNode = treeNode->getParentNode();

        if (parentNode)
        {
            FUNCTION_EXIT;
            return parentNode;
        }
    }

    // Parent node could not be determined. Get the node IDs in the specified node's path and
    // go through them to see which exists.
    try
    {
        TA_Base_Core::NodeIds_var nodeIdsVar = PlanAgentAccess::getInstance().getNodeIdsInNodePath(nodeId);

        // Work from the node's parent towards the top of the tree
        for (CORBA::Long i = nodeIdsVar->length() - 2; i >= 0; i--)
        {
            TreeNode *ancestorNode = getTreeNode(nodeIdsVar[i]);

            if (ancestorNode)
            {
                FUNCTION_EXIT;
                return ancestorNode;
            }
        }
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", ex.what());

        FUNCTION_EXIT;
        return NULL;
    }

    FUNCTION_EXIT;
    return NULL;
}


TreeNode *TreeNodeFactory::getTreeNodeOrClosestAncestor(TreeNodeId nodeId)
{
    FUNCTION_ENTRY( "getTreeNodeOrClosestAncestor" );

    TreeNode *treeNode = getTreeNode(nodeId);

    if (treeNode)
    {
        FUNCTION_EXIT;
        return treeNode;
    }

    FUNCTION_EXIT;
    return getClosestAncestor(nodeId);
}


void TreeNodeFactory::getNodeIdsInNodePath(TreeNodeId nodeId, TreeNodeIdList &nodeIds)
{
    FUNCTION_ENTRY( "getNodeIdsInNodePath" );

    nodeIds.clear();

    // Find the specified node and walk up its ancestors towards the root, adding each node's ID
    // to the front of the node ID list.
    TreeNode *treeNode = getTreeNode(nodeId);
    while (treeNode)
    {
        nodeId = treeNode->getNodeId();
        nodeIds.insert(nodeIds.begin(), nodeId);

        treeNode = treeNode->getParentNode();
    }

    // The last node ID added to the list should have been that of the root node. If it isn't then
    // some of the nodes in the path must be missing from the factory. In such case resort to asking
    // the Plan Agent for the relevant info.
    if (nodeId != getRootNodeId())
    {
        nodeIds.clear();

        TA_Base_Core::NodeIds_var nodeIdsVar = PlanAgentAccess::getInstance().getNodeIdsInNodePath(nodeId);
        for (CORBA::Long i = 0; i < static_cast<long>(nodeIdsVar->length()); i++)
        {
            nodeIds.push_back(nodeIdsVar[i]);
        }
    }

    FUNCTION_EXIT;
}


void TreeNodeFactory::createTreeNodes(const TA_Base_Core::SiblingNodes &nodes, TreeNodeList &treeNodes)
{
    FUNCTION_ENTRY( "createTreeNodes" );

    treeNodes.reserve(nodes.length());

    for (CORBA::ULong i = 0; i < nodes.length(); i++)
    {
        bool isVisible = true;

        if ( TA_Base_Core::PLAN_NODE == nodes[i].type )
        {
            unsigned long ulNodeId = nodes[i].node;

            isVisible = isTreeNodeVisible(ulNodeId);
            
        }

        if ( !Utilities::isPlanManagerConfigurationMode() && !isVisible )
        {
            continue;
        }

        TreeNode *treeNode = createTreeNode( nodes[i] );
        treeNode->setVisible(isVisible, false);
        treeNodes.push_back(treeNode);
    }

    FUNCTION_EXIT;
}


int TA_Base_App::TreeNodeFactory::getTreeNodeLevelInTree( TreeNodeId nodeId )
{
    FUNCTION_ENTRY( "getTreeNodeLevelInTree" );

    int nLevel = 0;
    TreeNode* pCurNode = getTreeNode( nodeId );

    while ( pCurNode != NULL )
    {
        if ( TA_Base_Core::ROOT_NODE_ID == nodeId )
        {
            break;
        }

        nodeId = pCurNode->getParentId();
        ++nLevel;

        pCurNode = getTreeNode( nodeId );
    }

    FUNCTION_EXIT;
    return nLevel;
}

bool TA_Base_App::TreeNodeFactory::isTreeNodeVisible( TreeNodeId nodeId )
{
    std::map<unsigned long, int>::iterator itFound = m_planTypeMap.find( nodeId );
    if (( itFound != m_planTypeMap.end() ) && ( itFound->second == TA_Base_Core::INVISIBLE_TYPE ))
    {
        return false;
    }
    return true;
}

bool TA_Base_App::TreeNodeFactory::canTreeNodeBeShowed( TreeNodeId nodeId )
{
    return !( !Utilities::isPlanManagerConfigurationMode() && ! isTreeNodeVisible(nodeId) );
}
