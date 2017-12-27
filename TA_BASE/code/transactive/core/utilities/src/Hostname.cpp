/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/Hostname.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This file contains the source code that implements the Hostname class.
  *
  */

#if !defined( WIN32 )
    #include <unistd.h>
    #include <netdb.h>
#else // defined( WIN32 )
    #include <winsock2.h>
    #include <winreg.h>
#endif // defined( WIN32 )

#include "Hostname.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/RunParams.h"

namespace TA_Base_Core
{
    std::string Hostname::getFullHostname()
    {
        //LOG( DebugUtil::DebugDebug, "Hostname::getHostname() being used to get the hostname" );

        //
        // Get the name of the host.  If it couldn't be determined, throw an
        // exception.
        //
        std::string lvCanononicalName = Hostname::getHostname();

        //
        // Now look up the fully qualified host name by getting the operating
        // systems host entry.
        //
        struct hostent* hp;
#if defined( SOLARIS )

        struct hostent lvHostEntry;
        char lvBuffer[ 1025 ];
        int lvError;
        hp = gethostbyname_r(lvCanononicalName.c_str(), & lvHostEntry, lvBuffer, sizeof(lvBuffer), & lvError);

#else // !defined( SOLARIS )

        hp = gethostbyname(lvCanononicalName.c_str());

#endif // !defined( SOLARIS )

        if (hp == 0)
        {
            throw TransactiveException("Unable to retrieve hostname");
        }

        std::string lvHostName;
        lvHostName = hp->h_name;
#if defined( WIN32 )

        //
        // Get the DNS suffix out of the registry because it isn't always
        // returned by gethostbyname().  If it doesn't already exist
        // in the host name, then we will append it.
        //
        HKEY hKey;

        if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                          TEXT("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters"),
                                          0,
                                          KEY_QUERY_VALUE,
                                          &hKey))
        {
            throw TransactiveException("Unable to retrieve hostname");
        }

        char lvQueryBuffer[ 1025 ];
        unsigned long lvQueryBufferLength = sizeof(lvQueryBuffer);
        RegQueryValueEx(hKey, TEXT("DhcpDomain"), NULL, NULL,
                        (LPBYTE)lvQueryBuffer, & lvQueryBufferLength);
        RegCloseKey(hKey);

        if (std::string::npos != lvHostName.find_first_of(lvQueryBuffer))
        {
            lvHostName += ".";
            lvHostName += lvQueryBuffer;
        }

#endif // defined( WIN32 )

        //
        // Return the fully qualified host name.
        //
        return lvHostName;
    }

    std::string Hostname::getHostname()
    {
        if (true == TA_Base_Core::RunParams::getInstance().isSet("LocalHostname"))
        {
            return TA_Base_Core::RunParams::getInstance().get("LocalHostname");;
        }

        //
        // Get the machines name.
        //
        const int maxNameSize = 1025;
        char unqualifiedHostName[ maxNameSize ];
        int lvError = gethostname(unqualifiedHostName, sizeof(unqualifiedHostName));

#if defined( WIN32 )

        if (WSANOTINITIALISED == WSAGetLastError())
        {
            //
            // The Windows sockets library is not smart enough to figure out
            // that it should be automatically started, therefore we must start
            // Windows sockets and then try again.
            //
            WORD wVersionRequested;
            WSADATA wsaData;
            int err;
            wVersionRequested = MAKEWORD(2, 2);
            err = WSAStartup(wVersionRequested, &wsaData);

            if (0 != err)
            {
                throw TransactiveException("Unable to retrieve hostname");
            }

            //
            // Try again.
            //
            lvError = gethostname(unqualifiedHostName, sizeof(unqualifiedHostName));
        }

#endif // defined( WIN32 )

        if (0 != lvError)
        {
            throw TransactiveException("Unable to retrieve hostname");
        }

        //
        // Return the canononical name.
        //
        return std::string(unqualifiedHostName);
    }
} // namespace TA_Base_Core
