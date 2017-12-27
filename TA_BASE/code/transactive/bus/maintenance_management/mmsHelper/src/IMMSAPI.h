/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP/3001/transactive/bus/maintenance_management/mmsHelper/src/IMMSAPI.h $
* @author: huirong.luo
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2011/09/29 14:12:57 $
* Last modified by:  $Author: grace.koh $
* 
* This class provides common interface of MMS API.
* a instance of the interface should be ensure that all functions are thread saft function.
*
*/
 
#if !defined(EA_1C63CC5E_B09D_4f08_BF4F_02C7A09EFB8E__INCLUDED_)
#define EA_1C63CC5E_B09D_4f08_BF4F_02C7A09EFB8E__INCLUDED_

#include <ctime>
#include <string>
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/data_access_interface/src/MmsRequestAlarmData.h"
#include "bus/maintenance_management/mmsCorbaDef/idl/src/IMmsJobRequestSubmitterCorbaDef.h"
#include "core/data_access_interface/entity_access/src/MmsAgentEntityData.h"

namespace TA_Base_Bus
{
	    class MMSMessageType
		{
		public:
			virtual ~MMSMessageType(void){};
		    enum MessageType {NONE,ALARM, COUNTER, MEASUREMENT, JOB_REQUEST_ALARM,JOB_REQUEST_SCHEMATIC, TEST}; 
		private:
			MMSMessageType(void){};
		};
	 
		class MMSScheduledMessage
		{
		public:
			virtual ~MMSScheduledMessage(void);
			unsigned long getEntityKey();
			std::string getAssetID();
			std::string getMeterCode();
			std::string getEquipmentDescription();
			double getDataPointValue();
			timeb getPlanDate();
			MMSMessageType::MessageType getMessageType();
			
			void setEntityKey(unsigned long value);
			void setAssetID(std::string& value);
			void setMeterCode(std::string& value);
			void setEquipmentDescription(std::string& value);
			void setDataPointValue(double value);
			void setMessageType(MMSMessageType::MessageType value);
			void setPlanDate(timeb value);

			 
			
			MMSScheduledMessage(const MMSScheduledMessage& other);
			MMSScheduledMessage& operator = (const MMSScheduledMessage& other);
			 
 
			MMSScheduledMessage(void);
		private:
			std::string m_assetID;
			std::string m_meterCode;
			std::string m_equipmentDescription;
			unsigned long m_entityKey;
			double m_dataPointValue;
			timeb m_planDate;   
			MMSMessageType::MessageType m_messageType;
		};
			  
		class IMMSAPI
		{
		public:		
		 
			virtual ~IMMSAPI(void);

			/**
			* submitAutoAlarmMessage
			*
			* use to submit auto alarm MMS request message to MMS
			*
			* @param MmsRequestAlarmData - MMS request message information
			* 
			* @return bool - true means submit successfully, false: submit failed.
			*                  could be determined.
			*/
			virtual bool submitAutoAlarmMessage(TA_Base_Core::MmsRequestAlarmData& alarm) =0;
	 
			virtual bool submitJobRequestMessage(const TA_Base_Bus::JobRequestDetailCorbaDef& details)=0;

			virtual bool submitScheduledMessage(MMSScheduledMessage& msg)=0;

			virtual bool isMMSLive()=0;

			virtual void initConfiguration(TA_Base_Core::MmsAgentEntityDataPtr pData)=0;

			virtual void setToControlMode()=0;

			virtual void setToMonitorMode()=0;

		protected:
			IMMSAPI(void);
		};
	 
}

#endif // !defined(EA_1C63CC5E_B09D_4f08_BF4F_02C7A09EFB8E__INCLUDED_)