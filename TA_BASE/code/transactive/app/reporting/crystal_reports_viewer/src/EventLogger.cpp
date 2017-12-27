/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/reporting/crystal_reports_viewer/src/EventLogger.cpp $
  * @author:  B. Carbonaro
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides a simplified API for writing log events into
  * the Windows Event Viewer log.
  */

#include "EventLogger.hpp"
#include "EventMessages.h"

#ifdef TRANSACTIVE_BUILD
namespace TA_Base_App
{
    namespace TA_Reporting
    {
#endif // TRANSACTIVE_BUILD

        EventLogger::EventLogger( const char* p_szEventSource, const char* p_szMessageFilename )
          : m_hEventViewer( NULL )
        {
	        initialise( p_szEventSource, p_szMessageFilename );
        }


        EventLogger::~EventLogger()
        {
	        DeregisterEventSource( m_hEventViewer ); 
        }


        void EventLogger::initialise( const char* p_szEventSource, const char* p_szMessageFilename )
        {
	        HKEY hKey;
	        char szRegKey[255];
	        strcpy( szRegKey, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\" );
	        strcat( szRegKey, p_szEventSource );

	        // First, lets create the base key.
	        LONG lResult = RegCreateKey( HKEY_LOCAL_MACHINE, szRegKey, &hKey );
	        if (lResult == ERROR_SUCCESS)
	        {
		        char szDir[255];
		        lResult = GetFullPathName( p_szMessageFilename, 255, szDir, 0);
		        if (lResult > 0)
		        {
			        // Next, lets create the EventMessageFile string.
			        lResult = RegSetValueEx( hKey,
								             "EventMessageFile",
									         0,                 
									         REG_EXPAND_SZ,     
									         (LPBYTE) szDir ,   
									         strlen(szDir) + 1);
			        if (lResult == ERROR_SUCCESS)
			        {
				        // Finally, lets register for this event source.
				        m_hEventViewer = RegisterEventSource( NULL, 
													          TEXT( p_szEventSource ));    
			        }
		        }

	        }
        }


        bool EventLogger::logInformation( char* p_szMsg )
        {
	        bool bResult = false;

	        if (m_hEventViewer)
	        {
		        bResult = writeEvent( EVENTLOG_INFORMATION_TYPE, p_szMsg );
 	        }

	        return( bResult );
        }


        bool EventLogger::logWarning( char* p_szMsg )
        {
	        bool bResult = false;

	        if (m_hEventViewer)
	        {
		        bResult = writeEvent( EVENTLOG_WARNING_TYPE, p_szMsg );
 	        }

	        return( bResult );
        }


        bool EventLogger::logError( char* p_szMsg )
        {
	        bool bResult = false;

	        if (m_hEventViewer)
	        {
		        bResult = writeEvent( EVENTLOG_ERROR_TYPE, p_szMsg );
 	        }

	        return( bResult );
        }


        bool EventLogger::writeEvent( WORD p_wEventType, char* p_szMsg )
        {
 	        bool bResult = false;

	        if (m_hEventViewer)
	        {
		        // Firstly, lets get the current user name.	
		        char szUserName[255];
		        DWORD lNumChar = 255;
		        BOOL bDone = GetUserName( szUserName, &lNumChar );
		        if (bDone)
		        {
			        char* pszMessages[2];
			        pszMessages[0] = szUserName;
			        pszMessages[1] = p_szMsg;
			        bDone = ReportEvent( m_hEventViewer,
		 						         p_wEventType,
								         0,            
								         EVMSG_1,
								         NULL, 
								         2,    
								         0,    
								         (LPCTSTR*)pszMessages,
								         NULL );
			        if (bDone) bResult = true;
		        }
	        }

	        return( bResult );
        }

#ifdef TRANSACTIVE_BUILD
	} // TA_Reporting
} // TA_Base_App
#endif // TRANSACTIVE_BUILD
