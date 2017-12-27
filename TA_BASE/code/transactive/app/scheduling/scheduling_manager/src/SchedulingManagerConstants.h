/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_manager/src/SchedulingManagerConstants.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This file specifies the constants and enumerated required by the Scheduling Manager.
  */

#ifndef SCHEDULING_MANAGER_CONSTANTS
#define SCHEDULING_MANAGER_CONSTANTS

#include "app/scheduling/scheduling_manager/src/resource.h"
#include <string>
#include "bus/scheduling/schedulingagentcorbadef/IDL/src/SchedulingAgentCorbaDef.h"

// Custom window messages
#define WM_JOBSELECTION_CHANGE (WM_USER + 200)
#define WM_JOB_CREATE (WM_USER + 201)
#define WM_JOBLIST_UPDATE (WM_USER + 202)
#define WM_STATUSTEXT_CHANGE (WM_USER + 203)
#define WM_CLEAN_UP (WM_USER + 204)
#define WM_SHOW_EXCEED_MAX_JOB_DLG (WM_USER + 205)
#define WM_JOB_ADDED (WM_USER + 206)
#define WM_JOB_UPDATED (WM_USER + 207)
#define WM_JOB_DELETED (WM_USER + 208)
#define WM_RIGHTS_AGENT_DIED (WM_USER + 209)
//TD16358++
#define WM_SYSTEM_INNORMALMODE (WM_USER + 210)
#define WM_SYSTEM_INDEGRADEDMODE (WM_USER + 211)
//++TD16358

namespace TA_Base_App
{
    namespace SchedulingManager
    {
        ///////////////////////////////////////////////////////////////////////
        //
        // Modify the following date time formats to suit a particular locale
        //
        ///////////////////////////////////////////////////////////////////////

        // The complete date time format. Displays the date and time in the form of
        // "25-Dec-04 12:00:00".  Will be used to show the start and end dates of 
        // jobs in the scheduled job list 
        //static const CString DATE_TIME_FORMAT = "%d-%b-%y %H:%M:%S"; //TD12474 

        // The time format.  This will be used in the Job Dialog to show the time jobs
        // are scheduled.  e.g. 13:00
        static const CString TIME_FORMAT = "%H:%M";

        // The complete date format which specifies the day, month, year as well as 
        // the day of week.  This will be used in the Job Dialog to show the end date
        // of the schedule.  e.g. "Thursday, 01-Nov-04
        //static const CString COMPLETE_DATE_FORMAT = "%A, %d-%b-%y"; //TD12474 

        // The date format which only specifies the day and month.  This will be used 
        // in the Job Dialog to show the date for yearly jobs.  e.g. November 1
        //static const CString DATE_FORMAT = "%B %d"; //TD12474 


        ///////////////////////////////////////////////////////////////////////
        //
        // Run params and values
        //
        ///////////////////////////////////////////////////////////////////////

        // The run param name for job info
        static const std::string RPARAM_JOBINFO = "JobInfo";

        // The delimiters that will be used in the JobInfo run param value to 
        // separate the values.  The JobInfo run param value is expected to be 
        // in the form of 
        // <Type>:<ParameterName1>/<ParameterValue1>,<ParameterName2>/<ParameterValue2> ...
        // e.g. --job-info=Plan:PlanID/123
        static const char DELIMITER_JOBTYPE = ':';
        static const char DELIMITER_JOBPARAMNAME = ',';
        static const char DELIMITER_JOBPARAMVALUE = '/';

        // The run param to indicate the server status
        static const std::string RPARAM_SERVERSTATUS = "ServerStatus";

        // The run param value to indicate the server is up
        static const std::string RVALUE_SERVER_UP = "ServerUp";

        // The run param value to indicate the server is down
        static const std::string RVALUE_SERVER_DOWN = "ServerDown";


        ///////////////////////////////////////////////////////////////////////
        //
        // Enumerated Types
        //
        ///////////////////////////////////////////////////////////////////////

        // list all the day strings here just in case the strings get reordered.  
        // Monday is always the first day.
        static const unsigned int DAY[] = 
        {
            IDS_MONDAY,
            IDS_TUESDAY,
            IDS_WEDNESDAY,
            IDS_THURSDAY,
            IDS_FRIDAY,
            IDS_SATURDAY,
            IDS_SUNDAY
        };

        // The flags representing the days of the week
        enum EDayOfWeek 
        {
            Monday = 0x1, 
            Tuesday = 0x2,
            Wednesday = 0x4,
            Thursday = 0x8,
            Friday = 0x10,
            Saturday = 0x20,
            Sunday = 0x40
        };

        // The range of the recurrence
        enum ERecurrenceRange 
        {
            NoEndDate,
            EndAfterNumTimes,
            EndByDate 
        };

        // we just going to make our types the same as the agent's so that GUI classes don't need to know the agent's type
        enum ERecurrenceType
        {
            OnceOff = TA_Base_Bus::SchedulingAgentCorbaDef::OnceOff,
            Daily =   TA_Base_Bus::SchedulingAgentCorbaDef::Daily,
            Weekly =  TA_Base_Bus::SchedulingAgentCorbaDef::Weekly,
            Monthly = TA_Base_Bus::SchedulingAgentCorbaDef::Monthly,
            Yearly =  TA_Base_Bus::SchedulingAgentCorbaDef::Yearly
        };

        // The types of actions that could be performed on the application
        enum EAction
        {
            Add,
            Delete,
            View,
            Filter,
            Edit,
            Enable,
            Disable,
            Print
        };

        
        ///////////////////////////////////////////////////////////////////////
        //
        // Constants
        //
        ///////////////////////////////////////////////////////////////////////

        // The maximum number of jobs to be displayed on the table
        static const int MAX_JOB_TO_DISPLAY = 1500;

        // The maximum number of times the job could recur
        static const int MAX_RECURRENCE = 100;

        // Maximum length of the description
        static const int MAX_DESCRIPTION_LENGTH = 100;

        // If a job is supposed to run on the last day of a month, then use this constant
        static const int LAST_DAY_OF_MONTH = 0;

        // If a job is supposed to run recurringly, use this as the day of month that it will end
        static const int RECUR_INDEFINITELY = 0;

        // Id for a job that has not been scheduled
        static std::string UNSCHEDULED_JOBID = "";

        // Equivalent of February in number
        static const int FEBRUARY = 2;

        // These are the number of pixels in the icons used in the application
		static const unsigned long ICON_SIZE = 16;

        // If the CString.Compare or std::string.compare methods return this value, the the
        // two strings used in the comparisons are equal
        static const int STRINGS_IDENTICAL = 0;

    }; // SchedulingManager
}; // TA_Base_App

#endif // SCHEDULING_MANAGER_CONSTANTS
