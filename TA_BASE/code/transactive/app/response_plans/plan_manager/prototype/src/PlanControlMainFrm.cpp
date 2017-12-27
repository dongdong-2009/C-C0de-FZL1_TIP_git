/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanControlMainFrm.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanControlMainFrm.cpp

#include "StdAfx.h"
#include "PlanControlMainFrm.h"
#include "MainFrm.h"
#include "Resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlanControlMainFrame

IMPLEMENT_DYNCREATE(CPlanControlMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CPlanControlMainFrame, CFrameWnd)
    // {{AFX_MSG_MAP(CPlanControlMainFrame)
    ON_WM_CREATE()
    ON_NOTIFY(TBN_DROPDOWN, IDR_PLANCTRL, OnButtonDropDown)
    ON_COMMAND(ID_PLAN_EXECUTE, OnStartPlan)
    ON_COMMAND(ID_PLAN_EXECUTE_CUSTOM, OnCustomisePlan)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanControlMainFrame construction/destruction

CPlanControlMainFrame::CPlanControlMainFrame()
{
    FUNCTION_ENTRY( "CPlanControlMainFrame" );

    m_bAutoMenuEnable = FALSE;

    FUNCTION_EXIT;
}


CPlanControlMainFrame::~CPlanControlMainFrame()
{
    FUNCTION_ENTRY( "~CPlanControlMainFrame" );
    FUNCTION_EXIT;
}


BOOL CPlanControlMainFrame::PreCreateWindow(CREATESTRUCT& cs)
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


int CPlanControlMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    FUNCTION_ENTRY( "OnCreate" );

    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
    {
        FUNCTION_EXIT;
        return -1;
    }


    // Load the control tool bar
    UINT tbarStyle1 = TBSTYLE_FLAT | TBSTYLE_TOOLTIPS;
    UINT tbarStyle2 =  WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_ALIGN_TOP | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;
    m_ctrlToolBar.CreateEx(this, tbarStyle1, tbarStyle2, CRect(0,0,0,0), IDR_PLANCTRL);
    m_ctrlToolBar.GetToolBarCtrl().SetExtendedStyle(m_ctrlToolBar.GetToolBarCtrl().GetExtendedStyle() | TBSTYLE_EX_DRAWDDARROWS);
    m_ctrlToolBar.LoadToolBar(IDR_PLANCTRL);
    m_ctrlToolBar.SetSizes(CSize(100, 44), CSize(24,24));

    // Label control toolbar buttons
    int buttonIdx = 0;
    m_ctrlToolBar.SetButtonText(buttonIdx++, "Start");
    m_ctrlToolBar.SetButtonText(buttonIdx++, "Stop");
    m_ctrlToolBar.SetButtonText(buttonIdx++, "Pause");
    m_ctrlToolBar.SetButtonText(buttonIdx++, "Resume");
    m_ctrlToolBar.SetButtonText(buttonIdx++, "Take Control");
    buttonIdx++; // skip the separator
    m_ctrlToolBar.SetButtonText(buttonIdx++, "Schedule");
    m_ctrlToolBar.SetButtonText(buttonIdx++, "Customise Step");

    // Adjust control toolbar button sizes
    for (int i = 0; i < m_ctrlToolBar.GetCount(); i++)
    {
        m_ctrlToolBar.SetButtonStyle(i, m_ctrlToolBar.GetButtonStyle(i) | TBSTYLE_AUTOSIZE | (i? 0 : TBSTYLE_DROPDOWN));
    }

    FUNCTION_EXIT;
    return 0;
}


void CPlanControlMainFrame::OnButtonDropDown(NMTOOLBAR* pnmtb, LRESULT *plr)
{
    FUNCTION_ENTRY( "OnButtonDropDown" );

    if (pnmtb->iItem == ID_PLAN_EXECUTE)
    {
        CMenu* executeMenu = GetMenu()->GetSubMenu(1)->GetSubMenu(0);

        CRect boundingRect;
        m_ctrlToolBar.GetToolBarCtrl().GetRect(pnmtb->iItem, &boundingRect);
        m_ctrlToolBar.GetToolBarCtrl().ClientToScreen(&boundingRect);

        executeMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
                                    boundingRect.left, boundingRect.bottom, this, &boundingRect);
    }

    FUNCTION_EXIT;
}


void CPlanControlMainFrame::OnStartPlan()
{
    FUNCTION_ENTRY( "OnStartPlan" );

    TRACE0("** Starting new instance **\n");

    FUNCTION_EXIT;
}


void CPlanControlMainFrame::OnCustomisePlan()
{
    FUNCTION_ENTRY( "OnCustomisePlan" );

    TRACE0("** Customising new instance **\n");

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// CPlanControlMainFrame diagnostics

#ifdef _DEBUG
void CPlanControlMainFrame::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CFrameWnd::AssertValid();

    FUNCTION_EXIT;
}


void CPlanControlMainFrame::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CFrameWnd::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG


