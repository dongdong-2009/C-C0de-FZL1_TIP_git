/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanCatEditMainFrm.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanCatEditMainFrm.cpp : implementation file
//

#include "stdafx.h"
#include "planviewer.h"
#include "PlanCatEditMainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlanCatEditMainFrame

IMPLEMENT_DYNCREATE(CPlanCatEditMainFrame, CFrameWnd)

CPlanCatEditMainFrame::CPlanCatEditMainFrame()
{
    FUNCTION_ENTRY( "CPlanCatEditMainFrame" );

    m_bAutoMenuEnable = FALSE;

    FUNCTION_EXIT;
}


CPlanCatEditMainFrame::~CPlanCatEditMainFrame()
{
    FUNCTION_ENTRY( "~CPlanCatEditMainFrame" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanCatEditMainFrame, CFrameWnd)
    // {{AFX_MSG_MAP(CPlanCatEditMainFrame)
    ON_WM_CREATE()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanCatEditMainFrame message handlers

BOOL CPlanCatEditMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    FUNCTION_ENTRY( "PreCreateWindow" );

    // TODO: Add your specialized code here and/or call the base class
    cs.cx = 700;
    cs.cy = 500;

    FUNCTION_EXIT;
    return CFrameWnd::PreCreateWindow(cs);
}


int CPlanCatEditMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    FUNCTION_ENTRY( "OnCreate" );

    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
    {
        FUNCTION_EXIT;
        return -1;
    }


    // TODO: Add your specialized creation code here
    UINT tbarStyle1 = TBSTYLE_FLAT | TBSTYLE_LIST | TBSTYLE_TOOLTIPS;
    UINT tbarStyle2 =  WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_ALIGN_TOP | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;
    m_toolBar.CreateEx(this, tbarStyle1, tbarStyle2);
    m_toolBar.LoadToolBar(IDR_CATEDIT);

    // Label toolbar buttons
    int buttonIdx = 0;
    m_toolBar.SetButtonText(buttonIdx++, "Save Category");

    // Adjust toolbar button sizes
    for (int i = 0; i < m_toolBar.GetCount(); i++)
    {
        m_toolBar.SetButtonStyle(i, m_toolBar.GetButtonStyle(i) | TBSTYLE_AUTOSIZE);
    }

    // Disable menus and buttons
    GetMenu()->EnableMenuItem(ID_CAT_SAVE, MF_BYCOMMAND | MF_GRAYED);
    m_toolBar.GetToolBarCtrl().EnableButton(ID_CAT_SAVE, FALSE);

    FUNCTION_EXIT;
    return 0;
}


