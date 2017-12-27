/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/ApplicationMover.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This thread is linked to a single instance of a RunningApplication. After a set period
  * of time it will attempt to move the application into the specified position.
  * 
  */

#include "app/system_control/control_station/src/StdAfx.h"
#include "app/system_control/control_station/src/ApplicationMover.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::ThreadGuard;
using TA_Base_Core::DebugUtil;


ApplicationMover::ApplicationMover( )
    : m_isTerminating(false)
{
    m_moveInformation.processId = 0;
    m_moveInformation.posFlag = 0;
    m_moveInformation.alignFlag = 0;
    m_moveInformation.objectDim = TA_Base_Bus::TA_GenericGui::DEFAULT_RECT;
    m_moveInformation.boundaryDim = TA_Base_Bus::TA_GenericGui::DEFAULT_RECT;
    m_moveInformation.appDim = TA_Base_Bus::TA_GenericGui::DEFAULT_RECT;
};

ApplicationMover::~ApplicationMover()
{
}


void ApplicationMover::setProcessDetails(const unsigned long processId,
                                        const unsigned long posFlag,
                                        const unsigned long alignFlag,
                                        const RECT& objectDim,
                                        const RECT& boundaryDim)
{
    m_moveInformation.processId = processId;
    m_moveInformation.posFlag = posFlag;
    m_moveInformation.alignFlag = alignFlag;
    m_moveInformation.objectDim = objectDim;
    m_moveInformation.boundaryDim = boundaryDim;
    m_moveInformation.appDim = TA_Base_Bus::TA_GenericGui::DEFAULT_RECT;
    m_moveInformation.finishedRepositioning = false;
};



void ApplicationMover::run()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "run");

    try
    {
        if ((m_moveInformation.processId == 0) &&
            (m_moveInformation.posFlag == 0))
        {
            // Nothign has been configured so we have nothing to move.
            return;
        }

        int maxTries = 5;
        int numTries = 0;

        // If we haven't been terminated in the meantime we can now attempt to move the window
        while ( !m_moveInformation.finishedRepositioning && !m_isTerminating && (numTries < maxTries) )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Attempt %d to move application %d", numTries++, static_cast<int>(m_moveInformation.processId));
            ::EnumWindows( WndPositionEnumHandler, reinterpret_cast<long>(&m_moveInformation) );
            // Give the window time to appear.
            Thread::sleep(100);
        }
    }
    catch ( ... )
    {
        // Catch all exceptions as there is nothing outside this method to catch them.
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "run() of ApplicationMover threw an unknown exception");
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "run");
}


BOOL CALLBACK ApplicationMover::WndPositionEnumHandler(HWND hWnd, LPARAM lParam)
{
    WindowMoveInformation* info = reinterpret_cast<WindowMoveInformation*>(lParam);
	if (info == NULL)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "info is NULL");
        return FALSE;
    }

	// find out which process & thread created this window
	DWORD dwThisWindowProcessID;
	DWORD dwThisWindowThreadID;
	dwThisWindowThreadID = ::GetWindowThreadProcessId(hWnd, &dwThisWindowProcessID);

	// if it's the process we are interested in then we want to move it
	if (info->processId == dwThisWindowProcessID)
	{
        // Create a temporary window
        CWnd tempWindow;
        tempWindow.Attach(hWnd);

        // Get some information about it to help determine if it is the correct one
        CWnd* parent = tempWindow.GetParent();
        BOOL isVisible = tempWindow.IsWindowVisible();

        if( parent != NULL || !isVisible )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "parent != NULL || !isVisible");
            // This child is not visible or it is not the parent window
            // Therefore we are not interested.
            tempWindow.Detach();
            return TRUE;
        }

        RECT windowRect;
        tempWindow.GetWindowRect(&windowRect);
        bool isRepositioned = TA_Base_Bus::AppLauncher::calculateNewCoordinates(windowRect, info->objectDim, info->boundaryDim, info->posFlag, info->alignFlag);
        // if its coordinates are changed, then move to the new coordinates, otherwise, centre the
        // application without changing the size of the application.

        if (isRepositioned)
        {
            info->appDim = windowRect;
            tempWindow.MoveWindow(&windowRect);
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Moved to: (%li,%li,%li,%li)", windowRect.left, windowRect.top, windowRect.right, windowRect.bottom);
        }
        else
        {
            tempWindow.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE);
            // we need to get this using the GetWindowRect method as the app is centred by the MFC library
            // and we don't know what the new coordinates are.
            tempWindow.GetWindowRect(&info->appDim);
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Either no positioning flag is defined or there's some problems with the arguments, centering application instead.");
        }

        // TD638 - Set the window to be the top most so it is given focus and then move it back again.
        tempWindow.SetForegroundWindow();
        tempWindow.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
        tempWindow.SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);

        // Now detach so that when this variable goes out of scope we don't kill the HWND.
        tempWindow.Detach();

        info->finishedRepositioning = true;

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Successfully moved application %d", static_cast<int>(info->processId));

        // We have found and repositioned the window so now we can return FALSE to indicate
        // it has been done.
        return FALSE;
	}
	
	return TRUE;
}


void ApplicationMover::terminate()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "terminate");
    m_isTerminating = true;
    LOG( SourceInfo, DebugUtil::FunctionExit, "terminate");
}


void ApplicationMover::resetThread()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "resetThread");
    m_isTerminating = false;
    LOG( SourceInfo, DebugUtil::FunctionExit, "resetThread");
}
