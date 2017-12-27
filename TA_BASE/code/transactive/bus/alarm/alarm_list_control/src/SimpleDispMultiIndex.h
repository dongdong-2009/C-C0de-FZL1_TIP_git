// SimpleDispMultiIndex.h: interface for the CSimpleDispMultiIndex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMPLEDISPMULTIINDEX_H__8B928CA8_8D9B_494F_829F_764EA68AB87E__INCLUDED_)
#define AFX_SIMPLEDISPMULTIINDEX_H__8B928CA8_8D9B_494F_829F_764EA68AB87E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_DISPLAY_ITEM 30000
#include "ace/Task.h"

#include "AlarmVectorWrap.h"
#include "AlarmSelectionWrap.h"
#include "IAlarmDisplayUpdate.h"
#include "bus/alarm/alarm_store_library/src/SharedAlarmAccessFactory.h"

namespace TA_Base_Bus
{	
//	typedef std::map<ULONG, AlarmDataItem*> DisplayMapT;
	typedef std::set<std::string> IndexSel;
	typedef std::vector<AlarmDataItem*> DisplayVecT;

	struct ListDispInfo
	{
		ULONG ulTopIndex;
		ULONG ulDisplayCount;
		ULONG ulPrevPageBuffer;
		ULONG ulNextPageBuffer;
		
		ListDispInfo() {
			ulTopIndex = 0;
			ulDisplayCount = 50; // default page count
			ulPrevPageBuffer = 50;
			ulNextPageBuffer = 50;
		};
	};

	class CSimpleDispMultiIndex : ACE_Task_Base
	{
	public: // methods
		// constructor
		CSimpleDispMultiIndex(ACE_Thread_Manager* thr_mgr = ACE_Thread_Manager::instance(), int syn_timeout = 5000);
		// destructor
		virtual ~CSimpleDispMultiIndex();
				
		virtual int svc();
		inline void start() { activate(); };
		inline void setTarget(HWND target) { m_target = target;	};
		inline bool getCanNotify() { return m_canNotify; };
		
		inline void setCanNotify(bool canNotify)
		{
			m_canNotify=canNotify;		
			if (m_canNotify==false) notifyUpdate(IAlarmDisplayUpdate::NOTIFY_REMAP);
		};

		inline void shutdown()
		{
			thr_mgr_->cancel_task(this);
			BOOL bRes = SetEvent(m_shutDownEv);
			
			if (bRes == false)
			{
				DWORD dwError = GetLastError();
			}
		};
		void notifyAlarmStoreResart();
		void notifyUnmapSharedMem();
		// This function will start the synchronization mechanism
		void initialize();

		// This used to filter the alarm items based on user configuration
		void applyDutyIndexFilter(AlarmFilterSet* pFilterSet);
		
		// Use to dynamically change the sorting mechanism for the display items
		void setDisplaySort(SORT_COLUMN sortType, bool bSortAccending);

		// retrieve the index information of the alarms
		void getIndexInfo(TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo& indexInfo, bool bUpdateFromShared = false);

		// Use to update the containers this method will be called by the synchronization mechanism
//		virtual bool update(/*IN*/ PDutyAlarmData pAlarmData);
			
		// Function to retrieve the display vector //
		AlarmVectorWrap* getDisplayData();
		AlarmSelectionWrap* getSelectionWrapper();

		// Methods for Select/UnSelect alarms
		// This function is used to check if alarm has been selected
		bool isAlarmItemSelected(std::string strAlarmID);
		
		// This function is used add alarms to the selected items
		void selectAlarmItem(unsigned long ulIndex);
		
		// This function is used to add multiple alarms to the selected items
		void selectRangeAlarms(unsigned long ulSelBegin, unsigned long ulSelEnd);

		// This function is used to unselect a particular alarm item.
		void unselectAlarmItem(unsigned long ulIndex);
		
		// Function to unselect all alarm items currently selected
		void unSelectAllAlarmItem();

		unsigned long getSelectedAlarmsCount();

		// Clear All the containers
		void clear();

		// Register an observer to the update alarms
		void registerUpdateObserver(IAlarmDisplayUpdate *pUpdateObserver);

		// Check if the containers is updating
		inline bool isUpdating() { return m_bIsUpdating; };
		
		// notify the observer of the alarm updates
		inline void notifyUpdate(IAlarmDisplayUpdate::NOTIFY_MGS mgs)
		{ 
			if (m_pUpdateObserver != NULL)
				m_pUpdateObserver->updateDisplay(mgs); 
		};

// 		// use to retrieve string attributes in AlarmDataItem using the alarm ID
// 		std::string getItemString(std::string strAlarmID, STRING_ATTR nSubIndex);
// 		
// 		// use to retrieve number attributes in AlarmDataItem using the alarm ID
// 		unsigned long getItemNumber(std::string strAlarmID, NUMBER_ATTR nSubIndex);

