/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_agent/command_line_interface/src/SchedulingAgentCLI.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This application provides a command line interface to the Scheduling Agent
  *
  */

#ifdef _MSC_VER
  #pragma warning(disable: 4786) // identifier was truncated in the browser information
#endif // _MSC_VER

#include <iomanip>
#include <sstream>
#include <time.h>

#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "core/corba/src/CorbaUtil.h"
#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"
#include "app/scheduling/scheduling_agent/command_line_interface/src/InvalidSyntaxException.h"

#include "app/scheduling/scheduling_agent/command_line_interface/src/SchedulingAgentCLI.h"
#include "core/threads/src/Thread.h"

using TA_Base_Core::CorbaUtil;
using TA_Base_Core::IOperator;
using TA_Base_Core::OperatorAccessFactory;
using TA_Base_Bus::AuthenticationLibrary;


namespace TA_Base_App
{


//
// TODO: remove comment before code review
// My god this class is ugly
//
SchedulingAgentCLI::SchedulingAgentCLI(
    const std::string& username,
    unsigned long profileId,
    unsigned long locationId,
    unsigned long workstationId,
    const std::string& schedulingAgentEntity)
    :
    m_scheduleFrequency(TA_Base_Bus::SchedulingAgentCorbaDef::Daily),
    m_minute(0),
    m_hour(0),
    m_dayOfMonth(1),
    m_month(TA_Base_Bus::SchedulingAgentCorbaDef::January),
    m_daysOfWeek( TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeekSequence() ),
    m_endDay(0),
    m_endMonth(TA_Base_Bus::SchedulingAgentCorbaDef::January),
    m_endYear(0),
    m_isInhibited(false),
    m_description(""),
    m_jobType(PlanRunnerJob),

    m_planPath(""),
    m_alarmSeverityKey(3),
    m_alarmDescription(""),

    m_sessionKey(""),
    m_profileId(profileId),
    m_locationId(locationId),
    m_schedulingAgentEntity(schedulingAgentEntity)
{
    CorbaUtil::getInstance().initialise();
	CorbaUtil::getInstance().activate();

    IOperator* operatorData =
        OperatorAccessFactory::getInstance().getOperator(username, false);

    std::string password     = operatorData->getPassword();
    unsigned long operatorId = operatorData->getKey();

    AuthenticationLibrary* authLib = new AuthenticationLibrary();
    m_sessionKey =
        authLib->requestSession(
            operatorId,
            profileId,
            locationId,
            workstationId,
            password);
    delete authLib;
    authLib = NULL;
}

SchedulingAgentCLI::SchedulingAgentCLI(
    const std::string& sessionId,
    unsigned long profileId,
    unsigned long locationId,
    const std::string& schedulingAgentEntity)
    :
    m_scheduleFrequency(TA_Base_Bus::SchedulingAgentCorbaDef::Daily),
    m_minute(0),
    m_hour(0),
    m_dayOfMonth(1),
    m_month(TA_Base_Bus::SchedulingAgentCorbaDef::January),
    m_daysOfWeek( TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeekSequence() ),
    m_endDay(0),
    m_endMonth(TA_Base_Bus::SchedulingAgentCorbaDef::January),
    m_endYear(0),
    m_isInhibited(false),
    m_description(""),
    m_jobType(PlanRunnerJob),

    m_planPath(""),
    m_alarmSeverityKey(3),
    m_alarmDescription(""),

