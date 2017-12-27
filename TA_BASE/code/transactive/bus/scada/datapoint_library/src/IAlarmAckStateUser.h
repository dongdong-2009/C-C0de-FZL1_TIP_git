/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/IAlarmAckStateUser.h $
  * @author:	Weimin
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *
  * IAlarmAckStateUser interface provide callback mechanism for notification of changes
  * in alarm status.  Client that wishes to receive notification of alarm ack state must
  * implements this interface and then register itself to the AlarmAckStateSingletonManager
  * when subscribing for a particular alarm type.
  *
  */

#ifndef IALARM_ACK_STATE_SUBSCRIBER_H
#define IALARM_ACK_STATE_SUBSCRIBER_H

#include "bus/scada/common_library/src/CommonDefs.h"

namespace TA_Base_Bus
{
	class IAlarmAckStateUser
	{
	public:

		virtual ~IAlarmAckStateUser() {};

		/**
		*
		* updateAlarmAckState
		*
		* This method is used as a callback mechnism by the AlarmAckState library to notify
		* the subscriber of the current status that is specified in the AlarmState object.
		*
		* @param	currentAlarmState	The current alarm status for processing
		*
		*/
		virtual void updateAlarmAckState ( EScadaAlarmAckState ackState ) = 0;
	};
}

#endif
