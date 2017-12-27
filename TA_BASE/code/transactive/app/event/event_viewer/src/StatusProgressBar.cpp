/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/event_viewer/src/StatusProgressBar.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * StatusProgressBar is an extesion of CStatsBar to include a CProgressCtrl in the status bar. It behaves like a normal
 * status bar, but provides methods to initialise, increment, and remove an embedded progress bar.
 */

#include "app/event/event_viewer/src/stdafx.h"
#include "app/event/event_viewer/src/EventViewer.h"
#include "app/event/event_viewer/src/StatusProgressBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// StatusProgressBar

StatusProgressBar::StatusProgressBar() : m_progressPane(0)
{
}

StatusProgressBar::~StatusProgressBar()
{
}


BEGIN_MESSAGE_MAP(StatusProgressBar, CStatusBar)
	//{{AFX_MSG_MAP(StatusProgressBar)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// StatusProgressBar message handlers

void StatusProgressBar::startProgressBar(UINT pane)
{
    // Set the pane that the progress bar is stored in. This is used for resizing
    m_progressPane = pane;

    // Get the rectangle representing the status bar pane in which the progress bar is to be placed
    CRect statusRect;
    GetItemRect(m_progressPane,&statusRect);

    // Create the progress bar - this will appear over the top of the status bar pane
    m_progressBar.Create(WS_CHILD | WS_VISIBLE,statusRect,this,IDC_PROGRESS);
    m_progressBar.SetPos(0);
    m_progressBar.ShowWindow(SW_SHOW);
}

void StatusProgressBar::setProgressRange(UINT lower,UINT upper)
{
    m_progressBar.SetRange32(lower,upper);
}

void StatusProgressBar::setProgressBarPos(UINT pos)
{
    // Find out the current range.
    int min,max;
    m_progressBar.GetRange(min,max);

    // Set the new position.
    m_progressBar.SetPos( static_cast<int>(pos)>max?max:static_cast<int>(pos) );
}

void StatusProgressBar::incrementProgressBar(UINT count)
{
    // This method guarantees that the progress bar will not increment beyond the maximum (if a standard CProgressPar is
    // incremented beyound its max, it resets to 0). To do that, need to find out where the progress bar is at the moment.
    int currentPosition = m_progressBar.GetPos();
    
    // Then find out what the current range is
    int min,max;
    m_progressBar.GetRange(min,max);

    if (currentPosition + count > (UINT)max /* cast away the signedness of max */)
    {
        // If adding "count" to the current progress bar value would push it over max, just set it to max
        m_progressBar.SetPos(max);
    }
    else
    {
        // Otherwise add count
        m_progressBar.SetPos(currentPosition + count);
    }
}

void StatusProgressBar::removeProgressBar()
{
    m_progressBar.DestroyWindow();
}

void StatusProgressBar::resizeProgressBar()
{
    if (NULL != m_progressBar.GetSafeHwnd())
    {
        // Get the new size of the status bar pane the progress bar is inside of
        CRect statusRect;
        GetItemRect(m_progressPane,&statusRect);

        // Set the progress bar to be the same size as the status bar
        m_progressBar.MoveWindow(statusRect);
    }
}

void StatusProgressBar::OnSize(UINT nType, int cx, int cy) 
{
    resizeProgressBar();

    CStatusBar::OnSize(nType, cx, cy);
}
