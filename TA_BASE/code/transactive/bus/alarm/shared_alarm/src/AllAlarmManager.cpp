//#include "bus/alarm/alarm_store_library/src/AlarmStoreManager.h"

#include "bus/alarm/shared_alarm/src/CommonDef.h"
#include "bus/alarm/shared_alarm/src/SharedAlarm.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include "bus/alarm/shared_alarm/src/AllAlarmManager.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DateFormat.h"
#include <Time.h>



/*************************** All Alarm **********************************/
using namespace boost::multi_index;

typedef composite_key< 
	TA_Base_Bus::AlarmDataItem*,
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Bus::AlarmDataItem, unsigned long, locationKey),
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Bus::AlarmDataItem, unsigned long, dataSource),
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Bus::AlarmDataItem, unsigned long, subsystemKey)
> ckey_t;

typedef multi_index_container<
	TA_Base_Bus::AlarmDataItem*, //HP++ comments, actually, it's better to be object instead of pointer.
	indexed_by<
		//HP++comments, sort by less<char*> on TA_Base_Core::AlarmItem::alarmID, the comparation function is charptrless.
		ordered_unique<BOOST_MULTI_INDEX_MEMBER(TA_Base_Bus::AlarmDataItem, std::string, alarmID)>,	// by default it sorted by std::less

		//HP++comments, sort by less<unsigned long> on TA_Base_Core::AlarmItem::subsystemKey.
		//ordered_non_unique<BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, unsigned long, subsystemKey)>
		ordered_non_unique< 
			ckey_t
			#if defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
			,boost::multi_index::composite_key_result_less<ckey_t::result_type>
			#endif
		>,
		ordered_non_unique<
			BOOST_MULTI_INDEX_MEMBER(TA_Base_Bus::AlarmDataItem, TA_Base_Core::ItemMillisecondTime, transactiveTime) ,
			std::less<TA_Base_Core::ItemMillisecondTime>
		>,
		ordered_non_unique< BOOST_MULTI_INDEX_MEMBER(TA_Base_Bus::AlarmDataItem, bool, isExternalAlarm) > 
	>
> AllAlarms;

typedef boost::multi_index::nth_index<AllAlarms, 0>::type	allalarms_by_id;
typedef boost::multi_index::nth_index<AllAlarms, 1>::type	allalarms_by_locSubSystem;
typedef boost::multi_index::nth_index<AllAlarms, 2>::type	allalarms_by_time;
typedef boost::multi_index::nth_index<AllAlarms, 3>::type	allalarms_by_isExternal;

typedef std::pair<allalarms_by_locSubSystem::iterator, allalarms_by_locSubSystem::iterator> T_allAlarmLocRange;
typedef std::pair<allalarms_by_isExternal::iterator, allalarms_by_isExternal::iterator> T_AllAlarmIsExternal;
/************************************************************************/


// All alarms container
AllAlarms * g_objAllAlarms = NULL;


namespace TA_Base_Bus
{
	enum
	{
		ALLALARM_SORT_BY_ID = 0,
		ALLALARM_SORT_BY_LOC_SUBSYSTEM,
		ALLALARM_SORT_BY_TIME,
		ALLALARM_SORT_EXTERNAL
	}SORT_ALL_ALARM;

	AllAlarmManager* AllAlarmManager::m_pInstance = 0;
	TA_Base_Core::NonReEntrantThreadLockable AllAlarmManager::m_singletonLock;


	struct updateAlarmDetail
	{
		updateAlarmDetail(const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate):m_alarmUpdate(alarmUpdate){}
		void operator()(TA_Base_Bus::AlarmDataItem* pAlarmItem)
		{
			if (m_alarmUpdate.alarmHasBeenAcknowledged)
			{				
				pAlarmItem->alarmAcknowledgedBy = m_alarmUpdate.alarmAcknowledgedBy.in();
			}

			if (m_alarmUpdate.alarmHasBeenClosed)
			{
				pAlarmItem->state = TA_Base_Core::AlarmClosed;
				pAlarmItem->transactiveTime = TA_Base_Core::ItemMillisecondTime(m_alarmUpdate.closeTime,0);
				pAlarmItem->alarmValue = m_alarmUpdate.closeValue; 

			}

			if (m_alarmUpdate.alarmHasBeenOpened)
			{
				pAlarmItem->state = TA_Base_Core::AlarmOpen;
			}

			if (m_alarmUpdate.commentsWereAdded)
			{
				pAlarmItem->alarmComments = m_alarmUpdate.alarmComments.in();
			}

			if (m_alarmUpdate.decisionSupportStateHasChanged)
			{
				pAlarmItem->decisionSupportState = TA_Base_Core::Utilities::getInstance().convertDSS2String(m_alarmUpdate.decisionSupportState);
			}

			if (m_alarmUpdate.mmsStateHasChanged)
			{
				pAlarmItem->mmsState = TA_Base_Core::Utilities::getInstance().convertMMS2String(m_alarmUpdate.mmsState);
			}
		}
	private:

		const TA_Base_Core::AlarmUpdateCorbaDef& m_alarmUpdate;
	};




	AllAlarmManager& AllAlarmManager::getInstance()
	{		 
		FUNCTION_ENTRY("AllAlarmManager::getInstance");
		TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_pInstance == 0)
		{
			m_pInstance = new AllAlarmManager();
		}
		FUNCTION_EXIT;
		return *m_pInstance;
	}

	void AllAlarmManager::freeInstance()
	{
		FUNCTION_ENTRY("AllAlarmManager::freeInstance");
		TA_Base_Core::ThreadGuard guard( m_singletonLock );
		if (m_pInstance != NULL)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
		FUNCTION_EXIT;
	}

	AllAlarmManager::AllAlarmManager(void) 
		: m_ulMaxAlarmCount(MAXALLALARMCOUNT)
	{
		FUNCTION_ENTRY("AllAlarmManager::AllAlarmManager");
		g_objAllAlarms = new AllAlarms;

		FUNCTION_EXIT;
	}

	AllAlarmManager::~AllAlarmManager(void)
	{
		FUNCTION_ENTRY("AllAlarmManager::~AllAlarmManager");
		deleteAllAlarmItems();
		FUNCTION_EXIT;
	}

	bool AllAlarmManager::insertAlarm(AlarmDataItem* pData, DutyTempList * pDutyList/* = NULL*/)
	{
		FUNCTION_ENTRY("AllAlarmManager::insertAlarm");
		TA_Base_Core::ThreadGuard guard(m_MapLock);
		bool bRes = false;
		if (NULL == pData) return false;
		
 		try
 		{
			// if Maximum alarm was define and we already reach the limit. 
			// we should remove the oldest alarm from the containers
			if (m_ulMaxAlarmCount != 0 && g_objAllAlarms->size() >= m_ulMaxAlarmCount)
			{
				
				allalarms_by_time& indexByTime = boost::multi_index::get<ALLALARM_SORT_BY_TIME>(*g_objAllAlarms);
				allalarms_by_time::iterator itDel = indexByTime.begin();
				TA_Base_Bus::AlarmDataItem* pAlarm = *itDel;

				if (pAlarm != NULL)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Maximum limit reach.. removing the oldest alarm: ID = %s, Time = %s", (pAlarm->alarmID.c_str()), (_formatTime(pAlarm->transactiveTime.time).c_str()));
					
					// if from reload check the dutylist and remove the alarm if found
					if (pDutyList != NULL && pDutyList->size() != 0)
					{
						DWORD dwPtr = (DWORD)pAlarm;
						DutyTempList::iterator itFind = pDutyList->find(dwPtr);
						if (itFind != pDutyList->end())
						{
							pDutyList->erase(itFind);
						}
					}
					
					indexByTime.erase(itDel);

					// deallocate
					delete pAlarm;
					pAlarm = NULL;
				}
				else
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Alarm data is NULL, unable to remove the oldest alarm.");
				}
			}

			// Insert to all alarm container
			g_objAllAlarms->insert(pData);
			bRes = true;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Insert Alarm Success: alarmID = %s, CurrentSize = %ld", pData->alarmID.c_str(), g_objAllAlarms->size());
			
 		}
 		catch(...)
 		{
 			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception caught while inserting data into container");		
 		}

		FUNCTION_EXIT;
		return bRes;
	}
	

	bool AllAlarmManager::updateAlarm(AlarmDataItem* pData, const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate)
	{
		FUNCTION_ENTRY("AllAlarmManager::updateAlarm");
		TA_Base_Core::ThreadGuard guard(m_MapLock);
		bool bRes = false; 
		if (NULL == pData) return bRes;

		try
		{
			allalarms_by_id& indexByID = boost::multi_index::get<ALLALARM_SORT_BY_ID>(*g_objAllAlarms);
			allalarms_by_id::iterator itID = indexByID.find(alarmUpdate.alarmID.in());
			if (itID != indexByID.end())
			{
			
				g_objAllAlarms->modify(itID, updateAlarmDetail(alarmUpdate));

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Update Success Alarm ID = %s", pData->alarmID.c_str());
				bRes = true;
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to find Alarm Item with ID = %s", pData->alarmID.c_str());
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception caught while trying to update alarm in the container");
		}	 
		
		FUNCTION_EXIT;
		return bRes;
	}

	bool AllAlarmManager::deleteAlarm(std::string& strAlarmID)
	{
		FUNCTION_ENTRY("AllAlarmManager::deleteAlarm");
		TA_Base_Core::ThreadGuard guard(m_MapLock);
		bool bRes = false;
		try
		{
			allalarms_by_id& indexByID = boost::multi_index::get<ALLALARM_SORT_BY_ID>(*g_objAllAlarms);
			allalarms_by_id::iterator itID = indexByID.find(strAlarmID);
			if (itID != indexByID.end())
			{
				AlarmDataItem* pAlarmItem = *itID; 

				// Remove from all alarm
				g_objAllAlarms->erase(itID);

				// deallocate
				delete pAlarmItem;
				pAlarmItem = NULL;

				bRes = true;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Remove alarm ID = %s", strAlarmID.c_str());
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to find Alarm Item with ID = %s", strAlarmID.c_str());
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception caught while trying to update alarm in the container");
		}

		FUNCTION_EXIT;
		return bRes;
	}

	AlarmDataItem* AllAlarmManager::findAlarm(std::string alarmID)
	{
		FUNCTION_ENTRY("AllAlarmManager::findAlarm");
		TA_Base_Core::ThreadGuard guard(m_MapLock);
		AlarmDataItem* pAlarmItem = NULL;
		try
		{
			allalarms_by_id& indexByID = boost::multi_index::get<ALLALARM_SORT_BY_ID>(*g_objAllAlarms);
			allalarms_by_id::iterator itID = indexByID.find(alarmID);
			if (itID != indexByID.end())
			{
				pAlarmItem = *itID;
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception caught while trying to find the alarm item.");
		}	

		FUNCTION_EXIT;
		return pAlarmItem;
	}


	void AllAlarmManager::deleteAllAlarmItems()
	{
		FUNCTION_ENTRY("AllAlarmManager::deleteAllAlarmItems");
		 
		TA_Base_Core::ThreadGuard guard(m_MapLock);
		try
		{
			if(g_objAllAlarms != NULL && !g_objAllAlarms->empty())
			{
				std::for_each(g_objAllAlarms->begin(), g_objAllAlarms->end(), 
					std::bind1st(std::mem_fun1<int, AllAlarmManager,TA_Base_Bus::AlarmDataItem*>
					(&AllAlarmManager::deleteItem), this));
			}

			g_objAllAlarms->clear();
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occured while try to clean all containers");
		}		
		
		FUNCTION_EXIT;
	}

	void AllAlarmManager::buildIndexWithDuty(ICheckVisibility* pObjFilter, IDutyAlarmAccess* pObjDutyAlarmAccess)
	{
		FUNCTION_ENTRY("AllAlarmManager::buildIndexWithDuty");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"AllAlarmManager::buildIndexWithDuty() come in");
		TA_Base_Core::ThreadGuard guard(m_MapLock);
		try
		{
			if (NULL == pObjFilter) return;
			
			std::vector<AlarmDataItem*> vecDutyList;
			
			AllAlarms::iterator itr = g_objAllAlarms->begin();
			int nCounter = 0;
			unsigned long ulTimeStart = time(NULL);
			bool bIsFirstTimeOut = true;
			for (itr; itr != g_objAllAlarms->end(); itr++)
			{
				AlarmDataItem* pAlarm = *itr;
				unsigned long ulCurrTime=time(NULL);
				
				// Maximum build time is 3 mins
				if (ulCurrTime - ulTimeStart < MAXREBUILDTIME) 
				{
					if (pObjFilter->isAlarmVisible(pAlarm))
					{
						pAlarm->isVisible = true;
						vecDutyList.push_back(pAlarm);
						nCounter++;
					}
					else
					{
						pAlarm->isVisible = false;
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "isPassPreFilter false.");
					}
				}
				else
				{
					if (bIsFirstTimeOut)
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"buildIndexWithDuty passesprefilter time out");
						bIsFirstTimeOut = false;
					}

					pAlarm->isVisible = true;
					vecDutyList.push_back(pAlarm);
					nCounter++;
				}
				
				if (nCounter >= MAXBUILDCOUNT)
				{	
					pObjDutyAlarmAccess->insertAlarmlist(vecDutyList);
					vecDutyList.clear();
					nCounter = 0;
				}
			}
			
			if (nCounter != 0)
			{	
				pObjDutyAlarmAccess->insertAlarmlist(vecDutyList);
				vecDutyList.clear();
			}
			// forward vector list to shared memory
		}		
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occurred while trying to rebuild duty index.");
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"AllAlarmManager::buildIndexWithDuty() come out");
		FUNCTION_EXIT;
	}
	
	//++Noel
	void AllAlarmManager::UpdateDutyIndex(LocationSubSytemList& changes, ICheckVisibility* pObjFilter, IDutyAlarmAccess* pObjDutyAlarmAccess)
	{
		FUNCTION_ENTRY("AllAlarmManager::UpdateDutyIndex");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"AllAlarmManager::UpdateDutyIndex() come in");
		TA_Base_Core::ThreadGuard guard(m_MapLock);

		try
		{
			if (NULL == pObjFilter) return;

			std::vector<AlarmDataItem*> vecDutyList;
			vecDutyList.reserve(MAXBUILDCOUNT);

			LocationSubSytemList::iterator iterLoc = changes.begin();
			allalarms_by_locSubSystem& locSubSystemAlarms = boost::multi_index::get<ALLALARM_SORT_BY_LOC_SUBSYSTEM>(*g_objAllAlarms);

			bool isFirstTimeOut =false;
			unsigned long startTime=time(NULL);
			unsigned long ulCount = 0;
			
			for (iterLoc; iterLoc != changes.end(); iterLoc++)
			{
				// Remove Location's SubSystem alarms from dutyIndex
				pObjDutyAlarmAccess->removeLocSubSystemDutyAlarms(iterLoc->first, iterLoc->second, ISCS_SUBSYSTEM_ID);

				// Get All alarm From this location and reload all with the same subSystem
				std::list<unsigned long>::iterator itSubSytem = iterLoc->second.begin();
				for (itSubSytem; itSubSytem != iterLoc->second.end(); itSubSytem++)
				{
					T_allAlarmLocRange locRange = locSubSystemAlarms.equal_range(boost::make_tuple(iterLoc->first, *itSubSytem));					
					allalarms_by_locSubSystem::iterator itr = locRange.first;

					while(itr != locRange.second)
					{
						AlarmDataItem* pAlarmItem = (AlarmDataItem*)*itr;

						try
						{			 
							unsigned long currTime=time(NULL);
							if (currTime-startTime < MAXREBUILDTIME)
							{						  		
								if(pObjFilter->isAlarmVisible(pAlarmItem)) // if visible
								{
									pAlarmItem->isVisible = true;
									vecDutyList.push_back(pAlarmItem);
									ulCount++;
								}
								else
								{
									pAlarmItem->isVisible = false;
									LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "isPassPreFilter false.");
								}
							}	
							else // if timeout then alarm is defaulted to visible
							{
								pAlarmItem->isVisible = true;
								vecDutyList.push_back(pAlarmItem);
								if (isFirstTimeOut)
								{							 
									LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"buildIndexWithDuty passesprefilter time out");
									isFirstTimeOut=false;
								}
								ulCount++;
							}
						}
						catch(...)
						{				
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"buildIndexWithDuty passesprefilter error ID:%s, loc:%d", pAlarmItem->alarmID,pAlarmItem->locationKey);
						}

						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Handle=%d",ulCount);				
						itr++; // Next Item

						// if count has reach the maximum build count for the visible items then display it
						if ((ulCount != 0) && ((ulCount % MAXBUILDCOUNT) == 0))
						{
							pObjDutyAlarmAccess->insertAlarmlist(vecDutyList);
							vecDutyList.clear();
						}
					}
				}
			}

			if (vecDutyList.size() != 0)
			{
				pObjDutyAlarmAccess->insertAlarmlist(vecDutyList);
				vecDutyList.clear();
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "catch an exception of TA_SharedAlarm::buildIndexWithDuty()");
		}
	
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"AllAlarmManager::UpdateDutyIndex() come out");		
		FUNCTION_EXIT;
	}

	unsigned long AllAlarmManager::removeLocationAlarms(unsigned long locationKey, unsigned long dataSource /*=ISCS_SUBSYSTEM_ID*/)
	{	 
		FUNCTION_ENTRY("AllAlarmManager::removeLocationAlarms");
		TA_Base_Core::ThreadGuard guard(m_MapLock);
		unsigned long ulRetVal = removeLocationAlarmsInternal(locationKey);
		FUNCTION_EXIT;
		return ulRetVal;
	}

	unsigned long AllAlarmManager::removeDataSourceAlarms(unsigned long dataSource)
	{
		FUNCTION_ENTRY("AllAlarmManager::removeDataSourceAlarms");
		unsigned long lRes = 0;
		try
		{
			allalarms_by_isExternal& dataSource_index = boost::multi_index::get<ALLALARM_SORT_EXTERNAL>(*g_objAllAlarms);

			allalarms_by_isExternal::iterator lIter = dataSource_index.lower_bound( dataSource );
			allalarms_by_isExternal::iterator hIter = dataSource_index.upper_bound( dataSource );

			while(lIter != hIter)
			{
				deleteItem(*lIter);

				dataSource_index.erase(lIter);

				++lRes;
				lIter = dataSource_index.lower_bound( dataSource );
				hIter = dataSource_index.upper_bound(dataSource );
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception occurred when trying to delete alarms by datasource. datasource key %ld", dataSource);
		}

		FUNCTION_EXIT;
		return lRes;
	}	

	unsigned long AllAlarmManager::getAllAlarmCount()
	{
		FUNCTION_ENTRY("AllAlarmManager::getAllAlarmCount");
		FUNCTION_EXIT;
		return g_objAllAlarms->size();
	}

	unsigned long AllAlarmManager::removeLocationAlarmsInternal(unsigned long locationKey, unsigned long dataSource /*=ISCS_SUBSYSTEM_ID*/)
	{
		FUNCTION_ENTRY("AllAlarmManager::removeLocationAlarmsInternal");
		unsigned long lRes = 0;
		try
		{
			allalarms_by_locSubSystem& locSubSystem_index = boost::multi_index::get<ALLALARM_SORT_BY_LOC_SUBSYSTEM>(*g_objAllAlarms);

			allalarms_by_locSubSystem::iterator lIter = locSubSystem_index.lower_bound( boost::make_tuple(locationKey, dataSource) );
			allalarms_by_locSubSystem::iterator hIter = locSubSystem_index.upper_bound( boost::make_tuple(locationKey, dataSource) );

			while(lIter != hIter)
			{
				deleteItem(*lIter);
				
				locSubSystem_index.erase(lIter);
				
				++lRes;
				lIter = locSubSystem_index.lower_bound( boost::make_tuple(locationKey, dataSource) );
				hIter = locSubSystem_index.upper_bound( boost::make_tuple(locationKey, dataSource) );
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception occurred when trying to delete alarms by location. Location key %ld", locationKey);
		}
		
		FUNCTION_EXIT;
		return lRes;
	}

	void AllAlarmManager::setAllAlarmsMaxCount(unsigned long ulMaxCount)
	{
		FUNCTION_ENTRY("AllAlarmManager::setAllAlarmsMaxCount");
		if (ulMaxCount < 1)
		{
			return;
		}

		m_ulMaxAlarmCount = ulMaxCount;
		FUNCTION_EXIT;
	}

	std::string AllAlarmManager::_formatTime(long ulTime)
	{
		FUNCTION_ENTRY("AllAlarmManager::_formatTime");
		tm* ptm = gmtime(&ulTime);
		
		char szTime[256];
		strftime (szTime,256,"%H:%M:%S - %d-%b-%y", ptm);
		FUNCTION_EXIT;
		return std::string(szTime);
	}

	int AllAlarmManager::deleteItem(TA_Base_Bus::AlarmDataItem* pAlarmDataItem)
	{
		FUNCTION_ENTRY("AllAlarmManager::deleteItem");
		delete pAlarmDataItem;
		pAlarmDataItem = NULL;

		FUNCTION_EXIT;
		return 0;
	}
}

