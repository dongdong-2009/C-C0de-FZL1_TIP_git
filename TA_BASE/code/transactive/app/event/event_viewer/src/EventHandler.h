/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/EventHandler.h $
  * @author huirong.luo
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * EventHandler is ...
  *
  */

#if !defined(EA_50DA79A0_39F3_40d0_A22D_BC2460CDAFC5__INCLUDED_)
#define EA_50DA79A0_39F3_40d0_A22D_BC2460CDAFC5__INCLUDED_

#include <map>
#include <set>

#include "EventDisplayMode.h"
#include "SearchStatus.h"
#include "IEventWorkObserver.h"
#include "FilterCriteria.h"
#include "IEventWorker.h"
#include "GUIObserver.h"
#include "app/event/event_viewer/EventListComponent/src/DisplayItem.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/data_access_interface/src/ICombinedEventData.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/event/src/EventItem.h"
using namespace std;


namespace TA_Base_App{


	class EventHandler : public IEventWorkObserver
	{

	public:
		
		/** 
		 * constructor
		 *
		 */ 
		EventHandler(EventDisplayMode dispMode, GUIObserver * observer);

		/** 
		 * desctructor
		 */ 
		virtual ~EventHandler();

		virtual void setATSSubscriptionErr(long err);
		virtual long getATSSubscriptionErr();

		virtual int mergeEventFromAgent(TA_Base_Core::T_EventItemList* eventSeq);

		virtual void mergeEventFromDB( std::vector<TA_Base_Core::ICombinedEventData*> & eventlist);

		virtual void reload();


		//virtual void mergeEventFromDB( std::vector<TA_Base_Core::ICombinedEventData*> & eventlist);
		
		/** 
		 * getSearchStatus
		 *
		 * return search status of the worker who is working for the specific EventCategory.
		 * 
		 * @return SearchStatus, the searchStatus of the specific worker.
		 * @param cat, the EventCategory which the worker is working for.
		 */ 
		SearchStatus getSearchStatus(TA_Base_Core::EventCategory cat);


		/** 
		 * getColumnData
		 * 
		 * get the columndata(will transform it sometimes, e.g. AVL status, MMS status) 
		 * from displaylist by the given index and column Name
		 * 
		 * @return string,the columndata
		 * @param index, the given index of the displaylist
		 * @param columnName, the column name.
		 */ 
		const std::string getColumnData (unsigned long index,string columnName);

		//void setColumnData (unsigned long index,string columnName, string value);

		/** 
		 * getCompleteColumnData
		 *
		 * get the completeColumnData ( doesn't transform anything) from displaylist by the given index and columnName
		 * 
		 * @return string,the columndata
		 * @param index, the given index of the displaylist
		 * @param columnName, the column name.
		 */
		const std::string getCompleteColumnData (unsigned long index,string columnName);

		/** 
		 * startParameter
		 *
		 * clear the 2 list(displaylist and storelist) and update the UI, set the parameters into the member variable
		 * initialize the necessary worker(s) if it(they) haven't been initialized. And set the new task information to the
		 * worker(s)' next task.
		 * 
		 * @param searchfilter, the filter criteria for this task.
		 * @param seq_ID, the sequence id for this task
		 * @param ISCSEnabled, the ISCS switch value. If true, need to query for ISCS, if false, ignore ISCS system
		 * @param ATSEnabled, the ATS switch value. If true, need to query for ATS, if false, ignore ATS system
		 */
		void startParameter(FilterCriteria searchfilter, long seq_ID,bool ISCSEnabled=true, bool ATSEnabled=true);

		/** 
		 * clearList
		 *
		 * For clearing the 3 list (m_toBeDelList, m_dispalylist and m_eventStoreList), and also free the memory of eventStoreList
		 */
		void clearList();

		/** 
		 * clearDisplayList
		 *
		 * For clearing m_dispalylist  and also free the memory of m_dispalylist
		 */
		void clearDisplayList();

		/** 
		 * clearEventStoreList
		 *
		 * For clearing  m_eventStoreList and m_eventIdSet
		 */
		void clearEventStoreList();
		

