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

#include "app/response_plans/common/src/ProjectDefinition.h"

#if defined ( STEPTYPE_RUN_PLAN )
// namespace alias for Corba structures/types

#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/exceptions/src/PlanAgentException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "bus/response_plans/plan_manager/IDL/src/IPlanManagerCorbaDef.h"
#include "app/response_plans/plan_agent/src/PlanNotFoundException.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/PlanExecutor.h"
#include "app/response_plans/plan_agent/src/ActiveRunPlanStep.h"
#include "app/response_plans/common/src/StringConstants.h"
 
namespace TA_Base_App
{

	ActiveRunPlanStep::ActiveRunPlanStep( const TA_Base_Core::StepDetail& stepDetail, 
        const TA_Base_Core::RunPlanParameters& params, IActivePlanFlow& flowControl ) : 
    ActiveStep(stepDetail, flowControl, TA_Base_Bus::IPlanManagerCorbaDef::RUNPLAN_RESPONSE_TIMEOUT ),
    m_params(params)
	{
        m_childPlan.plan = m_params.plan;
        m_childPlan.instance = 0u;
        m_childPlan.triggerEntity = 0u;
        m_childPlan.entitykey = -1;
	}

	TA_Base_Core::EActiveStepState ActiveRunPlanStep::executeImpl( void )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s: running plan id %lu", dumpContext().c_str(), m_params.plan );

        // Reset instance number
		m_childPlan.instance = 0u;

	    std::string session = ActivePlanBulletinBoard::instance()->getSession( m_flowControl.getPlan() );
   	    std::string pmId = ActivePlanBulletinBoard::instance()->getPlanManagerId( m_flowControl.getPlan() );

		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::UNDEFINED_ACTIVE_STEP_STATE;

		if( m_params.manualLaunch )
		{
            stepState = TA_Base_Core::LOADED_ACTIVE_STEP_STATE;
            m_remark = RemarkWaitPlanControllerLaunch;

			TA_Base_Bus::IPlanManagerCorbaDef_ptr planManager = NULL;

			try
			{
				ActivePlanBulletinBoard::instance()->getPlanManagerObjRef(m_flowControl.getPlan(), planManager);

				if ( planManager != NULL )
				{
					if ( planManager->_non_existent() )
					{
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Reference to non existent Plan Manager");
						m_remark = RemarkPlanManagerIsNotRegistered;
						stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;						
					}
					else
                    {
                        try
                        {
				            PlanExecutor::instance()->spawnPending( session, pmId, m_flowControl.getPlan(), m_childPlan );
                            std::ostringstream message;
                            message << "ActiveRunPlanStep: pending plan[" << m_childPlan.plan << ":" << m_childPlan.instance << "] "
                                    << " reserved by the Plan Agent for session id[" << session.c_str() << "]";
				            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, message.str().c_str());

			                TA_Base_Core::CurrentPlanStepDetail detail;

			                detail.activePlan = m_flowControl.getPlan();
			                detail.step = getStepNumber();
                            detail.data.runPlan(m_childPlan);

						    planManager->executeInteractiveStep(detail);
                            m_flowControl.setInteractiveStep(true);
                        }
                        catch (const TA_Base_Bus::IPlanManagerCorbaDef::InteractiveStepExecuteError& isee)
                        {   
                            std::string message = "ActiveRunPlanStep: Exception caught IPlanManagerCorbaDef::InteractiveStepExecuteError. ";
                            message += isee.reason.in();
				            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, message.c_str() );

                            m_remark = RemarkPlanControllerChildPlanCanNotLaunch;
                            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;                            
                        }
			            catch (const PlanNotFoundException& ex)
			            {
							LOG_EXCEPTION_CATCH(SourceInfo, "PlanNotFoundException", ex.what());
				            m_remark = RemarkNoFoundPlan;
				            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			            }
			            catch ( ... )
			            {
							LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception when run a plan");
				            m_remark = RemarkRunPlanFailed;
				            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			            }
                    }
				}
				else
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error: Unregistered Plan Manager is running the interactive plan.");
					m_remark = RemarkNoPlanManagerForOperatorInput;
					stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
				}
			}
			catch ( const CORBA::Exception& e)
			{
				LOG_EXCEPTION_DETAILS( SourceInfo, "TA_Base_Core::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
				m_remark = RemarkPlanManagerIsNotRegistered;
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			}
			catch ( ... )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error: Unregistered Plan Manager is running the interactive plan.");
				m_remark = RemarkPlanManagerIsNotRegistered;
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			}

            if ( TA_Base_Core::FAILED_ACTIVE_STEP_STATE != stepState )
            {
                m_flowControl.pauseForOperator();
            }
		}
		else
		{
            stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;
            m_remark = RemarkPlanStepExecuted;

			try
			{
                PlanExecutor::instance()->run( session, pmId, m_childPlan, false, false );
                std::ostringstream message;
                message << "ActiveRunPlanStep: Execution of the plan[" << m_childPlan.plan << ":" << m_childPlan.instance << "] "
                        << " started by the Plan Agent for session id[" << session.c_str() << "]";
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, message.str().c_str());

		        if( m_params.waitForCompletion )
		        {
			        // Tell the active plan that this plan needs to wait for the
			        // child plan to complete successfully (or fail if it does not).
			        m_flowControl.setCompletedInterlock( m_childPlan );
			        
			        stepState = TA_Base_Core::LOADED_ACTIVE_STEP_STATE;

                    m_remark = generateCompletionPendingRemark();
		        }

		        m_flowControl.setNextStep();	//Increment the step

			}
			catch (const PlanNotFoundException& ex)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "PlanNotFoundException", ex.what());
				m_remark = RemarkNoFoundPlan;
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			}
			catch ( const TA_Base_Core::PlanAgentException& ex)
			{                
				LOG_EXCEPTION_CATCH(SourceInfo, "PlanAgentException", ex.what());
				m_remark = RemarkRunPlanFailed;
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			}
			catch ( ... )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception when Run Plan");
				m_remark = RemarkRunPlanFailed;
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			}
        }

        return stepState;
	}

    std::string ActiveRunPlanStep::generateCompletionPendingRemark() const
    {
        // There should be a pending instance of the child plan.
        std::ostringstream remark;
        remark << "Awaiting completion of plan '" 
               << ActivePlanBulletinBoard::instance()->getActivePlanDetail(m_childPlan).path.in()
               << "' instance " << m_childPlan.instance;

        return remark.str();
    }

} // TA_Base_App


#endif // STEPTYPE_RUN_PLAN

