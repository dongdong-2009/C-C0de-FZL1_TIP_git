/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  Implementation of PlanAccessFactory.
  *  TODO: put hard coded column names and SQL in a config file or in the data
  *  access classes outlined in the DataAccessInterface usage guide.
  */
#include "app/response_plans/common/src/ProjectDefinition.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/PlanAgentException.h"
#include "core/threads/src/Thread.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataReader.h"
#include "bus/response_plans/plan_agent/src/SortableActiveStepId.h"
#include "app/response_plans/plan_agent/src/ActivePlanMessageBlock.h"
#include "app/response_plans/plan_agent/src/ActivePlanStopMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanCompleteMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanPauseMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanPauseForOperatorMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanResumeMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanReassignMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanUnassignMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanCustomiseStepMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanDetailUpdateMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanDecisionStepMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanPromptStepMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanLogStepMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanRunPlanStepMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanLaunchGuiAppStepMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanLaunchSchematicStepMessage.h"
#include "app/response_plans/plan_agent/src/ActivePlanFailureSummaryStepMessage.h"
#include "app/response_plans/plan_agent/src/PlanNotFoundException.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/PlanExecutionEngine.h"
#include "app/response_plans/plan_agent/src/PlanExecutor.h"

namespace TA_Base_App
{
	PlanExecutorImpl::PlanExecutorImpl() :
	m_executionEngine(new PlanExecutionEngine())
	{
			if( -1== m_executionEngine->open( 0 ))
			{
				std::string message = "Active plan thread pool could not be created.";
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, message.c_str());
                TA_THROW( TA_Base_Core::PlanAgentException( message.c_str(), TA_Base_Core::PAE_INITIALISATION_FAILURE ) );
			}
	}

	PlanExecutorImpl::~PlanExecutorImpl()
	{
	}

	void PlanExecutorImpl::waitForRunningPlans( void )
	{
		if( NULL != m_executionEngine )
		{
            // jian.huang use terminatePlanExecutionEngine instead of ACE_Message_Block::MB_HANGUP
            // to notify idle threads
// 			TA_Base_Core::ActivePlanId plan;
// 			plan.plan = 0;
// 			plan.instance = 0;
// 
// 			ActivePlanMessageBlock* msg = new ActivePlanMessageBlock(plan, NULL);

// 			msg->msg_type( ACE_Message_Block::MB_HANGUP );
// 			if( -1 != m_executionEngine->put(msg) )
// 			{
// 				m_executionEngine->wait();
// 			}
            m_executionEngine->terminateAndWait();
		}
	}

    void PlanExecutorImpl::processDecisionStepResponse(const TA_Base_Core::ActivePlanId& plan, const int status)
    {
#if defined ( STEPTYPE_DECISION )
        ActivePlanDecisionStepMessage* decisionStepMessage = new ActivePlanDecisionStepMessage(status);
        sendMessage( plan, decisionStepMessage );
#endif //STEPTYPE_DECISION

    }

    void PlanExecutorImpl::processPromptStepResponse(const TA_Base_Core::ActivePlanId& plan, const int status)
    {
#if defined ( STEPTYPE_PROMPT )
        ActivePlanPromptStepMessage* promptStepMessage = new ActivePlanPromptStepMessage(status);
        sendMessage( plan, promptStepMessage );
#endif //STEPTYPE_PROMPT
    }

    void PlanExecutorImpl::processLogStepResponse(const TA_Base_Core::ActivePlanId& plan, const int status, const std::string& logMessage)
    {
#if defined ( STEPTYPE_LOG )
        ActivePlanLogStepMessage* logStepMessage = new ActivePlanLogStepMessage(status, logMessage);
        sendMessage( plan, logStepMessage );
#endif //STEPTYPE_LOG
    }

    void PlanExecutorImpl::processRunPlanStepResult(const TA_Base_Core::ActivePlanId& plan, const int status, const std::string& error)
    {
#if defined ( STEPTYPE_RUN_PLAN )
        ActivePlanRunPlanStepMessage* runPlanStepMessage = new ActivePlanRunPlanStepMessage(status, error);
        sendMessage( plan, runPlanStepMessage );
#endif //STEPTYPE_RUN_PLAN
    }

    void PlanExecutorImpl::processLaunchGuiAppStepResult(const TA_Base_Core::ActivePlanId& plan, const int status, const std::string& error)
    {
#if defined ( STEPTYPE_LAUNCH_GUI_APP )
        ActivePlanLaunchGuiAppStepMessage* launchGuiAppStepMessage = new ActivePlanLaunchGuiAppStepMessage(status, error);
        sendMessage( plan, launchGuiAppStepMessage );
#endif //STEPTYPE_LAUNCH_GUI_APP
    }

    void PlanExecutorImpl::processLaunchSchematicStepResult(const TA_Base_Core::ActivePlanId& plan, const int status, const std::string& error)
    {
#if defined ( STEPTYPE_LAUNCH_SCHEMATIC )
        ActivePlanLaunchSchematicStepMessage* launchSchematicStepMessage = new ActivePlanLaunchSchematicStepMessage(status, error);
        sendMessage( plan, launchSchematicStepMessage );
#endif //STEPTYPE_LAUNCH_SCHEMATIC
    }

	void PlanExecutorImpl::processFailureSummaryStepResult(const TA_Base_Core::ActivePlanId& plan, const int status, const std::string& error)
    {
#if defined ( STEPTYPE_FAILURE_SUMMARY_STEP )
		ActivePlanFailureSummaryStepMessage* failureSummaryStepMessage = new ActivePlanFailureSummaryStepMessage(status,error);        
        sendMessage( plan, failureSummaryStepMessage );
#endif //STEPTYPE_FAILURE_SUMMARY_STEP
    }

	void PlanExecutorImpl::run(const std::string& session, const std::string& pmId, TA_Base_Core::ActivePlanId& plan, const bool manExecution, const bool bStartFromMonitorMode )
	{
		TA_Base_Core::NodeDetail* nodeDetail = NULL;
		std::ostringstream message;

		try
		{
			nodeDetail = TA_Base_Bus::PlanDataReader::instance()->getNodeDetail( plan.plan );
		}
		catch ( ... )
		{
			message << "The Plan[" << plan.plan << ":" << plan.instance << "] not found.";
            TA_THROW(PlanNotFoundException(message.str(), TA_Base_Core::PNFE_INSTANCE_NOT_FOUND, plan.plan, plan.instance));
		}

		TA_Base_Core::ENode nodeType = nodeDetail->_d(); //Retrieve the discriminator

        if ( TA_Base_Core::PLAN_NODE != nodeType )
        {                
			message << "The Plan[" << plan.plan << ":" << plan.instance << "] is not of type PLAN_NODE.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
        }

        TA_Base_Core::PlanDetail* planDetail = &(nodeDetail->plan());

        if ( TA_Base_Core::APPROVED_APPROVAL_STATE != planDetail->approvalState )
        {
			message << "The Plan[" << plan.plan << ":" << plan.instance << "] is not approved.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_APPROVED));
        }

		m_executionEngine->run( session, pmId, plan, TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE , manExecution, bStartFromMonitorMode );
	}

	void PlanExecutorImpl::customise(const std::string& session, const std::string& pmId, TA_Base_Core::ActivePlanId& plan, const bool bStartFromMonitorMode )
	{
		std::ostringstream message;

		TA_Base_Core::NodeDetail* nodeDetail = NULL;
		try
		{
			nodeDetail = TA_Base_Bus::PlanDataReader::instance()->getNodeDetail( plan.plan );
		}
		catch (...)
		{
			message << "The Plan[" << plan.plan << ":" << plan.instance << "] not found.";
            TA_THROW( PlanNotFoundException( message.str(), TA_Base_Core::PNFE_INSTANCE_NOT_FOUND, plan.plan, plan.instance) );
		}

		TA_Base_Core::ENode nodeType = nodeDetail->_d(); //Retrieve the discriminator

        if ( TA_Base_Core::PLAN_NODE != nodeType )
        {                
			message << "The Plan[" << plan.plan << ":" << plan.instance << "] is not of type PLAN_NODE.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
        }

        TA_Base_Core::PlanDetail* planDetail = &(nodeDetail->plan());

        if ( TA_Base_Core::APPROVED_APPROVAL_STATE != planDetail->approvalState )
        {
			message << "The Plan[" << plan.plan << ":" << plan.instance << "] is not approved.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_APPROVED));
        }

        // manExecution always true here as customisation can only be done manually
		m_executionEngine->run( session, pmId, plan, TA_Base_Core::PENDING_ACTIVE_PLAN_STATE, true, bStartFromMonitorMode );
	}

	void PlanExecutorImpl::spawnPending(const std::string& session, const std::string& pmId, const TA_Base_Core::ActivePlanId& parentPlan, TA_Base_Core::ActivePlanId& childPlan, const bool bStartFromMonitorMode )
	{
		std::ostringstream message;

		TA_Base_Core::NodeDetail* nodeDetail = NULL;
		try
		{
			nodeDetail = TA_Base_Bus::PlanDataReader::instance()->getNodeDetail( childPlan.plan );
		}
		catch (...)
		{
			message << "The Plan[" << childPlan.plan << ":" << childPlan.instance << "] not found.";
            TA_THROW( PlanNotFoundException( message.str(), TA_Base_Core::PNFE_INSTANCE_NOT_FOUND, childPlan.plan, childPlan.instance) );
		}

		TA_Base_Core::ENode nodeType = nodeDetail->_d(); //Retrieve the discriminator

        if ( TA_Base_Core::PLAN_NODE != nodeType )
        {                
			message << "The Plan[" << childPlan.plan << ":" << childPlan.instance << "] is not of type PLAN_NODE.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_PLAN));
        }

        TA_Base_Core::PlanDetail* planDetail = &(nodeDetail->plan());

        if ( TA_Base_Core::APPROVED_APPROVAL_STATE != planDetail->approvalState )
        {
			message << "The Plan[" << childPlan.plan << ":" << childPlan.instance << "] is not approved.";
            TA_THROW(TA_Base_Core::PlanAgentException(message.str(), TA_Base_Core::PAE_NOT_APPROVED));
        }

		m_executionEngine->spawn( session, pmId, parentPlan, childPlan, TA_Base_Core::PENDING_ACTIVE_PLAN_STATE, bStartFromMonitorMode );
	}

    void PlanExecutorImpl::stop(const TA_Base_Core::ActivePlanId& plan, const std::string& reason)
	{
		ActivePlanStopMessage* stopMessage = new ActivePlanStopMessage( reason );
		sendMessage( plan, stopMessage );
	}

	void PlanExecutorImpl::complete(const TA_Base_Core::ActivePlanId& plan)
	{
		ActivePlanCompleteMessage* completeMessage = new ActivePlanCompleteMessage;
		sendMessage( plan, completeMessage );
	}

	void PlanExecutorImpl::pause(const TA_Base_Core::ActivePlanId& plan)
	{
		ActivePlanPauseMessage* pauseMessage = new ActivePlanPauseMessage;
		sendMessage( plan, pauseMessage );
	}

	void PlanExecutorImpl::pauseForOperator(const TA_Base_Core::ActivePlanId& plan)
	{
		ActivePlanPauseForOperatorMessage* pauseForOperatorMessage = new ActivePlanPauseForOperatorMessage;
		sendMessage( plan, pauseForOperatorMessage );
	}

	void PlanExecutorImpl::resume(const TA_Base_Core::ActivePlanId& plan)
	{
		ActivePlanResumeMessage* resumeMessage = new ActivePlanResumeMessage();
		sendMessage( plan, resumeMessage );
	}


	void PlanExecutorImpl::reassign(const std::string& session, const std::string& planManagerId, const TA_Base_Core::ActivePlanId& plan )
	{
		ActivePlanReassignMessage* reassignMessage = new ActivePlanReassignMessage(session, planManagerId);
		sendMessage( plan, reassignMessage );
	}

	void PlanExecutorImpl::unassign(const TA_Base_Core::ActivePlanId& plan)
	{
		ActivePlanUnassignMessage* unassignMessage = new ActivePlanUnassignMessage;
		sendMessage( plan, unassignMessage );
	}

	void PlanExecutorImpl::customiseStep(const TA_Base_Core::ActivePlanId& plan, const TA_Base_Core::StepCustomisableDetail& detail)
	{
		//TD13442++
		try
        {
            // Move to PlanAgentCorbaDefImpl::customiseStep, just check in Control mode
            // In the monitor mode, Active plan state may be not ready
// 			TA_Base_Core::EActivePlanState planState =  ActivePlanBulletinBoard::instance()->getState( plan );
// 			if ( planState != TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE && planState != TA_Base_Core::PENDING_ACTIVE_PLAN_STATE )
// 			{
// 				TA_THROW( TA_Base_Core::PlanAgentException ("An active plan can only be customised in PAUSED or PENDING state.", TA_Base_Core::PAE_WRONG_STATE_TO_CUSTOMISE_ACTIVE_PLAN) );
// 			}
			ActivePlanCustomiseStepMessage* customiseStepMessage = new ActivePlanCustomiseStepMessage( detail );
			sendMessage( plan, customiseStepMessage );
		}
		catch ( ... )
        {
            // Do nothing as plan state was not found.
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Plan state not found");
        }
		//++TD13442
	}

    void PlanExecutorImpl::updateActivePlanDetail( TA_Base_Core::ActivePlanDetail& detail )
    {
        // Bad logic if ActivePlanDetail update is received when the plan is not start running.
//         if ( !ActivePlanBulletinBoard::instance()->existsPlan( detail.plan ) )
//         {
// 			if ( TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE == detail.activePlanState )
// 			{
//                 run( detail.owner.in(), "", detail.plan );
//                 return;
//             }
//         }

        if ( 0 == detail.currentStepDetail.position )
        {
            // indicate start running plan. do nothing
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Plan [ %d, %d ] start to run", detail.plan.plan, detail.plan.instance );
            return;
        }

        const unsigned char maxSleepNumberOfTime = 20;
		const unsigned char sleepTimeEachTime = 50;
		unsigned char indexSleep = 0;
        while ( !ActivePlanBulletinBoard::instance()->existsPlan( detail.plan ) )
		{
			TA_Base_Core::Thread::sleep( sleepTimeEachTime );
			indexSleep++;
			if (indexSleep > maxSleepNumberOfTime)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Not found the Plan [%d, %d] in [%d] millisecond, break the loop", 
					detail.plan.plan, detail.plan.instance, maxSleepNumberOfTime * sleepTimeEachTime);
				break;
			}
		}

		//TD13442++
		try
        {
// 			TA_Base_Core::EActivePlanState state = ActivePlanBulletinBoard::instance()->getState( detail.plan );
// 
// 			if (( TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE == state ) ||
// 				 ( TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE == state ) ||
// 				 ( TA_Base_Core::ABORTED_ACTIVE_PLAN_STATE == state ))
// 			{
// 				return;
// 			}

            if ( ActivePlanBulletinBoard::instance()->isActive( detail.plan ))
            {
                ActivePlanDetailUpdateMessage* planDetailMessage = new ActivePlanDetailUpdateMessage( detail );
			    sendMessage( detail.plan, planDetailMessage );
            }
            else
            {
                if ( TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE == detail.activePlanState )
                {
                    ActivePlanBulletinBoard::instance()->cullStoppedPlan( detail.plan );
                }
                else
                {
                    // Update the step state to complete or un-executed but not loaded
                    ActivePlanBulletinBoard::instance()->updateActivePlanDetail( detail );
                }
            }
		}
		catch(TA_Base_Core::TransactiveException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
		}
		catch ( ... )
        {
            // Do nothing as plan state was not found.
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Plan state not found");
        }
		//++TD13442
    }

	void PlanExecutorImpl::sendMessage( const TA_Base_Core::ActivePlanId& plan, IActivePlanMessage* controlMessage )
	{
		std::auto_ptr<ActivePlanMessageBlock> message( new ActivePlanMessageBlock( plan, controlMessage ) );

		m_executionEngine->put( message.release() );
	}

    void PlanExecutorImpl::processCustomisationHistoryUpdate( const TA_Base_Core::CustomisationHistoryUpdate& historyUpdate )
    {
        ActivePlanCustomiseStepMessage* customiseStepMessage = new ActivePlanCustomiseStepMessage( historyUpdate );
        sendMessage( historyUpdate.plan, customiseStepMessage );
    }

    void PlanExecutorImpl::clearAllPlans()
    {
        m_executionEngine->clearAllMessageQueue();
    }

    void PlanExecutorImpl::signalAllPlansIntoControl()
    {
        m_executionEngine->signalAllPlansIntoControl();
    }

    void PlanExecutorImpl::processBackgroundLogStepResponse( const TA_Base_Core::CurrentPlanStepDetail& step, const int status, const std::string& logMessage )
    {
#if defined ( STEPTYPE_BACKGROUND_LOG )

        if (TA_Base_Core::PSCS_SUCCESS == status)
        {
            TA_Base_Bus::SortableActiveStepId srtStepId(step);
            ActivePlanBulletinBoard::instance()->setStepCompletionStatus(srtStepId, status);
            return;
        }

        ActivePlanLogStepMessage* logStepMessage = new ActivePlanLogStepMessage(status, logMessage);
        sendMessage( step.activePlan, logStepMessage );

        TA_Base_Bus::SortableActiveStepId srtStepId(step);
        ActivePlanBulletinBoard::instance()->removeStepCompletionStatus( srtStepId );

#endif //STEPTYPE_BACKGROUND_LOG
    }

    void PlanExecutorImpl::processBackgroundDecisionStepResponse( const TA_Base_Core::CurrentPlanStepDetail& step, const int status )
    {
#if defined ( STEPTYPE_BACKGROUND_DECISION )

        if (TA_Base_Core::PSCS_SUCCESS == status)
        {
            TA_Base_Bus::SortableActiveStepId srtStepId(step);
            ActivePlanBulletinBoard::instance()->setStepCompletionStatus(srtStepId, status);
            return;
        }

        ActivePlanDecisionStepMessage* decisionStepMessage = new ActivePlanDecisionStepMessage(status);
        sendMessage( step.activePlan, decisionStepMessage );

        TA_Base_Bus::SortableActiveStepId srtStepId(step);
        ActivePlanBulletinBoard::instance()->removeStepCompletionStatus( srtStepId );

#endif //STEPTYPE_BACKGROUND_DECISION
    }

    void PlanExecutorImpl::processBackgroundPromptStepResponse( const TA_Base_Core::CurrentPlanStepDetail& step, const int status )
    {
#if defined ( STEPTYPE_BACKGROUND_PROMPT )

        if (TA_Base_Core::PSCS_SUCCESS == status)
        {
            TA_Base_Bus::SortableActiveStepId srtStepId(step);
            ActivePlanBulletinBoard::instance()->setStepCompletionStatus(srtStepId, status);
            return;
        }

        ActivePlanPromptStepMessage* promptStepMessage = new ActivePlanPromptStepMessage(status);
        sendMessage( step.activePlan, promptStepMessage );

        TA_Base_Bus::SortableActiveStepId srtStepId(step);
        ActivePlanBulletinBoard::instance()->removeStepCompletionStatus( srtStepId );

#endif //STEPTYPE_BACKGROUND_PROMPT
    }

    void PlanExecutorImpl::processBackgroundLaunchGuiAppStepResponse( const TA_Base_Core::CurrentPlanStepDetail& step, const int status )
    {
#if defined ( STEPTYPE_BACKGROUND_LAUNCH_GUI_APP )

        if (TA_Base_Core::PSCS_SUCCESS == status)
        {
            TA_Base_Bus::SortableActiveStepId srtStepId(step);
            ActivePlanBulletinBoard::instance()->setStepCompletionStatus(srtStepId, status);
            return;
        }

#endif //STEPTYPE_BACKGROUND_LAUNCH_GUI_APP
    }

    void PlanExecutorImpl::processBackgroundLaunchSchematicStepResponse( const TA_Base_Core::CurrentPlanStepDetail& step, const int status )
    {
#if defined ( STEPTYPE_BACKGROUND_LAUNCH_SCHEMATIC )

        if (TA_Base_Core::PSCS_SUCCESS == status)
        {
            TA_Base_Bus::SortableActiveStepId srtStepId(step);
            ActivePlanBulletinBoard::instance()->setStepCompletionStatus(srtStepId, status);
            return;
        }

#endif //STEPTYPE_BACKGROUND_LAUNCH_SCHEMATIC
    }

} // TA_Base_App