		/** 
		 * clearToBeDelList
		 *
		 * For clearing the temp list from memory
		 */
		void clearToBeDelList(vector<DisplayItem*> toBeDelList);

		/** 
		 * addWorker
		 *
		 * add a worker to the map m_workers with the given EventCategory value for the key
		 * but not start the thread
		 * 
		 * @param category, the key of the new item being added to the map.
		 * @param worker, the value of the new item being added to the map.
		 */
		void addWorker(TA_Base_Core::EventCategory category, IEventWorker * worker);

		/** 
		 * stopCurrentTask
		 * 
		 * set the flag m_CurrentTaskStopped as true. 
		 * If the current mode is Realtime and there is an ATSRealTimeWorker, it will also stop the last subsription.
		 *
		 */
		void stopCurrentTask();

		/** 
		 * IsThisTaskStopped
		 *
		 * return whether the task has been stopped given by the taskID. 
		 * If the taskID is not the latest one or the the flag m_CurrentTaskStopped is false, return true.
		 * 
		 * @return bool, true if the task is stopped, false if it's not stopped (means is still running.)
		 * @param taskID, the given task ID
		 */
		virtual bool IsThisTaskStopped( long taskID) ;

		/** 
		 * releaseWorkers
		 *
		 * This function will terminate all the thread (workers) first, then free vector m_worker
		 */
		void releaseWorkers();

		/** 
		 * EventNotification
		 *
		 * This method is for receiving the new data from workers, update displaylist and eventstorelist,
		 * and send message to update UI.
		 *
		 * @param cat, the Event category of the newly received events
		 * @param eventlist, the new events received.
		 * @param command, the error code for this transact
		 * @param seq_ID, the task sequence ID which these data belong to.
		 * @param isLoadedData, whether the new events is get by database loading or ATS backlog data.
		 *  (i.e. query result for historic mode, or backlog result for realtime mode).
		 * @param loadFinished, for the loadedData(isLoadedData=true), whether the loading proceess has been finished.  
		 *
		 */
		virtual void EventNotification(TA_Base_Core::EventCategory cat, 
			std::vector<TA_Base_Core::ICombinedEventData*> & eventlist , 
			ActionCommand command,long seq_ID, bool backLogJustFinished=false) ;



		virtual void ATSSubscriptionErrNotification();

		/** 
		 * clearAndReserveFor2Lists
		 *
		 * clear the 2 list(including free memory) and reserve for the given size for both lists.
		 * 
		 * @param maxEvents, the size which the 2 lists will reserve.
		 */
		void clearAndReserveFor2Lists();

		/** 
		 * reverseDispListByGUI
		 *
		 * reverse m_eventStorlist and then synchronize m_displaylist by m_storelist
		 */
		void reverseDispListByGUI();

	   /** 
	    * sortNSynchronizeList
		*
	    * sort m_eventStorlist and then synchronize m_displaylist
		*/
		//void sortAndSynchronizeList();

		void sortDispListByGUI();

		//void sortDispListByWorker();

		//void assignToDispList();

		//void assignToDispListAndSortDispList();

		void sortAndSynchronizeListAndDelTempList(vector<DisplayItem*> toBeDelList);


		/** 
		 * IsAllTheWorkerFinishedLoading
		 *
		 * check the searching status of all the workers in the map m_workers, 
		 * if any of them woth status "InSearching",return false, else return true.
		 *
		 */
		//bool IsAllTheWorkerFinishedLoading();

		/** 
		 * getDislplaySize
		 *
		 * return the size of m_displaylist 
		 */
		unsigned long getDislplaySize();

		/** 
		 * getCreateTimeStrAt
		 *
		 * Find the item in m_displaylist with the given index,then return the createTime string of it.
		 *
		 * @return string, the string of the createtime
		 *
		 * @param pos, the index for the m_displaylist which we want to get createtime 
		 */
		std::string getCreateTimeStrAt(unsigned long pos);


