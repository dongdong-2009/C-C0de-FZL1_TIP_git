/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/EventHandler.cpp $
  * @author:  huirong.luo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * EventHandler is ...
  *
  */


#include "EventHandler.h"
#include "app/event/event_viewer/src/ATSHistoricalWorker.h"
#include "app/event/event_viewer/src/ISCSHistoricalWorker.h"
#include "app/event/event_viewer/src/ISCSRealTimeWorker.h"
#include "app/event/event_viewer/src/ATSRealTimeWorker.h"
#include "app/event/event_viewer/src/CombinedEventItem.h"
#include "ConstantItems.h"
#include "CombinedEventFilter.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/Thread.h"
#include "core/event/src/EventItem.h"

using namespace TA_Base_App;
using namespace EventViewerConstants;


namespace TA_Base_App{

	std::string EventHandler::s_sortColumnForRealTime= CombinedEventFilter::COLUMN_DATETIME;;
	bool EventHandler::s_sortAscendingForRealTime= false;;

	std::string EventHandler::s_sortColumnForHist= CombinedEventFilter::COLUMN_DATETIME;;
	bool EventHandler::s_sortAscendingForHist= false;;

	std::string EventHandler::s_sortColumnForGUI= CombinedEventFilter::COLUMN_DATETIME;;
	bool EventHandler::s_sortAscendingForGUI= false;;


    void EventHandler::resetSortingRuleForRealTime(std::string sColumn, bool sAsc)
	{
		s_sortColumnForRealTime = sColumn.c_str();
		s_sortAscendingForRealTime = sAsc;
	}

	void EventHandler::resetSortingRuleForHist(std::string sColumn, bool sAsc)
	{
		s_sortColumnForHist = sColumn.c_str();
		s_sortAscendingForHist = sAsc;
	}

	void EventHandler::resetSortingRuleForGUI(std::string sColumn, bool sAsc)
	{
		s_sortColumnForGUI = sColumn.c_str();
		s_sortAscendingForGUI = sAsc;
	}
	

	EventHandler::EventHandler(EventDisplayMode dispMode, GUIObserver * observer):IEventWorkObserver()
	{

		this->m_mode = dispMode;
		m_GUIObserver = observer;
		m_CurrentTaskStopped = true;
		m_taskSeqID = 0;
		m_ISCSArrived =false;
		m_ATSArrived = false;
		m_ATSSubscriptionErr = 0; 
		//m_lastISCSStatusNotReady = true;
		m_lastTaskIDSubmitByWorker = 0;
	}

	EventHandler::~EventHandler(){
		releaseWorkers();
		clearList();
	}


	void EventHandler::setATSSubscriptionErr(long err)
	{
		m_ATSSubscriptionErr = err;
	}

	long EventHandler::getATSSubscriptionErr ()
	{
		return m_ATSSubscriptionErr;
	}

	SearchStatus EventHandler::getSearchStatus(TA_Base_Core::EventCategory eventCategory){

		SearchStatus ss =  TA_Base_App::FinishSearch;

		if(!m_workers.empty()){
			if ( m_workers.find(eventCategory)!= m_workers.end()){

				ss = m_workers.find(eventCategory)->second->getStatus();
			}
		}

		return ss;
	}


    
	const std::string  EventHandler::getColumnData (unsigned long index,string columnName)
	{
		TA_Base_Core::ThreadGuard guard2(m_displaylistLock);

		if( index<m_displaylist.size() ) 
		{
			DisplayItem * item = m_displaylist[index];
			if(item!= NULL)
			{
				return item->getColumnData(columnName).c_str();
			}
		}
		return "";

	}

    
	const std::string  EventHandler::getCompleteColumnData (unsigned long index,string columnName)
	{
		TA_Base_Core::ThreadGuard guard(m_displaylistLock);
		if( index<m_displaylist.size() ) 
		{
			return m_displaylist[index]->getCompleteColumnData(columnName).c_str();
		}
		return "";
		
	}