    m_sessionKey(sessionId),
    m_profileId(profileId),
    m_locationId(locationId),
    m_schedulingAgentEntity(schedulingAgentEntity)
{
    CorbaUtil::getInstance().initialise();
	CorbaUtil::getInstance().activate();
}

void SchedulingAgentCLI::execute()
{
    outputStartupMessage();

    //
    // Keep processing commands until the processInputCommand returns false
    //
    while( processInputCommand() ) { }
}


void SchedulingAgentCLI::outputStartupMessage()
{
    std::cout <<
        "Scheduling Agent Command Line Interface" << std::endl <<
        "Type help for a list of commands." << std::endl <<
        "All commands are lower case." << std::endl;
}


void SchedulingAgentCLI::outputHelpMessage()
{
    std::cout << 
        "Commands:" << std::endl <<
        "add       Add a new job" << std::endl <<
        "delete    Delete an existing job" << std::endl <<
        "get       Get an existing job and output its details" << std::endl <<
        "getall    Get all jobs and output their details" << std::endl <<
        "gettypes  Get the list of all supported job types" << std::endl <<
        "help      Display this help" << std::endl <<
        std::endl <<
        "Type help <command> for how to run the command, eg help add" <<
        std::endl;
}


bool SchedulingAgentCLI::processInputCommand()
{
    try
    {
        std::cout << "> ";
        CommandLine commandLine = getCommandLine();

    
        std::string command = commandLine.getCommand();

        if( command == "exit" || command == "quit" )
        {
            return false;
        }
        else if(command == "add")
        {
            processAddCommand(commandLine);
        }
        else if(command == "delete")
        {
            processDeleteCommand(commandLine);
        }
        else if(command == "get")
        {
            processGetCommand(commandLine);
        }
        else if(command == "getall")
        {
            processGetAllCommand(commandLine);
        }
        else if(command == "gettypes")
        {
            processGetTypesCommand(commandLine);
        }
        else if(command == "help")
        {
            processHelpCommand(commandLine);
        }
        else if(command != "")
        {
            outputHelpMessage();
        }
    }
    catch(const InvalidSyntaxException& ex)
    {
        std::cout << "Invalid syntax. " << ex.what() << std::endl;
    }
    catch(const TA_Base_Bus::SchedulingAgentCorbaDef::BadScheduleException& ex)
    {
        std::cout << "Caught a corba BadScheduleException exception: "
                  << std::string(ex.what)
                  << std::endl;
    }
    catch(const TA_Base_Bus::SchedulingAgentCorbaDef::BadJobException& ex)
    {
        std::cout << "Caught a corba BadJobException exception: "
                  << std::string(ex.what)
                  << std::endl;
    }
    catch(const TA_Base_Bus::SchedulingAgentCorbaDef::UnrecognisedJobTypeException& ex)
    {
        std::cout << "Caught a corba UnrecognisedJobTypeException exception: "
                  << std::string(ex.what)
                  << std::endl;
    }
    catch(const TA_Base_Bus::SchedulingAgentCorbaDef::JobDoesNotExistException& ex)
    {
        std::cout << "Caught a corba JobDoesNotExistException exception: "
                  << std::string(ex.what)
                  << std::endl;
    }
    catch(const TA_Base_Bus::SchedulingAgentCorbaDef::TooManyJobsException& ex)
    {
        std::cout << "Caught a corba TooManyJobsException exception: "
                  << std::string(ex.what)
                  << std::endl;
    }
    catch(const TA_Base_Bus::SchedulingAgentCorbaDef::JobInPastException& ex)
    {
        std::cout << "Caught a corba exception: "
                  << std::string(ex.what)
                  << std::endl;
    }
    catch(const CORBA::Exception& ex)
    {
        std::cout << "Caught a corba exception: "
                  << CorbaUtil::getInstance().exceptionToString(ex)
                  << std::endl;
    }
    catch(const std::exception& ex)
    {
        std::cout << "Caught an exception: " << ex.what() << std::endl;
    }
    catch (...)
    {
		std::cout << "Caught an unknown exception" << std::endl;
	}


    return true;
}


CommandLine SchedulingAgentCLI::getCommandLine()
{
    char line[1024];
    std::cin.getline(line, 1024);
    return CommandLine(line);
}


void SchedulingAgentCLI::processAddCommand(const CommandLine& commandLine)
{
    //
    // Set the variables to the default values.
    //
    TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleType frequency       = m_scheduleFrequency;
    unsigned short minute                                 = m_minute;
    unsigned short hour                                   = m_hour;
    unsigned short dayOfMonth                             = m_dayOfMonth;
    TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear month            = m_month;
    
    time_t t = time(0);
    tm *lt    = localtime(&t);
    unsigned short year                                   = lt->tm_year + 1900;

    TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeekSequence daysOfWeek = m_daysOfWeek;
    unsigned short endDay                                 = m_endDay;
    TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear endMonth         = m_endMonth;
    unsigned short endYear                                = m_endYear;
    bool isInhibited                                      = m_isInhibited;
    std::string description                               = m_description;
    EJobType jobType                                      = m_jobType;
    int numJobs = 1;    // number of jobs to be scheduled
    int offset = 1;    // the number of minutes that separates each schedule
    int delay = 100;    // the number of milliseconds for the delay before it actually sends the job request to the scheduling agent
    int profile = 0;    // super profile
    int location = 0;   // unallocated

    //
    // Get the values passed in on the command line
    //
    std::string frequencyStr   = commandLine.getParameter("f");
    std::string minuteStr      = commandLine.getParameter("m");
    std::string hourStr        = commandLine.getParameter("h");
    std::string dayOfMonthStr  = commandLine.getParameter("d");
    std::string monthStr       = commandLine.getParameter("month");
    std::string daysOfWeekStr  = commandLine.getParameter("w");
    std::string endDayStr      = commandLine.getParameter("ed");
    std::string endMonthStr    = commandLine.getParameter("em");
    std::string endYearStr     = commandLine.getParameter("ey");
    std::string isInhibitedStr = commandLine.getParameter("i");
    std::string descriptionStr = commandLine.getParameter("desc");
    std::string jobTypeStr     = commandLine.getParameter("t");
    std::string yearStr        = commandLine.getParameter("y");
    std::string numJobStr      = commandLine.getParameter("numJobs");
    std::string offsetStr      = commandLine.getParameter("offset");
    std::string delayStr       = commandLine.getParameter("delay");
    std::string profileStr     = commandLine.getParameter("profile");
    std::string locationStr    = commandLine.getParameter("location");

    //
    // If a variable is set, override the default value
    //
    if(frequencyStr   != "") { frequency   = getScheduleType(frequencyStr); }
    if(minuteStr      != "") { minute      = getStringAsUnsignedShort(minuteStr); }
    if(hourStr        != "") { hour        = getStringAsUnsignedShort(hourStr); }
    if(dayOfMonthStr  != "") { dayOfMonth  = getStringAsUnsignedShort(dayOfMonthStr); }
    if(monthStr       != "") { month       = getMonthOfYear(monthStr); }
    if(daysOfWeekStr  != "") { daysOfWeek  = getDaysOfWeek(daysOfWeekStr); }
    if(endDayStr      != "") { endDay      = getStringAsUnsignedShort(endDayStr); }
    if(endMonthStr    != "") { endMonth    = getMonthOfYear(endMonthStr); }
    if(endYearStr     != "") { endYear     = getStringAsUnsignedShort(endYearStr); }
    if(isInhibitedStr != "") { isInhibited = getStringAsBool(isInhibitedStr); }
    if(descriptionStr != "") { description = descriptionStr; }
    if(jobTypeStr     != "") { jobType     = getJobType(jobTypeStr); }
    if(yearStr        != "") { year        = getStringAsUnsignedShort(yearStr); };
    if(numJobStr      != "") { numJobs     = getStringAsUnsignedShort(numJobStr); }
    if(offsetStr      != "") { offset      = getStringAsUnsignedShort(offsetStr); };
    if(delayStr       != "") { delay       = getStringAsUnsignedShort(offsetStr); };
    if(profileStr     != "") { profile     = getStringAsUnsignedShort(profileStr); };
    if(locationStr    != "") { location    = getStringAsUnsignedShort(locationStr); };

	// don't let the offset be more than 1 day
	// (60 minutes * 24 hours = 1440 minutes)
	if(offset > 1440)
	{
		offset = 1440;
	}

    //
    // Save the current values as the default values
    //
    m_scheduleFrequency = frequency;
    m_minute            = minute;
    m_hour              = hour;
    m_dayOfMonth        = dayOfMonth;
    m_month             = month;
    m_daysOfWeek        = daysOfWeek;
    m_endDay            = endDay;
    m_endMonth          = endMonth;
    m_endYear           = endYear;
    m_isInhibited       = isInhibited;
    m_description       = description;
    m_jobType           = jobType;
    m_profileId         = profile;
    m_locationId        = location;


    //
    // Create the schedule
    //
    TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleCorbaDef* schedule =
        new TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleCorbaDef();
    schedule->type       = frequency;
    schedule->daysOfWeek = daysOfWeek;
    schedule->endDay     = endDay;
    schedule->endMonth   = endMonth;
    schedule->endYear    = endYear;


    //
    // Create the runTime
    //
    TA_Base_Bus::SchedulingAgentCorbaDef::RunTimeCorbaDef* runTime =
        new TA_Base_Bus::SchedulingAgentCorbaDef::RunTimeCorbaDef();
    runTime->minute     = 0;
    runTime->hour       = 0;
    runTime->dayOfMonth = 0;
    runTime->month      = TA_Base_Bus::SchedulingAgentCorbaDef::January;
    runTime->year       = 0;

    //
    // Create the job itself
    //
    TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef* job
        = new TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef();
    job->profileIdentifier  = m_profileId;
    job->locationIdentifier = m_locationId;
    job->runTime            = *runTime;
    job->isInhibited        = isInhibited;
    
    if(jobType == PlanRunnerJob)
    {
        //
        // Get the plan id if the user has supplied it, and save it as the
        // default.
        //
        std::string planPath = commandLine.getParameter("p");
        job->type = CORBA::string_dup("Plan");

        m_planPath = planPath;

        //
        // Create the required parameters
        //
        
        TA_Base_Bus::SchedulingAgentCorbaDef::NameValuePair* planPathParam =
            new TA_Base_Bus::SchedulingAgentCorbaDef::NameValuePair;
        planPathParam->name = CORBA::string_dup("PlanPath");
        planPathParam->value = CORBA::string_dup(m_planPath.c_str());
        job->data.length(1);
        job->data[0] =  *planPathParam;
    
    }
    /*else // jobType == AlarmGeneratorJob
    {
        //
        // Get the alarm severity key and alarm description, and save them as
        // defaults.
        //
        unsigned long alarmSeverityKey = m_alarmSeverityKey;
        std::string alarmSeverityKeyStr = commandLine.getParameter("ask");
        if(alarmSeverityKeyStr != "")
        {
            alarmSeverityKey = getStringAsUnsignedLong(alarmSeverityKeyStr);
        }
        
        std::string alarmDescription = m_alarmDescription;
        std::string alarmDescriptionParam = commandLine.getParameter("alarmdesc");
        if(alarmDescriptionParam != "")
        {
            alarmDescription = alarmDescriptionParam;
        }

        m_alarmSeverityKey = alarmSeverityKey;
        m_alarmDescription = alarmDescription;

        //
        // Create an alarm generator job and inject it into the Corba Any
        //
        TA_Base_Bus::SchedulingAgentCorbaDef::AlarmGeneratorCorbaDef* alarmGeneratorJob =
            new TA_Base_Bus::SchedulingAgentCorbaDef::AlarmGeneratorCorbaDef();

        alarmGeneratorJob->alarmSeverityKey = alarmSeverityKey;

        alarmGeneratorJob->alarmDescription =
            CORBA::string_dup( alarmDescription.c_str() );

        job->data <<= alarmGeneratorJob;
    }*/


	int oldHour = hour;
	int oldMinute = minute;

	// need to do this, since the first iteration loop will increment it again
	if(offset == 1440)
	{
		dayOfMonth--;
	}

    for (int i = 0; i < numJobs; i++)
    {
		// have we spilt over into a new day?
		// yes, if..
		//    - oldHour > hour
		//         eg. offset = 1
		//             23:59 day 1 --> 0:00 day 2. (23 > 0)
		//      OR
		//    - oldHour == hour AND oldMinute > minute
		//         eg. offset = 1400
		//             23:59 day 1 --> 23:39 day 2 (23 == 23, 59 > 39)
		//      OR
		//    - oldHour == hour AND oldMinute == minute AND offset == 1440
		//         eg. offset = 1440
		//             23:59 day 1 --> 23:59 day 2 (23 == 23, 59 == 59)
		if(  (oldHour > hour) || ( (oldHour == hour) && ((oldMinute > minute) || ((oldMinute == minute) && (offset == 1440))) )  )
		{
			// new day, so increment it.
			// has this spilt over into a new month?
			dayOfMonth++;
			int lastDayOfMonth;

			// need to check against the last day of the month..
			switch(month)
			{
				// need to check for a leap year if february
				case TA_Base_Bus::SchedulingAgentCorbaDef::February:
					// year is a leap year if:
					//    - year  is divisible by 400
					//      OR
					//    - year is divisible by 4, but NOT by 100
					if( ((year%4 == 0) && (year%100 != 0)) || (year%400 == 0) )
					{
						lastDayOfMonth = 29;
					}
					else
					{
						lastDayOfMonth = 28;
					}
					break;
				case TA_Base_Bus::SchedulingAgentCorbaDef::April:
				case TA_Base_Bus::SchedulingAgentCorbaDef::June:
				case TA_Base_Bus::SchedulingAgentCorbaDef::September:
				case TA_Base_Bus::SchedulingAgentCorbaDef::November:
					lastDayOfMonth = 30;
					break;
				default:
					lastDayOfMonth = 31;
					break;
			}

			// new month, so increment it, and reset the 'day of month' value to 1
			// also need to check if we've spilt over into a new year
			if(dayOfMonth > lastDayOfMonth)
			{
				dayOfMonth = 1;

				if(month == TA_Base_Bus::SchedulingAgentCorbaDef::January)
				{
					month = TA_Base_Bus::SchedulingAgentCorbaDef::February;
				}
				else if(month == TA_Base_Bus::SchedulingAgentCorbaDef::February)
				{
					month = TA_Base_Bus::SchedulingAgentCorbaDef::March;
				}
				else if(month == TA_Base_Bus::SchedulingAgentCorbaDef::March)
				{
					month = TA_Base_Bus::SchedulingAgentCorbaDef::April;
				}
				else if(month == TA_Base_Bus::SchedulingAgentCorbaDef::April)
				{
					month = TA_Base_Bus::SchedulingAgentCorbaDef::May;
				}
				else if(month == TA_Base_Bus::SchedulingAgentCorbaDef::May)
				{
					month = TA_Base_Bus::SchedulingAgentCorbaDef::June;
				}
				else if(month == TA_Base_Bus::SchedulingAgentCorbaDef::June)
				{
					month = TA_Base_Bus::SchedulingAgentCorbaDef::July;
				}
				else if(month == TA_Base_Bus::SchedulingAgentCorbaDef::July)
				{
					month = TA_Base_Bus::SchedulingAgentCorbaDef::August;
				}
				else if(month == TA_Base_Bus::SchedulingAgentCorbaDef::August)
				{
					month = TA_Base_Bus::SchedulingAgentCorbaDef::September;
				}
				else if(month == TA_Base_Bus::SchedulingAgentCorbaDef::September)
				{
					month = TA_Base_Bus::SchedulingAgentCorbaDef::October;
				}
				else if(month == TA_Base_Bus::SchedulingAgentCorbaDef::October)
				{
					month = TA_Base_Bus::SchedulingAgentCorbaDef::November;
				}
				else if(month == TA_Base_Bus::SchedulingAgentCorbaDef::November)
				{
					month = TA_Base_Bus::SchedulingAgentCorbaDef::December;
				}
				else if(month == TA_Base_Bus::SchedulingAgentCorbaDef::December)
				{
					month = TA_Base_Bus::SchedulingAgentCorbaDef::January;
					year++;
				}
			}
		}

		schedule->minute = minute;
		schedule->hour = hour;
		schedule->dayOfMonth = dayOfMonth;
		schedule->month      = month;
		schedule->year       = year;

        std::stringstream oss;
        oss << description << " " << std::setw(9) << std::setfill('0') << i;
        job->description        = CORBA::string_dup(oss.str().c_str());
        job->schedule           = *schedule;
        //
        // Schedule the job
        //
        SchedulingAgentNamedObjRef schedulingAgent(m_schedulingAgentEntity, true);

        char* id = NULL;
        CORBA_CALL_RETURN( id, schedulingAgent, scheduleJob, ( *job, m_sessionKey.c_str() ) );//limin

        TA_Base_Core::Thread::sleep(delay);

        //
        // Output a message stating the results.
        //
        std::cout << "Job Scheduled with ID " << id << std::endl;


		oldHour = hour;
        hour  = (hour + (minute + offset)/60)%24;
        oldMinute = minute;
		minute = (minute + offset)%60;        // each job is separated by offset
    }
}


TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleType SchedulingAgentCLI::getScheduleType(
    const std::string& asString)
{
    if(asString == "OnceOff")
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::OnceOff;
    }
    else if(asString == "Daily")
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::Daily;
    }
    else if(asString == "Weekly")
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::Weekly;
    }
    else if(asString == "Monthly")
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::Monthly;
    }
    else if(asString == "Yearly")
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::Yearly;
    }
    else
    {
        std::string message = "Unrecognised schedule type: '";
        message            += asString;
        message            += "'";
        
        TA_THROW(InvalidSyntaxException(message));
    }
}


TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear SchedulingAgentCLI::getMonthOfYear(
    const std::string& asString)
{
    if(asString == "January")
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::January;
    }
    else if(asString == "February")
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::February;
    }
    else if(asString == "March")
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::March;
    }
    else if(asString == "April")
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::April;
    }
    else if(asString == "May")
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::May;
    }
    else if(asString == "June")
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::June;
    }
    else if(asString == "July")
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::July;
    }
    else if(asString == "August")
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::August;
    }
    else if(asString == "September")
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::September;
    }
    else if(asString == "October")
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::October;
    }
    else if(asString == "November")
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::November;
    }
    else if(asString == "December")
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::December;
    }
    else
    {
        std::string message = "Unrecognised month: '";
        message            += asString;
        message            += "'";
        
        TA_THROW(InvalidSyntaxException(message));
    }
}


TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeekSequence
    SchedulingAgentCLI::getDaysOfWeek(
        const std::string& asString)
{
    const std::string DAYS_OF_WEEK_SEPARATOR = ",";
    
    typedef std::vector< std::string > DaysOfWeekVector;
    DaysOfWeekVector daysOfWeekVector;


    if(asString.length() == 0)
    {
        std::string message = "No days of week specified.";

        TA_THROW(InvalidSyntaxException(message));
    }

    //
    // Look for occurences of the separator string and extract the words
    // in between.
    //
    std::string::size_type startWord = 0;
    std::string::size_type endWord   = asString.find(DAYS_OF_WEEK_SEPARATOR);
    
    while( endWord != std::string::npos )
    {
        std::string::size_type strLength = endWord - startWord;
        daysOfWeekVector.push_back( std::string(asString, startWord, strLength) );

        startWord = endWord + 1;
        endWord   = asString.find(DAYS_OF_WEEK_SEPARATOR, startWord);
    }

    //
    // The last word in the string won't have a DAYS_OF_WEEK_SEPARATOR
    // after it; it will be terminated by the end of the string.
    //
    std::string::size_type strLength = asString.length() - startWord;
    daysOfWeekVector.push_back( std::string(asString, startWord, strLength) );


    TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeekSequence daysOfWeekSequence;
    daysOfWeekSequence.length(daysOfWeekVector.size());

    for(unsigned int i = 0; i < daysOfWeekVector.size(); i ++)
    {
        TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeek day =
            getDayOfWeek(daysOfWeekVector[i]);

        daysOfWeekSequence[i] = day;
    }


    return daysOfWeekSequence;
}



TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeek SchedulingAgentCLI::getDayOfWeek(
    const std::string& asString)
{
    const char* dayOfWeekChars = asString.c_str();

    //
    // Note that the comparison is case insensitive
    //
    if(      strcmp(dayOfWeekChars, "Monday") == 0 )
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::Monday;
    }
    else if( strcmp(dayOfWeekChars, "Tuesday") == 0 )
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::Tuesday;
    }
    else if( strcmp(dayOfWeekChars, "Wednesday") == 0 )
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::Wednesday;
    }
    else if( strcmp(dayOfWeekChars, "Thursday") == 0 )
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::Thursday;
    }
    else if( strcmp(dayOfWeekChars, "Friday") == 0 )
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::Friday;
    }
    else if( strcmp(dayOfWeekChars, "Saturday") == 0 )
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::Saturday;
    }
    else if( strcmp(dayOfWeekChars, "Sunday") == 0 )
    {
        return TA_Base_Bus::SchedulingAgentCorbaDef::Sunday;
    }
    else
    {
        std::string message = "Unrecognised day in sequence: '";
        message            += asString;
        message            += "'";

        TA_THROW(InvalidSyntaxException(message));
    }
}




SchedulingAgentCLI::EJobType
    SchedulingAgentCLI::getJobType(const std::string& asString)
{
    if(asString == "Plan")
    {
        return PlanRunnerJob;
    }
    /*else if(asString == "AlarmGeneratorJob")
    {
        return AlarmGeneratorJob;
    }*/
    else
    {
        std::string message = "Unrecognised job type: '";
        message            += asString;
        message            += "'";
        TA_THROW(InvalidSyntaxException(message));
    }
}


