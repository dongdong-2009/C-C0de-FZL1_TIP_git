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
  *  Implementation of .
  */

#include "ace/OS.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_agent/src/GlobalConstant.h"
#include "app/response_plans/plan_agent/src/PlanNotFoundException.h"
#include "app/response_plans/plan_agent/src/PlanAccessFactory.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/AAMessageSender.h"
#include "app/response_plans/plan_agent/src/ActivePlanExecutingState.h"
#include "app/response_plans/plan_agent/src/ActivePlanPendingState.h"
#include "app/response_plans/plan_agent/src/ActivePlanPausedState.h"
#include "app/response_plans/plan_agent/src/ActivePlanOperatorPausedState.h"
#include "app/response_plans/plan_agent/src/ActivePlanDelayedState.h"
#include "app/response_plans/plan_agent/src/ActiveStep.h"
#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/common/src/StringConstants.h"

// namespace alias for Corba structures/types
 
namespace TA_Base_App
{
	ActivePlan::ActivePlan( const TA_Base_Core::ActivePlanId& plan, const int initialState, const bool bStateSyncDone ) :
    // Initialize all member variables correctly
    m_plan(plan),
    m_state( NULL ),
	m_isActive( true ),
    m_remark( "" ),
    m_steps(),
    m_stepIterator(),
    m_stepsIndexMap(),
	m_terminatedInterlockIsSet( false ),
	m_completedInterlockIsSet( false ),
    m_operatorResponseInterlockIsSet( false ),
    m_hasPlanCompleted( false ),
    m_stepFailed( false ),
    m_failedStepAlarmId( "" ),
	m_terminatedInterlock( 0u ),
    m_completedInterlock(),
    m_lastExecutedStep( 0u ),
    m_newSessionId( "" ),
    m_newPmId( "" ),
    m_isTakeControlPending ( false ),
    m_isPausePending ( false ),
    m_isCurrentInteractiveStep( false ),
    m_hasInteractiveStepFailed( false ),
    m_bStateSyncDone( bStateSyncDone )
	{
		m_completedInterlock.plan = 0;
		m_completedInterlock.instance = 0;

		switch( initialState )
		{
				case TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE:
				m_state = ActivePlanExecutingState::instance();
				break;

				case TA_Base_Core::PENDING_ACTIVE_PLAN_STATE:
				m_state = ActivePlanPendingState::instance();
				break;

			default:
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "%s: Invalid initial state, defaulting to \"Executing\"", dumpContext().c_str() );
				m_state = ActivePlanExecutingState::instance();
				break;
		}

        std::string reason = "";
        m_steps.clear();
        PlanAccessFactory::instance()->getActiveStepList( m_plan.plan, *this, reason, m_steps );

        if ( !reason.empty() )
        {
            TA_THROW( PlanNotFoundException(reason, TA_Base_Core::PNFE_FAIL_FETCH_ACTIVE_LIST_STEP, plan.plan, plan.instance));
        }

		if( m_steps.empty() )
		{
            std::ostringstream message;
			message << "Active Plan: Failed  to get the active step list for plan[" << plan.plan << ":" << plan.instance << "]";
            TA_THROW(PlanNotFoundException(message.str(), TA_Base_Core::PNFE_FAIL_FETCH_ACTIVE_LIST_STEP, plan.plan, plan.instance));
		}
		else
		{
			m_stepIterator = m_steps.begin();
		}

