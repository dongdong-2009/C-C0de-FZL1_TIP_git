/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/ProtocolDevice.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SCADA_PROTOCOL_DEVICE_H_
#define SCADA_PROTOCOL_DEVICE_H_

#include "IProtocolDevice.h"

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Bus
{
	/** implement the IProtocolDevice interface, inclued IEntity interface.
     * 
	 * __NOTE__: IProtocolDevice derived from IEntity.
     */
	class ProtocolDevice : public IProtocolDevice
	{
	public:
		ProtocolDevice(TA_Base_Core::IEntityData*);
		virtual ~ProtocolDevice();

		//IEntity;
		virtual bool isValid(){return true;};
		void start(void){};
		void stop(void) {};
		void update(const TA_Base_Core::ConfigUpdateDetails &){};
		void remove(void){};
		void setToControlMode(void){};
		void setToMonitorMode(void){};

		virtual bool sendCommand(IDeviceCommand*) = 0;

		//entity attributes.
		unsigned long getEntityKey(){ return m_entityKey;};
		std::string getEntityName(){ return m_entityName;};
		std::string getEntityAddress(){ return m_entityAddress;};
		std::string getEntityType(){ return m_entityType;};
		std::string getEntityDescription(){ return m_entityDescription;};
		std::string getEntityParentName(){ return m_entityParentName;};
		std::string getEntityAgentName(){ return m_entityAgentName;};
		std::string getEntityLocationName(){ return m_entityLocationName;};
		std::string getEntitySubsystemName(){ return m_entitySubsystemName;};

	protected:
		//entity attributes.
		unsigned long	m_entityKey;
		std::string		m_entityName;
		std::string		m_entityAddress;
		std::string		m_entityType;
		std::string		m_entityDescription;
		std::string		m_entityParentName;
		std::string		m_entityAgentName;
		std::string		m_entityLocationName;
		std::string		m_entitySubsystemName;
	};

};

#endif
