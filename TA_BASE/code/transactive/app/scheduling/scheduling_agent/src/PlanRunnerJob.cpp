/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/PlanRunnerJob.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #7 $
  *
  * Last modification: $DateTime: 2013/06/03 11:49:57 $
  * Last modified by:  $Author: huangjian $
  *
  * This class implements the IJob interface by extending the AbstractJob class
  * to create a Job that can be used to run plans.
  *
  */


#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "app/scheduling/scheduling_agent/src/JobInstanceNotFoundException.h"
#include "app/scheduling/scheduling_agent/src/JobInstanceStateRetrievalException.h"
#include "app/scheduling/scheduling_agent/src/PlanRunnerJobInstance.h"
#include "app/scheduling/scheduling_agent/src/PlanRunnerJobFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "app/scheduling/scheduling_agent/src/PlanRunnerJob.h"


namespace TA_Base_App
{

const std::string PlanRunnerJob::JOB_TYPE = "Ô¤°¸";
const std::string PlanRunnerJob::PLAN_PATH_PARAMETER_NAME = "PlanPath";

IJobInstance* PlanRunnerJob::createJobInstance(unsigned long minuteStarted)
{
    FUNCTION_ENTRY("createJobInstance");

    return new PlanRunnerJobInstance(this, minuteStarted);
}

IJobInstance::JobInstanceState PlanRunnerJob::getJobInstanceState( unsigned long minuteStarted, std::string& errorMessage ) const
{
    FUNCTION_ENTRY("getJobInstanceState");

    unsigned long instanceID = 0;
    PlanRunnerJobInstance jobInstance(this, minuteStarted);

    //
    // Get the plan details from the Plan Agent
    //
    TA_Base_Core::ActivePlanDetails_var activePlanDetails;

    try
    {
        // throws DatabaseException, JobInstanceNotFoundException
        instanceID = jobInstance.getInstanceIDFromDB();

		// TD #12032
		// Get location specific plan agent
        //PlanAgentLibrary* planAgent = PlanRunnerJobFactory::getPlanAgent();
        TA_Base_Bus::PlanAgentLibrary* planAgent = PlanRunnerJobFactory::getPlanAgent(getLocationID());

        planAgent->getActivePlanDetailByName(getPlanPath().c_str(), activePlanDetails);

        std::stringstream message;
        message << "There are " << activePlanDetails->length() << " "
                << "Active plan details for plan " << getPlanPath() << " "
                << "(job ID " << getID() << ")";

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s", message.str().c_str());
    }
    catch(const TA_Base_Core::DatabaseException& ex)
    {
        TA_THROW(JobInstanceStateRetrievalException(ex.what()));
    }
    catch(const CORBA::Exception& ex)
    {
        TA_THROW(JobInstanceStateRetrievalException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex)));
    }
    catch(...)
    {
        TA_THROW(JobInstanceStateRetrievalException());
    }

    //
    // The Plan Agent returns all plans with a particular ID,
    // so we need to find the plan with the instance ID matching
    // the one we retrieved from the database.
    //
    for(unsigned int i = 0; i < activePlanDetails->length(); ++i)
    {
        const TA_Base_Core::ActivePlanDetail& activePlanDetail = activePlanDetails[i];

        if(activePlanDetail.plan.instance == instanceID)
        {
            //
            // Return the appropriate JobInstanceState
            //
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Active Plan State %d",activePlanDetail.activePlanState);
            switch(activePlanDetail.activePlanState)
            {
				// marvin++
				// TD13442
				//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Active Plan State %d",activePlanDetail.activePlanState);
				// ++marvin
				// TD13442

                case TA_Base_Core::PENDING_ACTIVE_PLAN_STATE:
                {
                    return IJobInstance::NotStarted;
                }

                case TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE:
                case TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE:
                case TA_Base_Core::OPERATOR_PAUSED_ACTIVE_PLAN_STATE:
				// marvin++
				// TD13442
				case TA_Base_Core::DELAYED_ACTIVE_PLAN_STATE:
				case TA_Base_Core::NEW_OWNER_ACTIVE_PLAN_STATE:
				// ++marvin
				// TD13442
                {
                    return IJobInstance::Started;
                }


                case TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE:
                {
                    errorMessage = "The plan was forcibly stopped.";
                    return IJobInstance::Completed;
                }
                case TA_Base_Core::ABORTED_ACTIVE_PLAN_STATE:
                {
                    errorMessage = "The plan was aborted.";
                    return IJobInstance::Completed;
                }
                case TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE:
                case TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE:
                {
                    return IJobInstance::Completed;
                }


                case TA_Base_Core::UNDEFINED_ACTIVE_PLAN_STATE:
                case TA_Base_Core::FAILED_ACTIVE_PLAN_STATE:
                {
                    //
                    // These states should never be returned by the plan
                    // agent.
                    //
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Plan in illegal state");
                }
                default:
                {
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Plan in unknown state");
                }
            }

        }// if(activePlanDetails[i].instance == instanceID)

    }// for(unsigned int i = 0; i < activePlanDetails.length(); i ++)


    //
    // The state should have been returned before this point.
    // It hasn't, so throw an exception.
    //
    std::stringstream errorMessageStream;
    errorMessageStream << "Plan " << getPlanPath() << " does not have an instance with ID " << instanceID;

    TA_THROW( JobInstanceNotFoundException(errorMessageStream.str()) );
}



TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef PlanRunnerJob::toCorbaObject()
{
    TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef job = AbstractJob::toCorbaObject();

    return job;
}


std::string PlanRunnerJob::getPlanPath() const
{
    return getParameter(PLAN_PATH_PARAMETER_NAME);
}


void PlanRunnerJob::updateJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job)
{
    AbstractJob::updateJob(job);
}



PlanRunnerJob::~PlanRunnerJob()
{
    FUNCTION_ENTRY("~PlanRunnerJob");
}



PlanRunnerJob::PlanRunnerJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job) : AbstractJob(job)
{
    FUNCTION_ENTRY("PlanRunnerJob");
}


PlanRunnerJob::PlanRunnerJob(const std::string& jobID) : AbstractJob(jobID)
{
}




}; // TA_Base_App

