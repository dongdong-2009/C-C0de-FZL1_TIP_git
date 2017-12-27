/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/IProtocolIOBase.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SCADA_I_PROTOCOL_IO_BASE_H_
#define SCADA_I_PROTOCOL_IO_BASE_H_

#include <string>
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "IDeviceData.h"

namespace TA_Base_Bus
{
	class IProtocolManager;
	class IDataPointProcessor;

	/** IProtocolIOBase define the in and out needed interface.
     * "in" is Device to DataPoint, "out" is DataPoint to Device.
	 * 
     */
	class IProtocolIOBase
	{
	public:
		virtual ~IProtocolIOBase(){};

		/** get the Name of object.
         *
         * @return std::string: return object name.
         */
		virtual std::string getName() = 0;

		/** add DataPoint the io object.
         *
         * @param dp: DataPoint pointer.
         * @return bool: if added, return true, else false.
         */
		virtual bool addDataPoint(DataPoint* dp) = 0;
		
		/** after all DataPoint added, call this function do some special thing if needed.
         *
		 * @param
         */
		virtual void finishedAddDataPoint() = 0;

		/** set the io object operation mode.
         *
         * @param mode: EOperationMode.
         */
		virtual void setOperationMode(TA_Base_Core::EOperationMode mode) = 0;

		/** register ProtocolManager interface, if needed.
         *
         * @param mgr: ProtocolManager interface.
         * @return void: 
         */
		virtual void registerProtocolManager(IProtocolManager* mgr) = 0;

		/** register IDataPointProcessor interface, for updating DataPoint function.
         *
         * @param psr: IDataPointProcessor interface.
         * @return void:
         */
		virtual void registerDPProcessor(IDataPointProcessor* psr) = 0;

		/** when device have data to process, call this function to do it.
         *
         * @param data: device got data, to process.
         * @return void:
         */
		virtual void submitDeviceData(IDeviceData* data) = 0;

		/** process DataPointWriteRequest.
         *
         * @param writeRequest: DataPointWriteRequest.
         * @return bool: if write successful, return true, else false.
         */
		virtual bool processWriteRequest(DataPointWriteRequest& writeRequest) = 0;

	};

};

#endif