unsigned long SchedulingAgentCLI::getStringAsUnsignedLong(
    const std::string& asString)
{
    std::stringstream stringStream;
    stringStream << asString;

    unsigned long number = 0;
    if(stringStream >> number)
    {
        return number;
    }

    std::string message = "Could not convert string to unsigned long: '";
    message            += asString;
    message            += "'";
    
    TA_THROW(InvalidSyntaxException(message));
}



unsigned short SchedulingAgentCLI::getStringAsUnsignedShort(
    const std::string& asString)
{
    std::stringstream stringStream;
    stringStream << asString;

    unsigned short number = 0;
    if(stringStream >> number)
    {
        return number;
    }

    std::string message = "Could not convert string to unsigned short: '";
    message            += asString;
    message            += "'";
    
    TA_THROW(InvalidSyntaxException(message));
}


bool SchedulingAgentCLI::getStringAsBool(const std::string& asString)
{
    if(asString == "true")
    {
        return true;
    }
    else if(asString == "false")
    {
        return false;
    }
    else
    {
        std::string message = "Could not convert string to bool: '";
        message            += asString;
        message            += "'";

        TA_THROW(InvalidSyntaxException(message));
    }
}


void SchedulingAgentCLI::processDeleteCommand(const CommandLine& commandLine)
{
	std::string id = commandLine.getParameter("id");
    SchedulingAgentNamedObjRef schedulingAgent(m_schedulingAgentEntity, true);

	/*
    unsigned short numJobs = 1;
    std::string numJobsStr;
    if ((numJobsStr = commandLine.getParameter("numJobs")) != "")
    {
        numJobs = getStringAsUnsignedShort(numJobsStr);
    }

    //
    // Delete the job.  If numJobs are specified, it will start deleting from the 
    // id specified to id+numJobs.
    //

    for (unsigned short i = 0; i < numJobs; i++)
    {
        CORBA_CALL( schedulingAgent, removeJob, ( id + i, m_sessionKey.c_str() ) );//limin

    //
    // Output a message stating the results.
    //
        std::cout << "Job with ID " << id+i << " deleted" << std::endl;
    }
	*/

	// Using uuid, we can not delete a sequence of jobs.
    CORBA_CALL( schedulingAgent, removeJob, ( id.c_str(), m_sessionKey.c_str() ) );//limin
	std::cout << "Job with ID " << id << " deleted" << std::endl;

}

