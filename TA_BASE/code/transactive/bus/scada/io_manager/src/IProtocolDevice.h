/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/IProtocolDevice.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SCADA_I_PROTOCOL_DEVICE_H_
#define SCADA_I_PROTOCOL_DEVICE_H_

#include <string>
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "bus/generic_agent/src/IEntity.h"

#include "IDeviceData.h"
#include "IDeviceCommand.h"

namespace TA_Base_Bus
{
	class IProtocolManager;
	class IDataProcessor;

	/** IProtocolDevice define the ProtocolDevice needed interface.
     * 
	 * __NOTE__: derived from IEntity.
     */
	class IProtocolDevice : public IEntity
	{
	public:

		/** send a DeviceCommand to outer device.
         *
		 * @param cmd: the command to send.
         * @return bool: if send successfully return true, else false.
         */
		virtual bool sendCommand(IDeviceCommand* cmd) = 0;


		//entity attributes.
		
		/** to get Entity key.
         *
         * @return unsigned long: Entity pkey.
         */
		virtual unsigned long getEntityKey() = 0;
		
		/** to get Entity Name.
         *
         * @return std::string: Entity Name.
         */
		virtual std::string getEntityName() = 0;

		/** to get Entity Address.
         *
         * @return std::string: Entity Address.
         */
		virtual std::string getEntityAddress() = 0;

		/** to get Entity Type Name.
         *
         * @return std::string: Entity Type Name.
         */
		virtual std::string getEntityType() = 0;

		/** to get Entity Description.
         *
         * @return std::string: Entity Description.
         */
		virtual std::string getEntityDescription() = 0;

		/** to get Entity Parent Name.
         *
         * @return std::string: Entity Parent Name.
         */
		virtual std::string getEntityParentName() = 0;

		/** to get Agent Name.
         *
         * @return std::string: Entity Agent Name.
         */
		virtual std::string getEntityAgentName() = 0;

		/** to get Location Name.
         *
         * @return std::string: Entity Location Name.
         */
		virtual std::string getEntityLocationName() = 0;

		/** to get Entity Subsystem Name.
         *
         * @return std::string: Entity Subsystem Name.
         */
		virtual std::string getEntitySubsystemName() = 0;
	};

};

#endif
