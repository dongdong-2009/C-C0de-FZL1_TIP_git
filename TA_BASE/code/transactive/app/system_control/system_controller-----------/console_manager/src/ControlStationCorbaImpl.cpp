/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_GenericGUI/TA_BASE/transactive/app/system_control/control_station/src/ControlStationCorbaImpl.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/09/03 14:11:12 $
  * Last modified by:  $Author: CM $
  *
  * This receives callbacks from the ManagedProcesses and will allow applications to be launched.
  * It also receives callbacks when the services have been restarted.
  * This implements the IControlStationCorbaDef corba definition which means this
  * object is what the ManagedProcess's get when they resolve the Control Station.
  */

//#include "app/system_control/control_station/src/StdAfx.h"
#include <afxwin.h>         // MFC core and standard components
#include "ControlStationCorbaImpl.h"
//#include "app/system_control/control_station/src/SessionManager.h"
#include "app/system_control/control_station/src/IProcessManager.h"
#include "app/system_control/control_station/src/ControlStationConstants.h"
#include "app/system_control/control_station/src/ControlStationRootDialog.h"

#include "core/exceptions/src/ApplicationException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"


using TA_Base_Core::DebugUtil;

ControlStationCorbaImpl::ControlStationCorbaImpl(const std::string& consoleName)
{
    TA_Base_Core::CorbaUtil::getInstance().setServantKey(this, "ControlStation");
    activateServant();
}


ControlStationCorbaImpl::~ControlStationCorbaImpl()
{
}

void ControlStationCorbaImpl::launchAnApplication(unsigned long appType,
        const char* extraCommandLine,
        unsigned long posFlag,
        unsigned long alignFlag,
        const TA_Base_Core::IControlStationCorbaDef::RECT& objectDim,
        const TA_Base_Core::IControlStationCorbaDef::RECT& boundaryDim)
{
    FUNCTION_ENTRY("launchAnApplication");
    FUNCTION_EXIT;
}


TA_Base_Core::IControlStationCorbaDef::RECT ControlStationCorbaImpl::getRect(unsigned long targetScreen, unsigned long targetArea, long val)
{
    FUNCTION_ENTRY("getRect");

    TA_Base_Core::IControlStationCorbaDef::RECT retval ;
    retval.left         = 0;
    retval.top          = 0;
    retval.right        = 1920;
    retval.bottom       = 1000;
    return retval;
    FUNCTION_EXIT;
}


void ControlStationCorbaImpl::launchDisplay(const char* display, const char* asset, long leftPosition)
{
    FUNCTION_ENTRY("launchDisplay");
    FUNCTION_EXIT;
}
