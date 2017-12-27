#if defined ( STEPTYPE_STATION_PUBLIC_ADDRESS_BY_ALARM )

#ifndef ACTIVE_STATION_PUBLIC_ADDRESS_BY_ALARM_STEP_HEADER_INCLUDED
#define ACTIVE_STATION_PUBLIC_ADDRESS_BY_ALARM_STEP_HEADER_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveAbortStep.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2012/06/12 13:35:44 $
  *  Last modified by:  $Author: builder $
  *
  *  <description>
  */

#include "app/response_plans/plan_agent/src/ActiveStep.h"

namespace TA_Base_App
{
	class ActiveStationPaByAlarmStep : public ActiveStep
	{
	public:
		ActiveStationPaByAlarmStep( const TA_Base_Core::StepDetail& stepDetail,
									const TA_Base_Core::StationPublicAddressByAlarmParameters& params,
									IActivePlanFlow& flowControl );
		virtual ~ActiveStationPaByAlarmStep(void);
	protected:
		virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
		ActiveStationPaByAlarmStep& operator=( const ActiveStationPaByAlarmStep& );
		ActiveStationPaByAlarmStep( const ActiveStationPaByAlarmStep& );
	private:
		const TA_Base_Core::StationPublicAddressByAlarmParameters m_params;
		IActivePlanFlow& m_flowControl;
	};
}
#endif //ACTIVE_STATION_PUBLIC_ADDRESS_BY_ALARM_STEP_HEADER_INCLUDED
#endif //STEPTYPE_STATION_PUBLIC_ADDRESS_BY_ALARM