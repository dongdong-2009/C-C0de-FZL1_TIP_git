//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/AllPlanDoc.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "StdAfx.h"
#include "PlanManager.h"
#include "AllPlanDoc.h"
#include "TreeNodeFactory.h"
#include "RootNode.h"
#include "LabelMaker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CAllPlanDoc

IMPLEMENT_DYNCREATE(CAllPlanDoc, CPlanTreeDoc)

CAllPlanDoc::CAllPlanDoc()
{
    FUNCTION_ENTRY( "CAllPlanDoc" );
    FUNCTION_EXIT;
}


CAllPlanDoc::~CAllPlanDoc()
{
    FUNCTION_ENTRY( "~CAllPlanDoc" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CAllPlanDoc, CPlanTreeDoc)
    // {{AFX_MSG_MAP(CAllPlanDoc)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAllPlanDoc diagnostics

#ifdef _DEBUG
void CAllPlanDoc::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CPlanTreeDoc::AssertValid();

    FUNCTION_EXIT;
}


void CAllPlanDoc::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CPlanTreeDoc::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CAllPlanDoc commands

const std::string &CAllPlanDoc::GetRootNodeLabel()
{
    FUNCTION_ENTRY( "GetRootNodeLabel" );

    static const std::string ROOT_NODE_LABEL = (LPCTSTR) LabelMaker::getAllPlanTreeRootNodeLabel();

    FUNCTION_EXIT;
    return ROOT_NODE_LABEL;
}


// ITreeNodeFilter interface
bool CAllPlanDoc::isRelevant(TreeNode &treeNode)
{
    FUNCTION_ENTRY( "isRelevant" );

    // PW#2920: If the operator is locked into a particular category, then all that is relevant are
    // nodes at or above the locked category or below it.
    if (IsPlanTreeLocked())
    {
        FUNCTION_EXIT;
        return (IsNodeOnLockedCategoryPath(treeNode.getNodeId()) || IsNodeAccessible(treeNode.getNodeId()));
    }

    FUNCTION_EXIT;
    return true;
}


