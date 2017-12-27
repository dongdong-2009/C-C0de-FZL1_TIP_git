/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/PlanRunnerJobInstance.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #9 $
  *
  * Last modification: $DateTime: 2013/08/26 10:54:54 $
  * Last modified by:  $Author: qi.huang $
  *
  * Implements the IJobInstance interface for PlanRunnerJobs.
  *
  */

// Disable warnings from boost
#if defined(_MSC_VER)
  #pragma warning(disable:4244) // conversion - possible loss of data
#endif // _MSC_VER

#include <sstream>

#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
// TD20368++
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
// ++TD20368
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/PlanAgentBroadcastComms_MessageTypes.h"
#include "core/threads/src/Thread.h"
#include "core/exceptions/src/DataException.h"
#include "bus/response_plans/plan_agent/IDL/src/IPlanAgentCorbaDef.h"
#include "app/scheduling/scheduling_agent/src/JobInstanceExecutionException.h"
#include "app/scheduling/scheduling_agent/src/JobInstanceNotFoundException.h"
#include "app/scheduling/scheduling_agent/src/PlanRunnerJobFactory.h"
#include "app/scheduling/scheduling_agent/src/SchedulingSessionManager.h"
#include "app/scheduling/scheduling_agent/src/SchedulingTimer.h"
#include "app/scheduling/scheduling_agent/src/PlanRunnerJobInstance.h"


namespace TA_Base_App
{


void PlanRunnerJobInstance::executeJob()
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
        "Executing job instance at minute %lu since the epoch that is a child of job with ID %s.", getMinuteStarted(), getJobId().c_str());

	std::auto_ptr<TA_Base_Core::IProfile> taSupperProfile(TA_Base_Core::ProfileAccessFactory::getInstance().getTransActiveSuperProfile());
	unsigned long profileId = taSupperProfile->getKey();

	std::string sessionID = SchedulingSessionManager::getInstance().getSessionKey(profileId, getLocationId());

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TD20368 PlanRunnerJobInstance execute, profileId=%lu, sessionId=%s", profileId, sessionID.c_str());
	// ++TD20368

	try
    {
        // TD #12032
        // Get location specific plan agent
        //TA_Base_Bus::PlanAgentLibrary* planAgent =  PlanRunnerJobFactory::getPlanAgent();
		TA_Base_Bus::PlanAgentLibrary* planAgent = PlanRunnerJobFactory::getPlanAgent(getLocationId());

		m_planComplete = false;

		// TD20368 Yanrong++
		//unsigned long planInstance = planAgent->runPlanByName(sessionID.c_str(), m_job->getPlanPath().c_str());
		unsigned long planInstance = planAgent->runPlanByNameWithoutActionPermittedCheck( sessionID.c_str(), m_job->getPlanPath().c_str() );
		// ++Yanrong
		
		//
		// Throws DataException.
		// Unfortunately the plan has already started at this point, even
		// if the details of the plan cannot be written to the database.
		// This means that a plan may be running that the database doesn't
		// know about. In the event of a failover the system will not know
		// about such an instance of a plan.
		// An alarm will be raised when an exception is thrown from this
		// method though.
		// This is unavoidable because the plan instance is returned from
		// the runPlan method.
		//
		saveMinuteStartedInstancePairToDB(getMinuteStarted(), planInstance);

		m_planInstance = planInstance;

		//
		// Sleep one second at a time until a message is received saying
		// the plan is complete.
		// Every couple of minutes, check the state through CORBA,
		// just in case the message wasn't received for some reason.
		//
		unsigned int secondsSlept = 0;
		while( ! m_planComplete )
		{
            TA_Base_Core::Thread::sleep(1000);

			if(secondsSlept >= 120)
			{
				m_planComplete = isPlanCompleteCorbaCall();

				secondsSlept = 0;
			}
			++secondsSlept;
		}

		// Job has completed so this is no longer required
		removeMinuteStartedInstancePairFromDB( getMinuteStarted() );
	}// try
	catch(const TA_Base_Core::DataException& ex)
	{
		std::string message  = ex.what();
		message += "Warning, job instance was started but no record was made in database!";

		throwJobInstanceExecutionException("A DataException", message);
	}
	catch(const TA_Base_Core::AccessDeniedException& ex)
	{
		throwJobInstanceExecutionException("Access failure", ex.reason.in());
	}
	catch(const TA_Base_Bus::IPlanAgentCorbaDef::PlanNotFoundError& ex)
	{		
		throwJobInstanceExecutionException("Plan not found", ex.what.in());
	}
	catch(const TA_Base_Bus::IPlanAgentCorbaDef::NoPermission& ex)
	{		
		throwJobInstanceExecutionException("Access denied", ex.what.in());
	}
	catch(const TA_Base_Bus::IPlanAgentCorbaDef::PlanAgentError& ex)
	{		
		throwJobInstanceExecutionException("Plan agent error", ex.what.in());
	}
	catch(const CORBA::Exception& ex)
	{
		throwJobInstanceExecutionException("Plan agent error", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex));
	}
	catch (const TA_Base_Core::ObjectResolutionException&)
	{
		throwJobInstanceExecutionException("Fail to contact Plan Agent", "");
	}
	catch(const std::exception& ex)
	{
		throwJobInstanceExecutionException("std::exception", ex.what());
	}
	catch(...)
	{
		throwJobInstanceExecutionException("Unknown error", "");
	}
}


