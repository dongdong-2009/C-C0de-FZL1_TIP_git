

#include "StdAfx.h"

#ifdef STEPTYPE_STATION_PUBLIC_ADDRESS_BY_ALARM

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_manager/src/StationPAByAlarmStep.h"
#include "app/response_plans/plan_manager/src/StationPAByAlarmParameter.h"

namespace TA_Base_App
{
	const std::string StationPAByAlarmStep::m_strMsgName = "Station Public Access By Alarm"; 
	const std::string StationPAByAlarmStep::m_strMsgValue = "This is a Station Public Address trigger with fire Alarm.";

	StationPAByAlarmStep::StationPAByAlarmStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail)
	:PlanStep(parent, stepDetail)
	{
	}

	StationPAByAlarmStep::StationPAByAlarmStep(PlanNode* parent, PlanStepId stepId)
		:PlanStep(parent, stepId, TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP_BY_ALARM)
	{
	}

	StationPAByAlarmStep::StationPAByAlarmStep(const StationPAByAlarmStep& planStep)
		:PlanStep(planStep)
	{
	}

	StationPAByAlarmStep::~StationPAByAlarmStep(void)
	{
	}


	PlanStep* StationPAByAlarmStep::clone()
	{
		FUNCTION_ENTRY( "clone" );

		// Ensure parameters are loaded
		PlanStepParameterList stepParameters;
		getPlanStepParameters(stepParameters);

		FUNCTION_EXIT;
		return new StationPAByAlarmStep(*this);
	}

	void StationPAByAlarmStep::getUpdatedParameters(TA_Base_Core::Step& step)
	{
		TA_Base_Core::StationPublicAddressByAlarmParameters parameters;
		parameters.message = CORBA::string_dup(m_strMsgValue.c_str());
		step.parameters.stationPublicAddrssByAlarm(parameters);
	}

	void StationPAByAlarmStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,	PlanStepParameterList &stepParameters)
	{
		stepParameters.clear();
		TA_Base_App::StationPAByAlarmParameter* parameters = new TA_Base_App::StationPAByAlarmParameter(*this, m_strMsgName, m_strMsgValue);
		
		stepParameters.push_back(parameters);
	}

	void StationPAByAlarmStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
	{
		stepParameters.clear();
		TA_Base_App::StationPAByAlarmParameter* parameters = new TA_Base_App::StationPAByAlarmParameter(*this, m_strMsgName, m_strMsgValue);

		stepParameters.push_back(parameters);
	}
}





#endif //STEPTYPE_STATION_PUBLIC_ADDRESS_BY_ALARM