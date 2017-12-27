// AlarmSelectionWrap.cpp: implementation of the AlarmSelectionWrap class.
//
//////////////////////////////////////////////////////////////////////

#include "AlarmSelectionWrap.h"
#include "SimpleDispMultiIndex.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Bus
{
	AlarmSelectionWrap::AlarmSelectionWrap(CSimpleDispMultiIndex* pParent, TA_Base_Core::NonReEntrantThreadLockable& threadLock, std::set<std::string>& selectedAlarms)
						: m_ThreadLock(threadLock)
						, m_SelectedAlarms(selectedAlarms)
	{
		m_pParentContainer = pParent;	
	}
	
	AlarmSelectionWrap::~AlarmSelectionWrap()
	{
		
	}

	// All alarm items converted into AlarmModifyItems
	// The user is responsible for releasing the memory
	std::vector<TA_Base_Core::AlarmModifyItem*> AlarmSelectionWrap::getAllAlarmModifyItem()
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);
		std::vector<TA_Base_Core::AlarmModifyItem*> allAlarms;
		
		try
		{
			m_pParentContainer->_getAllAlarmModifyItem(allAlarms);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occurred while trying to create AlarmModifyItem");
		}
		
		return allAlarms;
	}
	
	// The user is responsible for releasing the memory
	std::vector<TA_Base_Core::AlarmModifyItem*> AlarmSelectionWrap::getSelAlarmModifyItems()
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);
		std::vector<TA_Base_Core::AlarmModifyItem*> selectedAlarms;
		
		try
		{
			
			selectedAlarms = m_pParentContainer->_getAlarmModifyItem(m_SelectedAlarms);
// 			IndexSel::iterator itr = m_SelectedAlarms.begin();
// 			for (itr; itr != m_SelectedAlarms.end(); itr++)
// 			{
// 				TA_Base_Core::AlarmModifyItem* alarmModifyItem = NULL;
// 				alarmModifyItem = m_pParentContainer->_getAlarmModifyItem(*itr);
// 				if (alarmModifyItem != NULL)
// 				{
// 					selectedAlarms.push_back(alarmModifyItem);
// 				}
// 			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occurred while trying to create AlarmModifyItem");
		}
		
		return selectedAlarms;	
	}
	
	std::vector<std::string> AlarmSelectionWrap::getAllVisibleAlarms()
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);
		
		std::vector<std::string> vectSelectedAlarms;
		
		m_pParentContainer->_getAllVisibleAlarms(vectSelectedAlarms);
		
		return vectSelectedAlarms;
	}
	
	std::vector<std::string> AlarmSelectionWrap::getSelectedAlarms()
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);
		std::vector<std::string> vectSelectedAlarms;

		std::copy(m_SelectedAlarms.begin(), m_SelectedAlarms.end(), std::back_inserter(vectSelectedAlarms));
		
		return vectSelectedAlarms;
	}
	
	// The user is responsible for releasing the memory
	TA_Base_Core::AlarmModifyItem* AlarmSelectionWrap::getAlarmModifyItem(std::string strAlarmID)
	{		
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);	
		TA_Base_Core::AlarmModifyItem* pAlarmModifyItem = NULL;
		
		std::vector<TA_Base_Core::AlarmModifyItem*> alarmModifyItems;
		std::set<std::string > vecStringID;
		vecStringID.insert(strAlarmID);

		alarmModifyItems = m_pParentContainer->_getAlarmModifyItem(vecStringID);
			
		if (alarmModifyItems.size() != 0)
		{
			pAlarmModifyItem = alarmModifyItems[0];
		}

		return pAlarmModifyItem;
	}
	
	// use to retrieve string attributes in AlarmDataItem
	std::string AlarmSelectionWrap::getItemString(std::string strAlarmID, STRING_ATTR nSubIndex)
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);
		AlarmDataItem* pAlarmItem = NULL;
		
		std::string strResult = "";
		try
		{
			bool bFoundInCache = false;
			
			pAlarmItem = m_pParentContainer->_getItemCacheByID(strAlarmID);

			if (pAlarmItem)
				bFoundInCache = true;
			
// 			DisplayVecT::iterator itr = m_pParentContainer->m_displayVec.begin();
// 			for (itr; itr != m_pParentContainer->m_displayVec.end(); itr++)
// 			{
// 				if (lstrcmpi((*itr)->alarmID.c_str(), strAlarmID.c_str()) == 0)
// 				{
// 					pAlarmItem = (*itr);
// 					bFoundInCache = true;
// 				}
// 			}

			if (bFoundInCache == false)
			{
				pAlarmItem = m_pParentContainer->_getItemByID(strAlarmID);
			}

			if (pAlarmItem)
			{
				m_pParentContainer->_getStringAttribute(pAlarmItem, nSubIndex, strResult);
				if (bFoundInCache == false)
				{
					delete pAlarmItem;
					pAlarmItem = NULL;
				}
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occured while trying to retrieve alarm details.");
		}

		return strResult;
	}
	
	// use to retrieve number attributes in AlarmDataItem
	unsigned long AlarmSelectionWrap::getItemNumber(std::string strAlarmID, NUMBER_ATTR nSubIndex)
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);
		AlarmDataItem* pAlarmItem = NULL;
	
		unsigned long ulResult = 0;
		try
		{
			bool bFoundInCache = false;
			pAlarmItem = m_pParentContainer->_getItemCacheByID(strAlarmID);
			
			if (pAlarmItem)
				bFoundInCache = true;

// 			DisplayVecT::iterator itr = m_pParentContainer->m_displayVec.begin();
// 			for (itr; itr != m_pParentContainer->m_displayVec.end(); itr++)
// 			{
// 				if (lstrcmpi((*itr)->alarmID.c_str(), strAlarmID.c_str()) == 0)
// 				{
// 					pAlarmItem = (*itr);
// 					bFoundInCache = true;
// 				}
// 			}
			if (bFoundInCache == false)
			{
				pAlarmItem = m_pParentContainer->_getItemByID(strAlarmID);
			}
			
			if (pAlarmItem)
			{
				m_pParentContainer->_getNumberAttribute(pAlarmItem, nSubIndex, ulResult);
				if (bFoundInCache == false)
				{
					delete pAlarmItem;
					pAlarmItem = NULL;
				}
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occured while trying to retrieve alarm details.");
		}

		return ulResult;
	}

	AlarmDataItem* AlarmSelectionWrap::getItemCopyID(std::string strAlarmID)
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);
		AlarmDataItem* pAlarmItem = NULL;
		AlarmDataItem* pTemp = NULL;
		// The returned item is a new instance of AlarmDataItem

		pTemp = m_pParentContainer->_getItemCacheByID(strAlarmID);
		if (pTemp)
			pAlarmItem = new AlarmDataItem(*pTemp);
		else
			pAlarmItem = m_pParentContainer->_getItemByID(strAlarmID);
		
		return pAlarmItem;
	}

	void AlarmSelectionWrap::getAvalanchedChildAlarms(std::string avalanchedHeadID, std::vector<std::string>& vecChildAlarms)
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);		
		m_pParentContainer->_getAvalanchedChildAlarms(avalanchedHeadID, vecChildAlarms);
	}

	bool AlarmSelectionWrap::isAlarmsVisible()
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);
		return (m_pParentContainer->m_indexInfo.totalAlarms > 0)?true : false;
	}
}

