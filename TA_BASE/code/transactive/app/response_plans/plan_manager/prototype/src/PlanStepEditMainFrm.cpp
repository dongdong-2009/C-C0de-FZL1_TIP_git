/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanStepEditMainFrm.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanStepEditMainFrm.cpp : implementation file
//

#include "stdafx.h"
#include "planviewer.h"
#include "PlanStepEditMainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditMainFrame

IMPLEMENT_DYNCREATE(CPlanStepEditMainFrame, CFrameWnd)

CPlanStepEditMainFrame::CPlanStepEditMainFrame()
{
    FUNCTION_ENTRY( "CPlanStepEditMainFrame" );

    m_bAutoMenuEnable = FALSE;

    FUNCTION_EXIT;
}


CPlanStepEditMainFrame::~CPlanStepEditMainFrame()
{
    FUNCTION_ENTRY( "~CPlanStepEditMainFrame" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditMainFrame, CFrameWnd)
    // {{AFX_MSG_MAP(CPlanStepEditMainFrame)
    ON_WM_CREATE()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditMainFrame message handlers

BOOL CPlanStepEditMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    FUNCTION_ENTRY( "PreCreateWindow" );

    // TODO: Add your specialized code here and/or call the base class
    cs.cx = 700;
    cs.cy = 550;

    FUNCTION_EXIT;
    return CFrameWnd::PreCreateWindow(cs);
}


int CPlanStepEditMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    FUNCTION_ENTRY( "OnCreate" );

    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
    {
        FUNCTION_EXIT;
        return -1;
    }


    // TODO: Add your specialized creation code here
    // Load the edit tool bar
    UINT tbarStyle1 = TBSTYLE_FLAT | TBSTYLE_TOOLTIPS;
    UINT tbarStyle2 =  WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_ALIGN_TOP | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;
    m_toolBar.CreateEx(this, tbarStyle1, tbarStyle2);
    m_toolBar.LoadToolBar(IDR_STEPEDIT);
    m_toolBar.SetSizes(CSize(100, 44), CSize(24,24));

    // Label toolbar buttons
    int buttonIdx = 0;
    m_toolBar.SetButtonText(buttonIdx++, "Save Step");
    m_toolBar.SetButtonText(buttonIdx++, "Edit Step Parameter");
    m_toolBar.SetButtonText(buttonIdx++, "Accept Changes");

    // Adjust toolbar button sizes
    for (int i = 0; i < m_toolBar.GetCount(); i++)
    {
        m_toolBar.SetButtonStyle(i, m_toolBar.GetButtonStyle(i) | TBSTYLE_AUTOSIZE);
    }

    FUNCTION_EXIT;
    return 0;
}


