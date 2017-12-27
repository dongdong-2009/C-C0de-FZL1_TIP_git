/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/AlarmActionException.h $
  * @author Karen Graham
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This file outlines a simple exception class for use with
  * AlarmAction. It is derived from TransactiveException.
  * This exception class will encompass all AlarmAction errors that
  * can possibly occur.
  */

#if !defined(ALARM_ACTION_EXCEPTION)
#define ALARM_ACTION_EXCEPTION

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Core
{
    class AlarmActionException : public TransactiveException  
    {
    public:

        //
        // Used to specify how AlarmAction has failed. The application can use this
        // to choose which error message to display to the user.
        // Details are not important to the operator so all details are logged and operator
        // can be given a simple error dialog.
        //
        enum EAlarmActionFail
        {
            CANNOT_FIND_ALARM,                      // Unable to retrieve active alarms for the following reason: Could not find the selected alarm.
            CANNOT_UPDATE_ALARM_COMMENTS,           // Some (or all) of the selected alarms could not have their comment updated.
            CANNOT_ACKNOWLEDGE_ALARMS,              // Some (or all) of the selected alarms could not be acknowledged.
            CANNOT_ACKNOWLEDGE_ALARM,               // The alarm could not be acknowledged.
            CANNOT_ACKNOWLEDGE_AND_CLOSE_ALARMS,    // Some (or all) of the selected alarms could not be acknowledged and closed.
            CANNOT_FIND_ASSOCIATED_PLAN,            // Plan association for selected alarm cannot be retrieved.
            CANNOT_RUN_ASSOCIATED_PLAN,             // The alarm plan could not be run.
            CANNOT_POSTPONE_PLAN,                   // The alarm plan could not be postponed.
            CANNOT_LAUNCH_SCHEMATIC,                // Failed to launch schematic.
            CANNOT_LAUNCH_REQUEST_MANAGER,          // The Job Request Manager cannot be launched.
            CANNOT_LAUNCH_EVENT_VIEWER,             // Event Viewer cannot be launched.
            CANNOT_LAUNCH_INSPECTOR_PANEL,          // Inspector Panel cannot be launched
            CANNOT_LAUNCH_TREND_VIEWER,             // The Trend Viewer cannot be launched
            CANNOT_CREATE_SUPPRESSION_RULE,         // The suppression rule could not be created
            CANNOT_LAUNCH_INCIDENT_MANAGER          // The Incident Manager cannot be launched
        };

     /**
       * AlarmActionException (Constructor)
       * 
       * This constructs a AlarmActionException.
       *
       * @param EAlarmActionFailed - This indicates what sort of failure occurred. This allows the code
       *                            catching the exception to decide how it wants to deal with the error
       *                            and what it wants to display to the user
       *
       */
        AlarmActionException(const EAlarmActionFail failType) throw();


      /**
       * AlarmActionException (Constructor)
       * 
       * This constructs a AlarmActionException.
       *
       * @param EAlarmActionFailed - This indicates what sort of failure occurred. This allows the code
       *                            catching the exception to decide how it wants to deal with the error
       *                            and what it wants to display to the user
       * @param string& - This contains as much information as possible about the error that occurred. This can be any
       *                  additional information about the error.
       *
       */
        AlarmActionException(const EAlarmActionFail failType, const std::string& what) throw();


        virtual ~AlarmActionException() throw();

     /**
       * getFailType
       * 
       * This returns the type of failure that occurred in AlarmAction. The value
       * returned should allow the application to determine an appropriate course of
       * action. The application should always retrieve this so it can determine
       * what to do next.
       *
       * @return EAlarmActionFail This indicates the type of failure that occurred.
       *
       */
        EAlarmActionFail getFailType() const { return m_failType; };

	private:
		AlarmActionException() throw();

        //
        // Member Variables
        //

        // This is a set failure type so the application can deal with it appropriately
        EAlarmActionFail   m_failType;
    };
}

#endif // !defined(AFX_AlarmActionException_H__44BE187B_4870_11D7_B150_0080C8E32E69__INCLUDED_)