		for(ActiveStepListIterator it = m_stepIterator; it != m_steps.end(); ++it)
        {
            if (NULL != (*it))
            {
                m_stepsIndexMap[(*it)->getStepId()] = *it;
            }
        }
	}

	ActivePlan::~ActivePlan()
	{
		// It would have been nice to be able to use a "list of auto_ptr" but
		// the g++ implementation of the STL didn't allow it. So instead we
		// have to delete all the individual steps. As the list itself is an
		// auto_ptr we don't have to delete that here.
		for( ActiveStepListIterator it = m_steps.begin(); it != m_steps.end(); ++it )
		{
			delete (*it);
            *it = NULL;
		}

        m_steps.clear();
	}

	int ActivePlan::executeNextStep( void )
	{
        //The next step of a Plan is executed only if the PlanAgent is in Control mode
        // and not in Monitor mode. And Plan state synchronization from Control mode is done.

        if ( !m_bStateSyncDone || !ActivePlanBulletinBoard::instance()->isControlMode() )
        {
            return NORMAL_NSEM;
        }

        return m_state->executeNextStep( *this );
	}

    void ActivePlan::stop( const std::string& reason )
	{
		FUNCTION_ENTRY( "ActivePlan::stop" );

		m_state->stop( *this, reason );

        // TD8788: The alarm ID is set when the plan pauses due to step failure.
        // Where the plan is subsequently stopped, the alarm should be cleared.
        if (!m_failedStepAlarmId.empty())
        {
            AAMessageSender::instance()->closeAlarm(m_failedStepAlarmId);
            m_failedStepAlarmId = "";
        }

        std::string session = ActivePlanBulletinBoard::instance()->getSession(m_plan);
        const std::string strPlanName = ActivePlanBulletinBoard::instance()->getActivePlanDetail( m_plan ).path.in();
        const unsigned long ulInstanceNum = m_plan.instance;
		std::string pmid = ActivePlanBulletinBoard::instance()->getPlanManagerId( m_plan );//CL-21338 PlanAgent wrong session and high CPU usage in monitor mode 
		AAMessageSender::instance()->submitAuditPlanStop( session, strPlanName, ulInstanceNum );

        //Send State Update message for the stop plan
		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData();

        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method

		TA_Base_Core::PlanStateUpdate_var psu = new TA_Base_Core::PlanStateUpdate;
		psu->session = session.c_str();
		psu->pmId = pmid.c_str();
		psu->plan = m_plan;
        psu->remark = m_remark.c_str();
		pasUpdateData->update.stopPlan(psu);
        ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);

        FUNCTION_EXIT
	}

    void ActivePlan::complete( void )
    {
        m_state->complete ( *this );
    }

	void ActivePlan::pause( void )
	{
        FUNCTION_ENTRY( "ActivePlan::pause" );

        transferControl();  //This transfers control if pending

		m_state->pause( *this ); 

        if (!m_isCurrentInteractiveStep)
        {
            m_isPausePending = false;

			if (m_remark.empty ())
			{
                m_remark = RemarkPlanPausedByOperator;
			}

   		   ActivePlanBulletinBoard::instance()->updateStepState( m_plan, getStepNumber(), TA_Base_Core::PAUSED_ACTIVE_STEP_STATE, m_remark );
        }

        const std::string session = ActivePlanBulletinBoard::instance()->getSession(m_plan);
        const std::string strPlanName = ActivePlanBulletinBoard::instance()->getActivePlanDetail( m_plan ).path.in();
        const unsigned long ulInstanceNum = m_plan.instance;
		const std::string pmid = ActivePlanBulletinBoard::instance()->getPlanManagerId( m_plan );
        //send this message for DSS plans only
		if (!ActivePlanBulletinBoard::instance()->isGRCPlan( m_plan ))
		{
			AAMessageSender::instance()->submitAuditPlanPause( session, strPlanName, ulInstanceNum );
		}
        
        //Send State Update message for the stop plan
		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;

        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method

		TA_Base_Core::PlanStateUpdate_var psu = new TA_Base_Core::PlanStateUpdate;
		psu->session = session.c_str();
		psu->pmId = pmid.c_str();
		psu->plan = m_plan;
		pasUpdateData->update.pausePlan(psu);
        ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);

        FUNCTION_EXIT
	}

	void ActivePlan::pausePlan ( const std::string& remark, const std::string& failedStepAlarmId )
	{
        m_remark = remark;
        m_failedStepAlarmId = failedStepAlarmId;

        pause();
    }

	void ActivePlan::pauseForOperator( void )
	{
		FUNCTION_ENTRY( "ActivePlan::pauseForOperator" );
        m_state->pauseForOperator( *this );
        FUNCTION_EXIT
	}

	void ActivePlan::resume()
    {
        FUNCTION_ENTRY( "ActivePlan::resume" );

		if ( m_hasInteractiveStepFailed )
		{
			m_hasInteractiveStepFailed = false;
			m_isPausePending = false;
			setOperatorResponseInterlock(false);
		}
		else if ( m_isPausePending )
		{
			m_isPausePending = false;
			setOperatorResponseInterlock(false);

			if ( m_remark.empty() )
            {
				m_remark = RemarkPlanPausedByOperator;
            }
			
			ActivePlanBulletinBoard::instance()->updateStepState( m_plan, getStepNumber(), TA_Base_Core::PAUSED_ACTIVE_STEP_STATE, m_remark );				

            FUNCTION_EXIT
			return;
		}

		//m_operatorResponseInterlock = false;            
		m_state->resume( *this );

		// TD8788: The alarm ID is set when the plan pauses due to step failure.
		// Where the plan is subsequently resumed, the alarm should be cleared.
		if (!m_failedStepAlarmId.empty())
		{
			AAMessageSender::instance()->closeAlarm(m_failedStepAlarmId);
			m_failedStepAlarmId = "";
		}

		std::string session = ActivePlanBulletinBoard::instance()->getSession(m_plan);
		std::string pmid = ActivePlanBulletinBoard::instance()->getPlanManagerId(m_plan);

		//Send State Update message for the stop plan
		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;

		pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method
		
		TA_Base_Core::PlanStateUpdate_var psu = new TA_Base_Core::PlanStateUpdate;
		psu->session = session.c_str();
		psu->pmId = pmid.c_str();
		psu->plan = m_plan;
		pasUpdateData->update.resumePlan(psu);
		ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);

        FUNCTION_EXIT
	}

	void ActivePlan::reassign( const std::string& sessionId, const std::string& pmId )
	{
		FUNCTION_ENTRY( "ActivePlan::reassign" );
		m_state->reassign( *this, sessionId, pmId );
        FUNCTION_EXIT
	}

	void ActivePlan::unassign( void )
	{
		FUNCTION_ENTRY( "ActivePlan::unassign" );
        m_state->unassign( *this );
        FUNCTION_EXIT
	}

	void ActivePlan::customiseStep( const TA_Base_Core::StepCustomisableDetail& newDetail )
	{
		FUNCTION_ENTRY( "ActivePlan::customiseStep" );
        m_state->customiseStep( *this, newDetail );
        FUNCTION_EXIT
    }

	std::string ActivePlan::getRemark( void )
	{
		return m_remark;
	}

    void ActivePlan::scheduleTakeControl( const std::string session, const std::string& pmId )
    {
        m_isTakeControlPending = true;
        m_newSessionId = session;
        m_newPmId = pmId;
    }

    void ActivePlan::schedulePause()
    {
        m_isPausePending = true;
    }

    void ActivePlan::transferControl(void)
    {
        if ( !m_isTakeControlPending )
        {
            return;
        }

        reassignImpl( m_newSessionId, m_newPmId );
        m_isTakeControlPending = false;
    }

    void ActivePlan::setCurrentStepPosition( const unsigned long stepNumber )
    {
        //Set the current step position
		if( (m_steps.end() == m_stepIterator) || (stepNumber < (*m_stepIterator)->getStepNumber()) )
        {
            // If stepNum = 0, stepNumber - 1 will be -1, list iterator out will out of boundary
            if ( 0u < stepNumber )
            {
			    m_stepIterator = m_steps.begin();
			    std::advance( m_stepIterator, stepNumber - 1u );
            }
            else
            {
                // stepNumber is 0, keep m_stepIterator = m_steps.end()
            }
		}
		else
		{
			std::advance( m_stepIterator, (stepNumber - (*m_stepIterator)->getStepNumber()) );
		}

		if( m_steps.end() != m_stepIterator )
		{
            if ( stepNumber != (*m_stepIterator)->getStepNumber() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to set new step number." );
                return;
            }
		}

		if (m_steps.size() < stepNumber)
		{
			m_hasPlanCompleted = true;
		}
        else if ( m_isPausePending )
        {
            pause();
        }
    }

	bool ActivePlan::setNextStep( const unsigned long step )
	{
        ActiveStepIndexMapIt stepIt = m_stepsIndexMap.find(step);

        if ( ( m_stepsIndexMap.end() != stepIt)  && ( NULL != stepIt->second ) )
        {
            TA_Base_Core::StepNumber stepNumber = stepIt->second->getStepNumber();
            setCurrentStepPosition(stepNumber);
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "step [%lu] can not be found.", step);
            return false;
        }

        return true;
	}

	void ActivePlan::publishNextStepDetail( void ) const
	{
		if( m_steps.end() != m_stepIterator )
		{
			(*m_stepIterator)->load();
		}
	}

	void ActivePlan::setNextStep( void )
	{
		m_stepIterator++;

        // This is a problem. current step is changed but Monitor Agent doesn't know this
        // when paused, Monitor Agent will be paused at the old step. 
        // although the other following update message can update the current step in Monitor Agent,
        // The execution history can't be updated and remain pause state for the old step.
        // Consider to send step position update message to Monitor Agent
		if( m_steps.end() != m_stepIterator )
		{
            if ( m_isPausePending )
            {
                pause();
            }

            return;
        }

		m_hasPlanCompleted = true;
		setIsActive( false );

        if ( anyPlanStepFailed() )
        {
			m_remark = RemarkPlanCompletedWithFailedSteps;
        }
        else
        {
			m_remark = RemarkPlanCompletedSuccess;
        }

		ActivePlanBulletinBoard::instance()->updatePlanState( m_plan, TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE, m_remark );

        std::string session = ActivePlanBulletinBoard::instance()->getSession( m_plan );
        const std::string strPlanName = ActivePlanBulletinBoard::instance()->getActivePlanDetail( m_plan ).path.in();
        const unsigned long ulInstanceNum = m_plan.instance;
        AAMessageSender::instance()->submitAuditPlanCompleted( session, strPlanName, ulInstanceNum );

		std::ostringstream auditMessage;
		auditMessage << strPlanName << "[Id:" << m_plan.plan << ",Instance:" << m_plan.instance << "]. has completed.";

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", auditMessage.str().c_str());
	}

	void ActivePlan::setTerminatedInterlock( const unsigned long interlock )
	{
		m_terminatedInterlock = interlock;
		m_terminatedInterlockIsSet = true;
	}

	void ActivePlan::setCompletedInterlock( const TA_Base_Core::ActivePlanId& interlock )
	{
		m_completedInterlock = interlock;
		m_completedInterlockIsSet = true;
	}

	void ActivePlan::setOperatorResponseInterlock( bool interlock )
	{
		m_operatorResponseInterlockIsSet = interlock;
	}

    ActiveStep* ActivePlan::getCurrentStep()
    {
        if (m_steps.end() == m_stepIterator)
        {
            return NULL;
        }

        return *m_stepIterator;            
    }

	TA_Base_Core::StepNumber ActivePlan::getStepNumber()
	{
		return (*m_stepIterator)->getStepNumber();
	}

	bool ActivePlan::interlockOk( void )
	{
		bool rval = true;
		int result = UNDEFINED_WR;

        static const ACE_Time_Value resultTimeout( 0, 10000 );

		if( m_terminatedInterlockIsSet )
		{
			ACE_Time_Value absTime( ACE_OS::gettimeofday() + resultTimeout );

			result = ActivePlanBulletinBoard::instance()->waitForTermination( m_terminatedInterlock, absTime );

			if( SUCCESS_WR == result )
			{
				m_terminatedInterlockIsSet = false;
			}
		}

		if( m_completedInterlockIsSet )
		{
			ACE_Time_Value absTime( ACE_OS::gettimeofday() + resultTimeout );

			result = ActivePlanBulletinBoard::instance()->waitForCompletion( m_completedInterlock, absTime );

			if( SUCCESS_WR == result )
			{
				m_completedInterlockIsSet = false;
			}
		}
		
        if (m_operatorResponseInterlockIsSet)
        {
			ACE_Time_Value absTime( ACE_OS::gettimeofday() + resultTimeout );

			result = ActivePlanBulletinBoard::instance()->waitForOperatorResponse( m_plan, absTime );

			if( SUCCESS_WR == result )
			{
				m_operatorResponseInterlockIsSet = false;
			}
        }


		// Note that we have to set the status for the interlocked steps here.
		switch( result )
		{
			case TIMEOUT_WR:
			{
				rval = false;
			}
			break;

			case SUCCESS_WR:
			{
				m_remark = RemarkStepExecutionSucceed;
				ActivePlanBulletinBoard::instance()->updateStepState( m_plan, m_lastExecutedStep, TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE, m_remark );
				rval = true;
			}
			break;

			case FAILURE_WR:
			{
				m_remark = RemarkStepExecutionFailed;
				ActivePlanBulletinBoard::instance()->updateStepState( m_plan, m_lastExecutedStep, TA_Base_Core::FAILED_ACTIVE_STEP_STATE, m_remark );
				ActivePlanBulletinBoard::instance()->updatePlanState( m_plan, TA_Base_Core::FAILED_ACTIVE_PLAN_STATE, m_remark );
				setIsActive( false );
				rval = false;
			}
			break;

			default:
			// There was no interlocking.
			break;
		}

		return rval;
    }

    // Synchronize customise steps date from Control Agent to Monitor Agent
    void ActivePlan::customiseSteps( const TA_Base_Core::StepCustomisableDetails& newDetails )
    {
        for ( unsigned long nLoop = 0u; nLoop < newDetails.length(); ++nLoop )
        {
            const TA_Base_Core::StepCustomisableDetail& refDetail = newDetails[nLoop];

            if (( refDetail.position > m_steps.size() ) || ( refDetail.position <= 0 ))
            {
                continue;
            }

            ActiveStepListIterator itSeek = m_steps.begin();
            std::advance( itSeek, refDetail.position - 1 );
            
		    int stepNumber = (*itSeek)->getStepNumber();

            if( m_steps.end() != itSeek )
            {
                (*itSeek)->customise( refDetail );
            }
        }
    }

	void ActivePlan::customiseStepImpl( const TA_Base_Core::StepCustomisableDetail& newDetail )
	{
		ActiveStepListIterator it = m_steps.begin();
		std::advance( it, newDetail.position - 1 );

		int stepNumber = (*it)->getStepNumber();

		if( m_steps.end() != it )
		{
			(*it)->customise( newDetail );

			ActivePlanBulletinBoard::instance()->updateCustom(m_plan, newDetail );	

            std::string session = ActivePlanBulletinBoard::instance()->getSession( m_plan );
            const std::string strPlanName = ActivePlanBulletinBoard::instance()->getActivePlanDetail( m_plan ).path.in();
            const unsigned long ulInstanceNum = m_plan.instance;
            const unsigned long ulStepNum = getStepNumber();
		    AAMessageSender::instance()->submitAuditStepCustomize( session, strPlanName, ulInstanceNum, ulStepNum );

            //Send State Update message for this customisation
		    TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;

            pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method

		    TA_Base_Core::PlanCustomiseStepUpdate_var customiseStepUpdate = new TA_Base_Core::PlanCustomiseStepUpdate;
		    customiseStepUpdate->session = session.c_str();
		    customiseStepUpdate->plan = m_plan;
            customiseStepUpdate->detail = newDetail;
		    pasUpdateData->update.customisedStep(customiseStepUpdate);
            ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);
		}
	}

	void ActivePlan::reassignImpl( const std::string& sessionId, const std::string& pmId )
	{
		ActivePlanBulletinBoard::instance()->updateOwner( m_plan, sessionId, pmId );

        std::string session = ActivePlanBulletinBoard::instance()->getSession(m_plan);
        const std::string strPlanName = ActivePlanBulletinBoard::instance()->getActivePlanDetail( m_plan ).path.in();
        std::string operatorName = ActivePlanBulletinBoard::instance()->getOperatorName( m_plan );
        const unsigned long ulInstanceNum = m_plan.instance;

		AAMessageSender::instance()->submitAuditControlAcquire( session, strPlanName, operatorName, ulInstanceNum );

        //Send State Update message for Taking control
		TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData;

        pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method

		TA_Base_Core::PlanStateUpdate_var psu = new TA_Base_Core::PlanStateUpdate;
		psu->session = sessionId.c_str();
        psu->pmId = pmId.c_str();
		psu->plan = m_plan;
		pasUpdateData->update.takeControl(psu);
        ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);
	}

    int ActivePlan::waitCurStepImpl()
    {
        ENextStepExecuteMethod rval = UNDEFINED_NSEM;

        if ( !getIsActive() )
        {
            return rval;
        }

        if( m_steps.end() == m_stepIterator )
        {
            rval = NORMAL_NSEM;
        }
        else
        {
            rval = INTERLOCKED_NSEM;

            // Record the execution of this step.
            m_lastExecutedStep = (*m_stepIterator)->getStepNumber();;

            (*m_stepIterator)->wait();
        }

        return rval;
    }

    int ActivePlan::executeNextStepImpl( void )
	{
		ENextStepExecuteMethod rval = UNDEFINED_NSEM;

		if ( !getIsActive() )
        {
            return rval;
        }

		if( m_steps.end() == m_stepIterator )
		{
			rval = NORMAL_NSEM;
		}
		else
		{
			// Perform any interlocking.
			if( interlockOk() )
			{
                transferControl();  //This transfers control if pending
				// Record the execution of this step.
				m_lastExecutedStep = (*m_stepIterator)->getStepNumber();

				(*m_stepIterator)->execute();

                //The next step to be set up will be set up by the step itself and
                //not set here in the ActivePlan. //MP 30 July 2003

				// Get ready to execute the next step.
				//setNextStep(); 

				if( m_terminatedInterlockIsSet || m_completedInterlockIsSet )
				{
					rval = INTERLOCKED_NSEM;
				}
				else
				{
					rval = NORMAL_NSEM;
				}
			}
			else
			{
				//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s: Not executing next step due to interlocking.", dumpContext().c_str() );
				rval = INTERLOCKED_NSEM;
			}

			if ( m_hasPlanCompleted )
			{
				setIsActive( false );

                m_remark = anyPlanStepFailed() ? RemarkPlanCompletedWithFailedSteps : RemarkPlanCompletedSuccess;

				ActivePlanBulletinBoard::instance()->updatePlanState( m_plan, TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE, m_remark );

                std::string session = ActivePlanBulletinBoard::instance()->getSession( m_plan );
                const std::string strPlanName = ActivePlanBulletinBoard::instance()->getActivePlanDetail( m_plan ).path.in();
                const unsigned long ulInstanceNum = m_plan.instance;
                AAMessageSender::instance()->submitAuditPlanCompleted( session, strPlanName, ulInstanceNum );
			}
		}

		return rval;
	}

	void ActivePlan::changeState(ActivePlanState* state)
	{
		m_state = state;
	}

	std::string ActivePlan::dumpContext( void )
	{
		std::ostringstream context;

		context << "[P:" << m_plan.plan << ",I:" << m_plan.instance << "]";

		return context.str();
	}

    void ActivePlan::setStateSyncDone( bool bSyncDone )
    {
        m_bStateSyncDone = bSyncDone;
    }

    const TA_Base_Core::ActivePlanId& ActivePlan::getPlan() const
    {
        return m_plan;
    }

    bool ActivePlan::isPausePending()
    {
        return m_isPausePending;
    }

    void ActivePlan::setInteractiveStep( bool isInteractiveStep )
    {
        m_isCurrentInteractiveStep = isInteractiveStep;
    }

    void ActivePlan::setInteractiveStepFailed()
    {
        m_hasInteractiveStepFailed = true;
    }
} // TA_Base_App
