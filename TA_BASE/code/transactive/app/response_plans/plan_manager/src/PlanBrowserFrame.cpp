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

#include "StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "MainFrame.h"
#include "PlanBrowserFrame.h"
#include "AllPlanTreeFrame.h"
#include "ApprovedPlanTreeFrame.h"
#include "UnapprovedPlanTreeFrame.h"
#include "ActivePlanFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanBrowserFrame

IMPLEMENT_DYNCREATE(CPlanBrowserFrame, CMDIChildWnd)

CPlanBrowserFrame::CPlanBrowserFrame()
{
    FUNCTION_ENTRY( "CPlanBrowserFrame" );
    FUNCTION_EXIT;
}


CPlanBrowserFrame::~CPlanBrowserFrame()
{
    FUNCTION_ENTRY( "~CPlanBrowserFrame" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanBrowserFrame, CMDIChildWnd)
    // {{AFX_MSG_MAP(CPlanBrowserFrame)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanBrowserFrame message handlers

CMainFrame *CPlanBrowserFrame::GetMainFrame()
{
    FUNCTION_ENTRY( "GetMainFrame" );

    CMDIFrameWnd *mdiFrame = GetMDIFrame();
    ASSERT(mdiFrame);
    CMainFrame *mainFrame = DYNAMIC_DOWNCAST(CMainFrame, mdiFrame);

    FUNCTION_EXIT;
    return mainFrame;
}


BOOL CPlanBrowserFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    FUNCTION_ENTRY( "PreCreateWindow" );

    // TODO: Modify the Window class or styles here by modifying
    // the CREATESTRUCT cs

    if( !CMDIChildWnd::PreCreateWindow(cs) )
    {
        FUNCTION_EXIT;
        return FALSE;
    }


    cs.style = WS_CHILD & !WS_VISIBLE;

    FUNCTION_EXIT;
    return TRUE;
}


void CPlanBrowserFrame::ActivateFrame(int nCmdShow)
{
    FUNCTION_ENTRY( "ActivateFrame" );

    // TODO: Modify this function to change how the frame is activated.
    nCmdShow = SW_SHOWMAXIMIZED;

    CMDIChildWnd::ActivateFrame(nCmdShow);

    UpdateWindow();
    SendMessageToDescendants(WM_WINDOW_HAS_BECOME_VISIBLE);

    FUNCTION_EXIT;
}


