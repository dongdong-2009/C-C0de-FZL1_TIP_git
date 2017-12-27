/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/SchedulingMonitor.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #8 $
  *
  * Last modification: $DateTime: 2013/05/30 15:41:36 $
  * Last modified by:  $Author: qi.huang $
  *
  *
  * This class encapsulates the logic for the Scheduling Agent in monitor
  * mode.
  *
  */



#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/SchedulingComms_MessageTypes.h"
#include "core/message/types/SchedulingStateUpdate_MessageTypes.h"
#include "core/message/types/SchedulingAudit_MessageTypes.h"
#include "app/scheduling/scheduling_agent/src/JobFactory.h"
#include "app/scheduling/scheduling_agent/src/JobQueue.h"
#include "app/scheduling/scheduling_agent/src/SchedulingException.h"
#include "app/scheduling/scheduling_agent/src/SchedulingAlarmSender.h"

#include "app/scheduling/scheduling_agent/src/SchedulingMonitor.h"


using TA_Base_Core::SpecialisedMessageSubscriber;
using TA_Base_Core::MessageSubscriptionManager;
using TA_Base_Core::CommsMessageCorbaDef;
using TA_Base_Core::StateUpdateMessageCorbaDef;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::CorbaUtil;


namespace TA_Base_App
{

SchedulingMonitor* SchedulingMonitor::m_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable SchedulingMonitor::m_singletonLock;


SchedulingMonitor::SchedulingMonitor() :
m_entityKey(0),
m_subsystemKey(0),
m_locationKey(0)
{
}

SchedulingMonitor::~SchedulingMonitor()
{
    deactivate();
}


SchedulingMonitor& SchedulingMonitor::getInstance()
{
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    //
    // If this method has been called before and initialise was not called,
    // assert.
    //
    TA_ASSERT(m_instance == NULL || m_instance->m_entityKey != 0,
              "SchedulingMonitor::initialise() must be called the "
              "first time getInstance() is invoked");

    if(m_instance == NULL)
    {
        m_instance = new SchedulingMonitor();
        TA_ASSERT(m_instance != NULL, "m_instance should never be NULL");
    }
    
    return *m_instance;
}


void SchedulingMonitor::initialise(
    unsigned long entityKey,
    unsigned long subsystemKey,
    unsigned long locationKey)
{
    TA_ASSERT(entityKey   != 0, "Entity key must not be zero");
    TA_ASSERT(locationKey != 0, "Location key must not be zero");

    m_entityKey    = entityKey;
    m_subsystemKey = subsystemKey;
    m_locationKey  = locationKey;
}


void SchedulingMonitor::activate()
{
    FUNCTION_ENTRY("activate");

    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Activating Scheduling Monitor");


    //
    // Subscribe to JobAdded, JobUpdated and JobDeleted Comms messages.
    //
    MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
        TA_Base_Core::SchedulingComms::SchedulingCommsJobAdded,
        dynamic_cast<SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*>(this),
        m_entityKey,
        m_subsystemKey,
        m_locationKey);

    MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
        TA_Base_Core::SchedulingComms::SchedulingCommsJobUpdated,
        dynamic_cast<SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*>(this),
        m_entityKey,
        m_subsystemKey,
        m_locationKey);
    
    MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
        TA_Base_Core::SchedulingComms::SchedulingCommsJobDeleted,
        dynamic_cast<SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*>(this),
        m_entityKey,
        m_subsystemKey,
        m_locationKey);


    //
    // Subscribe to JobStarting and JobCompleted StateUpdate messages.
    //
    MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
        TA_Base_Core::SchedulingStateUpdate::SchedulingStateUpdateJobStarting,
        dynamic_cast<SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>*>(this),
        RunParams::getInstance().get(RPARAM_ENTITYNAME));

    MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
        TA_Base_Core::SchedulingStateUpdate::SchedulingStateUpdateJobCompleted,
        dynamic_cast<SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>*>(this),
        RunParams::getInstance().get(RPARAM_ENTITYNAME));
}


void SchedulingMonitor::deactivate()
{
    FUNCTION_ENTRY("deactivate");

    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Deactivating Scheduling Monitor");

    // Unsubscribe from all messages.
    SpecialisedMessageSubscriber<CommsMessageCorbaDef>* commsPtr = this;
    SpecialisedMessageSubscriber<StateUpdateMessageCorbaDef>* stateUpdatePtr = this;
    MessageSubscriptionManager::getInstance().unsubscribeToMessages(commsPtr);
    MessageSubscriptionManager::getInstance().unsubscribeToMessages(stateUpdatePtr);
}



void SchedulingMonitor::batchUpdateReceivedFromController(const TA_Base_Bus::SchedulingAgentCorbaDef::JobSequence& scheduledJobs)
{
    FUNCTION_ENTRY("batchUpdateReceivedFromController");

    LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Processing batch update received from controlling agent" );

    try
    {
        // Get each job from the sequence and schedule it.
        for(unsigned int i = 0; i < scheduledJobs.length(); i ++)
        {
            try
            {
                JobQueue::getInstance().scheduleJob(scheduledJobs[i]);
            }
            // JobInPastExceptions may occur if the job has completed but not
            // yet been removed from the Job Queue when the controlling agent
            // sends the batch update.
            catch(const TA_Base_Bus::SchedulingAgentCorbaDef::JobInPastException&)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
                    "JobInPastException caught for job with id '%s'. Ignoring job as it has probably already been run by controlling agent.",
                    scheduledJobs[i].identifier.in());
            }
            catch(const DataException& ex)
            {
                std::ostringstream action;
                action << "schedule job with ID '" << scheduledJobs[i].identifier.in() << "'" 
                    << " and description '" << scheduledJobs[i].description.in() << "'";

                SchedulingAlarmSender::getInstance().schedulingMonitorError(action.str(), ex.what());
            }
        }// for(unsigned int i = 0; i < scheduledJobs.length(); i++)
    }
    catch(const DatabaseException& ex)
    {
        SchedulingAlarmSender::getInstance().schedulingMonitorError("schedule a batch update", ex.what());
    }
    catch(...)
    {
        SchedulingAlarmSender::getInstance().schedulingMonitorError("schedule a batch update", "unknown");
    }
}


void SchedulingMonitor::receiveSpecialisedMessage(const CommsMessageCorbaDef& message)
{
    FUNCTION_ENTRY("receiveSpecialisedMessage");

    // JobAdded message
    if( 0 == TA_Base_Core::SchedulingComms::SchedulingCommsJobAdded.getTypeKey().compare( message.messageTypeKey.in() ))
    {
        TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef* job = NULL;

		// Extract to the JobCorbaDef
		if( ( message.messageState >>= job ) != 0 )
		{
			receiveJobAddedMessage(*job);
		}
	}
    // JobUpdated message
    else if( 0 == TA_Base_Core::SchedulingComms::SchedulingCommsJobUpdated.getTypeKey().compare( message.messageTypeKey.in() ))
    {
       	TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef* job = NULL;

		// Extract to the JobCorbaDef
		if( ( message.messageState >>= job ) != 0 )
		{
			receiveJobUpdatedMessage(*job);
		}
	}
    // JobDeleted message
    else if( 0 == TA_Base_Core::SchedulingComms::SchedulingCommsJobDeleted.getTypeKey().compare( message.messageTypeKey.in() ) )
    {
        // Extract to the JobID
        const char* jobID = NULL;
        message.messageState >>= jobID;

        if ( NULL != jobID )
        {
            std::string strJobId = jobID;

            if( !strJobId.empty() )
            {
                receiveJobDeletedMessage(strJobId);
            }
        }
	}
}


void SchedulingMonitor::receiveSpecialisedMessage(const StateUpdateMessageCorbaDef& message)
{
    // JobStarting message
    if( std::string(message.messageTypeKey) == TA_Base_Core::SchedulingStateUpdate::SchedulingStateUpdateJobStarting.getTypeKey() )
    {
        TA_Base_Bus::SchedulingAgentCorbaDef::JobInstanceCorbaDef* jobInstance = NULL;

		// Extract to the JobInstance
		if( ( message.stateUpdateInfo >>= jobInstance ) != 0 )
		{
			receiveJobStartingMessage(*jobInstance);
		}
	}

    // JobCompleted message
    else if( std::string(message.messageTypeKey) == TA_Base_Core::SchedulingStateUpdate::SchedulingStateUpdateJobCompleted.getTypeKey() )
    {
       	TA_Base_Bus::SchedulingAgentCorbaDef::JobInstanceCorbaDef* jobInstance = NULL;

		// Extract to the JobInstance
		if( ( message.stateUpdateInfo >>= jobInstance ) != 0 )
		{
			receiveJobCompletedMessage(*jobInstance);
		}
	}

}



