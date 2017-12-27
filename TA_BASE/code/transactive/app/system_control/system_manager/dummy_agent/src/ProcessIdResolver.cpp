/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_manager/dummy_agent/src/ProcessIdResolver.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "core/process_management/src/ProcessIdResolver.h"
#include "core/exceptions/src/ManagedProcessException.h"
#include "core/utilities/src/DebugUtil.h"

#if !defined( WIN32 )
    #include <unistd.h>
#else
    #include <process.h>
#endif

using TA_Base_Core::ManagedProcessException;

namespace TA_Base_Core
{

    //
    // getProcessId
    //
    unsigned long ProcessIdResolver::getProcessId()
    {
        #if !defined( WIN32 )
            pid_t pid = getpid();
            if( pid <= 0 )
            {
                TA_THROW( ManagedProcessException("Failed to resolve the process id") );
            }
            return (unsigned long)pid;
        #else
            return (unsigned long)_getpid();
        #endif
    }
};
