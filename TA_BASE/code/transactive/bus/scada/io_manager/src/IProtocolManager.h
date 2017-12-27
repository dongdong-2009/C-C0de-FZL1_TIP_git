/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/IProtocolManager.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SCADA_I_PROTOCOL_MANAGER_H_
#define SCADA_I_PROTOCOL_MANAGER_H_

#include <string>
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "bus/generic_agent/src/IEntity.h"

#include "IProtocolIOBase.h"
#include "IProtocolDevice.h"

namespace TA_Base_Bus
{

	/** define some api for ProtocolManager.
	 * the ProtocolManager will manager all the ProtocolDevice s.
     * 
     */
	class IProtocolManager
	{
	public:

		/** create a new Entity.
         *
         * @param entityData: the entity data.
         * @return IEntity*: new IEntity* or NULL.
         */
		virtual IEntity* createEntity(TA_Base_Core::IEntityDataPtr entityData) = 0;

		/** add a DataPoint to the IProtocolIOBase, include input DataPoint and output DataPoint.
         *
         * @param dp: the pointer of DataPoint.
         * @return IProtocolIOBase*: DataPoint belonged io object.
         */
		virtual IProtocolIOBase* addDataPoint(DataPoint* dp) = 0;

		/** when all DataPoint added, then must call this function.
         *
         * @return void:
         */
		virtual void finishedAddDataPoint() = 0;

		/** set ProtocolManager operation mode, different mode, do different work.
		 *
         * @param mode: new operation mode.
         * @return void:
         */
		virtual void setOperationMode(TA_Base_Core::EOperationMode mode) = 0;

		/** update some more special parameters if needed.
         *
         * @param paras: parameter name to parameter value pair map.
         * @return void:
         */
		virtual void updateMoreParas(const std::map<std::string, std::string>&) = 0;

		/** when ProtocolDevice got data, call this function to process.
         *
         * @param device: ProtocolDevice, when got data.
		 * @param data: new data
         * @return void:
         */
		virtual void processData(IProtocolDevice* device, IDeviceData* data) = 0;

		/** get this ProtocolDevice dependent on the senderName.
         *
         * @param senderName: ProtocolSender, who need to send write command.
         * @return IProtocolDevice*: if find, return ProtocolDevice, else NULL.
         */
		virtual IProtocolDevice* getOutputDevice(std::string senderName) = 0;

		/** determine if the entity type is matching the specialized ProtocolManager.
         *
         * @param typeName: ProtocolDevice, when got data.
         * @return bool: if matched, return true, else false.
         */
		virtual bool isEntityTypeMatched(const std::string& typeName) = 0;

	};

};

#endif
