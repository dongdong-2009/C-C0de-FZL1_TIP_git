/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Bart Golab
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  Represents a Train Traveller Info System step in an executing plan.
  */

#include "app/response_plans/common/src/ProjectDefinition.h"

#if defined ( STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM )

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActiveTrainTisStep.h"
#include "app/response_plans/common/src/StringConstants.h"

// namespace alias for Corba structures/types

namespace TA_Base_App
{

    ActiveTrainTisStep::ActiveTrainTisStep(const TA_Base_Core::StepDetail& stepDetail,
        const TA_Base_Core::TrainTravellerInfoSystemParameters& params, IActivePlanFlow& flowControl ) :
	ActiveStep(stepDetail, flowControl),
    m_params(params)
	{
	}

	TA_Base_Core::EActiveStepState ActiveTrainTisStep::executeImpl( void )
	{
        // TODO: BartG 23/6/05
        //       TD6840 - to be implemented when the TTIS subsystem has been extended to meet all the requirements of this step.
        //
        // The following issues have been raised with the Visual team:
        // 1. It is not possible to target a message at inbound/outbound/all trains. This was raised as a requirement with 
        //    Craig Grant at the beginning of 2004 and he committed to extending the ITTISManagerCorbaDef interface to 
        //    accomodate this. It makes sense to have this in the agent, as opposed to every client generating the respective 
        //    train lists.
        // 2. Apparently, a TTIS request may take quite a while to complete if the train does not respond. For a group of 
        //    trains this will be even longer, so it might be a good idea to add support for some asynchronous calls... 

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Train TIS step %lu", getStepNumber() );

		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;

		m_remark = RemarkPlanStepExecuted;
		m_flowControl.setNextStep();

		return stepState;
	}

} // TA_Base_App


#endif //  STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM
