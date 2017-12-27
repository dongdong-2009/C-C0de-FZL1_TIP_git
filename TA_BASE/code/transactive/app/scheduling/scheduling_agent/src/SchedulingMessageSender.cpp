/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/SchedulingMessageSender.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #6 $
  *
  * Last modification: $DateTime: 2013/11/01 15:29:57 $
  * Last modified by:  $Author: huangjian $
  *
  *
  * This class is used to send messages on the Comms, StateUpdate and Audit
  * channels.
  *
  */

#include <iomanip>

#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/SchedulingAudit_MessageTypes.h"
#include "core/message/types/SchedulingComms_MessageTypes.h"
#include "core/message/types/SchedulingStateUpdate_MessageTypes.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/scheduling_agent/src/IConfigJobData.h"
#include "core/data_access_interface/scheduling_agent/src/JobDataAccessFactory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"

#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"
#include "app/scheduling/scheduling_agent/src/SchedulingMessageSender.h"
#include "app/scheduling/scheduling_agent/src/Schedule.h"

namespace TA_Base_App
{

SchedulingMessageSender* SchedulingMessageSender::m_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable SchedulingMessageSender::m_singletonLock;

    
SchedulingMessageSender::SchedulingMessageSender() :
m_auditSender(NULL),
m_commsSender(NULL),
m_stateSender(NULL),
m_entityKey(0),
m_parentEntityKey(0),
m_subsystemKey(0),
m_locationKey(0)
{
    FUNCTION_ENTRY("SchedulingMessageSender");

    m_auditSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(TA_Base_Core::SchedulingAudit::Context );
    m_commsSender =  TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender(TA_Base_Core::SchedulingComms::Context);
    m_stateSender = TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender(TA_Base_Core::SchedulingStateUpdate::Context);

    FUNCTION_EXIT;
}


SchedulingMessageSender::~SchedulingMessageSender()
{
    FUNCTION_ENTRY("~SchedulingMessageSender");

    delete m_auditSender;
    m_auditSender = NULL;

    delete m_commsSender;
    m_commsSender = NULL;

    delete m_stateSender;
    m_stateSender = NULL;

    FUNCTION_EXIT;
}


void SchedulingMessageSender::initialise( unsigned long entityKey, unsigned long parentEntityKey, unsigned long subsystemKey, unsigned long locationKey)
{
    FUNCTION_ENTRY("initialise");

    m_entityKey = entityKey;
    m_parentEntityKey = parentEntityKey;
    m_subsystemKey = subsystemKey;
    m_locationKey = locationKey;

    FUNCTION_EXIT;
}


SchedulingMessageSender& SchedulingMessageSender::getInstance()
{
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    if(m_instance == NULL)
    {
        m_instance = new SchedulingMessageSender();
    }

    return *m_instance;
}


void SchedulingMessageSender::jobAdded( const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job, const std::string& sessionKey )
{
    FUNCTION_ENTRY("jobAdded");

    CORBA::Any jobAsAny;
    jobAsAny <<= job;

    std::string details = "Job: ";
    details += getJobCorbaDefAsString(job);

    TA_Base_Core::DescriptionParameters params;
    TA_Base_Core::NameValuePair jobDescriptionPair("JobDescription", job.description.in());

    // TD #12041
    // Add the profile, location and type info to the event message
    std::auto_ptr<TA_Base_Core::IProfile> ptrProfile(TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(job.profileIdentifier));
    TA_Base_Core::NameValuePair profilePair("ProfileName", ptrProfile->getName());

    std::auto_ptr<TA_Base_Core::ILocation> ptrLocation(TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(job.locationIdentifier));
    TA_Base_Core::NameValuePair locationPair("LocationName", ptrLocation->getName());

    TA_Base_Core::NameValuePair typePair("TypeName", job.type.in());

    TA_Base_Core::NameValuePair frequencySummaryPair("FrequencySummary", getFrequencySummary(job));

    params.push_back(&jobDescriptionPair);
    params.push_back(&profilePair);
    params.push_back(&locationPair);
    params.push_back(&typePair);
    params.push_back(&frequencySummaryPair);

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Sending Comms and Audit messages: Job Added %s", job.identifier.in() );

    // Send a Comms message
    m_commsSender->sendCommsMessage( TA_Base_Core::SchedulingComms::SchedulingCommsJobAdded,
        m_entityKey, jobAsAny, m_subsystemKey, m_locationKey);

    // Send an Audit message
    sendAuditMessage( TA_Base_Core::SchedulingAudit::SchedulingAddJob, params, details, sessionKey );

    FUNCTION_EXIT;
}


void SchedulingMessageSender::jobScheduled( const std::string& jobID, const TA_Base_Bus::SchedulingAgentCorbaDef::RunTimeCorbaDef& runTime )
{
    FUNCTION_ENTRY("jobScheduled");

    TA_Base_Bus::SchedulingAgentCorbaDef::JobTimeCorbaDef jobTime;
    jobTime.jobIdentifer = jobID.c_str();
    jobTime.runTime = runTime;

    CORBA::Any jobTimeAsAny;
    jobTimeAsAny <<= jobTime;

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Sending Comms message: Job Scheduled %s", jobID.c_str() );

    // Send a Comms message
    m_commsSender->sendCommsMessage( TA_Base_Core::SchedulingComms::SchedulingCommsJobScheduled,
        m_entityKey, jobTimeAsAny, m_subsystemKey, m_locationKey);

    FUNCTION_EXIT;
}

void SchedulingMessageSender::jobUpdated( const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job,
    const std::string& sessionKey, bool  changeEnabled )
{
    FUNCTION_ENTRY("jobUpdated");

    CORBA::Any jobAsAny;
    jobAsAny <<= job;

    std::string details = "New job details: ";
    details += getJobCorbaDefAsString(job);

    TA_Base_Core::NameValuePair jobDescriptionPair("JobDescription", job.description.in());
    
    // TD #12041
    // Add the profile, location, type and frequency summary info to the event message
    std::auto_ptr<TA_Base_Core::IProfile> ptrProfile(TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(job.profileIdentifier));
    TA_Base_Core::NameValuePair profilePair("ProfileName", ptrProfile->getName());

    std::auto_ptr<TA_Base_Core::ILocation> ptrLocation(TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(job.locationIdentifier));
    TA_Base_Core::NameValuePair locationPair("LocationName", ptrLocation->getName());

    TA_Base_Core::NameValuePair typePair("TypeName", job.type.in());
    TA_Base_Core::NameValuePair frequencySummaryPair("FrequencySummary", getFrequencySummary(job));

    TA_Base_Core::DescriptionParameters params;
    params.push_back(&jobDescriptionPair);
    params.push_back(&profilePair);
    params.push_back(&locationPair);
    params.push_back(&typePair);
    params.push_back(&frequencySummaryPair);

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Sending Comms and Audit messages: Job Updated %s", job.identifier.in() );

    // Send a Comms message
    m_commsSender->sendCommsMessage( TA_Base_Core::SchedulingComms::SchedulingCommsJobUpdated,
        m_entityKey, jobAsAny, m_subsystemKey, m_locationKey);

    // Send an Audit message
     if (changeEnabled)
     {
         sendAuditMessage( job.isInhibited? TA_Base_Core::SchedulingAudit::SchedulingDisableJob : TA_Base_Core::SchedulingAudit::SchedulingEnableJob,
             params, details, sessionKey );
     }
     else
     {
         sendAuditMessage( TA_Base_Core::SchedulingAudit::SchedulingModifyJob, params, details, sessionKey );
     }

     FUNCTION_EXIT;
}


void SchedulingMessageSender::jobDeleted( const std::string& jobID, const std::string& sessionKey)
{
    FUNCTION_ENTRY("jobDeleted");

    std::string details = "";

    std::auto_ptr<TA_Base_Core::IConfigJobData> job( TA_Base_Core::JobDataAccessFactory::getInstance().getJob(jobID) );
    TA_Base_Core::NameValuePair jobDescriptionPair("JobDescription", job->getDescription());

    // TD #12041
    // Add the profile, location and type info to the event message
    std::auto_ptr<TA_Base_Core::IProfile> ptrProfile(TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(job->getProfileID()));
    TA_Base_Core::NameValuePair profilePair("ProfileName", ptrProfile->getName());

    std::auto_ptr<TA_Base_Core::ILocation> ptrLocation(TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(job->getLocationID()));
    TA_Base_Core::NameValuePair locationPair("LocationName", ptrLocation->getName());

    TA_Base_Core::NameValuePair typePair("TypeName", job->getJobType());

    TA_Base_Core::DescriptionParameters params;
    params.push_back(&jobDescriptionPair);
    params.push_back(&profilePair);
    params.push_back(&locationPair);
    params.push_back(&typePair);

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Sending Comms and Audit messages: Job Deleted %s", jobID.c_str() );

    CORBA::Any jobIDAsAny;
    jobIDAsAny <<= jobID.c_str();

    // Send a Comms message
    m_commsSender->sendCommsMessage( TA_Base_Core::SchedulingComms::SchedulingCommsJobDeleted,
        m_entityKey, jobIDAsAny, m_subsystemKey, m_locationKey);

    // Send an Audit message
    sendAuditMessage( TA_Base_Core::SchedulingAudit::SchedulingDeleteJob, params, details, sessionKey );

    FUNCTION_EXIT;
}


void SchedulingMessageSender::jobStarting( const std::string& jobID, unsigned long minuteStarted, const std::string& sessionKey)
{
    FUNCTION_ENTRY("jobStarting");

    // Create a JobInstanceCorbaDef from the parameters
    TA_Base_Bus::SchedulingAgentCorbaDef::JobInstanceCorbaDef instance = { jobID.c_str(), minuteStarted };

    // Create a CORBA::Any to contain the job instance
    CORBA::Any jobInstanceAsAny;
    jobInstanceAsAny <<= instance;

    std::string details = "";

    std::stringstream minuteStartedString;
    minuteStartedString << minuteStarted;

    std::auto_ptr<TA_Base_Core::IConfigJobData> job(TA_Base_Core::JobDataAccessFactory::getInstance().getJob(jobID));

    TA_Base_Core::NameValuePair jobDescriptionPair("JobDescription", job->getDescription());
    TA_Base_Core::NameValuePair minuteStartedPair("InstanceID", minuteStartedString.str());

    // TD #12041
    // Add the profile, location and type info to the event message
    std::auto_ptr<TA_Base_Core::IProfile> ptrProfile(TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(job->getProfileID()));
    TA_Base_Core::NameValuePair profilePair("ProfileName", ptrProfile->getName());

    std::auto_ptr<TA_Base_Core::ILocation> ptrLocation(TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(job->getLocationID()));
    TA_Base_Core::NameValuePair locationPair("LocationName", ptrLocation->getName());

    TA_Base_Core::NameValuePair typePair("TypeName", job->getJobType());

    TA_Base_Core::DescriptionParameters params;
    params.push_back(&minuteStartedPair);
    params.push_back(&jobDescriptionPair);
    params.push_back(&profilePair);
    params.push_back(&locationPair);
    params.push_back(&typePair);

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Sending State update and Audit messages: Job Started %s", jobID.c_str() );

    // Send a State Update message.
    m_stateSender->sendStateUpdateMessage( TA_Base_Core::SchedulingStateUpdate::SchedulingStateUpdateJobStarting, 
        m_entityKey, TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME), jobInstanceAsAny);

