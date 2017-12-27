/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/access_control/alarm_actions/src/AlarmActions.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
#ifndef ALARMACTIONS_H
#define ALARMACTIONS_H

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Bus
{
    namespace AlarmActions
    {
        enum AlarmActionType
        {
            ACK_ALARM, // Acknowledge an alarm
            CLOSE_ALARM, // Close an alarm
            COMMENT_ALARM, // Add a comment to an alarm
            VIEW_ALARM,     // View an alarm
            ACK_OPERATIONAL,
            ACK_MAINTENANCE,
            ACK_BOTH,
            CLOSE_OPERATIONAL,
            CLOSE_MAINTENANCE,
            CLOSE_BOTH,
            VIEW_OPERATIONAL,
            VIEW_MAINTENANCE,
            VIEW_BOTH
        };

		/** 
		  * getAlarmAction
		  *
		  * Base in the alarm type key, and the action you want to perform, and 
		  * an AccessControlledAction id will be returned.
		  *
		  * The alarm type key can be retrieved by:
		  * - accessing the messageTypeKey variable in the AlarmMessageCorbaDef idl
		  * - accessing the type directly TA_Base_Core::YourAlarm::YourAlarmType.getTypeKey()
		  *
		  * @return TA_Base_Bus::AccessControlledAction The Access controlled action
		  *
		  * @param  std::string& alarmTypeKey the alarm type key
		  * @param AlarmActionType actionType the type of action you want to perform
		  */
		static TA_Base_Bus::AccessControlledAction getAlarmAction( unsigned long alarmTypeKey, 
                                                              AlarmActionType actionType )
        {
			switch ( actionType )
            {
                case ACK_ALARM:
                    return alarmTypeKey;
                    break;
                case CLOSE_ALARM:
                    return alarmTypeKey + 100000;
                    break;
                case COMMENT_ALARM:
                    return aca_ADD_COMMENT_TO_ALARM;
                    break;
                case VIEW_ALARM:
                    return alarmTypeKey + 200000;
                    break;
                case ACK_OPERATIONAL:
                    return alarmTypeKey + 10000;
                    break;
                case ACK_MAINTENANCE:
                    return alarmTypeKey + 20000;
                    break;
                case ACK_BOTH:
                    return alarmTypeKey + 30000;
                    break;
                case CLOSE_OPERATIONAL:
                    return alarmTypeKey + 110000;
                    break;
                case CLOSE_MAINTENANCE:
                    return alarmTypeKey + 120000;
                    break;
                case CLOSE_BOTH:
                    return alarmTypeKey + 130000;
                    break;
                case VIEW_OPERATIONAL:
                    return alarmTypeKey + 210000;
                    break;
                case VIEW_MAINTENANCE:
                    return alarmTypeKey + 220000;
                    break;
                case VIEW_BOTH:
                    return alarmTypeKey + 230000;
                    break;
                default:
                    TA_ASSERT(false, "Invalid alarm action type was specified");
            }

			// Never get here
			return 0;
        }

    };
};

#endif //ALARMACTIONS_H