		/** 
		 * sortAndCutByMaxSizeIfNeeded
		 *
		 * compare size to the max size first, if beyond, sort m_eventStoreList by time and 
		 * cut the records in m_eventStoreList from rear ; else do nothing.
		 * 
		 */
		//void sortAndCutByMaxSizeIfNeeded (bool timeAscending);

		/** 
		 * sortAndPrepareCutListByMaxSizeIfNeeded
		 *
		 * compare size to the max size first, if beyond, sort m_eventStoreList by time and 
		 * prepare the records to be cut which is moved from m_eventStoreList from rear ; else do nothing.
		 * 
		 *  @return vector<DisplayItem*>, the list of events moved from m_eventStoreList, if return vector.size>0, means sorting done in this func.
		 *   otherwise, means no sorting done.
		 */
		//vector<DisplayItem*>  sortAndPrepareCutListByMaxSizeIfNeeded ( bool timeAscending);

		/** 
		 * cutLastByCategoryAndNum
		 * 
		 * This function is called with the premis that the m_eventStoreList has been sorted by createtime in descending order. 
		 * It will remove the specific number of events with the specific EventCategory from the end to front of m_eventStoreList.
		 * It will also free the momory of the deleted records.
         * 
		 * @param eventCategory, the specif EventCategory we are removing.
		 * @param cutNum, the number we will remove the event with the specific EventCategory(ATS/ISCS)
		 *
		 */
		//void cutLastByCategoryAndNum( TA_Base_Core::EventCategory eventCategory, unsigned long cutNum);


		/** 
		 * cutLastByNum
		 *
		 * This function is to  cut list from rear of m_eventStoreList, by a given number.
		 * @param cutNum, the number we will remove the event with the specific EventCategory(ATS/ISCS)
		 *
		 */
		//void cutLastByNum(unsigned long cutNum);

		/** 
		 * prepareCutListFromHeadOrRearByNum
		 *
		 * This function is to prepare a cut list which is moved from head or rear of m_eventStoreList, by a given number.
		 *
		 * @return vector<DisplayItem*>, the list of events moved from m_eventStoreList hear or rear by the given number
		 * @param cutNum, the number we will remove the event with the specific EventCategory(ATS/ISCS)
		 *
		 */
		std::vector<DisplayItem*> prepareCutListFromHeadOrRearByNum(unsigned long cutNum,  bool fromRear);

		// count the number in m_eventStoreList with the given EventCatogery
		/** 
		 * getSizeByCategory
		 *
		 * count the size of events in m_eventStoreList which are with the given EventCategory
		 *
		 * @return unsigned long, the size of events in m_storedlistLock with the given EventCategory
		 * @param eventCategory, the specific EventCategory we are counting.
		 */
		unsigned long getSizeByCategory(TA_Base_Core::EventCategory eventCategory);

		/** 
		 * currentListWasCutted
		 *
		 * For all the EventCategory, if the size of the events in m_eventStoreList is smaller than the max size, 
		 * then we consider the list has not been cutted before and return false.
		 * then we consider the list has been cutted before and return true.
		 *
		 */
		bool currentListWasCutted();

		/** 
		 * getEarliestTimeFromList
		 * 
		 * find the smallest(earliest) createtime in m_eventStoreList and return it.
		 * 
		 * @return time_t, the time_t value of the earliest createtime
		 */
		time_t getEarliestTimeFromList();

		/** 
		 * getLatestTimeFromList
		 * 
		 * find the biggest(latest) createtime in m_eventStoreList and return it.
		 * 
		 * @return time_t, the time_t value of the latest createtime
		 */
		time_t getLatestTimeFromList();

		/** 
		 * setBothPkeyRightBoundary
		 * 
		 * For ATS events in m_eventStoreList, find the latest ATSEventID and set it to pkeyBoundary_ATS
		 * For ISCS events in m_eventStoreList, find the latest pkey value and set it to pkeyBoundary_ISCS
		 * 
		 * @param filterCriteria, the filter Criteria which will be set value in pkeyBoundary_ATS and pkeyBoundary_ISCS
		 */
		void setBothPkeyRightBoundary(FilterCriteria & filterCriteria) ;

		/** 
		 * setBothPkeyLeftBoundary
		 * 
		 * For ATS events in m_eventStoreList, find the earliest ATSEventID and set it to pkeyBoundary_ATS
		 * For ISCS events in m_eventStoreList, find the earliest pkey value and set it to pkeyBoundary_ISCS
		 * 
		 * @param filterCriteria, the filter Criteria which will be set value in pkeyBoundary_ATS and pkeyBoundary_ISCS
		 */
		void setBothPkeyLeftBoundary(FilterCriteria & filterCriteria) ;

		/** 
		 * setCategoryArrived
		 * 
		 * set m_ATSArrived or m_ISCSArrived to true, depending on the parameter
		 * 
		 * @param eventCategory, determin which flag to be set true
		 */
		void setCategoryArrived(TA_Base_Core::EventCategory  eventCategory);

		/** 
		 * bothHaveRecord
		 *
		 * check m_eventStoreList, if both "ATS" event and "ISCS" event can be found, return true. 
		 * Else, return false
		 */
		bool bothHaveRecord();

		/** 
		 * mergeSort
		 *
		 * merge 2 sorted list into the one sorted list.
		 *
		 * @param a, the first sorted list to be merged and also will be save as the result.
		 * @param b, the second sorted list to be merged
		 * 
		 */
		void mergeSort(TA_Base_Core::EventCategory eventCategory,std::vector<DisplayItem*>& b);
		
		/** 
		 * transformData
		 *
		 * transform vector<ICombinedEventData*> to vector<DisplayItem* vector>
		 * 
		 * @param eventDatas, the vector<ICombinedEventData*> to be transformed
		 * @param eventCategory, the event category of the event list 
		 *
		 * @param vector<DisplayItem*> &, the result after transforming
		 */
		void transformData(std::vector<TA_Base_Core::ICombinedEventData*> & eventDatas,TA_Base_Core::EventCategory eventCategory, std::vector<DisplayItem*> &eventItems);
		
		/** 
		 * processNewEvents
		 * 
		 * the process after handler get realtime events (any events from realtime worker, backlog or refresh events)
		 */
		void processNewEvents(std::vector<TA_Base_Core::ICombinedEventData*> & refreshList,TA_Base_Core::EventCategory eventCategory);

		static std::string s_sortColumnForRealTime;
		static bool s_sortAscendingForRealTime;//boolean , whether the sorting order is ascending or descending

		static std::string s_sortColumnForHist;
		static bool s_sortAscendingForHist; //boolean , whether the sorting order is ascending or descending
		
		static std::string s_sortColumnForGUI;
		static bool s_sortAscendingForGUI; //boolean , whether the sorting order is ascending or descending

	/**
     * compare_events_for_RealTime 
	 * 
     * This function is only for sorting in Realtime Handler , and for worker thread (background thread).
     * This struct implements the std::binary_function template. It defines a method for comparing two DisplayItem pointers
     * to deterimine correct sort or positioning order.
     *
     * By using the std::binary_function template, this function can be used in conjunction with standard template functors
     * such as bind_1st and bind_2nd, which makes it quite powerful.
     */
    struct compare_events_for_RealTime : public std::binary_function<DisplayItem*,DisplayItem*,bool>
    {  
        /**
         * operator()
         *
         * Determines the sort order of two DisplayItem pointers. By default, standard template functions will only do
         * pointer comparisons (not useful), so this method de-references the pointers.
         *
         * @param arg1 The "left hand side" of the function
         * @param arg2 The "right hand side" of the function
         *
         * @return True if arg1 should come before arg2 under current sorting conditions.
         *
         * NOTE: This function MUST be declared const, otherwise it will not compile (a restriction of overriding the template)
         */
        bool operator()(DisplayItem* arg1,DisplayItem* arg2) const
        {
            //return *arg1 < *arg2;
			return arg1->compareTo(arg2, EventHandler::s_sortColumnForRealTime, EventHandler::s_sortAscendingForRealTime);
        }
    };


