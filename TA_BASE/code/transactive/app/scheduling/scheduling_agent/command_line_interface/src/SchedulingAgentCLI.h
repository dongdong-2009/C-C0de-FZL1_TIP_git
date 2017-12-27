/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_agent/command_line_interface/src/SchedulingAgentCLI.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This application provides a command line interface to the Scheduling Agent
  *
  */


#ifndef SCHEDULING_AGENT_CLI_H
#define SCHEDULING_AGENT_CLI_H


#include "core/naming/src/NamedObject.h"
#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"

#include "app/scheduling/scheduling_agent/command_line_interface/src/CommandLine.h"


namespace TA_Base_App
{


class SchedulingAgentCLI
{

public:

    typedef enum EJobType { PlanRunnerJob, AlarmGeneratorJob };

    typedef TA_Base_Core::NamedObject
    <
        TA_Base_Bus::SchedulingAgentCorbaDef,
	    TA_Base_Bus::SchedulingAgentCorbaDef_ptr,
	    TA_Base_Bus::SchedulingAgentCorbaDef_var
    >
    SchedulingAgentNamedObjRef;


    /**
     *  Constructor v1: creates a new session
     */
    SchedulingAgentCLI(
        const std::string& username,
        unsigned long profileId,
        unsigned long locationId,
        unsigned long workstationId,
        const std::string& schedulingAgentEntity);

    /**
     *  Constructor v2: uses an existing session.
     */
    SchedulingAgentCLI(
        const std::string& sessionId,
        unsigned long profileId,
        unsigned long locationId,
        const std::string& schedulingAgentEntity);

    /**
     *  Starts the Scheduling Agent command line interface.
     */
    void execute();


    void outputStartupMessage();
    void outputHelpMessage();


    bool processInputCommand();

    void processAddCommand(const CommandLine& commandLine);

    void processDeleteCommand(const CommandLine& commandLine);
    
    void processGetTypesCommand(const CommandLine& commandLine);

    void processGetCommand(const CommandLine& commandLine);

    void processGetAllCommand(const CommandLine& commandLine);

    void processHelpCommand(const CommandLine& commandLine);

    std::string SchedulingAgentCLI::getJobCorbaDefAsString(
        const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job);

    CommandLine getCommandLine();



    TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeekSequence getDaysOfWeek(
            const std::string& asString);
    TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeek getDayOfWeek(
        const std::string& asString);
    TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleType getScheduleType(
        const std::string& asString);
    TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear getMonthOfYear(
        const std::string& asString);
    unsigned long getStringAsUnsignedLong(const std::string& asString);
    unsigned short getStringAsUnsignedShort(const std::string& asString);
    bool getStringAsBool(const std::string& asString);
    EJobType getJobType(const std::string& asString);


    TA_Base_Bus::SchedulingAgentCorbaDef::ScheduleType m_scheduleFrequency;
    unsigned short m_minute;
    unsigned short m_hour;
    unsigned short m_dayOfMonth;
    TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear m_month;
    TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeekSequence m_daysOfWeek;
    unsigned short m_endDay;
    TA_Base_Bus::SchedulingAgentCorbaDef::MonthOfYear m_endMonth;
    unsigned short m_endYear;
    bool m_isInhibited;
    std::string m_description;
    EJobType m_jobType;
    std::string m_planPath;
    unsigned long m_alarmSeverityKey;
    std::string m_alarmDescription;
    std::string m_sessionKey;
    unsigned long m_profileId;
    unsigned long m_locationId;
    std::string m_schedulingAgentEntity;
};


} // TA_Base_App

#endif // SCHEDULING_AGENT_CLI_H

