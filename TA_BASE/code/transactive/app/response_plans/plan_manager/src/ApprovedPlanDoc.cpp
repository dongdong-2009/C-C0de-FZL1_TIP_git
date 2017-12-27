//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/ApprovedPlanDoc.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "StdAfx.h"
#include <sstream>
#include "PlanManager.h"
#include "ApprovedPlanDoc.h"
#include "TreeNodeFactory.h"
#include "TreeNode.h"
#include "RootNode.h"
#include "CategoryNode.h"
#include "PlanNode.h"
#include "LabelMaker.h"
#include "MessageBox.h"
#include "ViewUpdate.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CApprovedPlanDoc

IMPLEMENT_DYNCREATE(CApprovedPlanDoc, CPlanTreeDoc)

CApprovedPlanDoc::CApprovedPlanDoc()
{
    FUNCTION_ENTRY( "CApprovedPlanDoc" );
    FUNCTION_EXIT;
}


CApprovedPlanDoc::~CApprovedPlanDoc()
{
    FUNCTION_ENTRY( "~CApprovedPlanDoc" );
    FUNCTION_EXIT;
}


BOOL CApprovedPlanDoc::OnNewDocument()
{
    FUNCTION_ENTRY( "OnNewDocument" );

    // TODO: Add your specialized code here and/or call the base class
    if (!CPlanTreeDoc::OnNewDocument())
    {
        FUNCTION_EXIT;
        return FALSE;
    }

    FUNCTION_EXIT;
    return TRUE;
}


void CApprovedPlanDoc::OnCloseDocument()
{
    FUNCTION_ENTRY( "OnCloseDocument" );

    // TODO: Add your specialized code here and/or call the base class
    CPlanTreeDoc::OnCloseDocument();

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CApprovedPlanDoc, CPlanTreeDoc)
    // {{AFX_MSG_MAP(CApprovedPlanDoc)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CApprovedPlanDoc diagnostics

#ifdef _DEBUG
void CApprovedPlanDoc::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CPlanTreeDoc::AssertValid();

    FUNCTION_EXIT;
}


void CApprovedPlanDoc::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CPlanTreeDoc::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CApprovedPlanDoc commands

const std::string &CApprovedPlanDoc::GetRootNodeLabel()
{
    FUNCTION_ENTRY( "GetRootNodeLabel" );

    static const std::string ROOT_NODE_LABEL = (LPCTSTR) LabelMaker::getApprovedPlanTreeRootNodeLabel();

    FUNCTION_EXIT;
    return ROOT_NODE_LABEL;
}


// ITreeNodeFilter interface
bool CApprovedPlanDoc::isRelevant(TreeNode &treeNode)
{
    FUNCTION_ENTRY( "isRelevant" );

    // PW#2920: If the operator is locked into a particular category, then all that is relevant are
    // nodes at or above the locked category or below it.
    if (IsPlanTreeLocked() && !IsNodeOnLockedCategoryPath(treeNode.getNodeId()) && !IsNodeAccessible(treeNode.getNodeId()))
    {
        FUNCTION_EXIT;
        return false;
    }

    PlanNode *planNode = dynamic_cast<PlanNode *>(&treeNode);

    if (planNode)
    {
        FUNCTION_EXIT;
        return planNode->isApproved();
    }

    CategoryNode *categoryNode = dynamic_cast<CategoryNode *>(&treeNode);

    if (categoryNode)
    {
        // A category is relevant to the approved plan tree if either of the following applies:
        // 1. the category has approved descendants, or
        // 2. a plan tree lock is active and the category lies on the path leading to the locked category - this
        // allows the locked category to appear in the approved plan tree, even if it has no approved descendants.
        FUNCTION_EXIT;
        return (categoryNode->getNodeSummary().hasApprovedDescendant ||
                (IsPlanTreeLocked() && IsNodeOnLockedCategoryPath(categoryNode->getNodeId())));
    }

    FUNCTION_EXIT;
    return false;
}