    /**
     * compare_events_for_Hist 
	 * 
     * This function is only for sorting in Historical Handler , and for worker thread (background thread).
     * This struct implements the std::binary_function template. It defines a method for comparing two DisplayItem pointers
     * to deterimine correct sort or positioning order.
     *
     * By using the std::binary_function template, this function can be used in conjunction with standard template functors
     * such as bind_1st and bind_2nd, which makes it quite powerful.
     */
	    struct compare_events_for_Hist : public std::binary_function<DisplayItem*,DisplayItem*,bool>
    {  
        /**
         * operator()
         *
         * Determines the sort order of two DisplayItem pointers. By default, standard template functions will only do
         * pointer comparisons (not useful), so this method de-references the pointers.
         *
         * @param arg1 The "left hand side" of the function
         * @param arg2 The "right hand side" of the function
         *
         * @return True if arg1 should come before arg2 under current sorting conditions.
         *
         * NOTE: This function MUST be declared const, otherwise it will not compile (a restriction of overriding the template)
         */
        bool operator()(DisplayItem* arg1,DisplayItem* arg2) const
        {
            //return *arg1 < *arg2;
			return arg1->compareTo(arg2, EventHandler::s_sortColumnForHist, EventHandler::s_sortAscendingForHist);
        }
    };


    /**
     * compare_events_for_GUI
	 * 
     * This function is only for sorting in Historical/Realtime Handler, for GUI thread (main thread) only
     * This struct implements the std::binary_function template. It defines a method for comparing two DisplayItem pointers
     * to deterimine correct sort or positioning order.
     *
     * By using the std::binary_function template, this function can be used in conjunction with standard template functors
     * such as bind_1st and bind_2nd, which makes it quite powerful.
     */
	    struct compare_events_For_GUI : public std::binary_function<DisplayItem*,DisplayItem*,bool>
    {    

        /**
         * operator()
         *
         * Determines the sort order of two DisplayItem pointers. By default, standard template functions will only do
         * pointer comparrisons (not useful), so this method de-references the pointers.
         *
         * @param arg1 The "left hand side" of the function
         * @param arg2 The "right hand side" of the function
         *
         * @return True if arg1 should come before arg2 under current sorting conditions.
         *
         * NOTE: This function MUST be declared const, otherwise it will not compile (a restriction of overriding the template)
         */
        bool operator()(DisplayItem* arg1,DisplayItem* arg2) const
        {
            //return *arg1 < *arg2;
			return arg1->compareTo(arg2, EventHandler::s_sortColumnForGUI, EventHandler::s_sortAscendingForGUI);
        }
    };

		std::vector<DisplayItem*> m_displaylist;
		std::vector<DisplayItem*> m_eventStoreList;
		std::set<std::string> m_eventIdSet;
		  

		FilterCriteria m_curFilterCriteria;


		//set sorting rule for realtime handler, worker thread
		//This function must be followed by sorting(stable_sort) function. 
		//and should be done together with sorting (as a atomic operation) under the eventSotreList Lock.
		//so that when we recognize the current sorted list is under what rule (sorting column/asc ), by reading s_sortColumnForRealTime,...
		static void EventHandler::resetSortingRuleForRealTime(std::string sColumn, bool sAsc);
		//set sorting rule for historical handler, worker thread
		static void EventHandler::resetSortingRuleForHist(std::string sColumn, bool sAsc);

		//set sorting rule for historical handler, GUI thread
		static void EventHandler::resetSortingRuleForGUI(std::string sColumn, bool sAsc);

	protected:
		

	private:
		bool m_ISCSArrived;
		bool m_ATSArrived;
		std::map <TA_Base_Core::EventCategory,IEventWorker*> m_workers;
		long m_taskSeqID;
		bool m_CurrentTaskStopped;
		EventDisplayMode m_mode;
		GUIObserver* m_GUIObserver;
		TA_Base_Core::NonReEntrantThreadLockable m_taskIDLock;
		long m_ATSSubscriptionErr;
		//bool m_lastISCSStatusNotReady;
		long m_lastTaskIDSubmitByWorker;

		



