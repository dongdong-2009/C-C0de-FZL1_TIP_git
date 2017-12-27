/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/ControlStationCorbaImpl.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This receives callbacks from the ManagedProcesses and will allow applications to be launched.
  * It also receives callbacks when the services have been restarted.
  * This implements the IControlStationCorbaDef corba definition which means this
  * object is what the ManagedProcess's get when they resolve the Control Station.
  */

#include "app\system_control\control_station\src\StdAfx.h"
#include "app\system_control\control_station\src\ControlStationCorbaImpl.h"
#include "app\system_control\control_station\src\SessionManager.h"
#include "app\system_control\control_station\src\IProcessManager.h"
#include "app\system_control\control_station\src\ControlStationConstants.h"
#include "app\system_control\control_station\src\ControlStationRootDialog.h"

#include "core\exceptions\src\ApplicationException.h"
#include "core\utilities\src\DebugUtil.h"
#include "core\utilities\src\RunParams.h"


using TA_Base_Core::DebugUtil;

ControlStationCorbaImpl::ControlStationCorbaImpl(const std::string& consoleName)
{
    TA_Base_Core::CorbaUtil::getInstance().setServantKey(this, "ControlStation");

    activateServant();
}


ControlStationCorbaImpl::~ControlStationCorbaImpl()
{
}

void ControlStationCorbaImpl::launchAnApplication( unsigned long appType, 
                                                   const char* extraCommandLine, 
                                                   unsigned long posFlag,
                                                   unsigned long alignFlag,
                                                   const TA_Base_Core::IControlStationCorbaDef::RECT& objectDim,
                                                   const TA_Base_Core::IControlStationCorbaDef::RECT& boundaryDim)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "launchAnApplication");
    try
    {
        IProcessManager* processManager = SessionManager::getInstance().getProcessManager();
        if (processManager == NULL)
        {
            LOG( SourceInfo, DebugUtil::ExceptionConstruction, "IControlStationCorbaDef::ApplicationException", "Application could not be launched as there is no ProcessManager object.");
            TA_Base_Core::IControlStationCorbaDef::ApplicationException ex;
            throw ex;
        }

        RECT objectRect;
        objectRect.left = objectDim.left;
        objectRect.top = objectDim.top;
        objectRect.right = objectDim.right;
        objectRect.bottom = objectDim.bottom;

        RECT boundaryRect;
        boundaryRect.left = boundaryDim.left;
        boundaryRect.top = boundaryDim.top;
        boundaryRect.right = boundaryDim.right;
        boundaryRect.bottom = boundaryDim.bottom;

        // Pass the application type and command line.
        processManager->launchApplication(appType, std::string(extraCommandLine), posFlag, alignFlag, objectRect, boundaryRect);

    }
    catch ( TA_Base_Core::ApplicationException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionConstruction, "IControlStationCorbaDef::ApplicationException", "Application could not be launched. Since this is a CORBA method we must throw a non-TransActive exception");
        TA_Base_Core::IControlStationCorbaDef::ApplicationException ex;
        throw ex;
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "launchAnApplication");
}


TA_Base_Core::IControlStationCorbaDef::RECT ControlStationCorbaImpl::getRect(
                                                    unsigned long targetScreen, 
                                                    unsigned long targetArea, 
                                                    long val)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "getRect");
    RECT rect = SessionManager::getInstance().getScreenPositioning().getRect(
                                static_cast<TA_Base_Bus::TA_GenericGui::EScreen>(targetScreen), 
                                static_cast<TA_Base_Bus::TA_GenericGui::EArea>(targetArea), 
                                val);
    TA_Base_Core::IControlStationCorbaDef::RECT retval;
    retval.left = rect.left;
    retval.right = rect.right;
    retval.top = rect.top;
    retval.bottom = rect.bottom;
    return retval;
}


void ControlStationCorbaImpl::launchDisplay(const char* display, const char* asset, long leftPosition)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "launchDisplay");
    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Trying to launch display %s with asset %s on screen %li", display, asset, leftPosition);

    DisplayInformation* info = new struct DisplayInformation;

    if (info != NULL)
    {
        info->display = CString(display);
        info->asset = CString(asset);
        info->position = leftPosition;
        ControlStationRootDialog::getInstance().PostMessage(WM_LAUNCH_DISPLAY, (WPARAM)info);
    }
    else
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "new returned a NULL for display info");
        TA_Base_Core::IControlStationCorbaDef::ApplicationException ex;
        throw ex;
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "launchDisplay");
}
