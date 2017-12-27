/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/ProtocolManager.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#ifndef SCADA_DEVICE_MANAGER_H_
#define SCADA_DEVICE_MANAGER_H_

#include "IProtocolManager.h"
#include "ProtocolUpdater.h"
#include "ProtocolSender.h"

#include <map>
#include <vector>
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Bus
{

	/** implement the IProtocolManager interface.
     *  manager all ProtocolDevice and process DeviceData.
     */
	class ProtocolManager : public IProtocolManager
	{
	public:
		ProtocolManager();
		virtual ~ProtocolManager();

		virtual IEntity* createEntity(TA_Base_Core::IEntityDataPtr entityData);

		virtual IProtocolIOBase* addDataPoint(DataPoint*);
		virtual void finishedAddDataPoint();

		virtual void setOperationMode(TA_Base_Core::EOperationMode mode);

		virtual void processData(IProtocolDevice*, IDeviceData*);
		virtual void updateMoreParas(const std::map<std::string, std::string>&) = 0;
		virtual bool isEntityTypeMatched(const std::string&) = 0;

		/** add ProtocolDevice to the ProtocolManager.
         *
         * @param device: ProtocolDevice, need to add.
         * @return void:
         */
		void addProtocolDevice(IProtocolDevice* device);

		/** add ProtocolUpdater to the ProtocolManager.
         *
         * @param device: ProtocolUpdater, need to add.
         * @return void:
         */
		void addProtocolUpdater(ProtocolUpdater* updater);

		/** add ProtocolSender to the ProtocolManager.
         *
         * @param device: ProtocolSender, need to add.
         * @return void:
         */
		void addProtocolSender(ProtocolSender* sender);

		/** add IProtocolDevice and ProtocolUpdater pair to the ProtocolManager.
		 * when device:0 represent all device, updater:0 represent all updater.
         *
         * @param device: ProtocolDevice, pair one.
		 * @param updater: ProtocolUpdater, pair two.
         * @return void:
         */
		void addDeviceUpdaterPair(IProtocolDevice* device = 0, ProtocolUpdater* updater = 0);


	protected:
		std::vector<IProtocolDevice*>	m_devices;
		std::vector<ProtocolUpdater*>	m_protocolUpdaters;
		std::vector<ProtocolSender*>	m_protocolSenders;

		typedef std::vector<ProtocolUpdater*> DataUpdaterVector;
		///< define ProtocolUpdater vector.
		
		DataUpdaterVector m_dataUpdaters;

		std::map<unsigned long, DataUpdaterVector> m_deviceToUpdaters;
		///< device key to DataParserVector map. one device may need more ProtocolUpdater.

		TA_Base_Core::ReEntrantThreadLockable   m_lock;
	};

};

#endif
