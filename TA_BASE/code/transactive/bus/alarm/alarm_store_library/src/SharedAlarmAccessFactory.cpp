/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_LM/CODE_TA_BASE_4669_T01271300/transactive/core/data_access_interface/src/ActionAccessFactory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2008/10/06 18:41:13 $
  * Last modified by:  $Author: coreteam $
  * 
  * ActionAccessFactory is a singleton that is used to retrieve action objects either from the
  * database or newly created. All action objects returned will adhear to the IAction interface.
  * Data is primarily retrieved from the SE_ACTION table
  *
  */


#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "SharedAlarmAccessFactory.h"
#include "core/synchronisation/src/ThreadGuard.h"
using namespace I_ALARM_PROVIDER;

namespace TA_Base_Bus
{

    SharedAlarmAccessFactory* SharedAlarmAccessFactory::m_instance = 0;
	TA_Base_Core::NonReEntrantThreadLockable SharedAlarmAccessFactory::m_singletonLock;

    SharedAlarmAccessFactory& SharedAlarmAccessFactory::getInstance()
    {
		TA_Base_Core::ThreadGuard guard(m_singletonLock);
        if( m_instance == 0 )
        {
            m_instance = new SharedAlarmAccessFactory();
        }
        return *m_instance;
    }

	void SharedAlarmAccessFactory::freeInstance()
	{
		TA_Base_Core::ThreadGuard guard(m_singletonLock);
		if (NULL != m_instance)
		{
			delete m_instance;
			m_instance = NULL;
		}
	}

    void SharedAlarmAccessFactory::init(bool alwaysRecreateMapfile /*= false*/)
    {	
        m_sa->init(alwaysRecreateMapfile);
    }

    void SharedAlarmAccessFactory::updateIndexInfo(TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo& info, int cs_pos)
    {
        TA_Base_Core::ThreadGuard guard(m_lock);
        
        m_sa->updateIndexInfo(info, cs_pos);
    }
    
    int SharedAlarmAccessFactory::addClient(const char* server, const char* client, const char* mapEvName, const char* unmapEvName, int& csPos, bool mayExist /*= true*/)
    {
        return m_sa->addClient(server, client, mapEvName, unmapEvName, csPos, mayExist);
    }
    
    bool SharedAlarmAccessFactory::removeClient(const char* server, const char* client, const char* mapEvName, const char* unmapEvName)
    {
        return m_sa->removeClient(server, client, mapEvName, unmapEvName);
    }
    
	bool SharedAlarmAccessFactory::RemoveAlarm(const char* alarmID)
    {
        TA_Base_Core::ThreadGuard guard(m_lock);
	
        return m_sa->deleteAlarm(std::string(alarmID));
    }
    
	bool SharedAlarmAccessFactory::isUpdating()
	{
		if (m_sa->isUpdating()) 
		{		    
			return true;
		}
		return false;
	}	
    
    void SharedAlarmAccessFactory::buildIndexWithFilter(TA_Base_Bus::AlarmFilterSet* pAlarmFilterSet, fp vfp,int cs_pos,bool isDefault)
    {
        TA_Base_Core::ThreadGuard guard(m_lock);
        
        m_sa->buildIndexWithFilter(pAlarmFilterSet, vfp, cs_pos, isDefault);
    }
    
    void SharedAlarmAccessFactory::clean()
    {
        TA_Base_Core::ThreadGuard guard(m_lock);
        
        m_sa->clean();
    }

	int SharedAlarmAccessFactory::healthMonitor()
	{        
        return m_sa->healthMonitor();
	}

