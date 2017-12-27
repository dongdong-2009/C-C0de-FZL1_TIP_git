/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/HIAlarmDetails.cpp $
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

#include "bus/scada/datapoint_library/src/HIAlarmDetails.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/message/types/DataPointAlarms_MessageTypes.h"

namespace TA_Base_Bus
{

    HIAlarmDetails::HIAlarmDetails( TA_Base_Core::DataPointEntityDataPtr dataPointEntityAccess,
        unsigned long  dpParentEntityKey,
        unsigned long        dpEntityKey,
        unsigned long        dpEntityTypeKey,
        unsigned long	    regionKey,
        unsigned long	    subsystemKey,
        const std::string &  entityName,
        bool                 almEnabled,
        int                  almDelay,
        int                  almSeverity,
        const std::string &  almMessage,
        const std::string &	MMSConfiguration,
        const DpValue &      almValue,
        MmsScadaUtility &    mmsScadaUtility )
        :
    AnalogueAlarmDetails( dataPointEntityAccess, TA_Base_Core::DataPointAlarms::HI, dpParentEntityKey, dpEntityKey, dpEntityTypeKey, regionKey,
        subsystemKey, entityName, almEnabled, almDelay, almSeverity,
        almMessage, MMSConfiguration,
        TA_Base_Bus::DptHi, almValue, mmsScadaUtility )
    {
    }
    
    
    HIAlarmDetails::~HIAlarmDetails() 
    {
    }
    

    bool HIAlarmDetails::inAlarm( const DataPointState & state )
	{
		//
		// TD 8307: Raise alarm when the dp value crosses the alarm limit and close
		//			the alarm when the dp value is below the alarm threshold
		//

		// IF HI=40, VH=60, THRESHOLD=5, THEN ALARM BELOW
		// ------------|-----|---------------------------|-----|-------------------
		// NORMAIL     35 \/ 40       HI ALARM           55 \/ 60    VH ALARM
		//           HI THRESHOLD                      VH THRESHOLD

		// VALUE EXAMPLE:
		// 1, CURRENT=38, NO ALARM
		// 2, CURRENT=40, HI ALARM
		// 3, CURRENT=58, HI ALARM
		// 4, CURRENT=60, HI CLOSE, VH ALARM
		// 5, CURRENT=58, HI CLOSE, VH ALARM
		// 6, CURRENT=54, HI ALARM, VH CLOSE
		// 7, CURRENT=38, HI ALARM
		// 8, CURRENT=34, NO ALARM

        FUNCTION_ENTRY("inAlarm");
		// save the current alarm state
		bool hasBeenInAlarm = isInAlarm();
		bool inAlarm = true;

		// if current value is less than the HI alarm limit
		if ( state.getValue().getFloat() < getValue().getFloat() )
		{
			// check if the value is below the alarm threshold limit
			// before we can consider the alarm condition is normalised
			inAlarm = false;

			double alarmNormalisedThreshold = getValue().getFloat() - m_alarmThresholdConstant;

			// if the current value is greater/equal the alarm normalised threshold, alarmstatus should be kept.
			//19620++ keep alarm state when the difference is less than alarmthreshold (not less or equal)
			//if ( state.getValue().getFloat() >= alarmNormalisedThreshold )
			if ( state.getValue().getFloat() > alarmNormalisedThreshold )
			//++19620
			{
				inAlarm = hasBeenInAlarm;
			}
		}
		// if there is an upper value limit, then check it.
		// maybe, in some time, the upper limit value is equal to or less then the lower limit value, 
		// the following statements will never be excuted, just like 'm_hasOtherLimit' is false
		else if (true == m_hasOtherLimit)
		{
			// if the current value is larger than VH alarm limit, close alarm, because the VH alarm is triggered
			if ( state.getValue().getFloat() >= getOtherLimitValue().getFloat() )
			{
				inAlarm = false;
			}
			// if the current value is between (VH - threshold) and VH, alarm status will be kept.
			else
			{
				inAlarm = true;
				double alarmOtherLimitNormalisedThreshold = getOtherLimitValue().getFloat() - m_alarmThresholdConstant;

				if ( state.getValue().getFloat() >= alarmOtherLimitNormalisedThreshold )
				{
					//TD15874, keep alarm state only when there is VH/OH already in alarm
					if (m_dataPoint != NULL)
					{
                        if (TA_Base_Bus::DptOverScaleHi == m_dataPoint->getAlarmState() ||
							TA_Base_Bus::DptHiHi == m_dataPoint->getAlarmState())
						{
							inAlarm = false;
						}
					}
				}
			}
		}

		FUNCTION_EXIT;

		return inAlarm;
	}


    std::string HIAlarmDetails::getAlarmTypeStr()
	{
		std::string alarmTypeStr = DPTALM_HI_SHORT;
		
		return alarmTypeStr;
	}

}
