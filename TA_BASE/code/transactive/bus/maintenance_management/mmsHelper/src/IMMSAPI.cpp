#include "IMMSAPI.h"

namespace TA_Base_Bus
{
	 
		MMSScheduledMessage::MMSScheduledMessage(void):m_assetID(""),m_meterCode(""),m_entityKey(0),
			m_equipmentDescription(""),m_dataPointValue(0),m_messageType(MMSMessageType::NONE)
		{
			m_planDate.dstflag=-1;
			m_planDate.timezone=-1;
			m_planDate.time=0;
			m_planDate.millitm=0;
		}
		MMSScheduledMessage::~MMSScheduledMessage(void)
		{
		}
		unsigned long  MMSScheduledMessage::getEntityKey()
		{
			return m_entityKey;
		}
		std::string MMSScheduledMessage::getAssetID()
		{
			return m_assetID.c_str();
		}
		std::string MMSScheduledMessage::getMeterCode()
		{
			return m_meterCode.c_str();
		}
		std::string MMSScheduledMessage::getEquipmentDescription()
		{
			return m_equipmentDescription.c_str();
		}
		double MMSScheduledMessage::getDataPointValue()
		{
			return m_dataPointValue;
		}

		MMSMessageType::MessageType MMSScheduledMessage::getMessageType()
		{
			return m_messageType;
		}

		timeb MMSScheduledMessage::getPlanDate()
		{
			return m_planDate;
		}

		void MMSScheduledMessage::setEntityKey(unsigned long value)
		{
			m_entityKey=value;
		}

		void MMSScheduledMessage::setAssetID(std::string& value)
		{
			m_assetID=value.c_str();
		}
		void MMSScheduledMessage::setMeterCode(std::string& value)
		{
			m_meterCode=value.c_str();
		}
		void MMSScheduledMessage::setEquipmentDescription(std::string& value)
		{
			m_equipmentDescription=value.c_str();
		}
		void MMSScheduledMessage::setDataPointValue(double value)
		{
			m_dataPointValue=value;
		}

		void MMSScheduledMessage::setMessageType(MMSMessageType::MessageType value)
		{
			m_messageType=value;
		}
		void MMSScheduledMessage::setPlanDate(timeb value)
		{
			m_planDate=value;
		}

		MMSScheduledMessage::MMSScheduledMessage(const MMSScheduledMessage& other)
		{  
			 m_assetID= other.m_assetID;
			 m_dataPointValue=other.m_dataPointValue;
			 m_equipmentDescription=other.m_equipmentDescription;
			 m_meterCode=other.m_meterCode;
			 m_messageType=other.m_messageType;
		}  

		MMSScheduledMessage& MMSScheduledMessage::operator= (const MMSScheduledMessage& other)  
		{  			 
			if(this == &other)  
				return *this;  
			m_assetID= other.m_assetID;
			m_dataPointValue=other.m_dataPointValue;
			m_equipmentDescription=other.m_equipmentDescription;
			m_meterCode=other.m_meterCode;
			m_messageType=other.m_messageType;
			return *this;  
		}  
		
		IMMSAPI::IMMSAPI(void)
		{
		}

		IMMSAPI::~IMMSAPI(void)
		{
		}
	 
}