    // Send an Audit message
    sendAuditMessage( TA_Base_Core::SchedulingAudit::SchedulingJobStarting, params, details, sessionKey );

    FUNCTION_EXIT;
}


void SchedulingMessageSender::jobCompleted( const std::string& jobID, unsigned long minuteStarted, const std::string& sessionKey, const std::string& errorMessage)
{
    FUNCTION_ENTRY("jobCompleted");

    // Create a JobInstanceCorbaDef from the parameters
    TA_Base_Bus::SchedulingAgentCorbaDef::JobInstanceCorbaDef instance = { jobID.c_str(), minuteStarted };

    // Create a CORBA::Any to contain the job instance
    CORBA::Any jobInstanceAsAny;
    jobInstanceAsAny <<= instance;

    std::string details = "";
    if( !errorMessage.empty() )
    {
        details += "Error message: ";
        details += errorMessage;
    }

    std::stringstream minuteStartedString;
    minuteStartedString << minuteStarted;

    std::auto_ptr<TA_Base_Core::IConfigJobData> job(TA_Base_Core::JobDataAccessFactory::getInstance().getJob(jobID));

    TA_Base_Core::NameValuePair jobDescriptionPair("JobDescription", job->getDescription());
    TA_Base_Core::NameValuePair minuteStartedPair("InstanceID", minuteStartedString.str());

    // TD #12041
    // Add the profile, location and type info to the event message
    std::auto_ptr<TA_Base_Core::IProfile> ptrProfile(TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(job->getProfileID()));
    TA_Base_Core::NameValuePair profilePair("ProfileName", ptrProfile->getName());

    std::auto_ptr<TA_Base_Core::ILocation> ptrLocation(TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(job->getLocationID()));
    TA_Base_Core::NameValuePair locationPair("LocationName", ptrLocation->getName());

    TA_Base_Core::NameValuePair typePair("TypeName", std::string(job->getJobType()));

    TA_Base_Core::DescriptionParameters params;
    params.push_back(&minuteStartedPair);
    params.push_back(&jobDescriptionPair);
    params.push_back(&profilePair);
    params.push_back(&locationPair);
    params.push_back(&typePair);

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Sending State Updated and Audit messages: Job Completed %s", jobID.c_str() );

    // Send a State Update message.
    m_stateSender->sendStateUpdateMessage( TA_Base_Core::SchedulingStateUpdate::SchedulingStateUpdateJobCompleted,
        m_entityKey, TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME), jobInstanceAsAny);

    // Send an Audit message
    sendAuditMessage( TA_Base_Core::SchedulingAudit::SchedulingJobCompleted,
        params, details, sessionKey );

    FUNCTION_EXIT;
}