		void setListDisplayInfo(ListDispInfo& listInfo);

		void getListDisplayData();

		int getClientPos() { return m_csPos;};

		void setFreezeRefreshCache(bool bFreezeRefreshCache);

		std::string getAlarmID(unsigned long ulIndex);

		// Grant access to private data members
		friend class AlarmVectorWrap;
		friend class AlarmSelectionWrap;

	private: // methods	
		/*
		* Function		: _getItemByIndex 
		* Purpose		: retrieve the AlarmDataItem directly to the Shared Memory
		* Parameters	: ULONG ulIndex		- index position of the AlarmDataItem in the vector list
		* Return		: AlarmDataItem*	- copy of the AlarmDataItem in the given index on success otherwise NULL
		* Note			: return data must be deallocated by the calling function
		*/
		AlarmDataItem* _getItemByIndex(ULONG ulIndex);

		/*
		* Function		: _getItemByID 
		* Purpose		: retrieve the AlarmDataItem directly to the Shared Memory
		* Parameters	: std::string strAlarmID	- AlarmID of the AlarmDataItem to query
		* Return		: AlarmDataItem*			- copy of the AlarmDataItem in the given index on success otherwise NULL
		* Note			: return data must be deallocated by t he calling function
		*/
		AlarmDataItem* _getItemByID(std::string strAlarmID);

		/*
		* Function		: _findItemPosition 
		* Purpose		: find the current position of the AlarmDataItem, this will query directly to shared memory.
		* Parameters	: std::string strAlarmID - AlarmID of the AlarmDataItem to find
		* Return		: long - the current position the specified Alarm on success otherwise -1
		* Note			: None
		*/
		long _findItemPosition(std::string strAlarmID);

		/*
		* Function		: _getAllAlarmModifyItem 
		* Purpose		: retrieve the AlarmModifyItems of all the AlarmDataItems, this will query directly to shared memory.
		* Parameters	: std::vector<TA_Base_Core::AlarmModifyItem*>&allAlarmModifyItems - reference to the vector container which will hold all the AlarmModifyItems retrived.
		* Return		: void
		* Note			: retrieved data must be deallocated by the calling function
		*/
		void _getAllAlarmModifyItem(std::vector<TA_Base_Core::AlarmModifyItem*>&allAlarmModifyItems);

		/*
		* Function		: _getAllVisibleAlarms 
		* Purpose		: retrieve the alarmID of all the visible alarms in list this will query directly to shared memory
		* Parameters	: std::vector<std::string>&allAlarmModifyItems - reference to the vector container which will hold all the alarmID retrived.
		* Return		: void
		* Note			: None
		*/
		void _getAllVisibleAlarms(std::vector<std::string>& allvisibleAlarms);

		/*
		* Function		: _getAlarmModifyItem 
		* Purpose		: retrieve the AlarmModifyItem with the specified alarmID this will query directly to shared memory
		* Parameters	: std::string strAlarmID	- alarmID of the alarm to query
		* Return		: AlarmModifyItem*			- new instance of the AlarmModify item on success otherwise NULL
		* Note			: return data must be deallocated by the calling function
		*/
		std::vector<TA_Base_Core::AlarmModifyItem*> _getAlarmModifyItem(std::set<std::string >& vecAlarmIDs);

		/*
		* Function		: _getItemCacheByIndex 
		* Purpose		: retrieve the AlarmDataItem* in the given index using the current alarms in the cache
		* Parameters	: ULONG ulIndex		- relative position of the alarm in the list
		* Return		: AlarmDataItem*	- pointer to the AlarmDataItem at the given index on success otherwise NULL
		* Note			: None
		*/
		AlarmDataItem* _getItemCacheByIndex(ULONG ulIndex, bool bRequestAddCache = false);

		/*
		* Function		: _getItemCacheByID 
		* Purpose		: retrieve the AlarmDataItem* with the given specified alarmID using the current alarms in the cache
		* Parameters	: std::string& strAlarmID	- alarmID to query
		* Return		: AlarmDataItem*			- pointer to the AlarmDataItem at the given index on success otherwise NULL
		* Note			: None
		*/
		AlarmDataItem* _getItemCacheByID(std::string& strAlarmID);

