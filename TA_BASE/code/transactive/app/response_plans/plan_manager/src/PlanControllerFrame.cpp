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

#include "app/response_plans/plan_manager/src/StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"
#include "app/response_plans/plan_manager/src/resource.h"
#include "app/response_plans/plan_manager/src/PlanControllerFrame.h"
#include "app/response_plans/plan_manager/src/PlanControllerDoc.h"
#include "app/response_plans/plan_manager/src/PlanGeneralDetailPage.h"
#include "app/response_plans/plan_manager/src/PlanStepDetailPage.h"
#include "app/response_plans/plan_manager/src/PlanFlowchartPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TransActiveFrame;
using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanControllerFrame

IMPLEMENT_DYNCREATE(CPlanControllerFrame, TransActiveFrame)

CPlanControllerFrame::CPlanControllerFrame()
{
    FUNCTION_ENTRY( "CPlanControllerFrame" );

    TA_Base_Bus::ResizingProperties properties;
    properties.canMaximise = true;
    properties.maxHeight = -1;
    properties.maxWidth = -1;
    properties.minHeight = 500;
    properties.minWidth = 900;
    setResizingProperties(properties);

    FUNCTION_EXIT;
}


CPlanControllerFrame::~CPlanControllerFrame()
{
    FUNCTION_ENTRY( "~CPlanControllerFrame" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanControllerFrame, TransActiveFrame)
    // {{AFX_MSG_MAP(CPlanControllerFrame)
    ON_WM_CREATE()
    ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateToolbarView)
    ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
    ON_NOTIFY(TBN_DROPDOWN, IDR_PLAN_CONTROLLER_GENERAL_TOOLBAR, OnButtonDropDown)
    ON_NOTIFY(TBN_DROPDOWN, IDR_PLAN_CONTROLLER_STEPS_TOOLBAR, OnButtonDropDown)
    ON_NOTIFY(TBN_DROPDOWN, IDR_PLAN_CONTROLLER_FLOWCHART_TOOLBAR, OnButtonDropDown)
    // }}AFX_MSG_MAP
    ON_MESSAGE(WM_PLAN_DETAIL_PAGE_SELECTION_CHANGED, OnPlanDetailPageSelectionChanged)
END_MESSAGE_MAP()

static UINT indicators[] =
{
    ID_SEPARATOR,           // status line indicator
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};


/////////////////////////////////////////////////////////////////////////////
// CPlanControllerFrame diagnostics

#ifdef _DEBUG
void CPlanControllerFrame::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    TransActiveFrame::AssertValid();

    FUNCTION_EXIT;
}


void CPlanControllerFrame::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    TransActiveFrame::Dump(dc);

    FUNCTION_EXIT;
}


CPlanControllerDoc* CPlanControllerFrame::GetDocument() // non-debug version is inline
{
    FUNCTION_ENTRY( "GetDocument" );

    ASSERT(GetActiveDocument()->IsKindOf(RUNTIME_CLASS(CPlanControllerDoc)));

    FUNCTION_EXIT;
    return (CPlanControllerDoc*)GetActiveDocument();
}


#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CPlanControllerFrame message handlers

void CPlanControllerFrame::ActivateFrame(int nCmdShow)
{
    FUNCTION_ENTRY( "ActivateFrame" );

    // TODO: Add your specialized code here and/or call the base class
    if (nCmdShow >= 0)
    {
        TransActiveFrame::ActivateFrame(nCmdShow);
    }

    FUNCTION_EXIT;
}


int CPlanControllerFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    FUNCTION_ENTRY( "OnCreate" );

    if (TransActiveFrame::OnCreate(lpCreateStruct) == -1)
    {
        FUNCTION_EXIT;
        return -1;
    }

    // TODO: Add your specialized creation code here
    if (!m_statusBar.Create(this) ||
        !m_statusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
    {
        TRACE0("Failed to create status bar\n");

        FUNCTION_EXIT;
        return -1;      // fail to create
    }

    m_toolBarManager.initialise(this);

    try
    {
        // Include in the title the location of the Plan Agent used to start plans.
        CString title;
        if (!title.LoadString(IDR_PLAN_CONTROLLER))
        {
            title = "Plan Controller";
        }

        title += " - ";
        title += TA_Base_Bus::PlanAgentLocationAccess::getInstance().getDefaultPlanAgentLocationName().c_str();

        SetTitle(title);
    }
    catch (...)
    {
        // Go with the default title
    }

    FUNCTION_EXIT;
    return 0;
}


void CPlanControllerFrame::OnViewToolbar()
{
    FUNCTION_ENTRY( "OnViewToolbar" );

    // TODO: Add your command handler code here
    m_toolBarManager.toggleToolBarDisplay();

    FUNCTION_EXIT;
}


void CPlanControllerFrame::OnUpdateToolbarView(CCmdUI* pCmdUI)
{
    FUNCTION_ENTRY( "OnUpdateToolbarView" );

    // TODO: Add your command update UI handler code here
    m_toolBarManager.updateCmdUI(*pCmdUI);

    FUNCTION_EXIT;
}


void CPlanControllerFrame::OnButtonDropDown(NMHDR* pNMHDR, LRESULT *pResult)
{
    FUNCTION_ENTRY( "OnButtonDropDown" );

    NMTOOLBAR* pnmtb = reinterpret_cast<NMTOOLBAR*>(pNMHDR);

    if (pnmtb->iItem == ID_PLAN_EXECUTE)
    {
        CMenu* startMenu = GetMenu()->GetSubMenu(2)->GetSubMenu(0);

        CRect boundingRect = m_toolBarManager.getBoundingRect(pnmtb->iItem);
        startMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
                                  boundingRect.left, boundingRect.bottom, this, &boundingRect);
    }

    *pResult = 0;

    FUNCTION_EXIT;
}


LRESULT CPlanControllerFrame::OnPlanDetailPageSelectionChanged(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnPlanDetailPageSelectionChanged" );

    CPlanDetailPage *planDetail = reinterpret_cast<CPlanDetailPage *>(wParam);

    if (dynamic_cast<CPlanGeneralDetailPage *>(planDetail))
    {
        m_toolBarManager.selectToolBar(PlanControllerToolBarManager::TBT_GENERAL);
    }
    else if (dynamic_cast<CPlanStepDetailPage *>(planDetail))
    {
        m_toolBarManager.selectToolBar(PlanControllerToolBarManager::TBT_STEPS);
    }
//     else if (dynamic_cast<CPlanFlowchartPage *>(planDetail))
//     {
//         m_toolBarManager.selectToolBar(PlanControllerToolBarManager::TBT_FLOWCHART);
//     }
    else
    {
        m_toolBarManager.selectToolBar(PlanControllerToolBarManager::TBT_NONE);
    }

    FUNCTION_EXIT;
    return 0;
}