std::string SchedulingMessageSender::getFrequencySummary(
     const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job)
{
    std::ostringstream frequencySummary;

    switch(job.schedule.type)
    {
        case TA_Base_Bus::SchedulingAgentCorbaDef::OnceOff:
            // Occurs once on <date> at <time>.z
            frequencySummary << "Occurs once on ";
            frequencySummary << getFormattedDate(job.schedule.year, job.schedule.month+1, job.schedule.dayOfMonth);
            frequencySummary << " at ";
            frequencySummary << getFormattedTime(job.schedule.hour, job.schedule.minute);
            break;
        case TA_Base_Bus::SchedulingAgentCorbaDef::Daily:
            // Occurs daily at <time>. [Ends on <end date>.]
            frequencySummary << "Occurs daily at ";
            frequencySummary << getFormattedTime(job.schedule.hour, job.schedule.minute);
            if(job.schedule.endDay > 0)
            {
                frequencySummary << ". Ends on ";
                frequencySummary << getFormattedDate(job.schedule.endYear, job.schedule.endMonth+1, job.schedule.endDay);
            }
            break;
        case TA_Base_Bus::SchedulingAgentCorbaDef::Weekly:
            // Occurs weekly on <comma delimited days of week> at <time>. [Ends on <end date>.]
            frequencySummary << "Occurs weekly on ";
            frequencySummary << getDayOfWeekSequenceAsString(job.schedule.daysOfWeek);
            frequencySummary << " at ";
            frequencySummary << getFormattedTime(job.schedule.hour, job.schedule.minute);
            if(job.schedule.endDay > 0)
            {
                frequencySummary << ". Ends on ";
                frequencySummary << getFormattedDate(job.schedule.endYear, job.schedule.endMonth+1, job.schedule.endDay);
            }
            break;
        case TA_Base_Bus::SchedulingAgentCorbaDef::Monthly:
            // Occurs monthly on the last day at <time>. [Ends on <end date>.]
            // OR
            // Occurs monthly on the <day of month>th day at <time>. [Ends on <end date>.]
            frequencySummary << "Occurs monthly on the ";
            if(job.schedule.dayOfMonth == 0)
            {
                frequencySummary << "last";
            }
            else
            {
                frequencySummary << job.schedule.dayOfMonth;
                switch(job.schedule.dayOfMonth)
                {
                    case 1:
                    case 21:
                    case 31:
                        frequencySummary << "st";
                        break;
                    case 2:
                    case 22:
                        frequencySummary << "nd";
                        break;
                    case 3:
                    case 23:
                        frequencySummary << "rd";
                        break;
                    default:
                        frequencySummary << "th";
                        
                }
            }
            frequencySummary << " day at ";
            frequencySummary << getFormattedTime(job.schedule.hour, job.schedule.minute);
            if(job.schedule.endDay > 0)
            {
                frequencySummary << ". Ends on ";
                frequencySummary << getFormattedDate(job.schedule.endYear, job.schedule.endMonth+1, job.schedule.endDay);
            }
            break;
        case TA_Base_Bus::SchedulingAgentCorbaDef::Yearly:
            // Occurs yearly on the last day of February at <time>. [Ends on <end date>.]
            // OR
            // Occurs yearly on <date> day at <time>. [Ends on <end date>.]
            frequencySummary << "Occurs yearly on ";
            if(job.schedule.dayOfMonth == 0)
            {
                frequencySummary << "the last day of February";
            }
            else
            {
                frequencySummary << getFormattedDate(job.schedule.year, job.schedule.month+1, job.schedule.dayOfMonth);
            }
            frequencySummary << " at ";
            frequencySummary << getFormattedTime(job.schedule.hour, job.schedule.minute);
            if(job.schedule.endDay > 0)
            {
                frequencySummary << ". Ends on ";
                frequencySummary << getFormattedDate(job.schedule.endYear, job.schedule.endMonth+1, job.schedule.endDay);
            }
            break;
    }

    frequencySummary << ".";
    return frequencySummary.str();
}