		/*
		* Function		: _getStringAttribute 
		* Purpose		: Retrieve the string property in the alarmDataItem
		* Parameters	: AlarmDataItem* pAlarmItem		- pointer the alarmDataItem
						  STRING_ATTR nSubIndex			- index of the string property
						  std::string& strResult		- string container to hold the query result
		* Return		: void 
		* Note			: None
		*/
		void _getStringAttribute(AlarmDataItem* pAlarmItem, STRING_ATTR nSubIndex, std::string& strResult);

		/*
		* Function		: _getNumberAttribute 
		* Purpose		: Retrieve the Number property in the alarmDataItem
		* Parameters	: AlarmDataItem* pAlarmItem		- pointer the alarmDataItem
						  NUMBER_ATTR nSubIndex			- index of the Number property
						  unsigned long & ulResult		- Number container to hold the query result
		* Return		: void 
		* Note			: None
		*/
		void _getNumberAttribute(AlarmDataItem* pAlarmItem, NUMBER_ATTR nSubIndex, unsigned long & ulResult);

		/*
		* Function		: _getAvalanchedChildAlarms 
		* Purpose		: Retrieve all the alarmID of the child avalanche alarms related to the give alarmID of the Head avalanche alarm, this will query directly to shared memory
		* Parameters	: std::string avalanchedHeadID	- alarmID of the Head avalanche
						  std::vector<std::string>& vecChildAlarms	- vector container for the alarmID of the child avalanche alarms.
		* Return		: void 
		* Note			: None
		*/
		void _getAvalanchedChildAlarms(std::string avalanchedHeadID, std::vector<std::string>& vecChildAlarms);


		void _cleanInternals();

		class SharedMemSync : public TA_Base_Core::Thread
		{
		public:
			SharedMemSync(SYN_EVENT& remapEv, SYN_EVENT& unmapEv, CSimpleDispMultiIndex* parent);
			~SharedMemSync();
			
			void run();
			void terminate();
			
		private:
			SYN_EVENT m_termiateEv;
			SYN_EVENT& m_remapEv;
			SYN_EVENT& m_unmapEv;
			
			bool m_terminated;
			CSimpleDispMultiIndex* m_parent;
			//static int m_mapShmemRef;
			int m_mapShmemRef;
		};

		friend class SharedMemSync;
	private: // variables
		TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo m_indexInfo;
		TA_Base_Core::NonReEntrantThreadLockable m_ThreadLock;
				
		// For currently selected items 
		IndexSel m_SelectedAlarms;
		AlarmSelectionWrap* m_pSelectionWrapper;

		// Vector Container for Display items
		AlarmVectorWrap * m_pDisplayVectWrapper;

		//Container for Real-time Display
		DisplayVecT m_displayVec; // primary cache
		DisplayVecT m_tempCacheVec; // secondary cache

		//determine if we need to refresh cache memory
		bool m_bRefreshCache;

		// This is used to halt all refresh in the cache data
		// so that display will not be distorted during column resize.
		bool m_bFreezeRefreshCache;

		// Sort type
		SORT_COLUMN m_sortCol;

		// Sorting Order
		bool m_bSortAccending;

		ListDispInfo m_listDispInfo;

		bool m_bIsUpdating;

		// Update Observer //
		IAlarmDisplayUpdate* m_pUpdateObserver;
		
		// Current position of the client in the shared memory filter index
		int m_csPos;
		 
		SharedMemSync* m_sharedMemsyn;
		std::string m_remapEvName;
		std::string m_unmapEvName;
		SYN_EVENT m_remapMemEv;
		SYN_EVENT m_unmapMemEv;

		// used for server synchronize client
		SYN_EVENT m_server;			
	
		// used for client synchronize server
		SYN_EVENT m_client;
		SYN_EVENT m_shutDownEv;
		SYN_EVENT m_rebuildEvent;
		
		//wait time out
		const int m_waitTime;
		
		//target window
		HWND m_target;
		bool m_canNotify;
		unsigned long m_dutyAlarmVersion;
		
		std::string m_servername;
		std::string m_clientname;
		TA_Base_Bus::SharedAlarmAccessFactory* m_pSharedAlarm;

		// This variable is for Virtual index of the vector display // primary cache
		ULONG m_ulCacheBegin;
		ULONG m_ulCacheEnd;

		// This variable is for Virtual index of the secondary cache //
		ULONG m_ulSecCacheBegin;
		ULONG m_ulSecCacheEnd;

		// Variable to indicate synchronization has already started
		bool m_bInitialize;
	};
}

#endif // !defined(AFX_SIMPLEDISPMULTIINDEX_H__8B928CA8_8D9B_494F_829F_764EA68AB87E__INCLUDED_)
