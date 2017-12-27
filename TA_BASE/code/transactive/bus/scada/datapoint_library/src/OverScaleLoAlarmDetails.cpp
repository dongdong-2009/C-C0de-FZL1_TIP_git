/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/OverScaleLoAlarmDetails.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4503)
#endif // defined _MSC_VER

#include "bus/scada/datapoint_library/src/OverScaleLoAlarmDetails.h"

#include "core/message/types/DataPointAlarms_MessageTypes.h"

namespace TA_Base_Bus
{
    OverScaleLoAlarmDetails::OverScaleLoAlarmDetails( TA_Base_Core::DataPointEntityDataPtr dataPointEntityAccess,
        unsigned long          dpParentEntityKey,
        unsigned long          dpEntityKey,
        unsigned long          dpEntityTypeKey,
        unsigned long          regionKey,
        unsigned long          subsystemKey,
        const std::string &    entityName,
        bool                   almEnabled,
        int                    almDelay,
        int                    almSeverity,
        const std::string &    almMessage,
        const std::string &    MMSConfiguration,
        const DpValue &        almValue,
        MmsScadaUtility &      mmsScadaUtility )
        :
    AnalogueAlarmDetails( dataPointEntityAccess, TA_Base_Core::DataPointAlarms::OL, dpParentEntityKey, 
        dpEntityKey, dpEntityTypeKey, regionKey, subsystemKey, entityName, almEnabled, almDelay, 
        almSeverity, almMessage, MMSConfiguration,
        TA_Base_Bus::DptOverScaleLo, almValue, mmsScadaUtility )
    {
    }
    
    
    OverScaleLoAlarmDetails::~OverScaleLoAlarmDetails() 
    {
    }
    
    
	bool OverScaleLoAlarmDetails::inAlarm( const DataPointState & state )
	{
		//
		// TD 8307: Raise alarm when the dp value crosses the alarm limit and close
		//			the alarm when the dp value is above the alarm threshold
		//

        FUNCTION_ENTRY("inAlarm");
		// save the current alarm state
		bool hasBeenInAlarm = isInAlarm();
		bool inAlarm = true;

		// if current value is less than the HI alarm limit
		if ( state.getValue().getFloat() >= getValue().getFloat() )//TD15180
		{
			// check if the value is below the alarm threshold limit
			// before we can consider the alarm condition is normalised
			inAlarm = false;

			double alarmNormalisedThreshold = getValue().getFloat() + m_alarmThresholdConstant;

			// if the current value is greater/equal the alarm normalised threshold, alarmstatus should be kept.
			if ( state.getValue().getFloat() < alarmNormalisedThreshold )//TD15180
			{
				inAlarm = hasBeenInAlarm;
			}
		}

		FUNCTION_EXIT;
		return inAlarm;
	}


    std::string OverScaleLoAlarmDetails::getAlarmTypeStr()
	{
		std::string alarmTypeStr = DPTALM_OL_SHORT;
		
		return alarmTypeStr;
	}

}
