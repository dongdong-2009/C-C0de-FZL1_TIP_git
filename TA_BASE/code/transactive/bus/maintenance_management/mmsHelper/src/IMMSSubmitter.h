/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP/3001/transactive/bus/maintenance_management/mmsHelper/src/IMMSSubmitter.h $
* @author: huirong.luo
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2011/09/29 14:12:57 $
* Last modified by:  $Author: grace.koh $
* 
* This class provides common interface to client who wants to submit job request message to MMS
* a instance of the interface should be ensure that all functions are thread saft function.
*
*/

#if !defined(EA_D2FA5381_40EC_479d_B593_93B5980458B6__INCLUDED_)
#define EA_D2FA5381_40EC_479d_B593_93B5980458B6__INCLUDED_

#include <string>
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/data_access_interface/src/MmsRequestAlarmData.h"
#include "bus/maintenance_management/mmsCorbaDef/idl/src/IMmsJobRequestSubmitterCorbaDef.h"

namespace TA_Base_Bus
{
	class IMMSSubmitter
	{

	public:
        virtual ~IMMSSubmitter(){}
		/**
		* submitAutoAlarmMessage
		*
		* use to submit alarm MMS request message to MMS Agent automatically
		*
		* @param AlarmDetailCorbaDef - alarm information
		* 
		* @return bool - true means submit successfully, false: submit failed.
		*                  could be determined.
		*/
		virtual bool submitAutoAlarmMessage(const TA_Base_Core::AlarmDetailCorbaDef& alarm) =0;

		/**
		* submitSEMIAlarmMessage
		*
		* use to submit alarm MMS request message by manual
		*
		* @param AlarmDetailCorbaDef - alarm information
		* @param std::string - ID of session
		* 
		* @return bool - true means submit successfully, false: submit failed.
		*                  could be determined.
		*/
		//virtual bool submitSEMIAlarmMessage(TA_Base_Core::AlarmDetailCorbaDef& alarm, std::string sessionID) =0;

		/**
		* submitJobRequest
		*
		* use to submit MMS request message in schematic or alarm GUI to MMS agent by manual
		*
		* @param JobRequestDetailCorbaDef - Job request information
		* @param std::string - ID of session
		* 
		* @return bool - true means submit successfully, false: submit failed.
		*                  could be determined.
		*/
		virtual bool submitJobRequest(JobRequestDetailCorbaDef& jobRequest, std::string sessionID) =0;

		virtual JobRequestDetailCorbaDef* getDataFromAlarm(std::string alarmID,unsigned long alarmLocationKey, std::string sessionID)=0;

		virtual JobRequestDetailCorbaDef* getDataFromSymbol(std::string entityName,  std::string sessionID)=0;
	};

}
#endif // !defined(EA_D2FA5381_40EC_479d_B593_93B5980458B6__INCLUDED_)
