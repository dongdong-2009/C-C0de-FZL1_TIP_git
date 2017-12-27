#pragma once


#ifdef STEPTYPE_STATION_PUBLIC_ADDRESS_BY_ALARM

#include "app/response_plans/plan_manager/src/PlanStep.h"
#include "PlanManagerCommonDefs.h"
namespace TA_Base_App
{
	class StationPAByAlarmStep : public PlanStep
	{
	public:
		StationPAByAlarmStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
		StationPAByAlarmStep(PlanNode* parent, PlanStepId stepId);
		virtual ~StationPAByAlarmStep(void);
	// -- the Plan step interface --
	public:
		
		virtual PlanStep* clone();
		virtual bool isStepBranching() const { return false; }
		virtual bool canIgnoreFailure() const { return true; }
	protected:
		// Get parameters for update back to agent
		virtual void getUpdatedParameters(TA_Base_Core::Step& step);
		virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,	PlanStepParameterList &stepParameters);
		virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
	//]]-- the Plan step interface --

	protected:
		StationPAByAlarmStep(const StationPAByAlarmStep& planStep);
	public:
		static const std::string m_strMsgName; 
		static const std::string m_strMsgValue;
	};
}

#endif //STEPTYPE_STATION_PUBLIC_ADDRESS_BY_ALARM