std::string SchedulingMessageSender::getFormattedDate(int year, int month, int day)
{
    // dd/mm/yyyy
    std::ostringstream formattedDate;
    formattedDate << std::setw( 2 ) << std::setfill( '0' ) << day;
    formattedDate << "/";
    formattedDate << std::setw( 2 ) << std::setfill( '0' ) << month;
    formattedDate << "/";
    formattedDate << year;
    return formattedDate.str();
}


std::string SchedulingMessageSender::getFormattedTime(int hour, int minute)
{
    // hh:mm
    std::ostringstream formattedTime;
    formattedTime << std::setw( 2 ) << std::setfill( '0' ) << hour;
    formattedTime << ":";
    formattedTime << std::setw( 2 ) << std::setfill( '0' ) << minute;
    return formattedTime.str();
}

std::string SchedulingMessageSender::getDayOfWeekAsString(
    TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeek day)
{
    switch(day)
    {
        case TA_Base_Bus::SchedulingAgentCorbaDef::Monday:
            return "Monday";
        case TA_Base_Bus::SchedulingAgentCorbaDef::Tuesday:
            return "Tuesday";
        case TA_Base_Bus::SchedulingAgentCorbaDef::Wednesday:
            return "Wednesday";
        case TA_Base_Bus::SchedulingAgentCorbaDef::Thursday:
            return "Thursday";
        case TA_Base_Bus::SchedulingAgentCorbaDef::Friday:
            return "Friday";
        case TA_Base_Bus::SchedulingAgentCorbaDef::Saturday:
            return "Saturday";
        case TA_Base_Bus::SchedulingAgentCorbaDef::Sunday:
            return "Sunday";
        default:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unrecognised day of week");
    }

    return "";
}


