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
#include "app/response_plans/plan_manager/src/AbstractToolBarManager.h"
#include "app/response_plans/plan_manager/src/ApplicationFont.h"

#include "ace/Tokenizer_T.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// AbstractToolBarManager

AbstractToolBarManager::AbstractToolBarManager() :
    m_toolBarParent(NULL), m_selectedToolBarIdx(-1), m_isToolBarVisible(true)
{
    FUNCTION_ENTRY( "AbstractToolBarManager" );
    FUNCTION_EXIT;
}


AbstractToolBarManager::~AbstractToolBarManager()
{
    FUNCTION_ENTRY( "~AbstractToolBarManager" );

    for (ToolBarList::iterator iter = m_toolBars.begin(); iter != m_toolBars.end(); iter++)
    {
        CToolBar *toolBar = *iter;

        toolBar->DestroyWindow();
        delete toolBar;
    }

    m_toolBars.clear();

    FUNCTION_EXIT;
}


void AbstractToolBarManager::initialise(CFrameWnd *toolBarParent)
{
    FUNCTION_ENTRY( "initialise" );

    TA_ASSERT(toolBarParent != NULL, "Specified parent frame pointer is null");

    m_toolBarParent = toolBarParent;

    // Create all managed toolbars
    initialiseToolBarList();

    FUNCTION_EXIT;
}


void AbstractToolBarManager::toggleToolBarDisplay()
{
    FUNCTION_ENTRY( "toggleToolBarDisplay" );

    TA_ASSERT(m_toolBarParent != NULL, "Undefined toolbar parent frame");

    if (isToolBarEnabled())
    {
        m_isToolBarVisible = !isToolBarVisible();

        m_toolBars[m_selectedToolBarIdx]->ShowWindow(m_isToolBarVisible? SW_SHOW : SW_HIDE);

        m_toolBarParent->RecalcLayout(FALSE);
    }

    FUNCTION_EXIT;
}


bool AbstractToolBarManager::isToolBarEnabled() const
{
    FUNCTION_ENTRY( "isToolBarEnabled" );
    FUNCTION_EXIT;
    return m_selectedToolBarIdx >= 0 && m_selectedToolBarIdx < static_cast<int>(m_toolBars.size());
}


bool AbstractToolBarManager::isToolBarVisible() const
{
    FUNCTION_ENTRY( "isToolBarVisible" );

    if (isToolBarEnabled())
    {
        FUNCTION_EXIT;
        return m_toolBars[m_selectedToolBarIdx]->IsWindowVisible() == TRUE;
    }

    FUNCTION_EXIT;
    return false;
}


CRect AbstractToolBarManager::getBoundingRect(int commandID)
{
    FUNCTION_ENTRY( "getBoundingRect" );

    CRect boundingRect(0, 0, 0, 0);

    if (isToolBarEnabled())
    {
        CToolBar *toolBar = m_toolBars[m_selectedToolBarIdx];

        toolBar->GetToolBarCtrl().GetRect(commandID, &boundingRect);
        toolBar->GetToolBarCtrl().ClientToScreen(&boundingRect);
    }

    FUNCTION_EXIT;
    return boundingRect;
}


void AbstractToolBarManager::updateCmdUI(CCmdUI &cmdUI)
{
    FUNCTION_ENTRY( "updateCmdUI" );

    if (cmdUI.m_nID == ID_VIEW_TOOLBAR)
    {
        cmdUI.SetCheck(isToolBarVisible());
        cmdUI.Enable(isToolBarEnabled());
    }

    FUNCTION_EXIT;
}


CToolBar *AbstractToolBarManager::addToolBarToList(UINT toolBarResourceId)
{
    FUNCTION_ENTRY( "addToolBarToList" );

    m_toolBars.push_back(createToolBar(toolBarResourceId));

    FUNCTION_EXIT;
    return m_toolBars[m_toolBars.size() - 1];
}


void AbstractToolBarManager::selectToolBar(int toolBarIdx)
{
    FUNCTION_ENTRY( "selectToolBar" );

    TA_ASSERT(m_toolBarParent != NULL, "Undefined toolbar parent frame");

    m_selectedToolBarIdx = toolBarIdx;

    for (int idx = 0; idx < static_cast<int>(m_toolBars.size()); idx++)
    {
        CToolBar *toolBar = m_toolBars[idx];

        toolBar->ShowWindow(idx == m_selectedToolBarIdx && m_isToolBarVisible? SW_SHOW : SW_HIDE);
    }

    m_toolBarParent->RecalcLayout(FALSE);

    FUNCTION_EXIT;
}


CToolBar *AbstractToolBarManager::createToolBar(UINT toolBarID)
{
    FUNCTION_ENTRY( "createToolBar" );

    DWORD style1 = TBSTYLE_FLAT | TBSTYLE_WRAPABLE | TBSTYLE_TOOLTIPS;
    DWORD style2 = WS_CHILD | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;

    // Create and load the toolbar
    CToolBar *toolBar = new CToolBar();
    toolBar->CreateEx(m_toolBarParent, style1, style2, CRect(0,0,0,0), toolBarID);
    toolBar->LoadToolBar(toolBarID);
    toolBar->SetSizes(CSize(50, 44), CSize(24,24));

    // Add a font to fit auto size button text width
    ApplicationFont::getInstance().adjustChildrenFont( toolBar->GetSafeHwnd() );

    // Label buttons and auto-adjust their width
    CString buttonLabels;
    buttonLabels.LoadString(toolBarID);

    ACE_Tokenizer tok(buttonLabels.GetBuffer(buttonLabels.GetLength()));
    tok.delimiter_replace('\n', 0);
    char* label = tok.next();

    for (int i = 0; i < toolBar->GetCount(); i++)
    {
        TA_ASSERT(label != NULL, "Missing toolbar button labels");

        UINT buttonStyle = toolBar->GetButtonStyle(i);
        if ((buttonStyle & TBBS_SEPARATOR) != TBBS_SEPARATOR)
        {
            toolBar->SetButtonText(i, label);
            toolBar->SetButtonStyle(i, buttonStyle | TBSTYLE_AUTOSIZE);
            label = tok.next();
        }
    }

    buttonLabels.ReleaseBuffer();

    FUNCTION_EXIT;
    return toolBar;
}
