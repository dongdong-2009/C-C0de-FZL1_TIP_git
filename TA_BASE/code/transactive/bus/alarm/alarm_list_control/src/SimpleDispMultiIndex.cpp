// SimpleDispMultiIndex.cpp: implementation of the CSimpleDispMultiIndex class.
//
//////////////////////////////////////////////////////////////////////

#include "SimpleDispMultiIndex.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"
//#include "bus/alarm/shared_alarm/src/ActionFlagGuard.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Bus
{
	/************************************************************************/
	/* Synchronization Methods                                              */
	/************************************************************************/
	CSimpleDispMultiIndex::CSimpleDispMultiIndex(ACE_Thread_Manager *thr_mgr, int syn_timeout):
		ACE_Task_Base(thr_mgr), 
		m_waitTime(syn_timeout),
		m_target(NULL),
		m_canNotify(false), 
		m_sharedMemsyn(NULL),
		m_dutyAlarmVersion(0),
		m_pSharedAlarm(0),
		m_pUpdateObserver(0),
		m_pDisplayVectWrapper(0),
		m_pSelectionWrapper(0),
		m_bIsUpdating(false),
		m_bRefreshCache(false),
		m_bFreezeRefreshCache(false),
		m_bInitialize(false),
		m_ulCacheBegin(0),
		m_ulCacheEnd(0),
		m_ulSecCacheBegin(0),
		m_ulSecCacheEnd(0)
		
	{
		std::string entity_name = "Global\\";
		entity_name += TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
		m_servername = entity_name + "s";
		m_clientname = entity_name + "c";
		m_server = CreateEvent(NULL, FALSE, FALSE, m_servername.c_str());
		m_client = CreateEvent(NULL, FALSE, FALSE, m_clientname.c_str());
		
		//zyr++: for map/unmap shared memory
		m_remapEvName = entity_name + "_remapEvName";
		m_unmapEvName = entity_name + "_unmapEvName";
		m_remapMemEv = CreateEvent(NULL, FALSE, FALSE, m_remapEvName.c_str());
		m_unmapMemEv = CreateEvent(NULL, FALSE, FALSE, m_unmapEvName.c_str());
		
 		m_csPos=-1;  //give a -1, means we dont' get any index position now.
		
		m_shutDownEv = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_rebuildEvent = CreateEvent(NULL,FALSE,FALSE, REBUILDEVENTNAME);

		m_pSharedAlarm = &SharedAlarmAccessFactory::getInstance();
		
		m_indexInfo.earliestSourceTime.millitm = 0;
		m_indexInfo.earliestSourceTime.time = 0;
		m_indexInfo.latestSourceTime.millitm = 0;
		m_indexInfo.latestSourceTime.time = 0;
		m_indexInfo.exceededMaxDisplay = false;
		m_indexInfo.highestSeverity = 0;
		m_indexInfo.isLoadingAlarms = false;
		m_indexInfo.totalAlarms = 0;
		m_indexInfo.unackAlarms = 0;

	}
	
	CSimpleDispMultiIndex::~CSimpleDispMultiIndex()
	{
		if (m_pDisplayVectWrapper != NULL)
		{
			delete m_pDisplayVectWrapper;
			m_pDisplayVectWrapper = NULL;
		}

		if (m_pSelectionWrapper != NULL)
		{
			delete m_pSelectionWrapper;
			m_pSelectionWrapper = NULL;
		}

		shutdown();
		thr_mgr_->wait();
		m_pSharedAlarm->removeClient(m_servername.c_str(), m_clientname.c_str(), m_remapEvName.c_str(), m_unmapEvName.c_str());
		CloseHandle(m_server);
		CloseHandle(m_client);

		if (m_pSharedAlarm != NULL)
		{
			//delete m_pSharedAlarm;
			SharedAlarmAccessFactory::freeInstance();
			m_pSharedAlarm = NULL;
		}

		//zyr++: for map/unmap shared memory
		if(m_sharedMemsyn != NULL)
		{
			delete m_sharedMemsyn;
			m_sharedMemsyn = NULL;
		}
		
		CloseHandle(m_remapMemEv);
		CloseHandle(m_unmapMemEv);
	}

	void CSimpleDispMultiIndex::notifyAlarmStoreResart()
	{
		m_bInitialize = true;
		notifyUpdate(IAlarmDisplayUpdate::NOTIFY_RESTART);
	}

	void CSimpleDispMultiIndex::notifyUnmapSharedMem()
	{
		{	// Reset all data inside this container
			TA_Base_Core::ThreadGuard guard(m_ThreadLock);
			_cleanInternals();
		}
		m_bInitialize = false;
		notifyUpdate(IAlarmDisplayUpdate::NOTIFY_UNMAP);
	}

	int CSimpleDispMultiIndex::svc()
	{
		const DWORD evcount = 3;
		SYN_EVENT evs[evcount] = {m_server, m_rebuildEvent, m_shutDownEv};
		DWORD result;
		bool mayExist = true;
		m_sharedMemsyn = new CSimpleDispMultiIndex::SharedMemSync(m_remapMemEv, m_unmapMemEv, this);
		
		m_pSharedAlarm->addClient(m_servername.c_str(), m_clientname.c_str(), m_remapEvName.c_str(), m_unmapEvName.c_str(), m_csPos, mayExist);
		m_canNotify=true;
		
		// Initialize Index Information //
		// This is necessary because banner does not necessarily repaint its list control so 
		// we have to initialize index info here to give banner the right alarm information.
		m_pSharedAlarm->updateIndexInfo(m_indexInfo, m_csPos);
		m_bInitialize = true;
		notifyUpdate(IAlarmDisplayUpdate::NOTIFY_REFRESH);	
		while(thr_mgr_->testcancel (ACE_OS::thr_self ()) == 0)
		{
			result = WaitForMultipleObjects(evcount, evs, FALSE, INFINITE);
			if( WAIT_OBJECT_0 == result )
			{			
				if (!m_bFreezeRefreshCache)
				{
					// Refresh the display data
					m_bRefreshCache = true;
					
					// Retrieve the alarm information here. We should not query the alarm information
					// together with retrieval of the display data because if we scroll down or
					// up then we also have to query the alarm information which is not necessary.
//					m_pSharedAlarm->updateIndexInfo(m_indexInfo, m_csPos);
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Update notification recieved.");
					notifyUpdate(IAlarmDisplayUpdate::NOTIFY_REFRESH); //++Noel			
				}
				else
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Cache Refresh has been freeze!!!");
				}
			}
			else if (WAIT_OBJECT_0 + 1 == result) 
			{
				//zyr++: for map/unmap shared memory
				Sleep(100);
				notifyAlarmStoreResart();
				//++zyr
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Shutting down...");
			}
		}
		return 0;
	}


	CSimpleDispMultiIndex::SharedMemSync::SharedMemSync(SYN_EVENT& remapEv, SYN_EVENT& unmapEv, CSimpleDispMultiIndex* parent)
		: m_parent(parent)
		, m_terminated(false)
		, m_remapEv(remapEv)
		, m_unmapEv(unmapEv)
		, m_mapShmemRef(1)
	{
		TA_ASSERT(m_parent != NULL, "m_parent should not be NULL");
		
		m_termiateEv = CreateEvent(NULL, TRUE, FALSE, NULL);
		
		start();
	}
	
	CSimpleDispMultiIndex::SharedMemSync::~SharedMemSync()
	{
		terminateAndWait();
		
		CloseHandle(m_termiateEv);
	}
	
	void CSimpleDispMultiIndex::SharedMemSync::terminate()
	{
		m_terminated = true;
		SetEvent(m_termiateEv);
	}
	
	void CSimpleDispMultiIndex::SharedMemSync::run()
	{
		m_terminated = false;
		
		const DWORD evcount = 3;
		SYN_EVENT evs[evcount] = {m_remapEv, m_unmapEv, m_termiateEv};
		DWORD result = 0;
		while(!m_terminated)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmSynchronisor::SharedMemSync::run()] Waiting for map/unmap Event.");
			result = WaitForMultipleObjects(evcount, evs, FALSE, INFINITE);
			if(result == WAIT_OBJECT_0)
			{
				// remap shared memory
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmSynchronisor::SharedMemSync::run()] %s remap shared memory.", 
					m_mapShmemRef == 0 ? "need to" : "ignore to");
				if(m_mapShmemRef == 0)
				{
					//m_mapShmemRef = TA_Base_Core::Shmem::mapSharedMem();
					m_mapShmemRef = m_parent->m_pSharedAlarm->remapSharedMem();
					m_parent->setCanNotify(true);
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmSynchronisor::SharedMemSync::run()] remap shared memory, m_mapShmemRef=%d.",
						m_mapShmemRef);
					
				}
				
				//notify alarm_list_control that alarm store restarted
				if(m_parent != NULL)
				{
					m_parent->notifyAlarmStoreResart();
				}
			}
			else if(result == WAIT_OBJECT_0 + 1)
			{
				// unmap shared memory
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmSynchronisor::SharedMemSync::run()] %s unmap shared memory.",
					m_mapShmemRef != 0 ? "need to" : "ignore to");
				if(m_mapShmemRef != 0)
				{
					//m_parent->notifyUpdate(IAlarmDisplayUpdate::NOTIFY_UNMAP);
					m_parent->notifyUnmapSharedMem();
					m_parent->setCanNotify(false);
					m_mapShmemRef = m_parent->m_pSharedAlarm->unmapSharedMem();
	
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmSynchronisor::SharedMemSync::run()] unmap shared memory, ref_counter=%d.",
						m_mapShmemRef);
					m_mapShmemRef = 0;					
				}
			}
			else
			{
				// here will be terminated.
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[AlarmSynchronisor::SharedMemSync::run()] receive m_terminateEv signal.");
			}
		}
	}	


	/************************************************************************/
	/* Data Management Methods                                              */
	/************************************************************************/

	void CSimpleDispMultiIndex::initialize()
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);
	}

	
	// This function is use to clear all data in the simple multi_index container
	void CSimpleDispMultiIndex::clear()
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);

		_cleanInternals();
	}

	void CSimpleDispMultiIndex::registerUpdateObserver(IAlarmDisplayUpdate *pUpdateObserver)
	{
		m_pUpdateObserver = pUpdateObserver;
	}

	void CSimpleDispMultiIndex::_cleanInternals()
	{
		try
		{
			// Reset the index information
			m_indexInfo.earliestSourceTime.millitm = 0;
			m_indexInfo.earliestSourceTime.time = 0;
			m_indexInfo.latestSourceTime.millitm = 0;
			m_indexInfo.latestSourceTime.time = 0;
			m_indexInfo.exceededMaxDisplay = false;
			m_indexInfo.highestSeverity = 0;
			m_indexInfo.isLoadingAlarms = false;
			m_indexInfo.totalAlarms = 0;
			m_indexInfo.unackAlarms = 0;

			// clear the selection container
			m_SelectedAlarms.clear();

			// Clear the primary cache
			DisplayVecT::iterator itr = m_displayVec.begin();
			for(itr; itr!=m_displayVec.end(); itr++)
			{
				delete (*itr);
				(*itr) = NULL;
			}
			m_displayVec.clear();

			// Clear the secondary cache
			DisplayVecT::iterator itr2 = m_tempCacheVec.begin();
			for (itr2; itr2!=m_tempCacheVec.end(); itr2++)
			{
				delete (*itr2);
				(*itr2) = NULL;
			}
			m_tempCacheVec.clear();
			
			// Reset Virtual index of the vector display // primary cache
			m_ulCacheBegin = 0;
			m_ulCacheEnd = 0;

			// Reset Virtual index of the secondary cache //
			m_ulSecCacheBegin = 0;
			m_ulSecCacheEnd = 0;
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "exception occurred while trying to clean all containers.");	
		}
	}

	AlarmVectorWrap* CSimpleDispMultiIndex::getDisplayData()
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);

		if (m_pDisplayVectWrapper == NULL)
		{
			m_pDisplayVectWrapper = new AlarmVectorWrap(this, m_ThreadLock, m_displayVec);
		}

		return m_pDisplayVectWrapper;
	}

	AlarmSelectionWrap * CSimpleDispMultiIndex::getSelectionWrapper()
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);
		
		if (m_pSelectionWrapper == NULL)
		{
			m_pSelectionWrapper = new AlarmSelectionWrap(this, m_ThreadLock, m_SelectedAlarms);
		}

		return m_pSelectionWrapper;
	}
	
	// Methods for Select/UnSelect alarms
	// This function is used to check if alarm has been selected
	bool CSimpleDispMultiIndex::isAlarmItemSelected(std::string strAlarmID)
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);
		
		if (m_SelectedAlarms.size() != 0 && m_SelectedAlarms.find(strAlarmID) != m_SelectedAlarms.end())
		{
			return true;
		}

		return false;
	}
	
	// This function is used add alarms to the selected items
	void CSimpleDispMultiIndex::selectAlarmItem(unsigned long ulIndex)
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);

		try
		{
			bool bFromShared = false;
			AlarmDataItem* pAlarm = NULL;
			if (m_displayVec.size() != 0 && (ulIndex >= m_ulCacheBegin && ulIndex <= m_ulCacheEnd))
				pAlarm = _getItemCacheByIndex(ulIndex);
			else
			{
				pAlarm = _getItemByIndex(ulIndex);
				bFromShared = true;
			}
	
			if (pAlarm != NULL)
			{
				if (m_SelectedAlarms.find(pAlarm->alarmID) == m_SelectedAlarms.end())
				{
					m_SelectedAlarms.insert(pAlarm->alarmID);
				}
				if (bFromShared)
					delete pAlarm;

				pAlarm = NULL;
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occured while trying to access alarm details. alarm may have been close.");
		}
	}

	// This function is used to add multiple alarms to the selected items
	void CSimpleDispMultiIndex::selectRangeAlarms(unsigned long ulSelBegin, unsigned long ulSelEnd)
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Range selection Start = %ld ->  End = %ld", ulSelBegin, ulSelEnd);
		
		try
		{
			// alarms is within the cache items
			if (ulSelBegin >= m_ulCacheBegin && ulSelEnd <= m_ulCacheEnd)
			{
				for (ULONG ulIndex = ulSelBegin; ulIndex <= ulSelEnd; ulIndex++)
				{
					AlarmDataItem* pAlarm = NULL;
					pAlarm = _getItemCacheByIndex(ulIndex);
					if (pAlarm != NULL)
					{
						m_SelectedAlarms.insert(pAlarm->alarmID);
					}
				}
			}
			// Query directly to the shared alarm
			else
			{
				m_pSharedAlarm->getRangeAlarmID(m_SelectedAlarms, ulSelBegin, ulSelEnd, m_sortCol, m_csPos, m_bSortAccending);
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occured while trying to access alarm details. alarm may have been close.");
		}
	}

	
	// This function is used to unselect a particular alarm item.
	void CSimpleDispMultiIndex::unselectAlarmItem(unsigned long ulIndex)
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);

		try
		{
			AlarmDataItem * pAlarm = NULL;
			pAlarm = _getItemCacheByIndex(ulIndex);
			if (pAlarm != NULL)
			{
				IndexSel::iterator itr = m_SelectedAlarms.find(pAlarm->alarmID);
				if (itr != m_SelectedAlarms.end())
				{
					m_SelectedAlarms.erase(itr);
				}
			}		
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occured while trying to access alarm details. alarm may have been close.");
		}
	}
	
	// Function to unselect all alarm items currently selected
	void CSimpleDispMultiIndex::unSelectAllAlarmItem()
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);

		m_SelectedAlarms.clear();
	}


	void CSimpleDispMultiIndex::setDisplaySort(SORT_COLUMN sortType, bool bSortAccending)
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);
		m_sortCol = sortType;
		m_bSortAccending = bSortAccending;
		m_bRefreshCache = true;

		// Refresh the cache
		getListDisplayData();

		notifyUpdate(IAlarmDisplayUpdate::NOTIFY_REFRESH);
	}

	void CSimpleDispMultiIndex::setListDisplayInfo(ListDispInfo& listInfo)
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);
		
		if (!m_bInitialize) return;
		if (listInfo.ulDisplayCount != -1)// && (listInfo.ulDisplayCount>0 && m_indexInfo.totalAlarms != 0))
		{
			m_listDispInfo = listInfo;
		}
		
		getListDisplayData();
	}
	
	// retrieve the index information of the alarms
	void CSimpleDispMultiIndex::getIndexInfo(TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo& indexInfo, bool bUpdateFromShared/* = false*/)
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);
		if (!m_bInitialize)	return;
		if (bUpdateFromShared)
		{
			m_pSharedAlarm->updateIndexInfo(indexInfo, m_csPos);
		}
		else
		{
			indexInfo.earliestSourceTime.millitm = m_indexInfo.earliestSourceTime.millitm;
			indexInfo.earliestSourceTime.time = m_indexInfo.earliestSourceTime.time;

			indexInfo.latestSourceTime.millitm = m_indexInfo.latestSourceTime.millitm;
			indexInfo.latestSourceTime.time = m_indexInfo.latestSourceTime.time;
			indexInfo.exceededMaxDisplay = m_indexInfo.exceededMaxDisplay;
			indexInfo.highestSeverity = m_indexInfo.highestSeverity;
			indexInfo.isLoadingAlarms = m_indexInfo.isLoadingAlarms;
			indexInfo.totalAlarms = m_indexInfo.totalAlarms;
			indexInfo.unackAlarms = m_indexInfo.unackAlarms;
			indexInfo.dutyAlarmVersion = m_indexInfo.dutyAlarmVersion;
		}
	}

	unsigned long CSimpleDispMultiIndex::getSelectedAlarmsCount()
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);

		return m_SelectedAlarms.size();
	}

	void CSimpleDispMultiIndex::setFreezeRefreshCache(bool bFreezeRefreshCache)
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);
		
		m_bFreezeRefreshCache = bFreezeRefreshCache;
	}

	void CSimpleDispMultiIndex::getListDisplayData()
	{
		ULONG ulIndexBegin = m_listDispInfo.ulTopIndex;
		ULONG ulDataCount = m_listDispInfo.ulDisplayCount; // initial count is the count per page
		long ulDiff = 0;

		// Freeze the cache so that display will not be distorted //
		// Cache will only be freeze on the event of column resize
		if ( m_bFreezeRefreshCache == true) return;
		
		// Determine if the range of the display page is out of range in our cache items
		if (m_displayVec.size() != 0)
		{
			if (!m_bRefreshCache && (m_listDispInfo.ulTopIndex >= m_ulCacheBegin && (m_listDispInfo.ulTopIndex + m_listDispInfo.ulDisplayCount) <= m_ulCacheEnd))
			{
				// Condition 1
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Display page is within the cache range...");
				return;
			}
// 			else //if (m_listDispInfo.ulTopIndex > itEnd->first || (m_listDispInfo.ulTopIndex + m_listDispInfo.ulDisplayCount) < itBegin->first) 
// 			{
// 				// Determine the beginning index of the cache items and calculate the 
// 				// previous page cache items
// 				if (m_listDispInfo.ulTopIndex > 0)
// 				{
// 					ulDiff = m_listDispInfo.ulTopIndex - m_listDispInfo.ulPrevPageBuffer;
// 					if (ulDiff > 0)
// 					{
// 						ulIndexBegin = ulDiff;
// 						ulDataCount += m_listDispInfo.ulPrevPageBuffer; // add the previous page count
// 					}
// 					else 
// 					{
// 						// if the difference less than or equal to 0 then the previous page count 
// 						// is equal to the TopIndex value and the beginning index is always 0
// 						ulIndexBegin = 0;
// 						ulDataCount += m_listDispInfo.ulTopIndex; 
// 					}
// 				}
// 				// add the next page buffer //
// 				ulDataCount += m_listDispInfo.ulNextPageBuffer;
// 
// 				m_pDisplayVectWrapper->_clearCache();
// 				m_pSharedAlarm->getDisplayData(m_indexInfo, m_displayVec, m_sortCol, m_csPos, m_bSortAccending, ulIndexBegin, ulDataCount, m_bRefreshCache);
// 				
// 				m_ulCacheBegin = ulIndexBegin;
// 				m_ulCacheEnd = ulIndexBegin + m_displayVec.size() - 1;
// 
// 				m_bRefreshCache = false;
// 				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Retrieve Display Cache Data size = %ld, TopIndex = %ld PageCount = %ld", m_displayVec.size(), m_listDispInfo.ulTopIndex, m_listDispInfo.ulDisplayCount);	
// 			}
		}
// 		else
// 		{
			// Determine the beginning index of the cache items and calculate the 
			// previous page cache items
			if (m_listDispInfo.ulTopIndex > 0)
			{
				ulDiff = m_listDispInfo.ulTopIndex - m_listDispInfo.ulPrevPageBuffer;
				if (ulDiff > 0)
				{
					ulIndexBegin = ulDiff;
					ulDataCount += m_listDispInfo.ulPrevPageBuffer; // add the previous page count
				}
				else 
				{
					// if the difference less than or equal to 0 then the previous page count 
					// is equal to the TopIndex value and the beginning index is always 0
					ulIndexBegin = 0;
					ulDataCount += m_listDispInfo.ulTopIndex; 
				}
			}
			// add the next page buffer //
			ulDataCount += m_listDispInfo.ulNextPageBuffer;

			if (NULL != m_pDisplayVectWrapper)
			{
				m_pDisplayVectWrapper->_clearCache();
			}
			
			m_pSharedAlarm->getDisplayData(m_indexInfo, m_displayVec, m_sortCol, m_csPos, m_bSortAccending, ulIndexBegin, ulDataCount, m_bRefreshCache);

			m_ulCacheBegin = ulIndexBegin;
			m_ulCacheEnd = ulIndexBegin + m_displayVec.size() - 1;

			// Reset secondary cache
			if (m_bRefreshCache)
			{
				for(int i = 0; i < m_tempCacheVec.size(); i++)
				{
					delete m_tempCacheVec[i];
					m_tempCacheVec[i] = NULL;
				}
				m_tempCacheVec.clear();
				m_ulSecCacheBegin = 0;
				m_ulSecCacheEnd = 0;
			}

			m_bRefreshCache = false;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Retrieve Display Cache Data size = %ld, TopIndex = %ld PageCount = %ld", m_displayVec.size(), m_listDispInfo.ulTopIndex, m_listDispInfo.ulDisplayCount);
//		}
	}


	std::string CSimpleDispMultiIndex::getAlarmID(unsigned long ulIndex)
	{
		TA_Base_Core::ThreadGuard guard(m_ThreadLock);
		std::string strAlarmID;

		bool bFromShared = false;
		AlarmDataItem* pAlarm = NULL;
		if (m_displayVec.size() != 0 && (ulIndex >= m_ulCacheBegin && ulIndex <= m_ulCacheEnd))
		{
			pAlarm = _getItemCacheByIndex(ulIndex);
		}
		else
		{
			pAlarm = _getItemByIndex(ulIndex);
			bFromShared = true;
		}	

		if (pAlarm != NULL)
		{
			strAlarmID = pAlarm->alarmID.c_str();
			if (bFromShared)
			{
				delete pAlarm;
			}
			pAlarm = NULL;
		}

		return strAlarmID;
	}

	/************************************************************************/
	/* Private member methods                                               */
	/************************************************************************/
	AlarmDataItem* CSimpleDispMultiIndex::_getItemByIndex(ULONG ulIndex)
	{
		return m_pSharedAlarm->getAlarmItemByIndex(ulIndex, m_sortCol, m_csPos, m_bSortAccending);
	}

	AlarmDataItem* CSimpleDispMultiIndex::_getItemByID(std::string strAlarmID)
	{
		return m_pSharedAlarm->getAlarmItemByID(strAlarmID);
	}

	long CSimpleDispMultiIndex::_findItemPosition(std::string strAlarmID)
	{
		return m_pSharedAlarm->findItemPosition(strAlarmID, m_sortCol, m_csPos, m_bSortAccending);
	}

	void CSimpleDispMultiIndex::_getAllAlarmModifyItem(std::vector<TA_Base_Core::AlarmModifyItem*>& allAlarmModifyItems)
	{
		allAlarmModifyItems = m_pSharedAlarm->getAllModifyItems(m_sortCol, m_csPos);
	}

	void CSimpleDispMultiIndex::_getAllVisibleAlarms(std::vector<std::string>& allvisibleAlarms)
	{
		IndexSel allVisible;

		allvisibleAlarms.clear();
		m_pSharedAlarm->getRangeAlarmID(allVisible, 0, m_indexInfo.totalAlarms-1, m_sortCol, m_csPos, m_bSortAccending);
		std::copy(allVisible.begin(), allVisible.end(), std::back_inserter(allvisibleAlarms));

	}

	std::vector<TA_Base_Core::AlarmModifyItem*> CSimpleDispMultiIndex::_getAlarmModifyItem(std::set<std::string >& vecAlarmIDs)
	{
		return m_pSharedAlarm->getAlarmModifyItem(vecAlarmIDs);
	}

	AlarmDataItem* CSimpleDispMultiIndex::_getItemCacheByIndex(ULONG ulIndex, bool bRequestAddCache/*= false*/)
	{
		AlarmDataItem* pAlarmItem = NULL;
		
		if (ulIndex >= m_indexInfo.totalAlarms)
			return pAlarmItem;

		if (m_displayVec.size() != 0 && (ulIndex >= m_ulCacheBegin && ulIndex <= m_ulCacheEnd))
		{
			ULONG ulBaseIndex = ulIndex - m_ulCacheBegin;
			pAlarmItem = m_displayVec[ulBaseIndex];
		}

		if (m_displayVec.size() != 0 && NULL == pAlarmItem && bRequestAddCache)
		{	// check if item is in the secondary cache
			if (m_tempCacheVec.size() != 0 && (ulIndex >= m_ulSecCacheBegin && ulIndex <= m_ulSecCacheEnd))
			{
				ULONG ulBaseIndex = ulIndex - m_ulSecCacheBegin;
				pAlarmItem = m_tempCacheVec[ulBaseIndex];

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "alarm item found in the secondary cache.");
				return pAlarmItem;
			}
			
			//Try to retrieve secondary cache item with full page size
			long ulRequestBegin = ulRequestBegin = ulIndex;
			long ulRequestEnd = ulRequestEnd  = ulRequestBegin + m_listDispInfo.ulNextPageBuffer;
// 			if (ulIndex > m_ulSecCacheEnd)
// 			{
// 				ulRequestBegin = ulIndex;
// 				
// 			}
// 			else 
			if(ulIndex < m_ulSecCacheBegin)
			{
				ulRequestEnd = ulIndex+1;
 				ulRequestBegin = ulRequestEnd - m_listDispInfo.ulPrevPageBuffer;
				if (ulRequestBegin < 0)
					ulRequestBegin = 0;
			}

			m_pSharedAlarm->getDisplayData(m_indexInfo, m_tempCacheVec, m_sortCol, m_csPos, m_bSortAccending, ulRequestBegin, ulRequestEnd, false);
			if (m_tempCacheVec.size() == 0)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Unable to retrieve secondary cache items");
				return pAlarmItem;
			}
			
			m_ulSecCacheBegin = ulRequestBegin;
			m_ulSecCacheEnd = m_ulSecCacheBegin + m_tempCacheVec.size()-1;

			// requery the item requested
			if (m_tempCacheVec.size() != 0 && (ulIndex >= m_ulSecCacheBegin && ulIndex <= m_ulSecCacheEnd))
			{
				ULONG ulBaseIndex = ulIndex - m_ulSecCacheBegin;
				pAlarmItem = m_tempCacheVec[ulBaseIndex];
			}
		}

		return pAlarmItem;	
	}

	void CSimpleDispMultiIndex::_getStringAttribute(AlarmDataItem* pAlarmItem, STRING_ATTR nSubIndex, std::string& strResult)
	{
		if (pAlarmItem != NULL)
		{
			switch(nSubIndex)
			{
			case ALARMID:
				strResult = pAlarmItem->alarmID.c_str();
				break;
			case ENTITY_NAME:			// entity name
				strResult = pAlarmItem->entityName.c_str();
				break;
			case AVALANCHE:				// avalanche
				strResult = pAlarmItem->avalanche.c_str();
				break;
			case ASSET_NAME:			// asset name
				strResult = pAlarmItem->assetName.c_str();
				break;
			case ALARM_DESCRIPTION:		// Description
				strResult = pAlarmItem->alarmDescription.c_str();
				break;
			case ALARM_ACKNOWLEDGE_BY:	// acknowledge
				strResult = pAlarmItem->alarmAcknowledgedBy.c_str();
				break;
			case ALARM_COMMENTS:		// comments
				strResult = pAlarmItem->alarmComments.c_str();
				break;
			case ALARM_VALUE:			// value
				strResult = pAlarmItem->alarmValue.c_str();
				break;
			case DDS:					// DecisionSupport
				strResult = pAlarmItem->decisionSupportState.c_str();
				break; 
			case MMS_STATE:				// mms state
				strResult = pAlarmItem->mmsState.c_str();
				break;
			case AVALANCHE_HEAD_ID:		// avalanche head id
				strResult = pAlarmItem->avalancheHeadID.c_str();
				break;
			case ALARM_OPERATOR_MODE:
				strResult = pAlarmItem->alarmOperatorMode.c_str();
				break;
			default:
				strResult = "";
				TA_Assert(false, "ERROR: String attribute not found.");
				break;
			}  
		}
	}

	void CSimpleDispMultiIndex::_getNumberAttribute(AlarmDataItem* pAlarmItem, NUMBER_ATTR nSubIndex, unsigned long & ulResult)
	{
		if (pAlarmItem != NULL)
		{
			switch(nSubIndex)
			{
			case STATE:
				ulResult = pAlarmItem->state;
				break;
			case ALARM_SEVERITY:
				ulResult = pAlarmItem->alarmSeverity;
				break;
			case SUBSYSTEM_KEY:
				ulResult = pAlarmItem->subsystemKey;
				break;
			case ASSOC_ENTITY_KEY:
				ulResult = pAlarmItem->assocEntityKey;
				break;
			case ASSOC_ENTITY_TYPE_KEY:
				ulResult = pAlarmItem->assocEntityTypeKey;
				break;
			case MESSAGE_TYPE_KEY:
				ulResult = pAlarmItem->messageTypeKey;
				break;
			case LOCATION_KEY:
				ulResult = pAlarmItem->locationKey;
				break;
			case TRANSACTIVETIME_TIME:
				ulResult = pAlarmItem->transactiveTime.time;
				break;
			case TRANSACTIVETIME_MILI:
				ulResult = pAlarmItem->transactiveTime.millitm;
				break;
			case IS_EXT_ALARM:
				ulResult = pAlarmItem->isExternalAlarm?1:0;
				break;
			default:
				ulResult = 0;
				TA_Assert(false, "ERROR: Number attribute not found");
				break;
			}
		}			

	}

	void CSimpleDispMultiIndex::_getAvalanchedChildAlarms(std::string avalanchedHeadID, std::vector<std::string>& vecChildAlarms)
	{
		m_pSharedAlarm->getAvalanchedChildAlarms(m_csPos, avalanchedHeadID, vecChildAlarms);
	}

	AlarmDataItem* CSimpleDispMultiIndex::_getItemCacheByID(std::string& strAlarmID)
	{
		AlarmDataItem* pAlarmItem = NULL;
		DisplayVecT::iterator itr = m_displayVec.begin();
		for (itr; itr != m_displayVec.end(); itr++)
		{
			if (lstrcmpi((*itr)->alarmID.c_str(), strAlarmID.c_str()) == 0)
			{
				pAlarmItem = (*itr);
			}
		}

		return pAlarmItem;
	}

// 	AlarmDataItem* CSimpleDispMultiIndex::_getItemCacheByIndex(unsigned long ulIndex)
// 	{
// 		AlarmDataItem* pAlarmItem = NULL;
// 		
// 		if (ulIndex >= m_ulCacheBegin && ulIndex <= m_ulCacheEnd)
// 		{
// 			pAlarmItem = m_displayVec[ulIndex];
// 		}
// 
// 		return pAlarmItem;
// 	}

}
