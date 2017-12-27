/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanEditMainFrm.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanEditMainFrm.cpp

#include "StdAfx.h"
#include "PlanEditMainFrm.h"
#include "MainFrm.h"
#include "Resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlanEditMainFrame

IMPLEMENT_DYNCREATE(CPlanEditMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CPlanEditMainFrame, CFrameWnd)
    // {{AFX_MSG_MAP(CPlanEditMainFrame)
    ON_WM_CREATE()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanEditMainFrame construction/destruction

CPlanEditMainFrame::CPlanEditMainFrame()
{
    FUNCTION_ENTRY( "CPlanEditMainFrame" );

    m_bAutoMenuEnable = FALSE;

    FUNCTION_EXIT;
}


CPlanEditMainFrame::~CPlanEditMainFrame()
{
    FUNCTION_ENTRY( "~CPlanEditMainFrame" );
    FUNCTION_EXIT;
}


BOOL CPlanEditMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    FUNCTION_ENTRY( "PreCreateWindow" );

    // TODO: Add your specialized code here and/or call the base class
    CMainFrame* mainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
    RECT rect;
    mainFrame->GetWindowRect(&rect);
    cs.cx = rect.right - rect.left - 100;
    cs.cy = rect.bottom - rect.top - 100;

    FUNCTION_EXIT;
    return CFrameWnd::PreCreateWindow(cs);
}


int CPlanEditMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    FUNCTION_ENTRY( "OnCreate" );

    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
    {
        FUNCTION_EXIT;
        return -1;
    }


    // Load the edit tool bar
    UINT tbarStyle1 = TBSTYLE_FLAT | TBSTYLE_TOOLTIPS;
    UINT tbarStyle2 =  WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_ALIGN_TOP | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;
    m_editToolBar.CreateEx(this, tbarStyle1, tbarStyle2);
    m_editToolBar.LoadToolBar(IDR_PLANEDIT);
    m_editToolBar.SetSizes(CSize(100, 44), CSize(24,24));


    // Label edit toolbar buttons
    int buttonIdx = 0;
    m_editToolBar.SetButtonText(buttonIdx++, "Save Plan");
    m_editToolBar.SetButtonText(buttonIdx++, "Approve Plan");
    m_editToolBar.SetButtonText(buttonIdx++, "Unapprove Plan");
    buttonIdx++; // skip the separator
    m_editToolBar.SetButtonText(buttonIdx++, "New Step");
    m_editToolBar.SetButtonText(buttonIdx++, "Delete Step");
    m_editToolBar.SetButtonText(buttonIdx++, "Cut Step");
    m_editToolBar.SetButtonText(buttonIdx++, "Copy Step");
    m_editToolBar.SetButtonText(buttonIdx++, "Paste Step");
    m_editToolBar.SetButtonText(buttonIdx++, "Edit Step");

    // Adjust edit toolbar button sizes
    for (int i = 0; i < m_editToolBar.GetCount(); i++)
    {
        m_editToolBar.SetButtonStyle(i, m_editToolBar.GetButtonStyle(i) | TBSTYLE_AUTOSIZE);
    }

    FUNCTION_EXIT;
    return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CPlanEditMainFrame diagnostics

#ifdef _DEBUG
void CPlanEditMainFrame::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CFrameWnd::AssertValid();

    FUNCTION_EXIT;
}


void CPlanEditMainFrame::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CFrameWnd::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG


