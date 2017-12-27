/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/io_manager/src/IOManager.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#include "IOManager.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    IOManager::IOManager()
    {
		m_dpProcessor = new DataPointQueueProcessor();
		m_commandSender	= new OutputCommandSender();        
    }

	IOManager::~IOManager()
	{
		delete m_dpProcessor;
		m_dpProcessor = 0;

		delete m_commandSender;
		m_commandSender	= 0;
	}

	IEntity* IOManager::createEntity(TA_Base_Core::IEntityDataPtr entityData)
    {
		IEntity* entity = 0;
		unsigned long groupId = 0;
		groupId = getEntityGroup(entityData);

		ProtocolManagerList* pManagers = 0;

		std::map<unsigned long, ProtocolManagerList*>::const_iterator it = m_groupManagersMap.find(groupId);
		if (m_groupManagersMap.end() != it)
		{
			pManagers = it->second;
		}
		else {
			pManagers = new ProtocolManagerList;
			m_groupManagersMap[groupId] = pManagers;
		}

		IProtocolManager* pDevManager = 0;
		std::string type = entityData->getType();
		
		for (int i = 0; i < pManagers->size(); ++i)
		{
			if (pManagers->at(i)->isEntityTypeMatched(type))
			{
				pDevManager = pManagers->at(i);
				break;
			}
		}
		if (0 == pDevManager)
		{
			pDevManager = createProtocolManager(type);
			if (0 != pDevManager)
			{
				pManagers->push_back(pDevManager);
				m_protocolManagers.push_back(pDevManager);
			}
		}
		if (0 != pDevManager)
		{
			IEntity* pDevice = pDevManager->createEntity(entityData);
			m_entitys[entityData->getKey()] = pDevice;
		}

		return entity;
    }

	void IOManager::addDataPoint(DataPoint* dp)
    {
		//if there is a directed db map, using map first.

		IProtocolIOBase* context = 0;
		std::vector<IProtocolManager*>::iterator it = m_protocolManagers.begin();
		while (m_protocolManagers.end() != it)
		{
			context = (*it)->addDataPoint(dp);
			if (context)
			{
				m_dpToIOBaseMap[dp->getEntityKey()] = context;
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "datapoint:%s added to %s",
					dp->getDataPointName().c_str(), context->getName().c_str());
				break;
			}
			++it;
		}
		if (0 == context)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "failed to add datapoint:%s to protocol manager", dp->getDataPointName().c_str());
		}
    }

	void IOManager::finishedAddDataPoint()
    {
		std::vector<IProtocolManager*>::iterator it_pm = m_protocolManagers.begin();
		while (m_protocolManagers.end() != it_pm)
		{
			(*it_pm)->finishedAddDataPoint();
			++it_pm;
		}
		std::map<unsigned long, IProtocolIOBase*>::const_iterator it = m_dpToIOBaseMap.begin();
		while (m_dpToIOBaseMap.end() != it)
		{
			it->second->registerDPProcessor(m_dpProcessor);
			++it;
		}

		if (m_commandSender)
		{
			m_commandSender->setDataPointSenderMap(m_dpToIOBaseMap);
		}
    }

	void IOManager::setOperationMode(TA_Base_Core::EOperationMode mode)
    {
        std::vector<IProtocolManager*>::const_iterator it = m_protocolManagers.begin();
		while (m_protocolManagers.end() != it)
		{
			(*it)->setOperationMode(mode);
			++it;
		}
    }

	void IOManager::updateMoreParas(std::map<std::string, std::string>& paras)
	{
		std::vector<IProtocolManager*>::const_iterator it = m_protocolManagers.begin();
		while (m_protocolManagers.end() != it)
		{
			(*it)->updateMoreParas(paras);
			++it;
		}
	}
}
