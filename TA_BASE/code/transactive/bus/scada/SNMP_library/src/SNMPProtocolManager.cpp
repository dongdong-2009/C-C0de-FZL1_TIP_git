/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/SNMP_library/src/SNMPProtocolManager.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#include "SNMPProtocolManager.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/entity_access/src/SNMPDeviceEntityData.h"

#include "SNMPDataUpdater.h"
#include "SNMPOutputSender.h"
#include "SNMPDevice.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	const std::string SNMPProtocolManager::SNMP_TRAP_LISTEN_PORT = "SNMPTrapListenPort";

    SNMPProtocolManager::SNMPProtocolManager()
    {
		SNMPGlobalHelper::setSNMPppDebugLevel();
		m_deviceWorkerPool = new WorkerPool();
		m_trapReceiver = new TrapReceiver(this);
	}
    

    SNMPProtocolManager::~SNMPProtocolManager()
    {
		delete m_deviceWorkerPool;
		m_deviceWorkerPool = 0;

		delete m_trapReceiver;
		m_trapReceiver = 0;
    }

	IEntity* SNMPProtocolManager::createEntity(TA_Base_Core::IEntityDataPtr entityData)
	{
		IEntity* pEntity = 0;
		if (TA_Base_Core::SNMPDeviceEntityData::getStaticType() == entityData->getType())
		{
			TA_Base_Core::SNMPDeviceEntityDataPtr pEntityDataPtr = boost::dynamic_pointer_cast<TA_Base_Core::SNMPDeviceEntityData>(entityData);
			TA_Base_Core::SNMPDeviceEntityData* pEntityData = pEntityDataPtr.get();
	        
			TA_ASSERT( NULL != pEntityData, "SNMPDeviceEntityData is NULL in createEntity" );
	        

			SNMPDevice* device = 0;
			device = new SNMPDevice(*this, pEntityData);

			if (0 != device)
			{
				addProtocolDevice(device);
				pEntity = device;

				SNMPDataUpdater* updater = new SNMPDataUpdater(pEntityData->getName());
				addProtocolUpdater(updater);
				addDeviceUpdaterPair(device, updater);

				SNMPOutputSender* sender = new SNMPOutputSender(pEntityData->getName(), device);
				addProtocolSender(sender);

				m_deviceWorkerPool->queueTask(device);
			}
		}
		return pEntity;
	}

	IProtocolIOBase* SNMPProtocolManager::addDataPoint(DataPoint* dp)
	{
		IProtocolIOBase* pBase = 0;
		bool matched = false;
		for (int i = 0; 0 == pBase && i < m_devices.size(); ++i)
		{
			matched = SNMPGlobalHelper::isNameMatchedDeviceName(m_devices[i]->getEntityName(), dp->getDataPointName());
			if (matched)
			{
				SNMPDevice* pDevice = dynamic_cast<SNMPDevice*> (m_devices[i]);
				if (pDevice)
				{
					pDevice->addDeviceOID(dp->getDataPointAddress());
				}
			}
		}
		pBase = ProtocolManager::addDataPoint(dp);
		return pBase;
	}

	void SNMPProtocolManager::setOperationMode(TA_Base_Core::EOperationMode mode)
	{
		ProtocolManager::setOperationMode(mode);

		if (TA_Base_Core::Control == mode)
		{
			m_deviceWorkerPool->setServiceState(true);
			m_trapReceiver->setServiceState(true);
		}
		else if(TA_Base_Core::Monitor == mode)
		{
			m_deviceWorkerPool->setServiceState(false);
			m_trapReceiver->setServiceState(false);
		}
	}

	IProtocolDevice* SNMPProtocolManager::getOutputDevice(std::string senderName)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "ProtocolManager have not ProtocolDevice for name:%s", senderName.c_str());
		return 0;
	}

	void SNMPProtocolManager::updateMoreParas(const std::map<std::string, std::string>& paras)
	{
		std::map<std::string, std::string>::const_iterator it =  paras.find(SNMP_TRAP_LISTEN_PORT);
		if (paras.end() != it)
		{
			std::string trapPort = it->second;
			int port = atoi(trapPort.c_str());
			m_trapReceiver->updateTrapPort(port);
		}
	}

	bool SNMPProtocolManager::isEntityTypeMatchedStatic(const std::string& entityType)
	{
		if (TA_Base_Core::SNMPDeviceEntityData::getStaticType() == entityType)
		{
			return true;
		}
		return false;
	}

	bool SNMPProtocolManager::isEntityTypeMatched(const std::string& entityType)
	{
		return isEntityTypeMatchedStatic(entityType);
	}

	void SNMPProtocolManager::processTrapData(const std::string ip, IDeviceData* data)
	{
		ThreadGuard guard(m_dpProcessorLock);
		for (int i = 0; i < m_devices.size(); ++i)
		{
			IProtocolDevice* device = m_devices[i];
			if (ip == device->getEntityAddress())
			{
				this->processData(device, data);
			}
		}
	}
	
}
