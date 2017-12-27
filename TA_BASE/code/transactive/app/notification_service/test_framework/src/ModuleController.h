/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/notification_service/test_framework/src/ModuleController.h $
  * @author:  Kent Yip
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Contains function prototypes for ModuleController executable.
  *
  */

// OBSOLETE FILE - TO BE DELETED

#include <iostream>
#include <string>
#include <map>
#include <sstream>

// STAF includes



bool startAllSimulators( STAFHandlePtr handle, HandleMap& simulatorHandles );

RemoteProcess spawnSimulator(
    STAFHandlePtr handle, 
    const std::string& executableName,
    const std::string& simulatorID, 
    const std::string& host);

void killAll(STAFHandlePtr handle, HandleMap& processHandles);



 
