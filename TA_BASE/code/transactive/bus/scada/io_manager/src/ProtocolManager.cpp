/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/ProtocolManager.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#include "ProtocolManager.h"
#include "core/synchronisation/src/ThreadGuard.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    ProtocolManager::ProtocolManager()
    {
        
    }

	ProtocolManager::~ProtocolManager()
	{
	}

	IEntity* ProtocolManager::createEntity(TA_Base_Core::IEntityDataPtr entityData)
    {
		return 0;
    }

	IProtocolIOBase* ProtocolManager::addDataPoint(DataPoint* dp)
	{
		IProtocolIOBase* pBase = 0;
		int i = 0, nSize = 0;
		bool added = false;

		if (dp->isWriteable())
		{
			nSize = m_protocolSenders.size();
			i =0;
			while (i < nSize && 0 == pBase)
			{
				added = m_protocolSenders[i]->addDataPoint(dp);
				if (added)
				{
					pBase = m_protocolSenders[i];
				}
				++i;
			}
		}
		else
		{
			nSize = m_protocolUpdaters.size();
			i =0;
			while (i < nSize && 0 == pBase)
			{
				added = m_protocolUpdaters[i]->addDataPoint(dp);
				if (added)
				{
					pBase = m_protocolUpdaters[i];
				}
				++i;
			}
		}
		return pBase;
	}

	void ProtocolManager::finishedAddDataPoint()
	{
		int i = 0, nSize = 0;

		nSize = m_protocolUpdaters.size();
		for (i = 0; i < nSize; ++i)
		{
			m_protocolUpdaters[i]->finishedAddDataPoint();
		}

		nSize = m_protocolSenders.size();
		for (i = 0; i < nSize; ++i)
		{
			m_protocolSenders[i]->finishedAddDataPoint();
		}
	}

	void ProtocolManager::setOperationMode(TA_Base_Core::EOperationMode mode)
    {
        for (int i = 0; i < m_devices.size(); ++i)
		{
			if (TA_Base_Core::Control == mode)
			{
				m_devices[i]->setToControlMode();
			}
			else if(TA_Base_Core::Monitor == mode)
			{
				m_devices[i]->setToMonitorMode();
			}
		}
    }

	void ProtocolManager::addProtocolDevice(IProtocolDevice* device)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		m_devices.push_back(device);
	}

	void ProtocolManager::addProtocolUpdater(ProtocolUpdater* updater)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		m_protocolUpdaters.push_back(updater);
	}

	void ProtocolManager::addProtocolSender(ProtocolSender* sender)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		m_protocolSenders.push_back(sender);
	}
	
	void ProtocolManager::addDeviceUpdaterPair(IProtocolDevice* device, ProtocolUpdater* updater)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		if (0 != device && 0 != updater)
		{
			m_deviceToUpdaters[device->getEntityKey()].push_back(updater);
		}
		else if(0 != device && 0 == updater)
		{
			DataUpdaterVector::const_iterator it = m_dataUpdaters.begin();
			while (m_dataUpdaters.end() != it)
			{
				m_deviceToUpdaters[device->getEntityKey()].push_back(*it);
				++it;
			}
		}
		else if(0 == device && 0 != updater)
		{
			std::vector<IProtocolDevice*>::const_iterator it = m_devices.begin();
			while (m_devices.end() != it)
			{
				m_deviceToUpdaters[(*it)->getEntityKey()].push_back(updater);
				++it;
			}
		}
		else
		{
			// log;
		}
	}

	void ProtocolManager::processData(IProtocolDevice* device, IDeviceData* data)
	{
		// if not primary device xxxx

		unsigned long key = device->getEntityKey();
		std::map<unsigned long, DataUpdaterVector>::const_iterator it = m_deviceToUpdaters.find(key);
		if (m_deviceToUpdaters.end() == it)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "failed to find updater to process this data for:%s",
					device->getEntityName().c_str());
		}
		else
		{
			DataUpdaterVector::const_iterator updater = it->second.begin();
			while (it->second.end() != updater)
			{
				(*updater)->submitDeviceData(data);
				++updater;
			}
		}
	}
}