void SchedulingAgentCLI::processGetTypesCommand(const CommandLine& commandLine)
{
    try
    {
        SchedulingAgentNamedObjRef schedulingAgent(m_schedulingAgentEntity, true);

        TA_Base_Bus::SchedulingAgentCorbaDef::JobTypeSeq* types = NULL;
        CORBA_CALL_RETURN( types, schedulingAgent, getSupportedJobTypes, ( m_sessionKey.c_str() ) );//limin        

        for (unsigned long i = 0; i < types->length(); i++)
        {
            std::string s((*types)[i]);
            
            std::cout << "JobType: '" << s << "'" << std::endl;
        }

        delete types;
    }
    catch(const CORBA::SystemException& /*e*/)
    {
        //
        std::cout << "Caught a CORBA::SystemException" << std::endl;
    }
    catch(const CORBA::UserException& /*e*/)
    {
        std::cout << "Caught a CORBA::UserException" << std::endl;
    }
    catch(...)
    {
        std::cout << "An unexpected error occured" << std::endl;
    }
}



void SchedulingAgentCLI::processGetCommand(const CommandLine& commandLine)
{
	std::string id = commandLine.getParameter("id");

    //
    // Get the job
    //
    SchedulingAgentNamedObjRef schedulingAgent(m_schedulingAgentEntity, true);

    try
    {
        TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef* job = NULL;
        CORBA_CALL_RETURN( job, schedulingAgent, getJob, ( id.c_str(), m_sessionKey.c_str() ) );//limin

        //
        // Output a message stating the results.
        //
        std::cout << getJobCorbaDefAsString(*job);
    }
    catch(TA_Base_Bus::SchedulingAgentCorbaDef::JobDoesNotExistException)
    {
        std::cout << "Job with id '" << id << "' does not exist";
    }
}

