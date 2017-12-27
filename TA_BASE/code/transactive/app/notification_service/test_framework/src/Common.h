/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/notification_service/test_framework/src/Common.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#ifndef COMMON_H
#define COMMON_H

#include "core/message/src/TA_CosUtility.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/database/src/SimpleDB.h"
#include "core/data_access_interface/src/DBConnection.h"
#include "app/notification_service/src/NSALocator.h"
#include <string>


const int DEFAULT_SUBSYSTEM_KEY = 1;

// A single console manager with --mgr-port=DEFAULT_MGRPORT will be spawned on each host.  
// All notification services and simulators running on a host will use the same console manager.
const unsigned int DEFAULT_MGRPORT = 9999;

// Retrieves the connection parameters from the database connection string file
// identified by the RPARAM_DBCONNECTIONFILE RunParam.  The EDataType SysController_Cd 
// must be defined in this file.
inline void getDbConnectParams(std::string& host, std::string& user, std::string& pass)
{
    std::string connectStr;
    TA_Base_Core::DbConnection::getInstance().getConnectionString(
        TA_Base_Core::SysController_Cd, TA_Base_Core::Read, connectStr);
    TA_Assert(!connectStr.empty());

    std::vector<std::string> components;
    SimpleDb::ParseConnectStr(connectStr, components, ':');
    TA_Assert(components.size() == 3);  // require host:user:pass

    host = components[0];
    user = components[1];
    pass = components[2]; 
}

// Returns a list that can be used as the --notify-hosts argument, of the form
// host1:port1, host2:port2, ...
// Broadcast notification services are NOT included.
inline std::string getNotifyHostsForLocation(unsigned int locationKey)
{
    TA_Assert(locationKey != 0);

    TA_Base_App::NSALocator locator;
    TA_Base_App::NSALocator::NSADetailVec vec;
    locator.findNSAsAtLocation(locationKey, vec);

    std::string str;
    for (unsigned int i = 0; i < vec.size(); ++i)
    {
        // Ignore all broadcast notification services
        if (!vec[i].m_isBroadcast)
        {
            str += TA_Base_Core::gMakeServiceAddr(vec[i].m_hostname, vec[i].m_portnum);

            if (i != vec.size() - 1)
            {
                str += ",";
            }
        }
    }
    
    return str;
}

#endif // COMMON_H