	void EventHandler::startParameter(FilterCriteria searchfilter, long seq_ID, 
		bool ISCSEnabled /*=true*/, bool ATSEnabled /*=true*/)
	{	
		/*GUI event list not cleared here as it should be only cleared after recovering successfully from database call*/
		clearAndReserveFor2Lists();
		//m_GUIObserver->sendListViewUpdateByObserver(0,0);
		{
			TA_Base_Core::ThreadGuard guard( m_taskIDLock );
			m_taskSeqID = seq_ID;
			m_curFilterCriteria = searchfilter;
			m_CurrentTaskStopped = false;
		}


		if(m_mode ==RealTimeMode)
		{
			if(ISCSEnabled)
			{
				std::map<TA_Base_Core::EventCategory,IEventWorker*>::iterator iscsWorkerIt = m_workers.find(TA_Base_Core::ISCSCategory);
				if(iscsWorkerIt == m_workers.end())
				{
					IEventWorker* iscsRealTWorker = new ISCSRealTimeWorker(this);
					iscsRealTWorker->addWork( searchfilter, seq_ID);
					addWorker(TA_Base_Core::ISCSCategory, iscsRealTWorker);
				}
				else
				{
					iscsWorkerIt->second->addWork( searchfilter, seq_ID);
				}
			}

			
			if(ATSEnabled)
			{
				std::map<TA_Base_Core::EventCategory,IEventWorker*>::iterator atsWorkerIt = m_workers.find(TA_Base_Core::ATSCategory);
				if(atsWorkerIt == m_workers.end())
				{
					IEventWorker* atsRealTWorker = new ATSRealTimeWorker(this);
					atsRealTWorker->addWork( searchfilter, seq_ID);
					addWorker(TA_Base_Core::ATSCategory, atsRealTWorker);
				}
				else
				{
					atsWorkerIt->second->addWork( searchfilter, seq_ID);
				}
			}

		}else
		{
			if(ISCSEnabled)
			{
				std::map<TA_Base_Core::EventCategory,IEventWorker*>::iterator iscsWorkerIt = m_workers.find(TA_Base_Core::ISCSCategory);
				if(iscsWorkerIt == m_workers.end())
				{
					IEventWorker* iscsHistWorker = new ISCSHistoricalWorker(this);
					iscsHistWorker->addWork( searchfilter, seq_ID);
					addWorker(TA_Base_Core::ISCSCategory, iscsHistWorker);
				}
				else
				{
					iscsWorkerIt->second->addWork( searchfilter, seq_ID);
				}

			}
			if(ATSEnabled)
			{
				std::map<TA_Base_Core::EventCategory,IEventWorker*>::iterator atsWorkerIt = m_workers.find(TA_Base_Core::ATSCategory);
				if(atsWorkerIt == m_workers.end())
				{
					IEventWorker* atsHistWorker = new ATSHistoricalWorker(this);
					atsHistWorker->addWork( searchfilter, seq_ID);
					addWorker(TA_Base_Core::ATSCategory, atsHistWorker);
				}
				else
				{
					atsWorkerIt->second->addWork( searchfilter, seq_ID);
				}
			}
		
		}
		
		m_ATSArrived = false;
		m_ISCSArrived = false;

	}


	void EventHandler::clearList()
	{

		TA_Base_Core::ThreadGuard guard(m_storedlistLock);
		TA_Base_Core::ThreadGuard displayGuard(m_displaylistLock);

		//clear m_displayList and m_eventStoreList
		m_displaylist.clear();
		{
			// All event items not excluded by current filter are stored in client side.
			for(int i=0; i<(int)m_eventStoreList.size(); i++)
			{
				delete m_eventStoreList[i];
				m_eventStoreList[i] = NULL;
			}
			m_eventStoreList.clear();

			m_eventIdSet.clear();
		}
	}

	void EventHandler::clearDisplayList()
	{
		TA_Base_Core::ThreadGuard displayGuard(m_displaylistLock);

		for(int i=0; i<(int)m_displaylist.size(); i++)
		{
			delete m_displaylist[i];
			m_displaylist[i] = NULL;
		}
		m_displaylist.clear();
	}

	void EventHandler::clearEventStoreList()
	{
		TA_Base_Core::ThreadGuard guard(m_storedlistLock);
		m_eventStoreList.clear();

		m_eventIdSet.clear();

	}
	
	
	
	void EventHandler::clearToBeDelList(vector<DisplayItem*> toBeDelList)
	{

		//clear m_toBeDelList
		for(int i=0; i<toBeDelList.size(); i++)
		{
			delete toBeDelList[i];
			toBeDelList[i] = NULL;
		}
		toBeDelList.clear();
	}

	void EventHandler::addWorker(TA_Base_Core::EventCategory eventCategory, TA_Base_App::IEventWorker* worker)
	{
		m_workers.insert(std::map<TA_Base_Core::EventCategory,IEventWorker*>::value_type(eventCategory, worker) );

 		worker->start();

	}

	// this will do terminate() first, then free vector m_worker
	void EventHandler::releaseWorkers()
	{

		std::map<TA_Base_Core::EventCategory,IEventWorker*>::iterator workerItr;
		for(workerItr=m_workers.begin(); workerItr!=m_workers.end(); workerItr++)
		{
			if( NULL != workerItr->second)
			{
				//to do : test here
				workerItr->second->terminateAndWait();
					//TerminateThread(); 
				delete workerItr->second;
				workerItr->second = NULL;
			}
		}	

		m_workers.clear();
	}

	void EventHandler::stopCurrentTask()
	{
		if (m_taskSeqID>0){
			TA_Base_Core::ThreadGuard guard(m_taskIDLock);
			m_CurrentTaskStopped = true;
		}		
	}

	bool EventHandler::IsThisTaskStopped( long taskID) 
	{
		TA_Base_Core::ThreadGuard guard(m_taskIDLock);
		if(taskID <m_taskSeqID || (taskID==m_taskSeqID && m_CurrentTaskStopped == true))
		{
			return true;
		}
		return false;
	}

	void EventHandler::setCategoryArrived(TA_Base_Core::EventCategory  eventCategory)
	{
		if(eventCategory==TA_Base_Core::ATSCategory)
		{
			m_ATSArrived = true;
		}
		else
		{
			m_ISCSArrived = true;
		}
	}

	void EventHandler::ATSSubscriptionErrNotification()
	{
		reload();
	}

	void EventHandler::reload()
	{
		m_GUIObserver->reloadEvents();
	}

