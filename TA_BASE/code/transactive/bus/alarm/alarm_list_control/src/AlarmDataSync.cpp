// AlarmDataSync.cpp: implementation of the AlarmDataSync class.
//
//////////////////////////////////////////////////////////////////////

#include "AlarmDataSync.h"
#include "bus/alarm/alarm_list_control/src/WinMsgDef.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Bus
{
	AlarmDataSync::AlarmDataSync() : m_pAlarmStore(0), m_pDisplayIndex(0)
	{
		m_pDisplayIndex = new CSimpleDispMultiIndex();
		m_pDisplayIndex->registerUpdateObserver(this);
	}
	
	AlarmDataSync::~AlarmDataSync()
	{
		if (m_pDisplayIndex != NULL)
		{
			delete m_pDisplayIndex;
			m_pDisplayIndex = NULL;
		}
	}

	void AlarmDataSync::updateDisplay(NOTIFY_MGS msg)
	{
		// Notify AlarmList Control to refresh the display
		switch (msg)
		{
		case NOTIFY_REFRESH:
			PostMessage(m_hwndTarget, ALARMSTORE_REFRESH,NULL, NULL);
			break;
		case NOTIFY_RESTART:
			PostMessage(m_hwndTarget, AlARMSTORE_RESTART,NULL, NULL);
			break;
		case NOTIFY_REMAP:
			SendMessage(m_hwndTarget, AlARMSTORE_REMAP, NULL, NULL);
		case NOTIFY_UNMAP:
			SendMessage(m_hwndTarget, AlARMSTORE_UNMAP, NULL, NULL);
		}
	}

// 	void AlarmDataSync::applyDutyIndexFilter(AlarmFilterSet* pFilterSet)
// 	{
// 		CSimpleDispMultiIndex::getInstance()->applyDutyIndexFilter(pFilterSet);	
// 	}

	void AlarmDataSync::applyDutyIndexFilter(const Filter& filter, bool isDefault)
	{
		// We should call alarmstore setfilter
		if (m_pAlarmStore != NULL)
		{
			m_pAlarmStore->setFilter(filter, m_pDisplayIndex->getClientPos(), isDefault);			
		}
		
	}
	
	void AlarmDataSync::setDisplaySort(SORT_COLUMN sortType, bool bSortAccending)
	{
		m_pDisplayIndex->setDisplaySort(sortType, bSortAccending);
	}

	AlarmVectorWrap* AlarmDataSync::getDisplayData()
	{
		return m_pDisplayIndex->getDisplayData();
	}

	AlarmSelectionWrap* AlarmDataSync::getSelectionWrapper()
	{
		return m_pDisplayIndex->getSelectionWrapper();
	}
	
	TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo& AlarmDataSync::getIndexInfo()
	{
		m_pDisplayIndex->getIndexInfo(m_info);
		return m_info;
	}


	bool AlarmDataSync::isAlarmItemSelected(std::string strAlarmID)
	{

		return m_pDisplayIndex->isAlarmItemSelected(strAlarmID);
	}
	
	void AlarmDataSync::selectAlarmItem(unsigned long ulIndex)
	{
		m_pDisplayIndex->selectAlarmItem(ulIndex);
	}
	
	void AlarmDataSync::selectRangeAlarms(unsigned long ulSelBegin, unsigned long ulSelEnd)
	{
		m_pDisplayIndex->selectRangeAlarms(ulSelBegin, ulSelEnd);
	}

	void AlarmDataSync::unselectAlarmItem(unsigned long ulIndex)
	{
		m_pDisplayIndex->unselectAlarmItem(ulIndex);
	}
	
	void AlarmDataSync::unSelectAllAlarmItem()
	{
		m_pDisplayIndex->unSelectAllAlarmItem();
	}

	unsigned long AlarmDataSync::getSelectedAlarmsCount()
	{
		return m_pDisplayIndex->getSelectedAlarmsCount();
	}

	bool AlarmDataSync::isUpdating()
	{
		return m_pDisplayIndex->isUpdating();
	}

// 	// use to retrieve string attributes in AlarmDataItem
// 	std::string AlarmDataSync::getItemString(std::string strAlarmID, STRING_ATTR nSubIndex)
// 	{
// //		return CSimpleDispMultiIndex::getInstance()->getItemString(strAlarmID, nSubIndex);
// 		return std::string("");
// 	}
// 	
// 	// use to retrieve number attributes in AlarmDataItem
// 	unsigned long AlarmDataSync::getItemNumber(std::string strAlarmID, NUMBER_ATTR nSubIndex)
// 	{
// //		return CSimpleDispMultiIndex::getInstance()->getItemNumber(strAlarmID, nSubIndex);
// 		return 0;
// 	}

	void AlarmDataSync::setListDisplayInfo(ListDispInfo& listInfo)
	{
		m_pDisplayIndex->setListDisplayInfo(listInfo);
	}

	void AlarmDataSync::setStoreService(AlarmStore * pAlarmStore)
	{
		m_pAlarmStore = pAlarmStore;
	}

	void AlarmDataSync::setFreezeCacheRefresh(bool bFreezeCacheRefresh)
	{ 
		m_pDisplayIndex->setFreezeRefreshCache(bFreezeCacheRefresh); 
	}

	std::string AlarmDataSync::getAlarmID(unsigned long ulIndex)
	{
		return m_pDisplayIndex->getAlarmID(ulIndex);
	}
}

