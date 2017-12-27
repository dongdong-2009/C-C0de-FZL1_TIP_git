// AlarmList.cpp: implementation of the CAlarmList class.
//
//////////////////////////////////////////////////////////////////////

#include "AlarmVectorWrap.h"
#include "SimpleDispMultiIndex.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Bus
{
	AlarmVectorWrap::AlarmVectorWrap(CSimpleDispMultiIndex * pParentContainer, TA_Base_Core::NonReEntrantThreadLockable& threadLock, std::vector<AlarmDataItem*>& displayVec) 
					: m_ThreadLock(threadLock)
					, m_displayVec(displayVec)
					, m_ulLastIndex(-1)
					, m_cacheDataItem(0)
	{
		m_pParentContainer = pParentContainer;
	}
	
	AlarmVectorWrap::~AlarmVectorWrap()
	{
		_clearCache();
	}


	// The calling function is responsible for freeing the memory allocated when function is called
	AlarmDataItem* AlarmVectorWrap::getItemCopyAt(unsigned long nIndex)
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);
		AlarmDataItem* pAlarmItem = NULL;
		
		if (m_displayVec.size() != 0)
		{
			try
			{
				AlarmDataItem * pTemp = m_pParentContainer->_getItemCacheByIndex(nIndex);
				if (pTemp != NULL)
				{
					pAlarmItem = new AlarmDataItem(*pTemp);
				}
				else
				{
					// we must query it from the shared alarm
					// ther return item is an new instance of AlarmDataItem
					pAlarmItem = m_pParentContainer->_getItemByIndex(nIndex);
				}
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occurred while trying to locate the last selected item.");
			}
		}
		
		return pAlarmItem;
	}

	// The calling function is responsible for freeing the memory allocated when function is called
	AlarmDataItem* AlarmVectorWrap::getItemCopyID(std::string strAlarmID)
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

	// use to retrieve string attributes in AlarmDataItem
	std::string AlarmVectorWrap::getItemString(unsigned long nIndex, STRING_ATTR nSubIndex)
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);

		try
		{
			if (m_ulLastIndex != nIndex)
			{			
				//TA_Base_Core::ThreadGuard guard(m_pParentContainer->m_ThreadLock);
				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Index Change: old = %ld, new = %ld, sub Index = %ld", m_ulLastIndex, nIndex, nSubIndex);

				// Deallocate the cache item and reset the lastIndex accessed to -1
				_clearCache();
				
				m_cacheDataItem = m_pParentContainer->_getItemCacheByIndex(nIndex, true);
				
				if (m_cacheDataItem == NULL)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to retrieve Alarm from cache %ld", nIndex);

					// we must query it from the shared alarm
					// ther return item is an new instance of AlarmDataItem
					// m_pParentContainer->_getItemByIndex(nIndex);
				}
			}

			if (m_cacheDataItem != NULL)
			{
				std::string strRetVal = "";
				m_ulLastIndex = nIndex;
				m_pParentContainer->_getStringAttribute(m_cacheDataItem, nSubIndex, strRetVal);
				return strRetVal;
			}
			else
			{
				m_ulLastIndex = -1;
				return std::string("");
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occurred while trying to retrieve AlarmItem details.");
		}

		return std::string("");
	}
	
	// use to retrieve number attributes in AlarmDataItem
	unsigned long AlarmVectorWrap::getItemNumber(unsigned long nIndex, NUMBER_ATTR nSubIndex)
	{		
		//TA_Base_Core::ThreadGuard cacheGuard(m_CacheLock);
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);
		try
		{
			if (m_ulLastIndex != nIndex)
			{			
				//TA_Base_Core::ThreadGuard guard(m_pParentContainer->m_ThreadLock);
				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Index Change: old = %ld, new = %ld, sub Index = %ld", m_ulLastIndex, nIndex, nSubIndex);

				// Deallocate the cache item and reset the lastIndex accessed to -1
				_clearCache();
				
				m_cacheDataItem = m_pParentContainer->_getItemCacheByIndex(nIndex, true);
				
				if (m_cacheDataItem == NULL)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Unable to retrieve Alarm from index %ld", nIndex);
				}
			}	

			if (m_cacheDataItem != NULL)
			{
				unsigned long ulRetVal = 0;
				m_ulLastIndex = nIndex;
				m_pParentContainer->_getNumberAttribute(m_cacheDataItem, nSubIndex, ulRetVal);
				return ulRetVal;
			}
			else
			{
				m_ulLastIndex = -1;
				return 0;
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occurred while trying to retrieve AlarmItem details.");
		}

		return 0;
	}	

	unsigned long AlarmVectorWrap::findItemPos(std::string strAlarmID)
	{
		TA_Base_Core::ThreadGuard guard(m_pParentContainer->m_ThreadLock);

		if(m_displayVec.size() == 0)
			return -1;
		
		// Verify that the selected alarm is not empty
		if (strAlarmID.empty())
		{
			return -1;
		}
		
		try
		{
			// try to find the alarm in the cache items
			DisplayVecT::iterator itFind = m_displayVec.begin();
			ULONG ulIndex = m_pParentContainer->m_ulCacheBegin;
			for (itFind; itFind != m_displayVec.end(); itFind++)
			{
				if (lstrcmpi(strAlarmID.c_str(), (*itFind)->alarmID.c_str()) == 0)
				{
					return ulIndex;
				}
				ulIndex++;
			}

			// if we reach this part meaning the alarm item were looking is not in cache items
			// we will try to look for the position of the item in the share memory directly.
			return m_pParentContainer->_findItemPosition(strAlarmID);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occured while trying to locate the last selected item.");
		}
		
		return -1; // Item was not found, alarm might have been closed
	}

	unsigned long AlarmVectorWrap::getSize()
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);

		unsigned long lRes = 0;		
		lRes = m_pParentContainer->m_indexInfo.totalAlarms;

		return lRes;
	}

// 	// Set the sorting order // by default its true
// 	void AlarmVectorWrap::setSortAccending(bool bSortAccending)
// 	{
// 		TA_Base_Core::ThreadGuard guard(m_ThreadLock);
// 
// 		m_pParentContainer->m_bSortAccending = bSortAccending;
// 		m_pParentContainer->m_bRefreshCache = true;
// 	}

	void AlarmVectorWrap::_clearCache()
	{
		if (m_cacheDataItem != NULL)
		{
			//delete m_cacheDataItem;
			m_cacheDataItem = NULL;
			m_ulLastIndex = -1;
		}
	}
}
