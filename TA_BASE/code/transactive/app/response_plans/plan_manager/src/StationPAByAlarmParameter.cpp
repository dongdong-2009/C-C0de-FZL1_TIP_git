
#include "StdAfx.h"

#ifdef STEPTYPE_STATION_PUBLIC_ADDRESS_BY_ALARM

#include "core/utilities/src/DebugUtil.h"
#include "StationPAByAlarmParameter.h"

namespace TA_Base_App
{
	StationPAByAlarmParameter::StationPAByAlarmParameter(PlanStep& parent,const std::string& name, const std::string& value)
		:PlanStepParameter(parent, name),m_PlanStep(parent)
	{
		m_strValue = value;
	}

	StationPAByAlarmParameter::~StationPAByAlarmParameter(void)
	{
	}

	void StationPAByAlarmParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
	{
		FUNCTION_ENTRY( "addToParameterList" );

		FUNCTION_EXIT;
	}

	TA_Base_App::CPlanStepParameterEditDlg* StationPAByAlarmParameter::getEditDialog(bool canEdit)
	{
		FUNCTION_ENTRY( "getEditDialog" );
		FUNCTION_EXIT;
		return NULL;
	}
};

#endif //STEPTYPE_STATION_PUBLIC_ADDRESS_BY_ALARM