void SchedulingMonitor::receiveJobAddedMessage(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& corbaJob)
{
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Job added message received. Job id: '%s'", corbaJob.identifier.in());

    // Used if an alarm is raised - the action that is being performed
    std::ostringstream action;
    action << "schedule job with ID '" << corbaJob.identifier.in() << "'" 
        << " and description '" << corbaJob.description.in() << "'";

    try
    {
        JobQueue::getInstance().scheduleJob(corbaJob);
    }
    catch(const TA_Base_Bus::SchedulingAgentCorbaDef::JobInPastException&)
    {
        // If a job is a once off, it may be in the past by the time the
        // monitor receives the message from the controller.
        // Thus it can be safely ignored.
        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
                    "JobInPastException caught. Probably job has already been run by controlling agent.");
    }
    catch(const TA_Base_Bus::SchedulingAgentCorbaDef::BadScheduleException& ex)
    {
        // BadScheduleExceptions should never occur because the controller
        // would not have sent this message if the schedule was invalid.
        SchedulingAlarmSender::getInstance().schedulingMonitorError(action.str(), std::string(ex.what));
    }
    // The monitoring agent should not actually go to the database,
    // but if a DataException, DatabaseException or any other kind is caught,
    // raise an alarm.
    catch(const DataException& ex)
    {
        SchedulingAlarmSender::getInstance().schedulingMonitorError(action.str(), ex.what());
    }
    catch(const DatabaseException& ex)
    {
        SchedulingAlarmSender::getInstance().schedulingMonitorError(action.str(), ex.what());
    }
    catch(...)
    {
        SchedulingAlarmSender::getInstance().schedulingMonitorError(action.str(), "unknown");
    }
}


void SchedulingMonitor::receiveJobUpdatedMessage(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& updatedJob)
{
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Job updated message received. Job id: '%s'", updatedJob.identifier.in());

    // Used if an alarm is raised - the action that is being performed
    std::ostringstream action;
    action << "update job with ID '" << updatedJob.identifier.in() << "'" << " and description '" << updatedJob.description.in() << "'";

    try
    {
        JobQueue::getInstance().updateJob(updatedJob);
    }
    // The monitoring agent should not actually go to the database,
    // but if a DataException, DatabaseException or any other kind is caught,
    // raise an alarm.
    catch(const DataException& ex)
    {
        SchedulingAlarmSender::getInstance().schedulingMonitorError(action.str(), ex.what());
    }
    catch(const DatabaseException& ex)
    {
        SchedulingAlarmSender::getInstance().schedulingMonitorError(action.str(), ex.what());
    }
    catch(const TA_Base_Bus::SchedulingAgentCorbaDef::BadScheduleException& ex)
    {
        SchedulingAlarmSender::getInstance().schedulingMonitorError(action.str(), std::string(ex.what));
    }
    catch(const CORBA::Exception& ex)
    {
        SchedulingAlarmSender::getInstance().schedulingMonitorError(action.str(), CorbaUtil::getInstance().exceptionToString(ex));
    }
    catch(const std::exception& ex)
    {
        SchedulingAlarmSender::getInstance().schedulingMonitorError(action.str(), ex.what());
    }
    catch(...)
    {
        SchedulingAlarmSender::getInstance().schedulingMonitorError(action.str(), "unknown");
    }
}


void SchedulingMonitor::receiveJobDeletedMessage(const std::string& jobID)
{
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Job deleted message received. Job id: '%s'", jobID.c_str());

    //
    // Used if an alarm is raised - the action that is being performed
    //
    std::ostringstream action;
    action << "delete job with ID " << jobID;

    try
    {
        JobQueue::getInstance().removeJob(jobID);
    }
    // The monitoring agent should not actually go to the database,
    // but if a DataException, DatabaseException or any other kind is caught,
    // raise an alarm.
    catch(const DataException& ex)
    {
        SchedulingAlarmSender::getInstance().schedulingMonitorError(action.str(), ex.what());
    }
    catch(const DatabaseException& ex)
    {
        SchedulingAlarmSender::getInstance().schedulingMonitorError(action.str(), ex.what());
    }
    catch(const CORBA::Exception& ex)
    {
        SchedulingAlarmSender::getInstance().schedulingMonitorError(action.str(), CorbaUtil::getInstance().exceptionToString(ex));
    }
    catch(const std::exception& ex)
    {
        SchedulingAlarmSender::getInstance().schedulingMonitorError(action.str(), ex.what());
    }
    catch(...)
    {
        SchedulingAlarmSender::getInstance().schedulingMonitorError(action.str(), "unknown");
    }
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "%s successfully", action.str().c_str());
}


void SchedulingMonitor::receiveJobStartingMessage(const TA_Base_Bus::SchedulingAgentCorbaDef::JobInstanceCorbaDef& jobInstance)
{
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Job started message received. Job id: '%s', minute started: '%d'",
        jobInstance.jobIdentifier.in(), jobInstance.minuteStarted);
}


void SchedulingMonitor::receiveJobCompletedMessage(const TA_Base_Bus::SchedulingAgentCorbaDef::JobInstanceCorbaDef& jobInstance)
{
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Job completed message received. Job id: '%s', minute started: '%d'",
        jobInstance.jobIdentifier.in(), jobInstance.minuteStarted);
}


}; // TA_Base_App
