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
#include "app/response_plans/plan_manager/src/PlanManager.h"
#include "app/response_plans/plan_manager/src/PlanEditorFrame.h"
#include "app/response_plans/plan_manager/src/PlanEditorDoc.h"
#include "app/response_plans/plan_manager/src/PlanGeneralDetailPage.h"
#include "app/response_plans/plan_manager/src/PlanStepDetailPage.h"
#include "app/response_plans/plan_manager/src/PlanFlowchartPage.h"
#include "app/response_plans/plan_manager/src/MessageBox.h"
#include "app/response_plans/plan_manager/src/PlanStep.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;
using namespace TA_Base_Bus;
using namespace TA_Base_Core;


/////////////////////////////////////////////////////////////////////////////
// CPlanEditorFrame

IMPLEMENT_DYNCREATE(CPlanEditorFrame, TransActiveFrame)

CPlanEditorFrame::CPlanEditorFrame()
{
    FUNCTION_ENTRY( "CPlanEditorFrame" );

    TA_Base_Bus::ResizingProperties properties;
    properties.canMaximise = true;
    properties.maxHeight = -1;
    properties.maxWidth = -1;
    properties.minHeight = 500;
    properties.minWidth = 900;
    setResizingProperties(properties);

    FUNCTION_EXIT;
}


CPlanEditorFrame::~CPlanEditorFrame()
{
    FUNCTION_ENTRY( "~CPlanEditorFrame" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanEditorFrame, TransActiveFrame)
    // {{AFX_MSG_MAP(CPlanEditorFrame)
    ON_WM_CREATE()
    ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
    ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateToolbarView)
    ON_MESSAGE(WM_PLAN_DETAIL_PAGE_SELECTION_CHANGED, OnPlanDetailPageSelectionChanged)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
    ID_SEPARATOR,           // status line indicator
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CPlanEditorFrame message handlers

void CPlanEditorFrame::ActivateFrame(int nCmdShow)
{
    FUNCTION_ENTRY( "ActivateFrame" );

    // TODO: Add your specialized code here and/or call the base class
    if (nCmdShow >= 0)
    {
        TransActiveFrame::ActivateFrame(nCmdShow);
    }

    FUNCTION_EXIT;
}


int CPlanEditorFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    FUNCTION_ENTRY( "OnCreate" );

    if (TransActiveFrame::OnCreate(lpCreateStruct) == -1)
    {
        FUNCTION_EXIT;
        return -1;
    }

    // TODO: Add your specialized creation code here
    if (!m_statusBar.Create(this) || !m_statusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
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
        if (!title.LoadString(IDR_PLAN_EDITOR))
        {
            title = "Plan Editor";
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


void CPlanEditorFrame::OnViewToolbar()
{
    FUNCTION_ENTRY( "OnViewToolbar" );

    // TODO: Add your command handler code here
    m_toolBarManager.toggleToolBarDisplay();

    FUNCTION_EXIT;
}


void CPlanEditorFrame::OnUpdateToolbarView(CCmdUI* pCmdUI)
{
    FUNCTION_ENTRY( "OnUpdateToolbarView" );

    // TODO: Add your command update UI handler code here
    m_toolBarManager.updateCmdUI(*pCmdUI);

    FUNCTION_EXIT;
}


LRESULT CPlanEditorFrame::OnPlanDetailPageSelectionChanged(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnPlanDetailPageSelectionChanged" );

    CPlanDetailPage *planDetail = reinterpret_cast<CPlanDetailPage *>(wParam);

    if (dynamic_cast<CPlanGeneralDetailPage *>(planDetail))
    {
        m_toolBarManager.selectToolBar(PlanEditorToolBarManager::TBT_GENERAL);
    }
    else if (dynamic_cast<CPlanStepDetailPage *>(planDetail))
    {
        m_toolBarManager.selectToolBar(PlanEditorToolBarManager::TBT_STEPS);
    }
//     else if (dynamic_cast<CPlanFlowchartPage *>(planDetail))
//     {
//         m_toolBarManager.selectToolBar(PlanEditorToolBarManager::TBT_FLOWCHART);
//     }
    else
    {
        m_toolBarManager.selectToolBar(PlanEditorToolBarManager::TBT_NONE);
    }

    FUNCTION_EXIT;
    return 0;
}