std::string SchedulingMessageSender::getDayOfWeekSequenceAsString(
    const TA_Base_Bus::SchedulingAgentCorbaDef::DayOfWeekSequence& daysOfWeek)
{
    if(daysOfWeek.length() == 0)
    {
        return "";
    }

    std::string daysOfWeekString = getDayOfWeekAsString(daysOfWeek[0]);

    for(unsigned int i = 1; i < daysOfWeek.length(); i ++)
    {
        daysOfWeekString += ", ";
        daysOfWeekString += getDayOfWeekAsString(daysOfWeek[i]);
    }

    return daysOfWeekString;
}


std::string SchedulingMessageSender::getJobCorbaDefAsString(
     const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job)
{
    // Get the job as a string
    std::stringstream jobAsString;
    jobAsString << "ID: '" << job.identifier.in() << "', "  << "Description: '" << job.description.in() << "', " <<
        "isInhibited: " << (job.isInhibited ? "Yes" : "No") << ", "  << "Schedule: '" << Schedule::toString( job.schedule ) << "', " << std::endl;

    return jobAsString.str();
}


void SchedulingMessageSender::jobCompletedAllExecutions(const std::string& jobID)
{
    TA_Base_Bus::SchedulingAgentCorbaDef::RunTimeCorbaDef runTime;
    runTime.minute = 0;
    runTime.hour = 0;
    runTime.dayOfMonth = 0;
    runTime.month = TA_Base_Bus::SchedulingAgentCorbaDef::January;
    runTime.year = 0;

    jobScheduled(jobID, runTime);
}

