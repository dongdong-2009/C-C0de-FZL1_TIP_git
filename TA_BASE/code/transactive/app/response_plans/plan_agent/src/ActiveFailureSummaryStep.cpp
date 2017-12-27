/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_Review_Branch_DBSync/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveFailureSummaryStep.cpp $
  *  @author:  Lizette Lingo
  *  @version: $Revision: #5 $
  *
  *  Last modification: $DateTime: 2013/08/26 11:10:02 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */
#include "app/response_plans/common/src/ProjectDefinition.h"

#if defined ( STEPTYPE_FAILURE_SUMMARY_STEP )

#include <string>

#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "bus/response_plans/plan_manager/IDL/src/IPlanManagerCorbaDef.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/PlanExecutor.h"
#include "app/response_plans/plan_agent/src/ActiveFailureSummaryStep.h"
#include "app/response_plans/common/src/StringConstants.h"

// namespace alias for Corba structures/types

namespace TA_Base_App
{

    ActiveFailureSummaryStep::ActiveFailureSummaryStep( const TA_Base_Core::StepDetail& stepDetail, 
        const TA_Base_Core::FailureSummaryParameters& params, IActivePlanFlow& flowControl ) : ActiveStep(stepDetail, flowControl),
    m_params(params),
    m_flowControl(flowControl)
    {
    }

    TA_Base_Core::EActiveStepState ActiveFailureSummaryStep::executeImpl( void )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Failure summary step %lu", getStepNumber() );

        TA_Base_Core::EActiveStepState stepState = TA_Base_Core::LOADED_ACTIVE_STEP_STATE;

        m_remark = RemarkWaitingForOperatorResponse;

        TA_Base_Core::FailedStepDetails failedSteps;

        try
        {
            TA_Base_Core::ExecutionHistory* executionHistory = ActivePlanBulletinBoard::instance()->getPlanExecutionHistory(m_flowControl.getPlan(), false);

            if (NULL != executionHistory)
            {
                std::vector<TA_Base_Core::ActiveStepDetail> stepDetailsList;
                for (CORBA::ULong i = 0, j = 0; i < executionHistory->details.length(); i++)
                {
                    TA_Base_Core::ActiveStepDetail& stepDetail =  executionHistory->details[i];

                    if (stepDetail.state == TA_Base_Core::FAILED_ACTIVE_STEP_STATE)
                    {
                        stepDetailsList.push_back(stepDetail);
                    }
                }

                // get failed step data
                int failedStepsCount = stepDetailsList.size();

                if (failedStepsCount > 0)
                {
                    failedSteps.length(failedStepsCount);

                    for ( int i=0; i<failedStepsCount; ++i )
                    {
                        TA_Base_Core::ActiveStepDetail& stepDetail = stepDetailsList[i];
                        TA_Base_Core::FailedStepDetail failedStep;
                        failedStep.position =  stepDetail.position;
                        failedStep.name = stepDetail.name;
                        failedStep.error = stepDetail.remark;
                        failedSteps[i] = failedStep;
                    }
                }

                stepDetailsList.clear();
                delete executionHistory;
                executionHistory = NULL;
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
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error: Unregistered PlanManager is running the interactive plan.");
            m_remark = RemarkPlanManagerIsNotRegistered;
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }

        // initialize structure to pass to Plan Manager
        TA_Base_Core::CurrentPlanStepDetail failureSummaryDetail;
        failureSummaryDetail.activePlan = m_flowControl.getPlan();
        failureSummaryDetail.step = getStepNumber();
        failureSummaryDetail.data.failedSteps(failedSteps);

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Failure summary step %lu", failureSummaryDetail.data._d());

        // ask Plan Manager to display the Failure Summary dialog
        TA_Base_Bus::IPlanManagerCorbaDef_ptr planManager = NULL;
        try
        {
            ActivePlanBulletinBoard::instance()->getPlanManagerObjRef(m_flowControl.getPlan(), planManager);

            if ( planManager != NULL )
            {
                if ( planManager->_non_existent() )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Reference to non existent Plan Manager");
                    stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
                    m_remark = RemarkPlanManagerIsNotRegistered;
                }
                else
                {
                    try
                    {
                        planManager->executeInteractiveStep(failureSummaryDetail);
                    }
                    catch (const TA_Base_Bus::IPlanManagerCorbaDef::InteractiveStepExecuteError& isee)
                    {   
                        std::string message = "ActiveLogStep: Exception caught IPlanManagerCorbaDef::InteractiveStepExecuteError. ";
                        message += isee.reason.in();
                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, message.c_str() );
                        m_remark = isee.reason.in();
                        stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;                            
                    }
                }
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error: Unregistered Plan Manager is running the interactive plan.");
                stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
                m_remark = RemarkNoPlanManagerForOperatorInput;
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
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error: Unregistered PlanManager is running the interactive plan.");
            m_remark = RemarkPlanManagerIsNotRegistered;
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }

        if ( TA_Base_Core::FAILED_ACTIVE_STEP_STATE != stepState )
        {
            m_flowControl.pauseForOperator();
        }

        return stepState;
    }

} // TA_Base_App


#endif //  STEPTYPE_FAILURE_SUMMARY_STEP
