/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/IAlarmUpdate.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  IAlarmUpdate.h
//  Implementation of the Interface IAlarmUpdate
//  Created on:      24-Jan-2004 04:14:33 PM
///////////////////////////////////////////////////////////

#if !defined(IAlarmUpdate_81C9C237_1918_4838_AF1D_E6E966FD767C__INCLUDED_)
#define IAlarmUpdate_81C9C237_1918_4838_AF1D_E6E966FD767C__INCLUDED_


#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include <string>

/**
 * This interface is used to notify the AlarmStoreManager of new alarms so that
 * they can be recorded and the indexes updated as required.
 * @version 1.0
 * @updated 24-Jan-2004 04:16:31 PM
 */

namespace TA_Base_Bus
{
   class IAlarmUpdate
   {

   public:
	/**
	 * New alarm
	 * @param alarm
	 * 
	 */
	virtual void newAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm) =0;
	/**
	 * Alarm has been removed from agent (closed and ack)
	 * @param alarmID
	 * 
	 */

	virtual void updateAlarm(const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdates) =0;
	/**
	 * This method is called when all the alarms from a location should be removed.
	 * This would happen if we disconnect from an Agent.
	 * @param locationID    Location for which the alarms should be removed.
	 * 
	 */
	//virtual void clearAlarms(unsigned long location) =0;

    /**
	 * shouldQueueAlarmUpdate
	 *
	 * check if alarmUpdate should be queued for later processing if AlarmStoreManager is busy
	 *
	 */
	virtual bool shouldQueueAlarmUpdate() =0;
   };
};
#endif // !defined(IAlarmUpdate_81C9C237_1918_4838_AF1D_E6E966FD767C__INCLUDED_)
