/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP/3001/transactive/bus/maintenance_management/mmsHelper/src/MMSUtility.h $
* @author: huirong.luo
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2011/09/29 14:12:57 $
* Last modified by:  $Author: grace.koh $
* 
* This class provides common functions.
*  
*
*/

// #if !defined(EA_D2FA5381_40EC_479d_B593_93B5980458B6__INCLUDED_)
// #define EA_D2FA5381_40EC_479d_B593_93B5980458B6__INCLUDED_

#if !defined(EA_5891EE00_0BC6_47c0_864F_9F5A4B079DC6__INCLUDED_)
#define EA_5891EE00_0BC6_47c0_864F_9F5A4B079DC6__INCLUDED_

#include <string>
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/data_access_interface/src/MmsRequestAlarmData.h"
#include "bus/maintenance_management/mmsCorbaDef/idl/src/IMmsJobRequestSubmitterCorbaDef.h"

namespace TA_Base_Bus
{
	class MMSUtility
	{

	public:
		MMSUtility();
		virtual ~MMSUtility();
         
	 
		/**
		* getAlarmFaultCode
		*
		* When passed an entity name this returns its fault code
		*
		* @param string - Name of an entity
		* 
		* @return string - The fault code of entity. Returns an empty string if no fault code
		*                  could be determined.
		*/
		static std::string  getAlarmFaultCode(std::string& entityName);
 

		/**
		* getMeterCode
		*
		* When passed an entity name this returns its meter code
		*
		* @param string - Name of an entity
		* 
		* @return string - The meter code of entity. Returns an empty string if no meter code
		*                  could be determined.
		*/
		static std::string  getMeterCode(std::string& entityName);

        static std::string getAlarmDescription( const unsigned long ulMsgKey, const std::string& strParamList, const std::string& strAlarmValue );

		/**
		* getMmsRequestAlarmData
		*
		* translate AlarmDetailCorbaDef into MmsRequestAlarmData 
		*
		* @param string - Name of an entity
		* 
		* @return string - The fault code of entity. Returns an empty string if no fault code
		*                  could be determined.
		*/
		static TA_Base_Core::MmsRequestAlarmData* getMmsRequestAlarmData(const TA_Base_Core::AlarmDetailCorbaDef& alarm, std::string sessionID ="");
		

		static bool saveMmsRequestAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm);


		static bool submitJobRequest(const TA_Base_Bus::JobRequestDetailCorbaDef& details);
	 

	};

}
#endif // !defined(EA_5891EE00_0BC6_47c0_864F_9F5A4B079DC6__INCLUDED_)