void SchedulingAgentCLI::processGetAllCommand(const CommandLine& commandLine)
{
    //
    // Get the job
    //
    SchedulingAgentNamedObjRef schedulingAgent( m_schedulingAgentEntity, true );

    TA_Base_Bus::SchedulingAgentCorbaDef::JobSequence* jobs = NULL;
    CORBA_CALL_RETURN( jobs, schedulingAgent, getAllJobs, ( m_sessionKey.c_str() ) );//limin

    //
    // Output a message stating the results.
    //
    std::cout << "There are " << jobs->length() << " jobs:" << std::endl;
    for(unsigned int i = 0; i < jobs->length(); i ++)
    {
        std::cout << getJobCorbaDefAsString((*jobs)[i]) << std::endl;
    }
}

void SchedulingAgentCLI::processHelpCommand(const CommandLine& commandLine)
{
    //
    // The second parameter is the command about which help is required.
    //
    std::string whichCommand = commandLine.getParameter(1);

    if(whichCommand == "")
    {
        outputHelpMessage();
    }
    else if(whichCommand == "add")
    {
        std::cout <<
            "add " << std::endl <<
            "\t[-f scheduleFrequency]\t " <<
            "must be OnceOff, Daily, Weekly, Monthly or Yearly" << std::endl <<
            "\t[-m minute]\t eg 32" << std::endl <<
            "\t[-h hour]\t eg 22" << std::endl <<
            "\t[-d day of month]\t eg 27" << std::endl <<
            "\t[-month month]\t eg February" << std::endl <<
            "\t[-w days of week]\t eg Monday,Tuesday,Friday" << std::endl <<
            "\t[-ed end day]\t eg 25" << std::endl <<
            "\t[-em end month]\t eg February" << std::endl <<
            "\t[-ey end year]\t eg 2005" << std::endl <<
            "\t[-i isInhibited]\t must be true or false" << std::endl <<
            "\t[-desc description]\t eg \"This is the description\"" << std::endl <<
            "\t[-t job type]\t must be Plan or AlarmGeneratorJob" << std::endl <<
            "\t[-numJobs OPTIONAL number of jobs]\t eg 5" << std::endl <<
            "\t[-offset OPTIONAL number of minutes between each job if numJobs is specified]\t eg 1" << std::endl <<
            "\t[-delay OPTIONAL the delay in milliseconds between each corba call to add a new job" << std::endl <<
            std::endl <<
            "Parameters specific to Plans:" << std::endl <<
            "\t[-p absolute plan path] eg \"/Category XYZ/Plan 123\"" << std::endl <<
            std::endl <<
            "Parameters specific to AlarmGeneratorJobs:" << std::endl <<
            "\t[-alarmdesc description] eg \"Hello there\"" << std::endl <<
            "\t[-ask alarm severity key] eg 3" << std::endl <<
            std::endl <<
            "Note that if a parameter is not supplied, the same value is " <<
            "used as last time the command was run." << std::endl <<
            std::endl <<
            "Example:" << std::endl <<
            "add -f Monthly -m 10 -h 10 -d 22 -month October -ed 0 -em October -ey 0 " <<
            "-i false -desc HiThree -t Plan -p /GRC/POW-BLV-NDP/CLOSE-DOT01&DIT02 -numJobs 50 " <<
            "-offset 1 -delay 100 -profile 11 -location 1" << std::endl;
    }
    else if(whichCommand == "delete")
    {
        std::cout <<
            "delete " << std::endl <<
            "\t[-id job identifier]\t eg 35" << std::endl <<
            "\t[-numJobs OPTIONAL number of jobs]\t eg 5" << std::endl <<
            std::endl <<
            "Example:" << std::endl <<
            "delete -id 5 -numJobs 10" << std::endl;
    }
    else if(whichCommand == "get")
    {
        std::cout <<
            "get " << std::endl <<
            "\t[-id job identifier]\t eg 35" << std::endl <<
            std::endl <<
            "Example:" << std::endl <<
            "get -id 5" << std::endl;
    }
    else if(whichCommand == "getall")
    {
        std::cout << "getall (no parameters)" << std::endl;
    }
    else if(whichCommand == "quit" || whichCommand == "exit")
    {
        std::cout << "quit or exit (no parameter)" << std::endl;
    }
    else
    {
        std::cout << "Unrecognised command." << std::endl;
        outputHelpMessage();
    }
}



