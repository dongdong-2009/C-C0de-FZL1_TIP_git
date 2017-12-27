/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/ProcessWatcher.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2013/06/12 11:17:12 $
  * Last modified by:  $Author: limin.zhu $
  * 
  * This will monitor the list of running applications to see when they are terminated.
  * If an application is no longer running this will alert the ProcessManager.
  * 
  */

#include "app\system_control\control_station\src\StdAfx.h"
#include "app/system_control/control_station/src/ProcessWatcher.h"
#include "app/system_control/control_station/src/ProcessManager.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/src/IGui.h"
#include "core/utilities/src/DebugUtil.h"
#include <windows.h>

using TA_Base_Core::ThreadGuard;
using TA_Base_Core::DebugUtil;


namespace
{
    int SetPrivileges()
    {
        static bool once = false;
        
        if ( true == once )
        {
            return 0;
        }

        once = true;

        try
        {
            HANDLE hToken = NULL;
            LUID privNameValue;
            
            TOKEN_PRIVILEGES privileges;
            DWORD dwRet;
            
            if ( ::OpenProcessToken( ::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) )
            {
                if ( ::LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &privNameValue ) )
                {
                    privileges.PrivilegeCount = 1;
                    privileges.Privileges[ 0 ].Luid = privNameValue;
                    privileges.Privileges[ 0 ].Attributes = SE_PRIVILEGE_ENABLED;
                    if ( !::AdjustTokenPrivileges( hToken, false, &privileges, sizeof( privileges ), NULL, &dwRet) ||
                         ::GetLastError() == ERROR_NOT_ALL_ASSIGNED )
                    {
                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Could not set privilege some option's may not wo" );
                    }
                }
                
                if ( ::LookupPrivilegeValue( NULL, SE_SHUTDOWN_NAME, &privNameValue ) )
                {
                    privileges.PrivilegeCount = 1;
                    privileges.Privileges[ 0 ].Luid = privNameValue;
                    privileges.Privileges[ 0 ].Attributes = SE_PRIVILEGE_ENABLED;
                    if ( !::AdjustTokenPrivileges( hToken, false, &privileges, sizeof( privileges ), NULL, &dwRet ) ||
                        ::GetLastError() == ERROR_NOT_ALL_ASSIGNED )
                    {
                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Could not set privilege some option's may not work" );
                    }
                }
            }
        }
        catch( ...  )
        {
            NULL;
        }

        return 0;
    }
}


ProcessWatcher::ProcessWatcher( ProcessManager& processManager )
    : m_processManager(processManager),
      m_isTerminating(false)
{
};


ProcessWatcher::~ProcessWatcher()
{
}


void ProcessWatcher::run()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "run" );

    SetPrivileges();

    try
    {
        while ( !isTerminating() )
        {
            std::map< unsigned long,std::string > apps;
            
            // false indicates we want to know about all running applications and not just the visible ones.
            apps = m_processManager.getRunningApplications( false ); 
       
            std::map< unsigned long, std::string >::iterator iter = apps.begin();
            std::map< unsigned long, std::string >::iterator iterEnd = apps.end();

            for ( ; ( iter != iterEnd ) && ( !isTerminating() ); ++iter )
            {
                // ::OpenProcess will return NULL or NOT NULL, if the process is killed by TaskMgr
                // if NULL: the process must not be exist, and needn't call WaitForSingleObject( will return WAIT_FAILED( -1 ) );
                // if NOT NULL: WaitForSingleObject will return WAIT_OBJECT_0 ( 0 );
                // NOTE: should use PROCESS_ALL_ACCESS, if use PROCESS_QUERY_INFORMATION, WaitForSingleObject will return WAIT_FAILED
                HANDLE process = ::OpenProcess( PROCESS_ALL_ACCESS, false, ( DWORD )( iter->first ) );
                bool isProcessNULL = ( NULL == process );
                DWORD exitCode = 0;
                ::GetExitCodeProcess( process, &exitCode );
                DWORD ret = ::WaitForSingleObject( process, 0 );

                ::CloseHandle( process );
                process = NULL;

                if ( STILL_ACTIVE == exitCode )
                {
                    continue;
                }

                if ( false == isProcessNULL && WAIT_TIMEOUT == ret )
                {
                    continue;
                }

                if ( ( true == isProcessNULL && WAIT_FAILED == ret ) ||
                     ( false == isProcessNULL && WAIT_OBJECT_0 == ret ) )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[id=%d][name=%s] terminated", iter->first, iter->second.c_str() );
                    m_processManager.applicationHasTerminated( iter->first );
                }
            }

            if( !isTerminating() )
            {
                Thread::sleep( 3000 );
            }
        }
    }
    catch ( ... )
    {
        // Catch all exceptions as there is nothing outside this method to catch them.
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "run() of ProcessWatcher threw an unknown exception");
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "run" );
}


void ProcessWatcher::terminate()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "terminate");
    setIsTerminating(true);
    LOG( SourceInfo, DebugUtil::FunctionExit, "terminate");
}


bool ProcessWatcher::isTerminating()
{
    ThreadGuard guard(m_terminateLock);
    return m_isTerminating;
}


void ProcessWatcher::setIsTerminating(bool isTerminating)
{
    ThreadGuard guard(m_terminateLock);
    m_isTerminating = isTerminating;
}

