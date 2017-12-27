//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/AllPlanTreeView.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "PlanManager.h"
#include "AllPlanTreeView.h"
#include "AllPlanDoc.h"
#include "TreeNodeFactory.h"
#include "PlanNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CAllPlanTreeView

IMPLEMENT_DYNCREATE(CAllPlanTreeView, CPlanTreeView)

CAllPlanTreeView::CAllPlanTreeView()
{
    FUNCTION_ENTRY( "CAllPlanTreeView" );
    FUNCTION_EXIT;
}


CAllPlanTreeView::~CAllPlanTreeView()
{
    FUNCTION_ENTRY( "~CAllPlanTreeView" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CAllPlanTreeView, CPlanTreeView)
    // {{AFX_MSG_MAP(CAllPlanTreeView)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAllPlanTreeView diagnostics

#ifdef _DEBUG
void CAllPlanTreeView::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CTreeView::AssertValid();

    FUNCTION_EXIT;
}


void CAllPlanTreeView::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CTreeView::Dump(dc);

    FUNCTION_EXIT;
}


CAllPlanDoc* CAllPlanTreeView::GetDocument() // non-debug version is inline
{
    FUNCTION_ENTRY( "GetDocument" );

    CPlanTreeDoc *document = CPlanTreeView::GetDocument();
    ASSERT(document->IsKindOf(RUNTIME_CLASS(CAllPlanDoc)));

    FUNCTION_EXIT;
    return (CAllPlanDoc*)document;
}


#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CAllPlanTreeView message handlers