	//
	void EventHandler::statusReadyNotification(TA_Base_Core::EventCategory eventCategory,long seq_ID)
	{
		//the task ID is old
		if(IsThisTaskStopped(seq_ID))
		{
			return;
		}
		if(eventCategory == TA_Base_Core::ATSCategory)
		{
			m_GUIObserver->statusBarUpdate(0, TA_Base_Core::ATSCategory,STATUS_ATS_READY);
		}
		else
		{
			m_GUIObserver->statusBarUpdate(0, TA_Base_Core::ISCSCategory,STATUS_ISCS_READY);
			//m_lastISCSStatusNotReady = false;
		}
		return;
	}
	//update status->unhealthy/Ready
	//For ISCS, will only update to "unhealthy" when both(DB and Agent) is unhealthy. 
	void EventHandler::EventNotification(TA_Base_Core::EventCategory eventCategory, std::vector<TA_Base_Core::ICombinedEventData*> & eventlist, 
		ActionCommand command, long seq_ID,bool backLogJustFinished )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "EventNotification(), EventCategory=%d(ATS:0||ISCS:1), EventSize=%d, taskID=%d, ActionCommand=(0:NoCommand|1:Reconnect), LoadJustFinished=%d",
			eventCategory , eventlist.size(), seq_ID, command , backLogJustFinished);

		//if error received. update status bar to show the unhealthy state and return 
		if (command ==Reconnect)
		{
			if(eventCategory == TA_Base_Core::ATSCategory)
			{
				m_GUIObserver->statusBarUpdate(0, TA_Base_Core::ATSCategory,STATUS_ATS_UNHEALTHY);
			}

			return;
		}
		
		//the task ID is old
		if(IsThisTaskStopped(seq_ID))
		{
			return;
		}

		//the first time a new task has data coming: clean the cache again in case last time there is some dirty data added.
		if(m_lastTaskIDSubmitByWorker<seq_ID)
		{
			TA_Base_Core::ThreadGuard guard(m_storedlistLock);
			if(m_eventStoreList.size()!=0)
			{
				//clear cache, clear GUI again 
				clearAndReserveFor2Lists();
				
			}
			
			//clean GUI List
			clearDisplayList();
			m_GUIObserver->sendListViewUpdateByObserver(0,0);

			m_lastTaskIDSubmitByWorker = seq_ID;
		}
		
		if( eventlist.size()>0 )
		{
			processNewEvents(eventlist, eventCategory ); 

			m_GUIObserver->refreshListView();
		}
		if(backLogJustFinished ==true )
		{
			m_GUIObserver->statusBarUpdate(0, eventCategory, TA_Base_Core::ISCSCategory == eventCategory ? STATUS_ISCS_READY :STATUS_ATS_READY); 
		}
	}




	void EventHandler::transformData(std::vector<TA_Base_Core::ICombinedEventData*> & eventDatas,TA_Base_Core::EventCategory eventCategory, std::vector<DisplayItem*> &eventItems)
	{

		TA_Base_Core::ICombinedEventData* tempData = NULL;
		for ( int i=0; i<eventDatas.size(); i++)
		{	
			try
			{
				tempData = eventDatas[i];
				//save into m_eventStoreList
				CombinedEventItem * newItem = new CombinedEventItem(tempData);

				newItem->setEventCategory(eventCategory) ;
				eventItems.push_back(newItem);	

// 				//release ICombinedEventData
// 				delete (tempData);
// 				tempData = NULL;
// 				eventDatas[i] = NULL;
			}
			catch ( ... )
			{
				// If there was any problem constructing the item don't add it to the display list.
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception caught when new CombinedEventItem from ICombinedEventData.");
				continue;
			}

		}
		//return eventItems;

	}

	void EventHandler::stableSortUsingCurrentRule(std::vector<DisplayItem*> & events)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "stableSortUsingCurrentRule() begin., event size = %d", events.size());

		if(m_mode==RealTimeMode)
		{
			stable_sort(events.begin(), events.end(), compare_events_for_RealTime());
		}
		else
		{
			stable_sort(events.begin(), events.end(), compare_events_for_Hist());
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "stableSortUsingCurrentRule() end.");
	}

	void EventHandler::processNewEvents(std::vector<TA_Base_Core::ICombinedEventData*> & refreshDatas,TA_Base_Core::EventCategory eventCategory)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Before transformData()");

		//transfer ICombinedEventData vector to DisplayItem vector
		std::vector<DisplayItem*> newItems;
		newItems.reserve(refreshDatas.size());
		transformData(refreshDatas,eventCategory, newItems);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "After transformData()");

		TA_Base_Core::ThreadGuard guard1( m_storedlistLock);

		int oldSize = m_eventStoreList.size();
		int cutNum = oldSize + newItems.size() - EventHandlerManager::getMaxEvents() ;
		vector<DisplayItem*> toBeDelList ;

		if(cutNum>0) //event size beyond max, need to cut.
		{
			bool cutFromRear = true;

			//( in realtime handler, s_sortColumnForRealTime/s_sortAscendingForRealTime always mean the same sorting rule 
			// in current m_eventStoreList: Because each time we set these 2 variable, we will sort/reverse the list according to them;
			// and each time we sort/reverse the list, we will reset these rule.

			if( (m_mode==RealTimeMode && s_sortColumnForRealTime == CombinedEventFilter::COLUMN_DATETIME )
				||(m_mode==HistoricalMode && s_sortColumnForHist == CombinedEventFilter::COLUMN_DATETIME ))  //if current m_eventStoreList is sorted on createtime
			{
				
				stableSortUsingCurrentRule(newItems);
				mergeSort(eventCategory, newItems);      //merge it to m_eventStoreList and check EventID set and update EventID set
				
				
				if( (m_mode==RealTimeMode && s_sortAscendingForRealTime == true)
					|| (m_mode==HistoricalMode && s_sortAscendingForHist == true && m_curFilterCriteria.searchDirection == TA_Base_App::Backward)
						||(m_mode==HistoricalMode && s_sortAscendingForHist == false && m_curFilterCriteria.searchDirection == TA_Base_App::Forward) )
				{
					cutFromRear = false;
				}
			}
			else  //if current m_eventStoreList is not sorted on createtime column
			{
				addCache(newItems,eventCategory);     //eventID has been added in the same function
				if(m_mode==RealTimeMode)
				{
					resetSortingRuleForRealTime(CombinedEventFilter::COLUMN_DATETIME, false);
					std::stable_sort(m_eventStoreList.begin(), m_eventStoreList.end(), compare_events_for_RealTime() );
				}
				else
				{
					resetSortingRuleForHist(CombinedEventFilter::COLUMN_DATETIME, false);
					std::stable_sort(m_eventStoreList.begin(), m_eventStoreList.end(), compare_events_for_Hist() );
					if(m_curFilterCriteria.searchDirection == TA_Base_App::Forward)
					{
						cutFromRear = false;
					}
				}
			}
			toBeDelList = prepareCutListFromHeadOrRearByNum(cutNum, cutFromRear);
		}
		else  //events not beyond max size, no need to cut
		{
			stableSortUsingCurrentRule(newItems);
			mergeSort(eventCategory,newItems);//merge to m_eventStoreList base on current sorting rule

		}
		sortAndSynchronizeListAndDelTempList(toBeDelList);
		
	}



	//Before this function is called, both vector m_eventStoreList and vector b has been sorted on "compare_events_for_RealTime".
	//use merge sort algorithm to merge m_eventStoreList and b into m_eventStoreList temp vector c, then copy c into m_eventStoreList.
	//in the same time ,we still need compare b's eventID with IDSet, if it's already existing, we should release the memory in b
	void EventHandler::mergeSort(TA_Base_Core::EventCategory eventCategory,std::vector<DisplayItem*>& b)
	{
		DisplayItem * tempItem = NULL;
		int duplicatedEventIDNum =0;
		TA_Base_Core::ThreadGuard guard(m_storedlistLock);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "merge_sort() begin. size1=%d, size2=%d", m_eventStoreList.size(), b.size());
		bool updateEventIDSet=false;
		if(eventCategory==ISCSCategory && m_mode ==RealTimeMode)
		{
			updateEventIDSet=true;
		}

		std::vector<DisplayItem*> c;
		int aLen=m_eventStoreList.size() ;
		int bLen =b.size();
		c.reserve(aLen + bLen);   

		compare_events_for_RealTime compare;
		int i = 0;
		int j=0;

		//get the smaller one and insert it into c.
		while(i<m_eventStoreList.size() && j<b.size())
		{
			if(compare(m_eventStoreList[i],b[j]))
			{
				c.push_back(m_eventStoreList[i]);
				i++;
			}
			else
			{
				if(updateEventIDSet)
				{
					//id is new
					if (m_eventIdSet.find(b[j]->getEventID())==m_eventIdSet.end())
					{
						m_eventIdSet.insert(b[j]->getEventID().c_str());
						c.push_back(b[j]);
					}else
					{
						duplicatedEventIDNum++;
						tempItem = b[j];
						delete tempItem;
						tempItem = NULL;
						 b[j] =NULL;
					}
				}
				else
				{
					c.push_back(b[j]);
				}
				j++;

			}
		}

		//if m_eventStoreList hasn't been fully accessed: insert the rest into c
		if(i<m_eventStoreList.size())
		{
			for(; i<m_eventStoreList.size(); i++)
			{
				c.push_back(m_eventStoreList[i]);
			}
		}

		//if b hasn't been fully accessed: insert the rest into c
		if(j<b.size())
		{
			for(; j<b.size(); j++)
			{
				if(updateEventIDSet)
				{
					//id is new
					if (m_eventIdSet.find(b[j]->getEventID())==m_eventIdSet.end())
					{
						m_eventIdSet.insert(b[j]->getEventID().c_str());
						c.push_back(b[j]);
					}
					else
					{
						duplicatedEventIDNum++;
						tempItem = b[j];
						delete tempItem;
						tempItem = NULL;
						b[j] =NULL;
					}
				}else
				{
					c.push_back(b[j]);
				}
			}
		}

		//clear m_eventStoreList and copy c into m_eventStoreList
		m_eventStoreList.clear();
		m_eventStoreList.reserve(c.size());
		m_eventStoreList.assign(c.begin(),c.end());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "merge_sort() end. %d events to be merged, %d events deleted for duplicate eventID, %d events added to cache", b.size(),duplicatedEventIDNum, b.size()-duplicatedEventIDNum);
		

	}
	
	bool EventHandler::bothHaveRecord()
	{
		TA_Base_Core::ThreadGuard guard (m_storedlistLock);
		bool ATS_found=false;
		bool ISCS_found = false;
		CombinedEventItem* tempItem = NULL;
		for(int i=0; i<(int)m_eventStoreList.size(); i++)
		{
			tempItem = dynamic_cast<CombinedEventItem*>(m_eventStoreList[i]);
			if ( ISCS_found && ATS_found )
			{
				break;
			}
			if( !ISCS_found && tempItem->getEventCategory() == TA_Base_Core::ISCSCategory)
			{
				ISCS_found = true;
			}else if( !ATS_found && tempItem->getEventCategory() == TA_Base_Core::ATSCategory)
			{
				ATS_found = true;
			}
		}

		return ATS_found&&ISCS_found ;
	
	}



	unsigned long EventHandler::getSizeByCategory(TA_Base_Core::EventCategory eventCategory)
	{
		TA_Base_Core::ThreadGuard guard (m_storedlistLock);
		unsigned long cnt=0;
		for ( unsigned long i=0; i< m_eventStoreList.size(); i++)
		{
			CombinedEventItem* tempItem = dynamic_cast<CombinedEventItem*>(m_eventStoreList[i]);
			if(tempItem->getEventCategory() == eventCategory )
			{
				cnt ++;
			}
		}
		return cnt;
	}
	
	

	std::vector<DisplayItem*> EventHandler::prepareCutListFromHeadOrRearByNum(unsigned long cutNum, bool fromRear)
	{
		TA_Base_Core::ThreadGuard guard1 (m_storedlistLock);

		std::vector<DisplayItem*> toBeDelList; 

		if(!fromRear)  //cut from head
		{
			for(int i=0; i<cutNum; i++)
			{
				CombinedEventItem* tempItem = dynamic_cast<CombinedEventItem*>(m_eventStoreList[i]);
				if( tempItem!=NULL )
				{
					toBeDelList.push_back(tempItem);  //move the old events to the temp list and delete them later.(after display list get the new list)
					if(m_mode==RealTimeMode)
					{
						m_eventIdSet.erase(tempItem->getEventID());
					}
					
				}
			}
			m_eventStoreList.erase(m_eventStoreList.begin(), m_eventStoreList.begin()+cutNum);
		}
		else  //cut from end
		{
			for( int i=m_eventStoreList.size()-1; cutNum>0 && i>=0; i--)
			{
				CombinedEventItem* tempItem = dynamic_cast<CombinedEventItem*>(m_eventStoreList[i]);
				if( tempItem!=NULL )
				{
					toBeDelList.push_back(tempItem);  //move the old events to the temp list and delete them later.(after display list get the new list)
					//m_eventStoreList.erase(m_eventStoreList.begin()+i);
					m_eventStoreList.pop_back();
					if(m_mode==RealTimeMode)
					{
						m_eventIdSet.erase(tempItem->getEventID());
					}
					cutNum --;
				}

			}
		}
		

		return toBeDelList;
	}


	void EventHandler::collectLastItemTimeForBothCategory(timeb & lastISCSItemsTime, timeb & lastATSItemsTime)
	{
		if(m_eventStoreList.size()==0) return;
		CombinedEventItem* tempItem = dynamic_cast<CombinedEventItem*>(m_eventStoreList.back());
		std::vector<DisplayItem *>::iterator itr = m_eventStoreList.end() -1 ;
		if ( tempItem->getEventCategory() == TA_Base_Core::ISCSCategory) 
		{
			lastISCSItemsTime = tempItem->getCreateTime();
			while ( itr != m_eventStoreList.begin() )
			{
				tempItem = dynamic_cast<CombinedEventItem*>( *itr );
				if ( tempItem->getEventCategory() == TA_Base_Core::ATSCategory)
				{
					lastATSItemsTime = tempItem->getCreateTime();
					return;
				}
				else 
				{
					itr --;
				}
			}

			// still need to check the first item because it is not included in the above loop
			tempItem = dynamic_cast<CombinedEventItem*>( m_eventStoreList.front() );
			if ( tempItem->getEventCategory() == TA_Base_Core::ATSCategory)
			{
				lastATSItemsTime = tempItem->getCreateTime();
			}

			return;
		}


		else
		{  
			lastATSItemsTime = tempItem->getCreateTime();
			while ( itr != m_eventStoreList.begin() )
			{
				tempItem = dynamic_cast<CombinedEventItem*>( *itr );
				if ( tempItem->getEventCategory() == TA_Base_Core::ISCSCategory)
				{
					lastISCSItemsTime = tempItem->getCreateTime();
					return;
				}
				else 
				{
					itr --;
				}
			}

			// still need to check the first item because it is not included in the above loop
			tempItem = dynamic_cast<CombinedEventItem*>( m_eventStoreList.front() );
			if ( tempItem->getEventCategory() == TA_Base_Core::ISCSCategory)
			{
				lastISCSItemsTime = tempItem->getCreateTime();
			}
			return;
		}
	}


	void EventHandler::addCache( std::vector<DisplayItem*>& eventItems,TA_Base_Core::EventCategory eventCategory)
	{
		int startSize = eventItems.size();
		int duplicateEvents =0;
		bool updateEventIDSet=false;
		if(eventCategory==ISCSCategory && m_mode ==RealTimeMode)
		{
			updateEventIDSet=true;
		}
		TA_Base_Core::ThreadGuard guard (m_storedlistLock);
		DisplayItem * tempItem=NULL;
		for(int i=0; i<eventItems.size(); i++)
		{
			tempItem = eventItems[i];
			if(updateEventIDSet)
			{
				//if ID not found, add to cache and update ID set
				if(m_eventIdSet.find(tempItem->getEventID())== m_eventIdSet.end())
				{
					m_eventStoreList.push_back(tempItem);
					m_eventIdSet.insert(tempItem->getEventID());
				}	
				else //release the eventItem as it's already existed in cache
				{
					duplicateEvents ++;
					delete tempItem;
					tempItem=NULL;
				}
			}
			else
			{
				m_eventStoreList.push_back(tempItem);
				m_eventIdSet.insert(tempItem->getEventID());
			}
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "addCache() finished. %d events, totally %d events deleted because eventID exists,  %d events added to cache.", startSize,duplicateEvents, startSize-duplicateEvents  );

	}


	void EventHandler::clearAndReserveFor2Lists()
	{
		//clear only m_eventStoreList and m_eventIdSet, m_displayList not cleared. 
		//m_displayList is used to display GUI even when database is down.

		clearEventStoreList();
		
		unsigned long maxEvents = EventHandlerManager::getMaxEvents();

		TA_Base_Core::ThreadGuard guard(m_storedlistLock);
		m_eventStoreList.reserve(maxEvents);
		TA_Base_Core::ThreadGuard displayGuard(m_displaylistLock);	
		m_displaylist.reserve(maxEvents);

	}

	//this function is only called by GUI thread
	void EventHandler::reverseDispListByGUI()
	{
	    TA_Base_Core::ThreadGuard guard (m_displaylistLock);
	 	std::reverse(m_displaylist.begin(), m_displaylist.end());
		
	}

	//This function is only called by GUI sorting(main thread)
	void EventHandler::sortDispListByGUI()
	{
		TA_Base_Core::ThreadGuard guard1 (m_displaylistLock);
		resetSortingRuleForGUI(DisplayItem::s_sortColumn, DisplayItem::s_sortAscending);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Before stable sorting, Event Count = %u", m_eventStoreList.size());
		std::stable_sort(m_displaylist.begin(), m_displaylist.end(), compare_events_For_GUI());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "After stable sorting");
	}



	void EventHandler::sortAndSynchronizeListAndDelTempList(vector<DisplayItem*> toBeDelList)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "sortAndSynchronizeListAndDelTempList() begin.");

		TA_Base_Core::ThreadGuard guard1 (m_storedlistLock);

		if(m_eventStoreList.size() == 0)
		{
			TA_Base_Core::ThreadGuard displayGuard(m_displaylistLock);  
			m_displaylist.clear();
			return;
		}

		if(m_mode == RealTimeMode) //real time mode
		{
			if( s_sortColumnForRealTime == DisplayItem::s_sortColumn ) //if m_eventStoreList is sorting on the same column as UI, 
			{
				if(s_sortAscendingForRealTime != DisplayItem::s_sortAscending)
				{
					//just reverse m_eventStoreList
					s_sortAscendingForRealTime = DisplayItem::s_sortAscending;
					std::reverse(m_eventStoreList.begin(), m_eventStoreList.end());
				}
			}
			else  //if m_eventStoreList is sorting on the different column with UI
			{
				resetSortingRuleForRealTime(DisplayItem::s_sortColumn, DisplayItem::s_sortAscending);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Before stable sorting, Event Count = %u", m_eventStoreList.size());
				std::stable_sort(m_eventStoreList.begin(), m_eventStoreList.end(), compare_events_for_RealTime());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "After stable sorting");
			}
		}
		else  //historical mode
		{
			if( s_sortColumnForHist == DisplayItem::s_sortColumn ) //if m_eventStoreList is sorting on the same column as UI, 
			{
				if(s_sortAscendingForHist != DisplayItem::s_sortAscending)
				{
					//just reverse m_eventStoreList
					s_sortAscendingForHist = DisplayItem::s_sortAscending;
					std::reverse(m_eventStoreList.begin(), m_eventStoreList.end());
				}
			}
			else  //if m_eventStoreList is sorting on the different column with UI
			{
				resetSortingRuleForHist(DisplayItem::s_sortColumn, DisplayItem::s_sortAscending);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Before stable sorting, Event Count = %u", m_eventStoreList.size());
				std::stable_sort(m_eventStoreList.begin(), m_eventStoreList.end(), compare_events_for_Hist());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "After stable sorting");
			}
		}

		TA_Base_Core::ThreadGuard displayGuard(m_displaylistLock); 
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Before assigning");
		m_displaylist.clear();
		m_displaylist.reserve(m_eventStoreList.size());
		m_displaylist.assign(m_eventStoreList.begin(), m_eventStoreList.end());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "After assigning");

		clearToBeDelList(toBeDelList);		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "sortAndSynchronizeListAndDelTempList() end.");
	}
	


	unsigned long EventHandler::getDislplaySize()
	{
		
		TA_Base_Core::ThreadGuard guard (m_displaylistLock);
		return m_displaylist.size();
	}



	std::string EventHandler::getCreateTimeStrAt(unsigned long pos)
	{
		TA_Base_Core::ThreadGuard guard(m_displaylistLock);

		if( pos<= m_displaylist.size())
		{
			return static_cast<CombinedEventItem*>( m_displaylist[pos])->getCreateTimeStr().c_str();
		}else
		{
			return "";
		}
	}


	bool EventHandler::currentListWasCutted()
	{
		unsigned long ISCSLength = 0;
		unsigned long ATSLength = 0;

		if( m_workers.find(TA_Base_Core::ISCSCategory) != m_workers.end())
		{
			ISCSLength = getSizeByCategory( TA_Base_Core::ISCSCategory);
			if( ISCSLength >= EventHandlerManager::getMaxISCSEvents())
			{
				return true;
			}
		}
		if ( m_workers.find(TA_Base_Core::ATSCategory) != m_workers.end())
		{
			ATSLength = getSizeByCategory( TA_Base_Core::ATSCategory);
			if( ATSLength >= EventHandlerManager::getMaxATSEvents())
			{
				return true;
			}
		}

		if( (ISCSLength + ATSLength) >= EventHandlerManager::getMaxEvents())
		{
			return true;
		}
		
		return false;
	}

	//this is always called by GUI thread
	time_t EventHandler::getEarliestTimeFromList()
	{
		TA_Base_Core::ThreadGuard guard1 (m_displaylistLock);

		std::vector<DisplayItem*>::iterator itr;

		resetSortingRuleForGUI(CombinedEventFilter::COLUMN_DATETIME, true);
		itr = std::min_element(m_displaylist.begin(), m_displaylist.end(),compare_events_For_GUI() );
	

		if(itr!=m_displaylist.end())
		{
			CombinedEventItem* tempItem = dynamic_cast<CombinedEventItem*>(*itr);
			return tempItem->getCreateTime().time;
		}
		else
		{
			return 0;
		}

	}

	//this is always called by GUI thread
	time_t EventHandler::getLatestTimeFromList()
	{
		TA_Base_Core::ThreadGuard guard1 (m_displaylistLock);

		std::vector<DisplayItem*>::iterator itr;

		resetSortingRuleForGUI(CombinedEventFilter::COLUMN_DATETIME, true);
		itr = std::max_element(m_displaylist.begin(), m_displaylist.end(),compare_events_For_GUI() );
		

		if(itr!=m_displaylist.end())
		{
			CombinedEventItem* tempItem = dynamic_cast<CombinedEventItem*>(*itr);
			return tempItem->getCreateTime().time;
		}
		else
		{
			return 0;
		}


	}

	void EventHandler::setBothPkeyRightBoundary(FilterCriteria & filterCriteria) 
	{
		// use eventStoreList to find the pkey right boundary
		TA_Base_Core::ThreadGuard guard (m_storedlistLock);

		CombinedEventItem* tempItem = NULL;
		unsigned long tempKey = 0; 
		std::string tempATSEventID = "";
		/*unsigned*/ long tempTime_t = 0;

		// For ISCS:
		if( m_workers.find(TA_Base_Core::ISCSCategory) != m_workers.end())
		{
			// set ISCS biggest pkey to filterCriteria:
			for(int i=0; i<(int)m_eventStoreList.size(); i++)
			{
				tempItem = dynamic_cast<CombinedEventItem*>(m_eventStoreList[i]);
				if(tempItem!=NULL && tempItem->getEventCategory() == TA_Base_Core::ISCSCategory 
					)
				{
					if( tempTime_t < tempItem->getCreateTime().time || (tempTime_t == tempItem->getCreateTime().time &&tempItem->getKey()> tempKey)) 
					{
						tempKey = tempItem->getKey();
						tempTime_t = tempItem->getCreateTime().time;
					}
					
				}
			}
			filterCriteria.pkeyBoundary_ISCS = tempKey;
		}
		else
		{
			filterCriteria.pkeyBoundary_ISCS = 0;
		}

		//For ATS
		if( m_workers.find(TA_Base_Core::ATSCategory) != m_workers.end())
		{
			tempKey = 0;
			// set ISCS biggest pkey to filterCriteria:
			for(int i=0; i<(int)m_eventStoreList.size(); i++)
			{
				tempItem = dynamic_cast<CombinedEventItem*>(m_eventStoreList[i]);
				if(tempItem!=NULL && tempItem->getEventCategory() == TA_Base_Core::ATSCategory 
					&& tempItem->getKey()> tempKey)
				{
					tempKey = tempItem->getKey();
					tempATSEventID = tempItem->getEventID();
				}

			}
			filterCriteria.pkeyBoundary_ATS = tempATSEventID;
		}
		else
		{
			filterCriteria.pkeyBoundary_ATS = "";
		}
		
	}

	void EventHandler::setBothPkeyLeftBoundary(FilterCriteria & filterCriteria) 
	{
		// use eventStoreList to find the pkey right boundary
		TA_Base_Core::ThreadGuard guard (m_storedlistLock);

		CombinedEventItem* tempItem = NULL;
		unsigned long tempKey = 0; 
		/*unsigned*/ long tempTime_t =0;
		std::string tempATSEventID = "";

		// For ISCS:
		if( m_workers.find(TA_Base_Core::ISCSCategory) != m_workers.end())
		{
			// set ISCS smallest pkey to filterCriteria:
			for(int i=0; i<(int)m_eventStoreList.size(); i++)
			{
				tempItem = dynamic_cast<CombinedEventItem*>(m_eventStoreList[i]);
				
				if ( tempKey ==0 && tempItem!=NULL 
					&& tempItem->getEventCategory() == TA_Base_Core::ISCSCategory )
				{
					tempKey = tempItem->getKey();
					tempTime_t = tempItem->getCreateTime().time;
				}
				else if( tempKey!=0 &&  tempItem!=NULL && tempItem->getEventCategory() == TA_Base_Core::ISCSCategory 
					  /*tempItem->getKey()< tempKey*/ )
				{
					if (tempTime_t> tempItem->getCreateTime().time || (tempTime_t == tempItem->getCreateTime().time &&tempItem->getKey()< tempKey  ))
					{
						tempKey = tempItem->getKey();
						tempTime_t = tempItem->getCreateTime().time;
					}
					
				}
			}
			filterCriteria.pkeyBoundary_ISCS = tempKey;
		}
		else
		{
			filterCriteria.pkeyBoundary_ISCS = 0;
		}

		//For ATS
		if( m_workers.find(TA_Base_Core::ATSCategory) != m_workers.end())
		{
			tempKey = 0;
			// set ATS "smallest" ID to filterCriteria:
			for(int i=0; i<(int)m_eventStoreList.size(); i++)
			{
				tempItem = dynamic_cast<CombinedEventItem*>(m_eventStoreList[i]);
				if ( tempKey ==0 && tempItem!=NULL 
					&& tempItem->getEventCategory() == TA_Base_Core::ATSCategory )
				{
					tempKey = tempItem->getKey();
					tempATSEventID = tempItem->getEventID();
				}
				if(tempKey !=0 && tempItem!=NULL && tempItem->getEventCategory() == TA_Base_Core::ATSCategory 
					 && tempItem->getKey()< tempKey)
				{
					tempKey = tempItem->getKey();
					tempATSEventID = tempItem->getEventID();
				}

			}
			filterCriteria.pkeyBoundary_ATS = tempATSEventID;
		}
		else
		{
			filterCriteria.pkeyBoundary_ATS = "";
		}


	}


	//Will not actually delete the duplicate event. But just mark it as deleted by set eventId to ""
	int EventHandler::mergeEventFromAgent(TA_Base_Core::T_EventItemList* eventSeq)
	{
		int size = eventSeq->size();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Begin merging for events from Agent. Total %d events",size );

		// loop eventSeq, check the eventID, if already exist in m_eventIDSet, mark it as deleted in eventSeq
		std::string tempEventID = "";
		TA_Base_Core::ThreadGuard guard(m_storedlistLock);
		int deleted =0;
		TA_Base_Core::T_EventItemList::iterator it;
		for (it=eventSeq->begin(); it!=eventSeq->end(); it++)
		{
			tempEventID =  (*it)->eventID;
			if(m_eventIdSet.find(tempEventID) != m_eventIdSet.end())  //if can find the eventID in the set
			{
				//need to delete the event in the eventSeq. 
				//what we do here is just set the eventID to "", to implicate that this event should be considered as deleted
				//(*eventSeq)[i].eventID = "";
				strncpy( (*it)->eventID, "",TA_Base_Core::EV_EVENT_ID_SIZE);
				deleted ++;
			}
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Totally %d events deleted during merging.", deleted);
		return deleted;
	}

	//Will delete the duplicate events from vector.
	void EventHandler::mergeEventFromDB( std::vector<TA_Base_Core::ICombinedEventData*> & eventlist)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Begin merging for events from DB, total %d events", eventlist.size());

		// loop eventSeq, check the eventID, if already exist in m_eventIDSet, mark it as deleted in eventSeq
		std::vector<TA_Base_Core::ICombinedEventData*> newEventList;
		std::vector<TA_Base_Core::ICombinedEventData*>::iterator itr = eventlist.begin() ;
		std::string tempEventID = "";
		TA_Base_Core::ThreadGuard guard(m_storedlistLock);
		int deleted =0;
		for (; itr!=eventlist.end(); itr++)
		{
			TA_Base_Core::ICombinedEventData* pEventData = *itr;
			tempEventID = pEventData->getEventID();
			if(m_eventIdSet.find(tempEventID) != m_eventIdSet.end())  //if can find the eventID in the set
			{
				//release the duplicate event:
				delete pEventData;
				pEventData = NULL;

				deleted ++;
			}
			else
			{
				newEventList.push_back(pEventData);
			}
		}
		eventlist = newEventList;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Totally %d events deleted during merging.", deleted);
	}

};

