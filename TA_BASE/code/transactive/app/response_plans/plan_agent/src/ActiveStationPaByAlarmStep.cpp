
#include "app/response_plans/common/src/ProjectDefinition.h"

#if defined ( STEPTYPE_STATION_PUBLIC_ADDRESS_BY_ALARM )

#include "core/corba/src/CorbaUtil.h"
#include "bus/pa/pa_agent_proxy/src/PaAgentAccessFactory.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/PlanAgentUser.h"
#include "app/response_plans/plan_agent/src/ActiveStationPaByAlarmStep.h"

namespace TA_Base_App
{
	ActiveStationPaByAlarmStep::ActiveStationPaByAlarmStep( const TA_Base_Core::StepDetail& stepDetail,
		const TA_Base_Core::StationPublicAddressByAlarmParameters& params, IActivePlanFlow& flowControl )
		:ActiveStep(stepDetail, flowControl),m_params(params),m_flowControl(flowControl)
	{
	}

	ActiveStationPaByAlarmStep::~ActiveStationPaByAlarmStep(void)
	{

	}

	TA_Base_Core::EActiveStepState ActiveStationPaByAlarmStep::executeImpl( void )
	{
		FUNCTION_ENTRY("executeImpl");
		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;

		try
		{
			unsigned long locationKey = PlanAgentUser::getInstance()->getAgentLocationKey();
			TA_Base_Bus::PaNamedObj& refPaAgent = TA_Base_Bus::PaAgentAccessFactory::getInstance().getPaAgentByLocationKey( locationKey );
			
			TA_Base_Core::ActivePlanId plan = m_flowControl.getPlan();
			unsigned long alarmEquipmentEntityKey = plan.entitykey;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ActiveStationPaByAlarmStep::executeImpl EntityKey:%lu LocationKey:%lu",alarmEquipmentEntityKey, locationKey);
			CORBA_CALL( refPaAgent, broadcastFasTriggeredStationDva, (alarmEquipmentEntityKey ,locationKey));
			m_flowControl.setNextStep();
		}
		catch (const CORBA::Exception& ex)
		{
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
		}
		catch (const TA_Base_Core::TransactiveException& ex)
		{
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
		}
		catch (const std::exception& ex)
		{
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			LOG_EXCEPTION_CATCH(SourceInfo, "std::exception", ex.what());
		}
		catch(...)
		{
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception when ActiveStationPaByAlarmStep::executeImpl");
		}

		FUNCTION_EXIT;
		return stepState ;
	}
}
#endif //STEPTYPE_STATION_PUBLIC_ADDRESS_BY_ALARM