	unsigned long SharedAlarmAccessFactory::removeLocationAlarms(unsigned long locationKey, unsigned long dataSource /*=ISCS_SUBSYSTEM_ID*/)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);

		return m_sa->removeLocationAlarms(locationKey, dataSource);
	}

	unsigned long SharedAlarmAccessFactory::removeATSAlarms()
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		
		return m_sa->removeATSAlarms();
	}
	unsigned long SharedAlarmAccessFactory::removeISCSAlarms()
	{
		TA_Base_Core::ThreadGuard guard(m_lock);

		return m_sa->removeISCSAlarms();
	}
	bool SharedAlarmAccessFactory::aliveCheck()
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		return m_sa->checkSharedMemory();
	}

	bool SharedAlarmAccessFactory::unmapSharedMem()
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		return m_sa->unmapSharedMem();
	}

	bool SharedAlarmAccessFactory::remapSharedMem()
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		return m_sa->remapSharedMem();
	}

	void SharedAlarmAccessFactory::notifyClientRemap()
	{
       m_sa->notifyClientRemap();
	}
	//add by luohr
	void SharedAlarmAccessFactory::insertOrUpdateAlarm(std::vector<AlarmDetail*>& alarmDetails, bool isPassPreFilter)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);        
		m_sa->insertOrUpdateAlarm(alarmDetails,isPassPreFilter);
	}

	//++ Added for the new AlarmStore Modification
	bool SharedAlarmAccessFactory::insertAlarm(AlarmDataItem* alarmUpdate)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		return m_sa->insertAlarm(alarmUpdate);
	}

	bool SharedAlarmAccessFactory::deleteAlarm(std::string alarmID)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		return m_sa->deleteAlarm(alarmID);
	}

	bool SharedAlarmAccessFactory::updateAlarm(AlarmDataItem* alarmData)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		return m_sa->updateAlarm(alarmData);
	}

	bool SharedAlarmAccessFactory::insertAlarmlist(std::vector<AlarmDataItem*>& alarmlist)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		return m_sa->insertAlarmlist(alarmlist);
	}

	//++ Added for DutyChange
	long SharedAlarmAccessFactory::removeLocSubSystemDutyAlarms(unsigned long ulLocationKey, std::list<unsigned long>& subSystemKeyList, unsigned long dataSource /*=ISCS_SUBSYSTEM_ID*/)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		return m_sa->removeLocSubSystemDutyAlarms(ulLocationKey, subSystemKeyList, dataSource);

	}
	
	AlarmDataItem* SharedAlarmAccessFactory::getAlarm(std::string alarmID)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		return m_sa->getAlarm(alarmID);
	}


	// additional methods for display modification
	//void SharedAlarmAccessFactory::getDisplayData(std::vector<AlarmDataItem*>& pDisplayData, SORT_COLUMN nCol, int nPos, bool bSortAccend, ULONG ulIndexBegin, ULONG ulCacheSize)
	void SharedAlarmAccessFactory::getDisplayData(TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo& info, std::vector<AlarmDataItem*>& pDisplayData, SORT_COLUMN nCol, int nPos, bool bSortAccend, ULONG ulIndexBegin, ULONG ulCacheSize, bool bUpdateIndexInfo)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		m_sa->getDisplayData(info, pDisplayData, nCol, nPos, bSortAccend, ulIndexBegin, ulCacheSize, bUpdateIndexInfo);
	}

	AlarmDataItem* SharedAlarmAccessFactory::getAlarmItemByID(std::string strAlarmID)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		return m_sa->getAlarmItemByID(strAlarmID);
	}

	AlarmDataItem* SharedAlarmAccessFactory::getAlarmItemByIndex(ULONG ulIndex, SORT_COLUMN nCol, int csPos, bool bSortAccend)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		return m_sa->getAlarmItemByIndex(ulIndex, nCol, csPos, bSortAccend);
	}

	std::vector<TA_Base_Core::AlarmModifyItem*> SharedAlarmAccessFactory::getAllModifyItems(SORT_COLUMN nCol, int csPos)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		return m_sa->getAllModifyItems(nCol, csPos);
	}

	long SharedAlarmAccessFactory::findItemPosition(std::string strAlarmID, SORT_COLUMN nCol, int nPos, bool bSortAccend)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		return m_sa->findItemPosition(strAlarmID, nCol, nPos, bSortAccend);
	}

	void SharedAlarmAccessFactory::getRangeAlarmID(std::set<std::string>& setAlarmIDs, ULONG ulBegin, ULONG ulEnd, SORT_COLUMN nCol, int nPos, bool bSortAccend)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		m_sa->getRangeAlarmID(setAlarmIDs, ulBegin, ulEnd, nCol, nPos, bSortAccend);
	}

	void SharedAlarmAccessFactory::getAvalanchedChildAlarms(int csPos, std::string avalanchedHeadID, std::vector<std::string>& vecChildAlarms)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		m_sa->getAvalanchedChildAlarms(csPos, avalanchedHeadID, vecChildAlarms);
	}

	std::vector<TA_Base_Core::AlarmModifyItem*> SharedAlarmAccessFactory::getAlarmModifyItem(std::set<std::string >& vecAlarmIDs)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		return m_sa->getAlarmModifyItem(vecAlarmIDs);
	}

	void SharedAlarmAccessFactory::setLatestFASAlarm(unsigned long entityKey)
	{
		m_sa->setLatestFASAlarm(entityKey);
	}

	unsigned long SharedAlarmAccessFactory::getLatestFASAlarm()
	{
		return m_sa->getLatestFASAlarm();
	}

 

} // closes TA_Base_Bus
