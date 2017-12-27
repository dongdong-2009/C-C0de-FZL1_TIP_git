/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/IIOManager.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SCADA_I_IO_MANAGER_H_
#define SCADA_I_IO_MANAGER_H_

#include <string>
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "bus/generic_agent/src/IEntity.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "IProtocolManager.h"

namespace TA_Base_Bus
{
	/** define the IOManager's interface.
	 * derived manager must implement the createProtocolManager vritual function.
     * 
	 * __NOTE__: the defualt return value of function getEntityGroup is 0.
	 * if the entity data have group concept, should return the real group value.
     */
	class IIOManager
	{
	public:

		/** create a new sub ProtocolManager to manager the special entity type.
         * if exist a ProtocolManager for the special entity type with same group, will not call this function.
         *
         * @param entityType: the entity type.
         * @return IProtocolManager*: new sub ProtocolManager or NULL.
         */
		virtual IProtocolManager* createProtocolManager(std::string entityType) = 0;

		/** create a new Entity.
         *
         * @param entityData: the entity data.
         * @return IEntity*: new IEntity* or NULL.
         */
		virtual IEntity* createEntity(TA_Base_Core::IEntityDataPtr entityData) = 0;

		/** get entity group from entityData
         *
         * @param entityData: the entity data.
         * @return unsigned long: group id, if no group id, value is 0.
         */
		virtual unsigned long getEntityGroup(TA_Base_Core::IEntityDataPtr entityData) = 0;

		/** add a DataPoint to the IOManager.
         *
         * @param dp: the pointer of DataPoint.
         * @return void
         */
		virtual void addDataPoint(DataPoint* dp) = 0;

		/** when all DataPoint added, then must call this function.
         *
         * @return void:
         */
		virtual void finishedAddDataPoint() = 0;

		/** set IOManager operation mode, different mode, do different work.
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
		virtual void updateMoreParas(std::map<std::string, std::string>& paras) = 0;

	};

};

#endif