std::string SchedulingAgentCLI::getJobCorbaDefAsString(
    const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job)
{
    
    std::string monthAsString = "";
    switch(job.schedule.month)
    {
        case TA_Base_Bus::SchedulingAgentCorbaDef::January:
            monthAsString = "January";
            break;
        case TA_Base_Bus::SchedulingAgentCorbaDef::February:
            monthAsString = "February";
            break;
        case TA_Base_Bus::SchedulingAgentCorbaDef::March:
            monthAsString = "March";
            break;
        case TA_Base_Bus::SchedulingAgentCorbaDef::April:
            monthAsString = "April";
            break;
        case TA_Base_Bus::SchedulingAgentCorbaDef::May:
            monthAsString = "May";
            break;
        case TA_Base_Bus::SchedulingAgentCorbaDef::June:
            monthAsString = "June";
            break;
        case TA_Base_Bus::SchedulingAgentCorbaDef::July:
            monthAsString = "July";
            break;
        case TA_Base_Bus::SchedulingAgentCorbaDef::August:
            monthAsString = "August";
            break;
        case TA_Base_Bus::SchedulingAgentCorbaDef::September:
            monthAsString = "September";
            break;
        case TA_Base_Bus::SchedulingAgentCorbaDef::October:
            monthAsString = "October";
            break;
        case TA_Base_Bus::SchedulingAgentCorbaDef::November:
            monthAsString = "November";
            break;
        case TA_Base_Bus::SchedulingAgentCorbaDef::December:
            monthAsString = "December";
            break;
    }

//    std::string daysOfWeekAsString = "";
        //timer.getDayOfWeekSequenceAsString(job.schedule.daysOfWeek);

    std::string isInhibited = (job.isInhibited ? "Yes" : "No");

    //
    // Get the schedule type as a string
    //
    std::string scheduleType = "";
    switch(job.schedule.type)
    {
        case TA_Base_Bus::SchedulingAgentCorbaDef::OnceOff:
            scheduleType = "Once off";
        break;
        case TA_Base_Bus::SchedulingAgentCorbaDef::Daily:
            scheduleType = "Daily";
        break;
        case TA_Base_Bus::SchedulingAgentCorbaDef::Weekly:
            scheduleType = "Weekly";
        break;
        case TA_Base_Bus::SchedulingAgentCorbaDef::Monthly:
            scheduleType = "Monthly";
        break;
        case TA_Base_Bus::SchedulingAgentCorbaDef::Yearly:
            scheduleType = "Yearly";
        break;
    }


    //
    // Get the job as a string
    //
    std::stringstream jobAsString;
    jobAsString <<
        "ID: "                    << job.identifier          << ", "  <<
        "Type: "                  << job.type.in()           << ", "  <<
        "Description: '"          << job.description.in()    << "', " <<
        "isInhibited: "           << isInhibited             << ", "  <<
        "Schedule: '"             << scheduleType            << "', " <<
        "Time: "
            << std::setw(2) << std::setfill('0') << job.schedule.hour << ":"
            << std::setw(2) << std::setfill('0') << job.schedule.minute;
    
    //
    // Add week days or date fields depending on the type
    //
    switch(job.schedule.type)
    {
        case TA_Base_Bus::SchedulingAgentCorbaDef::Weekly:
//            jobAsString << ", Days of week: " << daysOfWeekAsString;
            break;

        case TA_Base_Bus::SchedulingAgentCorbaDef::Monthly:
            jobAsString << ", Day of month: " << job.schedule.dayOfMonth;
            break;

        case TA_Base_Bus::SchedulingAgentCorbaDef::OnceOff:
        case TA_Base_Bus::SchedulingAgentCorbaDef::Yearly:
            jobAsString << ", Date: "
                        << job.schedule.dayOfMonth << " " << monthAsString;
            break;
        case TA_Base_Bus::SchedulingAgentCorbaDef::Daily:
            break;
        default:
            TA_ASSERT(false, "Unknown job.schedule.type");
    }

    //
    // If the job is recurring, add the end date
    //
    if(job.schedule.type != TA_Base_Bus::SchedulingAgentCorbaDef::OnceOff)
    {
        jobAsString << ", End date: ";

        if(job.schedule.endDay == 0)
        {
            jobAsString << "None";
        }
        else
        {
            jobAsString
                << std::setw(2) << std::setfill('0') << job.schedule.endDay << "/"
                << std::setw(2) << std::setfill('0') << job.schedule.endMonth << "/"
                << job.schedule.endYear;
        }
    }
    
    jobAsString << std::endl;


    //
	// add the data field(s)
    //
    jobAsString << "data: ";
    for (unsigned int i = 0; i < job.data.length(); i++)
    {
        jobAsString << "\"" << job.data[i].name.in() << ":" << job.data[i].value.in() << "\" " << std::endl;
	}


    return jobAsString.str();
}


}// TA_Base_App