		/** 
		 * addCache
		 * 
		 * add given vector to the end of m_eventStoreList.
		 *
		 * @param eventItems, the vector which will be added to m_eventStoreList
		 */ 
		void addCache( std::vector<DisplayItem*>& eventItems,TA_Base_Core::EventCategory eventCategory);

		/** 
		 * transferAndAddCache
		 * 
		 * transform the vector add it into m_eventStoreList.
		 * 
		 * @param listToBeAdded, the events to be transformed and added into m_eventStoreList
		 * @param eventCategory, the event category that the new events belong to.
		 */
		//void transformAndAddCache(std::vector<TA_Base_Core::ICombinedEventData*>& listToBeAdded, TA_Base_Core::EventCategory eventCategory);

		/** 
		 * truncateFor2Catogery
		 *
		 * for cutting some part from m_eventStoreList when merging ATS and ISCS
		 * It can only be called when both ATS and ISCS are enabled.
		 * 
		 */
		//void truncateFor2Catogery();

		/** 
		 * collectLastItemTimeForBothCategory
		 *
		 * check the m_eventStorelist from back to front, 
		 * and when find the first ISCS item, save the createTime to lastISCSItemsTime,
		 * when find the first ATS item, save the createTime to lastATSItemsTime,
		 *
		 * @param lastISCSItemsTime, the reference to be set value with createTime of the first found ISCS event 
		 *  when searching m_eventStorelist from back to front.
		 * @param lastATSItemsTime, the reference to be set value with createTime of the first found ATS event 
		 *  when searching m_eventStorelist from back to front.
		 */
		void collectLastItemTimeForBothCategory(timeb & lastISCSItemsTime, timeb & lastATSItemsTime);
		void EventHandler::stableSortUsingCurrentRule(std::vector<DisplayItem*> & events);
		virtual void EventHandler::statusReadyNotification(TA_Base_Core::EventCategory eventCategory,long seq_ID=0);
		/**
		* findBothEndTimeNCut 
		*
		* check m_eventStoreList's ISCS and ATS parts, and put the biggest(latest) timestamp for both part to t1 and t2.
		* And then "cut" some from one part if another part has reach max size.
		*
		* @param t1, reference to save the biggest(latest) time for ISCS, 
		* @param t2, reference used to save the biggest(latest) time for ATS.
		*/
		//void findBothEndTimeAndCut(timeb & t1 , timeb & t2);

	   /**
	    * findBothStartTimeAndCut
		*
		* check m_eventStoreList's ISCS and ATS parts, and put the smallest(earliest) timestamp for both part to t1 and t2.
		* And then "cut" some from one part if another part has reach max size.
		*
		* @param t1, reference to save the smallest(earliest) time for ISCS, 
		* @param t2, reference used to save the smallest(earliest) time for ATS.
		*/
		//void findBothStartTimeAndCut(timeb & t1 , timeb & t2);

		//bool EventHandler::ATSEventsReachMaxLength();

		//bool EventHandler::ISCSEventsReachMaxLength();


		/** 
		 * cutLastOnesWithTimeStampSmallerThan
		 *
		 * cut items of m_eventStorelist(this list should have already been sorted by createtime desc) 
		 * from back to front if timestamp is smaller than the given timestamp,
		 * will stop once the timestamp is not smaller than the given timestamp
		 * and also free memory for those deleted items.
		 *
		 */ 
		//void cutLastOnesWithTimeStampSmallerThan (timeb timePoint);


		/** 
		 * cutLastOnesWithTimeStampSmallerThan
		 *
		 * cut items of m_eventStorelist(this list should have already been sorted by createtime asc) 
		 * from back to front if timestamp is bigger than the given timestamp,
		 * will stop once the timestamp is not bigger than the given timestamp
		 * and also free memory for those deleted items.
		 *
		 */ 
		//void cutLastOnesWithTimeStampBiggerThan (timeb timePoint);
		
	};

};

#endif // !defined(EA_50DA79A0_39F3_40d0_A22D_BC2460CDAFC5__INCLUDED_)