// DTL-1114
void SchedulingMessageSender::sendAuditMessage( const TA_Base_Core::MessageType& messageType,
    TA_Base_Core::DescriptionParameters& parameters, const std::string& details, const std::string& sessionId )
{
    FUNCTION_ENTRY("submitAuditMessage");

    if (NULL == m_auditSender)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Fail to send audit message: m_auditSender is NULL");
        return;
    }

    bool submitted = false;
    std::string reason = "";

    std::string alarmID = "";
    std::string incidentID = "";
    std::string parentEventID = ""; 

    // Add a empty parameter to the first position to let event value empty
    TA_Base_Core::NameValuePair pirNullParam( "EmptyValueString", " " );
    parameters.insert( parameters.begin(), &pirNullParam );

    try
    {            
        m_auditSender->sendAuditMessage(
            messageType,                                 // Message Type
            m_entityKey,
            parameters,                       // Description
            details,                                          // Additional details
            sessionId,                                   // SessionID if applicable
            alarmID,                                          // AlarmID if an alarm associated with event
            incidentID,                                          // IncidentKey if incident associated with event
            parentEventID);                                         // EventID of a parent event, used to link events

        submitted = true;

    } // try block
    catch( TA_Base_Core::TransactiveException& ex )
    {
        reason = ex.what();
    }
    catch( ... )
    {
        reason = "Unknown reason.";
    }

    // failed to submit - make sure all the details are logged
    if (!submitted)
    {
        std::ostringstream msg;
        msg << "Failed to submit audit message " << messageType.getTypeKey() << ". " << reason << std::endl << "Parameters are: ";

        for ( TA_Base_Core::DescriptionParameters::const_iterator iter = parameters.begin();
            iter != parameters.end(); iter++ )
        {
            msg << (*iter)->name << "=" << (*iter)->value << ", ";
        }
        msg << "SessionId=" << sessionId;

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", msg.str().c_str());
    }

    FUNCTION_EXIT;
}


}; // TA_Base_App