bool PlanRunnerJobInstance::isPlanCompleteCorbaCall()
{
    try
    {
        std::string errorMessage = "";
        if( IJobInstance::Completed == getJobInstanceState(getMinuteStarted(), errorMessage) )
        {
            return true;
        }
    }
    //
    // If an exception is thrown log an error message
    // but hopefully we'll still receive a message from
    // the messaging subsystem at some stage anyway, so don't
    // propagate the exception.
    //
    catch(const std::exception& ex)
    {
        std::stringstream message;
        message <<
            "An exception has occurred while " <<
            "getting state of plan job with ID " <<
            getJobId().c_str() << " and description '" <<
            getDescription().c_str() <<
            "' that was started " << getMinuteStarted() << " " <<
            "minutes after the epoch. " <<
            "Continuing to wait for Plan Completed message. " <<
            "Exception: '" << ex.what() << "'";
        
        LOG_EXCEPTION_CATCH(SourceInfo, "Exception", message.str());
    }
    
    return false;
}


void PlanRunnerJobInstance::throwJobInstanceExecutionException(
    const std::string& exceptionName,
    const std::string& message)
{
    std::stringstream errorMessage;

    errorMessage << exceptionName << " has occurred while " <<
        "running plan job with ID " << getJobId().c_str() << " " <<
        "and description '" << getDescription().c_str() << "' " <<
        "that was started at " << SchedulingTimer::getFormatedTime( getMinuteStarted() ) ;

    if(!message.empty())
    {
        errorMessage << ": '" << message << "'.";
    }

    TA_THROW( JobInstanceExecutionException(errorMessage.str()));
}


void PlanRunnerJobInstance::receiveSpecialisedMessage(const CommsMessageCorbaDef& message)
{
	// Extract the messageState from the Comms Message
	TA_Base_Core::ActivePlanDetail* planUpdate = NULL;

	// Check this is a plan update
	if ( ( message.messageState >>= planUpdate ) == 0)
	{
        return;
    }

    // Check the plan update is for this plan instance. Don't need to test the plan instance
    // location as we are subscribed to messages from the single Plan Agent location where
    // the plan associated with this job is executing.
    std::string strPlanPath = m_job->getPlanPath();

	if(0 != strPlanPath.compare( planUpdate->path.in() ) || m_planInstance != planUpdate->plan.instance)
    {
        return;
    }

    switch(planUpdate->activePlanState)
    {
        // If the plan is completed
        case TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE:
        case TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE:
        case TA_Base_Core::ABORTED_ACTIVE_PLAN_STATE:
        case TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE:
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Plan completed message received for plan %s and instance ID %d that was started at %d minutes after the epoch",
                strPlanPath.c_str(), m_planInstance, getMinuteStarted() );

            // Mark the plan as complete.
            m_planComplete = true;
        }
    }
}



void PlanRunnerJobInstance::saveMinuteStartedInstancePairToDB(
    unsigned long minuteStarted,
    CORBA::ULong instance)
{
    //
    // Get the minute started and instance as strings,
    // and store them to the database with the minute started
    // as the key, and the instance as the value.
    //
    std::stringstream minuteStream;
    minuteStream << minuteStarted;

    std::stringstream instanceStream;
    instanceStream << instance;

    m_job->setParameter(minuteStream.str(), instanceStream.str());
}

void PlanRunnerJobInstance::removeMinuteStartedInstancePairFromDB(
    unsigned long minuteStarted )
{
    //
    // Get the minute started as a string
    // and use it as a key to the pair to be removed.
    //
    std::stringstream minuteStream;
    minuteStream << minuteStarted;

    m_job->deleteParameter( minuteStream.str() );
}


CORBA::ULong PlanRunnerJobInstance::getInstanceIDFromDB() const
{
    //
    // Get the instance from the database using the minute started
    // as the key.
    //
    unsigned long instanceID = 0;

    std::stringstream minuteStream;
    minuteStream << getMinuteStarted();
    std::string instanceString = m_job->getParameter(minuteStream.str());

    std::stringstream instanceStream;
    instanceStream << instanceString;
    
    //
    // If the instance cannot be cast to an unsigned long,
    // throw an exception.
    //
    if( ! (instanceStream >> instanceID) )
    {
        std::string message = "Could not convert instance ID '";
        message            += instanceString;
        message            += "' to an unsigned long. Invalid datatype.";
        
        TA_THROW( JobInstanceNotFoundException(message.c_str()) );
    }

    return instanceID;
}



PlanRunnerJobInstance::~PlanRunnerJobInstance()
{
    FUNCTION_ENTRY("~PlanRunnerJobInstance");

    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
}


	
PlanRunnerJobInstance::PlanRunnerJobInstance(
    const PlanRunnerJob* job,
    const unsigned long minuteStarted)
    :
    AbstractJobInstance(*job, minuteStarted),
    m_job(const_cast<PlanRunnerJob*>(job)),
    m_planInstance(0),
    m_planComplete(false)
{
    FUNCTION_ENTRY("PlanRunnerJobInstance");

    //
    // Register to receive plan completed messages. We are only interested in messages
    // sent by the Plan Agent used to spawn the plan job, so filter messages using the 
    // agent's location key.
    //
    //TA_Base_Bus::PlanAgentLibrary* planAgent = PlanRunnerJobFactory::getPlanAgent();
    TA_Base_Bus::PlanAgentLibrary* planAgent = PlanRunnerJobFactory::getPlanAgent(m_job->getLocationID());

    std::ostringstream agentLocation;
    agentLocation << planAgent->getPlanAgentLocationKey();
    TA_Base_Core::NameValuePair locationFilter("LocationKey", agentLocation.str());
    TA_Base_Core::FilterData filterData;
    filterData.push_back(&locationFilter);

    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
        TA_Base_Core::PlanAgentBroadcastComms::ActivePlanUpdate, this, &filterData);

    FUNCTION_EXIT;
}



}; // TA_Base_App

