/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/event_viewer/EventListComponent/src/ActionThread.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * This class performs an action in a separate thread to the main thread. This is to ensure the action
 * does not freeze the main application's update.
 *
 * Borrow from the Alarm List Control.
 */

#include "app/event/event_viewer/EventListComponent/src/stdafx.h"
#include "app/event/event_viewer/EventListComponent/src/ActionThread.h"
#include "app/event/event_viewer/EventListComponent/src/Filter.h"
#include "app/event/event_viewer/EventListComponent/src/IEventAction.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

const UINT ActionThread::PERFORM_ACTION = WM_USER + 200;

IMPLEMENT_DYNCREATE(ActionThread, CWinThread)

ActionThread::ActionThread()
{
}

ActionThread::~ActionThread()
{
}


BOOL ActionThread::InitInstance()
{
	// No per-thread initialization required here
	return TRUE;
}

int ActionThread::ExitInstance()
{
	// No per-thread cleanup required here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(ActionThread, CWinThread)
    //{{AFX_MSG_MAP(ActionThread)
	    // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_MSG_MAP
    ON_THREAD_MESSAGE(PERFORM_ACTION, performThreadedAction)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// ActionThread message handlers

void ActionThread::performThreadedAction( WPARAM wParam, LPARAM lParam )
{
    FUNCTION_ENTRY("performThreadedAction");

    IEventAction* action = reinterpret_cast<IEventAction*>(wParam);
    Filter* filter = reinterpret_cast<Filter*>(lParam);

    // Make sure all data is valid.
    if ( ( NULL == action ) || ( NULL == filter ) )
    {
        AfxEndThread(0,true);
		return;
    }

    try
    {
        // Now execute the action.
        action->executeAction( filter );
    }
    catch( ... )
    {
        // Actions are responsible for handling all exceptions they can generate.
        // If we get here just log it.
        LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Action execution threw an unexpected exception." );
    }

    AfxEndThread(0,true);
    FUNCTION_EXIT;
}
