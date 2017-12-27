//////////////////////////////////////////////////////////////////////
//		@file		DutyAlarmManager.cpp
///		@author		zhilin,ouyang
///		@date		2011-4-24 11:22:41
///
///		@brief	    implementation of class DutyAlarmManager.               
///                 
//////////////////////////////////////////////////////////////////////
#include "DutyAlarmManager.h"
#include "bus/alarm/shared_alarm/src/SharedAlarm.h"

namespace TA_Base_Bus
{
	DutyAlarmManager* DutyAlarmManager::m_pInstance = NULL;
	TA_Base_Core::NonReEntrantThreadLockable DutyAlarmManager::m_singletonLock;

	DutyAlarmManager::DutyAlarmManager() : m_pSharedAlarm(0)
	{
		m_pSharedAlarm = &SharedAlarmAccessFactory::getInstance();
	}

	DutyAlarmManager::~DutyAlarmManager()
	{
		if ( NULL != m_pSharedAlarm )
		{
			SharedAlarmAccessFactory::getInstance().unmapSharedMem();
			SharedAlarmAccessFactory::freeInstance();
			m_pSharedAlarm = NULL;
		}
	}

	DutyAlarmManager& DutyAlarmManager::getInstance()
	{
		TA_Base_Core::ThreadGuard guard(m_singletonLock);
		
		if (NULL == m_pInstance)
		{
			m_pInstance = new DutyAlarmManager();
		}
		
		return *m_pInstance;
	}

	void DutyAlarmManager::freeInstance()
	{
		TA_Base_Core::ThreadGuard guard(m_singletonLock);		
		if (NULL != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

	void DutyAlarmManager::init(bool alwaysRecreateMapfile /*= false*/)
	{
		m_pSharedAlarm->init(alwaysRecreateMapfile);
	}

	bool DutyAlarmManager::insertAlarm(AlarmDataItem* alarmData)
	{
// 		TA_Base_Core::ThreadGuard guard(m_lock);
		return m_pSharedAlarm->insertAlarm(alarmData);
	}

	bool DutyAlarmManager::deleteAlarm(std::string alarmID)
	{
//		TA_Base_Core::ThreadGuard guard(m_lock);
		return m_pSharedAlarm->deleteAlarm(alarmID.c_str());
	}

	bool DutyAlarmManager::updateAlarm(AlarmDataItem* alarmData)
	{
// 		TA_Base_Core::ThreadGuard guard(m_lock);

		return m_pSharedAlarm->updateAlarm(alarmData);
	}

	unsigned long DutyAlarmManager::removeLocationAlarms(unsigned long locationKey, unsigned long dataSource /*= ISCS_SUBSYSTEM_ID*/)
	{
		if ( NULL != m_pSharedAlarm )
		{
			return m_pSharedAlarm->removeLocationAlarms(locationKey, dataSource);
		}
		return false;

	}

	void DutyAlarmManager::deleteAllAlarmItems()
	{
//		TA_Base_Core::ThreadGuard guard(m_lock);
		if ( NULL != m_pSharedAlarm )
		{
			m_pSharedAlarm->clean();
		}
	}
	
	void DutyAlarmManager::notifyClientRemap()
	{
		if ( NULL != m_pSharedAlarm )
		{
			m_pSharedAlarm->notifyClientRemap();
		}
	}
		
	void DutyAlarmManager::buildIndexWithFilter(TA_Base_Bus::AlarmFilterSet* pAlarmFilterSet, fp vfp,int cs_pos,bool isDefault)
	{
		if ( NULL != m_pSharedAlarm )
		{
			m_pSharedAlarm->buildIndexWithFilter(pAlarmFilterSet, vfp, cs_pos, isDefault);
		}
	}

	bool DutyAlarmManager::insertAlarmlist(std::vector<AlarmDataItem*>& alarmlist)
	{
		if ( NULL != m_pSharedAlarm )
		{
			return m_pSharedAlarm->insertAlarmlist(alarmlist);
		}
		return false;
	}
	
	bool DutyAlarmManager::aliveCheck()
	{
		if ( NULL != m_pSharedAlarm )
		{
			return m_pSharedAlarm->aliveCheck();
		}
		return false;
	}
	
	bool DutyAlarmManager::isUpdating()
	{
		if ( NULL != m_pSharedAlarm )
		{
			return m_pSharedAlarm->isUpdating();
		}
		return false;
	}
	
	int DutyAlarmManager::healthMonitor()
	{
		if ( NULL != m_pSharedAlarm )
		{
			return m_pSharedAlarm->healthMonitor();
		}
		return -1;
	}

	unsigned long DutyAlarmManager::removeATSAlarms()
	{
		if (NULL != m_pSharedAlarm)
		{
			return m_pSharedAlarm->removeATSAlarms();
		}
		return 0;
	}

	unsigned long DutyAlarmManager::removeISCSAlarms()
	{
		if (NULL != m_pSharedAlarm)
		{
			return m_pSharedAlarm->removeISCSAlarms();
		}
		return 0;
	}

	//++ Added for DutyChange
	long DutyAlarmManager::removeLocSubSystemDutyAlarms(unsigned long ulLocationKey, std::list<unsigned long>& subSystemKeyList, unsigned long dataSource /*=ISCS_SUBSYSTEM_ID*/)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		if (NULL != m_pSharedAlarm)
		{
			return m_pSharedAlarm->removeLocSubSystemDutyAlarms(ulLocationKey, subSystemKeyList, dataSource);
		}
		return 0;
	}

	AlarmDataItem* DutyAlarmManager::getAlarm(std::string alarmID)
	{
		if (NULL != m_pSharedAlarm)
		{
			return m_pSharedAlarm->getAlarm(alarmID);
		}
		return NULL;
	}

	void DutyAlarmManager::setLatestFASAlarm(unsigned long entityKey)
	{
		if (NULL != m_pSharedAlarm)
		{
			m_pSharedAlarm->setLatestFASAlarm(entityKey);
		}
		
	}

	unsigned long DutyAlarmManager::getLatestFASAlarm()
	{
		if (NULL != m_pSharedAlarm)
		{
			return m_pSharedAlarm->getLatestFASAlarm();
		}
		return 0;
	}

} // namespace TA_Base_bus
