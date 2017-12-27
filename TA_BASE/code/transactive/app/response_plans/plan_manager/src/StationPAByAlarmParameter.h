#pragma once

#ifdef STEPTYPE_STATION_PUBLIC_ADDRESS_BY_ALARM

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStepParameter.h"
#include "PlanStepParameterListCtrl.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
	class StationPAByAlarmParameter : public PlanStepParameter
	{
	public:
		StationPAByAlarmParameter(PlanStep& parent, const std::string& name, const std::string& value);
		~StationPAByAlarmParameter(void);
	public:
		virtual void addToParameterList(CPlanStepParameterListCtrl &listCtrl);
	protected:
		virtual TA_Base_App::CPlanStepParameterEditDlg* getEditDialog(bool canEdit);
	private:
		PlanStep& m_PlanStep;
		std::string m_strValue;
	};
};


#endif //STEPTYPE_STATION_PUBLIC_ADDRESS_BY_ALARM

