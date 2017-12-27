/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveBackgroundLaunchSchematicStep.cpp $
  *  @author:  Bart Golab
  *  @version: $Revision: #5 $
  *
  *  Last modification: $DateTime: 2013/01/18 17:55:27 $
  *  Last modified by:  $Author: huangjian $
  *
  *  Represents a Launch Schematic step in an executing plan.
  */

#include "app/response_plans/common/src/ProjectDefinition.h"

#if defined ( STEPTYPE_BACKGROUND_LAUNCH_SCHEMATIC )

#include "core/utilities/src/DebugUtil.h"
#include "core/threads/src/Thread.h"
#include "bus/response_plans/plan_agent/src/SortableActiveStepId.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/CMMessageSender.h"
#include "app/response_plans/plan_agent/src/ActiveBackgroundLaunchSchematicStep.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/common/src/StringConstants.h"

// namespace alias for Corba structures/types
 
namespace TA_Base_App
{

    static const unsigned short SEND_BACKGROUND_LAUNCH_SCHEMATIC_TRY_MAX_TIMES = 3;
    static const long STEP_SLEEP_SLICE = 100; // millisecond
    static const long STEP_RESUME_INTERVAL = 1000; // millisecond

	ActiveBackgroundLaunchSchematicStep::ActiveBackgroundLaunchSchematicStep( const TA_Base_Core::StepDetail& stepDetail, 
        const TA_Base_Core::BackgroundLaunchSchematicParameters& params, IActivePlanFlow& flowControl ) : ActiveStep(stepDetail, flowControl),
    m_params(params),
    m_sendLauchSchemsticTryTimes(0)
	{
        
	}

	TA_Base_Core::EActiveStepState ActiveBackgroundLaunchSchematicStep::executeImpl( void )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Launch Schematic step %lu", getStepNumber() );

        TA_Base_Core::EActiveStepState stepState = TA_Base_Core::UNEXECUTED_ACTIVE_STEP_STATE;

        m_remark = RemarkWaitingForOperatorResponse;

        TA_Base_Core::CurrentPlanStepDetail detail;
        detail.activePlan = m_flowControl.getPlan();
        detail.step = getStepNumber();
        detail.data.bgLaunchSchematic(m_params);

        TA_Base_Bus::SortableActiveStepId srtStep = detail;
        long sleepTime = 0;
        m_sendLauchSchemsticTryTimes = 0;

        while (m_sendLauchSchemsticTryTimes <= SEND_BACKGROUND_LAUNCH_SCHEMATIC_TRY_MAX_TIMES)
        {
            const int executeStatus = ActivePlanBulletinBoard::instance()->getStepCompletionStatus( srtStep );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[MftMsg] Plan [%lu, %lu, %lu] current state is %d", 
                detail.activePlan.plan, detail.activePlan.instance, m_detail.position, executeStatus );

            switch (executeStatus)
            {
            case TA_Base_Core::PSCS_UNDEFINED:
                {
                    // send message
                    if ( m_sendLauchSchemsticTryTimes < SEND_BACKGROUND_LAUNCH_SCHEMATIC_TRY_MAX_TIMES )
                    {
                        if ( 0 == ( sleepTime += STEP_SLEEP_SLICE ) % STEP_RESUME_INTERVAL )
                        {
                            ++m_sendLauchSchemsticTryTimes;

                            if ( !CMMessageSender::instance()->sendActiveStepUpdateMessage(detail) )
                            {
                                stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
                                ActivePlanBulletinBoard::instance()->removeStepCompletionStatus( srtStep );
                            }
                        }
                    }
                    else
                    {
                        m_remark = RemarkStepExecutionFailed;
                        ActivePlanBulletinBoard::instance()->removeStepCompletionStatus( srtStep );
                        stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
                    }
                }
                break;
            case TA_Base_Core::PSCS_SUCCESS:
                {
                    stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;
                    m_flowControl.setNextStep();
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MFT has response for Plan [%lu, %lu, %lu]", detail.activePlan.plan, detail.activePlan.instance, m_detail.position);
                }
                break;
            default:
                {
                    m_sendLauchSchemsticTryTimes = 0;
                    ActivePlanBulletinBoard::instance()->removeStepCompletionStatus( srtStep );
                    m_remark = RemarkInvalidStepParameter;
                    stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Result can not be recognized.");
                }
                break;
            }

            // Step status updated, break the loop
            if ( TA_Base_Core::UNEXECUTED_ACTIVE_STEP_STATE != stepState )
            {
                break;
            }

            TA_Base_Core::Thread::sleep( STEP_SLEEP_SLICE );
        }

        return stepState;
	}

} // TA_Base_App


#endif //  STEPTYPE_BACKGROUND_LAUNCH_SCHEMATIC
