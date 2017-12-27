/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/reporting/crystal_reports_viewer/src/EventLogger.hpp $
  * @author:  B. Carbonaro
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the header file for the EventLogger class.
  */

#ifndef EVENTLOGGER_HPP
#define EVENTLOGGER_HPP

#include "Windows.h"

#ifdef TRANSACTIVE_BUILD
namespace TA_Base_App
{
    namespace TA_Reporting
    {
#endif // TRANSACTIVE_BUILD

        class EventLogger
        {
        public:
            /**
              * EventLogger
              * 
              * This is the constructor for the EventLogger class.
              *
              * @param p_szEventSource is a string value that specifies the Event Viewer
              *        source name. The name actual represents the label that is displayed
              *        in the Source column for each log entry in the Event Viewer.
              *
              * @param p_szMessageFilename is a string value that specifies the Event 
              *        Viewer Messages filename. The Event Viewer Messages file is the DLL
              *        or EXE that contains string that is pre-pended to each message 
              *        that is logged.
              */
	        EventLogger( const char* p_szEventSource, const char* p_szMessageFilename );

            /**
              * ~EventLogger
              * 
              * This is the destructor for the EventLogger class.
              *
              */
	        ~EventLogger();

            /**
              * logInformation
              * 
              * This method writes the string as supplied via the parameter to the
              * Event Viewer as an informational log message.
              *
              * @return True if writing the log message was successful otherwise
              *         it returns false.
              *
              * @param p_szMsg is a string value that specifies the message to be 
              *        written to the Event Viewer's log.
              *
              */
	        bool logInformation( char* p_szMsg );

            /**
              * logWarning
              * 
              * This method writes the string as supplied via the parameter to the
              * Event Viewer as an warning log message.
              *
              * @return True if writing the log message was successful otherwise
              *         it returns false.
              *
              * @param p_szMsg is a string value that specifies the message to be 
              *        written to the Event Viewer's log.
              *
              */
            bool logWarning( char* p_szMsg );

            /**
              * logError
              * 
              * This method writes the string as supplied via the parameter to the
              * Event Viewer as an error log message.
              *
              * @return True if writing the log message was successful otherwise
              *         it returns false.
              *
              * @param p_szMsg is a string value that specifies the message to be 
              *        written to the Event Viewer's log.
              *
              */
            bool logError( char* p_szMsg );

        protected:
        private:
            /**
              * initialise
              * 
              * This method performs the initialisation required using the values
              * supplied via the parameters.
              *
              * @param p_szEventSource is a string value that specifies the Event Viewer
              *        source name. The name actual represents the label that is displayed
              *        in the Source column for each log entry in the Event Viewer.
              *
              * @param p_szMessageFilename is a string value that specifies the Event 
              *        Viewer Messages filename. The Event Viewer Messages file is the DLL
              *        or EXE that contains string that is pre-pended to each message 
              *        that is logged.
              *
              */
	        void initialise( const char* p_szEventSource, const char* p_szMessageFilename );

            /**
              * writeEvent
              * 
              * This method provides low level functtionality that actually performs
              * the write operation to the Event Viewer's log. It provides support
              * for the public logInformation, logWarning and logError methods.
              *
              * @return True if writing the log message was successful otherwise
              *         it returns false.
              *
              * @param p_wEventType is a Windows enumerated value that specifies the
              *        level for the log message that is to be written.
              *
              * @param p_szMsg is a string value that specifies the message to be 
              *        written to the Event Viewer's log.
              *
              */
	        bool writeEvent( WORD p_wEventType, char* p_szMsg );

            /**
              * Stores a handle to an open Event Viewer's registered
              * event source.
              */
	        HANDLE m_hEventViewer;
        };

#ifdef TRANSACTIVE_BUILD
	} // TA_Reporting
} // TA_Base_App
#endif // TRANSACTIVE_BUILD

#endif
