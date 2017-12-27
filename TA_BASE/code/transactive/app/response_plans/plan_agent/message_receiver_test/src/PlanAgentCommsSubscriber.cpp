/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_agent/message_receiver_test/src/PlanAgentCommsSubscriber.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "PlanAgentCommsSubscriber.h"
#include <iomanip>

namespace PaData = TA_Base_Core;

PlanAgentCommsSubscriber::PlanAgentCommsSubscriber()
{
	m_session = "";

	m_planStateMap.insert( PlanStateMap::value_type(PaData::EXECUTING_ACTIVE_PLAN_STATE, "executing") );
	m_planStateMap.insert( PlanStateMap::value_type(PaData::PAUSED_ACTIVE_PLAN_STATE   , "paused")    );
	m_planStateMap.insert( PlanStateMap::value_type(PaData::STOPPED_ACTIVE_PLAN_STATE  , "stopped")   );
	m_planStateMap.insert( PlanStateMap::value_type(PaData::COMPLETED_ACTIVE_PLAN_STATE, "completed") );
	m_planStateMap.insert( PlanStateMap::value_type(PaData::ABORTED_ACTIVE_PLAN_STATE  , "aborted")   );
	m_planStateMap.insert( PlanStateMap::value_type(PaData::FAILED_ACTIVE_PLAN_STATE   , "failed")    );
	m_planStateMap.insert( PlanStateMap::value_type(PaData::PENDING_ACTIVE_PLAN_STATE  , "pending")   );
	m_planStateMap.insert( PlanStateMap::value_type(PaData::UNDEFINED_ACTIVE_PLAN_STATE, "undefined") );
	m_planStateMap.insert( PlanStateMap::value_type(PaData::OPERATOR_PAUSED_ACTIVE_PLAN_STATE  , "pausedforOperator")   );
	m_planStateMap.insert( PlanStateMap::value_type(PaData::EXPIRED_ACTIVE_PLAN_STATE,		"expired") );

	m_stepStateMap.insert( StepStateMap::value_type(PaData::EXECUTED_ACTIVE_STEP_STATE  , "executed")   );
	m_stepStateMap.insert( StepStateMap::value_type(PaData::SKIPPED_ACTIVE_STEP_STATE   , "skipped")    );
	m_stepStateMap.insert( StepStateMap::value_type(PaData::UNEXECUTED_ACTIVE_STEP_STATE, "unexecuted") );
	m_stepStateMap.insert( StepStateMap::value_type(PaData::FAILED_ACTIVE_STEP_STATE    , "failed")     );
	m_stepStateMap.insert( StepStateMap::value_type(PaData::LOADED_ACTIVE_STEP_STATE    , "loaded")     );
	m_stepStateMap.insert( StepStateMap::value_type(PaData::PAUSED_ACTIVE_STEP_STATE    , "paused")     );
	m_stepStateMap.insert( StepStateMap::value_type(PaData::UNDEFINED_ACTIVE_STEP_STATE , "undefined")  );	

	std::cout << "-------------------------------------------------------------------------------------------------" << std::endl
			  << "|              Plan              | Plan State |           Step           | Step State | UpdateId I" << std::endl;
}

void PlanAgentCommsSubscriber::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
	PaData::ActivePlanDetail* activePlanDetail;

	if ((message.messageState >>= activePlanDetail))
	{
		if( 0 == activePlanDetail->currentStepDetail.position )
		{
			std::cout << "---------------------------------------------------------------------------------------" << std::endl;
		}
		std::cout << "| " << std::setw(22)  << std::left << activePlanDetail->path.in() << std::internal << " ("
				  << std::setw(2)  << activePlanDetail->plan.plan << ","
				  << std::setw(2)  << activePlanDetail->plan.instance << ") "
			      << "| " << std::setw(10) << m_planStateMap[activePlanDetail->activePlanState] << " "
				  << "| " << std::setw(2)  << activePlanDetail->currentStepDetail.position << ": " 
				  << std::setw(20) << std::left << activePlanDetail->currentStepDetail.name.in() << std::internal << " "
				  << "| " << std::setw(10) << m_stepStateMap[activePlanDetail->currentStepDetail.state] 
				  << " |" << std::setw(10) << activePlanDetail->activeUpdateId << std::endl;


				  /*
		std::cout << "\tPath         : " << activePlanDetail->path.in() << std::endl;
		std::cout << "\tSession      : " << activePlanDetail->session << std::endl;
		std::cout << "\tOwner        : " << activePlanDetail->owner.in() << std::endl;
		std::cout << "\tLast transn  : " << activePlanDetail->timeOfLastStateTransition.in() << std::endl;
		std::cout << "\tInstantiation: " << activePlanDetail->timeOfInstantiation.in() << std::endl;
		std::cout << "Step:" << std::endl;
		std::cout << "\tName         : " << activePlanDetail->currentStepDetail.name.in() << std::endl;
		*/

		//std::string newSession = activePlanDetail->session.in();
		//if (m_session.compare(newSession) != 0 )
		//{
		//	m_session = newSession;
		//	std::cout << "\tSession Changed TO: " << m_session.c_str() << std::endl;
		//}
	}
}

enum EActivePlanState
{
	EXECUTING_ACTIVE_PLAN_STATE,
	PAUSED_ACTIVE_PLAN_STATE   ,
	STOPPED_ACTIVE_PLAN_STATE  ,
	COMPLETED_ACTIVE_PLAN_STATE,
	ABORTED_ACTIVE_PLAN_STATE  ,
	FAILED_ACTIVE_PLAN_STATE   ,
	PENDING_ACTIVE_PLAN_STATE  ,
	UNDEFINED_ACTIVE_PLAN_STATE,
    OPERATOR_PAUSED_ACTIVE_PLAN_STATE,
    EXPIRED_ACTIVE_PLAN_STATE
};


enum EActiveStepState
{
	EXECUTED_ACTIVE_STEP_STATE  ,
	SKIPPED_ACTIVE_STEP_STATE   ,
	UNEXECUTED_ACTIVE_STEP_STATE,
	FAILED_ACTIVE_STEP_STATE    ,
	LOADED_ACTIVE_STEP_STATE    ,
	UNDEFINED_ACTIVE_STEP_STATE
};
