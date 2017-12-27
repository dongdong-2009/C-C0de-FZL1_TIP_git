
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SHAREDALARM_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SHAREDALARM_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef SHAREDALARM_EXPORTS
#define SHAREDALARM_API __declspec(dllexport)
#else
#define SHAREDALARM_API __declspec(dllimport)
#endif
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// This class is exported from the SharedAlarm.dll

#include "bus/alarm/shared_alarm/src/CommonDef.h"
#include "core/alarm/src/AlarmItem.h"
#include "bus/alarm/ATS_alarm_bridge_library/src/DataStructInfo.h"
#include <set>

typedef bool (TA_Base_Bus::AlarmFilterSet::* fp)(const TA_Base_Core::AlarmItem* pAlarmItem);
typedef bool (TA_Base_Bus::AlarmStoreManager::* fp2)(const TA_Base_Core::AlarmItem* pAlarmItem);

extern fp g_fp1;

// forward declaration
namespace TA_Base_Core
{
	class NonReEntrantProcessRWLockable;
};

namespace TA_Base_Bus
{
	class SHAREDALARM_API TA_SharedAlarm {
	public:
		// zyr++: for map/unmap shared memeory

		/* init
		 *
		 * initializes Shared Memory and shared alarm information
		 *
		 * @Param	: bool alwaysRecreateMapfile - determines if Shared Memomy map file is always created. 
					: if true means if shared memory map file is found function will notify clients to unmap 
					: so we can remap a fresh instance of the share memory segment, otherwise function will
					: only clean all the containers and resets all shared alarm information
		 *
		 * @Return	: none
		 */
		void init(bool alwaysRecreateMapfile = false);

		/* updateIndexInfo
		 *
		 * retrieves alarm information by client
		 *
		 * @Param	: IndexInfo& info - reference to a structure where function will store alarm information
					: int cs_pos - position of the client in the client mappings for client specific settings
		 *
		 * @Return	: none
		 */
		void updateIndexInfo(TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo& info, int cs_pos);

		/**
		 * addClient
		 *
		 * @param	: const char* server - the name of the Event 
					: const char* client - the name of the Event used to notify client of alarm updates
					: const char* unmapEvName - the name of Event used to notify client to remap shared memory.
					: const char* mapEvName - the name of Event used to notify client to unmap shared memory.
					: int& csPos - stores client position from the client mappings
					: bool mayExist - use to verify if client has been registered before
		 *
		 * alarm store service will use the named Events to notify all clients to map/unmap shared memory
		 * Note: before addMapSynClient being called the two Events should be created.
		 */
		int addClient(const char* server, const char* client, const char* mapEvName, const char* unmapEvName, int& csPos, bool mayExist = true);	//return non zero indicate that the alarm index is ready
		
		/* removeClient
		 *
		 * remove a Client from the shared memory based on the event name mappings
		 *
		 * @Param	: const char* server - the name of the Event 
					: const char* client - the name of the Event used to notify client of alarm updates
					: const char* unmapEvName - the name of Event used to notify client to remap shared memory.
					: const char* mapEvName - the name of Event used to notify client to unmap shared memory.
		 *
		 * @Return	: bool	- returns true if the client is found and removed successfully otherwise false
		 */
		bool removeClient(const char* server, const char* client, const char* mapEvName, const char* unmapEvName);

		/* insertOrUpdateAlarm
		 *
		 * function tries to find the alarm and perform an update if found otherwise the function creates a new 
		 * instance in the shared memory and inserts in the containers. function calls addAlarmItem
		 *
		 * @Param	: std::vector<I_ATS_ISCS_ALARM::AlarmDetail* >& atsAlarms - vector container for Pointers to alarmDetail to insert to the shared memory
					: bool isPassPreFilter - determines if alarms have passed the pre-filter
		 *
		 * @Return	: None
		 *
		 * @Note	: function is used only for ATS alarms
		 */
		void insertOrUpdateAlarm( std::vector<I_ALARM_PROVIDER::AlarmDetail* >& atsAlarms, bool isPassPreFilter);
		
		/* buildIndexWithFilter
		 *
		 * creates a client specific alarm index container with client specific filters. alarm visibility
		 * will be recalculated based on the new filters.
		 *
		 * @Param	: TA_Base_Bus::AlarmFilterSet* pAlarmFilterSet - Pointer to the alarmFilterSet which hold the filters
					: fp vfp - filter function callback
					: int cs_pos - client position in the shared memory synchronization
					: bool isDefault - determines if filterset is set to default which is no filter applied.
		 *
		 * @Return	: None
		 */
		void buildIndexWithFilter(TA_Base_Bus::AlarmFilterSet* pAlarmFilterSet, fp vfp,int cs_pos,bool isDefault);
		
		/* clean
		 *
		 * remove and deallocate all alarms in the shared memory both ATS and ISCS alarms
		 *
		 * @Param	: none
		 *
		 * @Return	: none
		 */
		void clean();
		
		/* notifyClients
		 *
		 * sets the name event for client synchronization to inform all clients of the changes
		 *
		 * @Param	: none
		 *
		 * @Return	: none
		 */
		static void notifyClients();
		//HP++, 
//		unsigned long getAlarmAmount();

		/* removeLocationAlarms
		 *
		 * removes alarms from the shared memory that matches the location key
		 *
		 * @Param	: unsigned long locationKey - location key
		 *
		 * @Return	: unsigned long - the number of alarms that were remove 
		 */
		unsigned long removeLocationAlarms(unsigned long locationKey, unsigned long dataSource = ISCS_SUBSYSTEM_ID);

		/* removeATSAlarms
		 *
		 * removes all ATS alarms from the shared memory
		 *
		 * @Param	: none
		 *
		 * @Return	: unsigned long - the number of alarms that were remove 
		 */
		unsigned long removeATSAlarms();
		
		/* removeISCSAlarms
		 *
		 * removes all ISCS alarms from the shared memory
		 *
		 * @Param	: none
		 *
		 * @Return	: unsigned long - the number of alarms that were remove 
		 */
		unsigned long removeISCSAlarms();

		/* checkSharedMemory
		 *
		 * used to check shared memory on-line.
		 *
		 * @Param	: none
		 *
		 * @Return	: bool - true if shared memory is still working fine otherwise false
		 */
		bool checkSharedMemory();
		
		/* isUpdating
		 *
		 * used to check shared memory is still busy performing other operations.
		 *
		 * @Param	: none
		 *
		 * @Return	: bool - true if shared memory is still updating otherwise false
		 */
		bool isUpdating();

		/* unmapSharedMem
		 *
		 * used to unmap shared memory.
		 *
		 * @Param	: none
		 *
		 * @Return	: bool - true if shared memory was successfully unmapped otherwise false
		 */
		bool unmapSharedMem();

		/* remapSharedMem
		 *
		 * used to remap shared memory.
		 *
		 * @Param	: none
		 *
		 * @Return	: bool - true if shared memory was successfully remapped otherwise false
		 */
		bool remapSharedMem();

		/* notifyClientRemap
		 *
		 * sets named event for all clients to notify clients to invoke shared memory remap.
		 *
		 * @Param	: none
		 *
		 * @Return	: none
		 */
		void notifyClientRemap();	 

		/* notifyClientUnmap
		 *
		 * sets named event for all clients to notify clients to invoke shared memory unmap.
		 *
		 * @Param	: none
		 *
		 * @Return	: none
		 */
		void notifyClientUnmap();

		/* healthMonitor
		 *
		 * retrieves shared memory status or operation
		 *
		 * @Param	: none
		 *
		 * @Return	: int - returns none zero if an operation failed or if shared memory is not working properly
					otherwise function returns zero.
		 */
		int  healthMonitor();
	
		//////////////////////////////////////////////////////////////////////////
		//++ Added for the new AlarmStore Modification

		/* insertAlarm
		 *
		 * insert a single alarm item to the shared memory if alarm is already existing operation returns false
		 *
		 * @Param	: AlarmDataItem* alarmUpdate - Pointer to alarm item to insert to the shared memory for display
		 *
		 * @Return	: bool	- returns true if the insertion is successful otherwise false
		 */
		bool insertAlarm(AlarmDataItem* alarmUpdate);			

		/* deleteAlarm
		 *
		 * search and deletes the alarm based on the alarm ID. If alarm is not found operation will return false
		 *
		 * @Param	: std::string& alarmID - reference to the alarm ID to delete
		 *
		 * @Return	: bool - returns true on success otherwise false
		 */
		bool deleteAlarm(std::string& alarmID);
		
		/* updateAlarm
		 *
		 * search and updates the alarm that matches the alarmID. 
		 * if alarm is not found in the container function return false
		 *
		 * @Param	: AlarmDataItem* alarmData - Pointer to alarm item for update
		 *
		 * @Return	: bool - returns true on success otherwise false
		 */
		bool updateAlarm(AlarmDataItem* alarmData);	

		/* insertAlarmlist
		 *
		 * insert multiple alarm
		 *
		 * @Param	: std::vector<AlarmDataItem*>& alarmlist - reference to the vector of alarms to insert
		 *
		 * @Return	: bool - returns true on success otherwise false
		 */
		bool insertAlarmlist(std::vector<AlarmDataItem*>& alarmlist);

		//++Noel
		long removeLocSubSystemDutyAlarms(unsigned long ulLocationKey, std::list<unsigned long>& subSystemKeyList, unsigned long dataSource =ISCS_SUBSYSTEM_ID);
		
		/************************************************************************/
		/*                       CLIENT SPECIFIC METHODS                        */
		/************************************************************************/

		/* getDisplayData
		 *
		 * retrieve the alarm information and the display cache data.
		 * user is responsible for releasing the cache items or if user uses the same
		 * cache container then this function will release the memory of any existing cache data.
		 *
		 * @Param	: IndexInfo& info - reference to the IndexInfo to save the alarm information
					: std::vector<AlarmDataItem*>& pDisplayData - reference to the vector container for the display cache alarms
					: SORT_COLUMN nCol - the sorting type to use when retrieving cache data
					: int nPos - client position in the shared memory synchronization
					: bool bSortAccend - specifies the order of sorting
					: ULONG ulIndexBegin - specifies the begining index of the cache data to retrieve
					: ULONG ulCacheSize - specifies the size of the cache data to retrieve
					: bool bUpdateIndexInfo - specify if we should retrieve alarm information
		 *
		 * @Return	: none
		 */
		void getDisplayData(TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo& info, std::vector<AlarmDataItem*>& pDisplayData, SORT_COLUMN nCol, int nPos, bool bSortAccend, ULONG ulIndexBegin, ULONG ulCacheSize, bool bUpdateIndexInfo);
		
		/* getAlarmItemByID
		 *
		 * search the alarm using the alarmID and create a new instance of alarmDataItem
		 * user is responsible for releasing the memory
		 *
		 * @Param	: std::string strAlarmID - alarmID used to identify the alarm to retrieve
		 *
		 * @Return	: AlarmDataItem* - pointer to the new AlarmDataItem
		 */
		AlarmDataItem* getAlarmItemByID(std::string strAlarmID);
		
		/* getAlarmItemByIndex
		 *
		 * get the alarm using the index position of the alarm relative to the sort position 
		 * and create a new instance of alarmDataItem. user is responsible for releasing the memory
		 *
		 * @Param	: ULONG ulIndex - index position of the alarm to retrieve
					: SORT_COLUMN nCol - the sorting type
					: int csPos - client position in the shared memory synchronization
					: bool bSortAccend - specifies the order of sorting
		 *
		 * @Return	: AlarmDataItem* - pointer to the new AlarmDataItem
		 */
		AlarmDataItem* getAlarmItemByIndex(ULONG ulIndex, SORT_COLUMN nCol, int csPos, bool bSortAccend);
	
		/* getAllModifyItems
		 *
		 * retrieves all the visible alarms, alarm details needed for the alarm to be acknowledge
		 * please note that the user is responsible for releasing the memory.
		 *
		 * @Param	: SORT_COLUMN nCol - the sorting type
					: int csPos - client position in the shared memory synchronization
		 *
		 * @Return	: std::vector<TA_Base_Core::AlarmModifyItem*> - vector container for the alarmModifyItems
		 */
		std::vector<TA_Base_Core::AlarmModifyItem*> getAllModifyItems(SORT_COLUMN nCol, int csPos);
		
		/* findItemPosition
		 *
		 * find the index position of the alarm using alarmID based on the sorting type and the sorting order
		 *
		 * @Param	: std::string strAlarmID - Alarm ID to search
					: SORT_COLUMN nCol - the sorting type
					: int nPos - client position in the shared memory synchronization
					: bool bSortAccend - sort order of the display
		 *
		 * @Return	: long - index position of the alarm on success otherwise -1
		 */
		long findItemPosition(std::string strAlarmID, SORT_COLUMN nCol, int nPos, bool bSortAccend);
		
		/* getRangeAlarmID
		 *
		 * retrieves the alarmIDs of alarms specified with the range of index, sort type and sort order of the display
		 *
		 * @Param	: std::set<std::string>& setAlarmIDs - container for the alarmIDs retrieved
					: ULONG ulIndexBegin - specifies the begining index of the range 
					: ULONG ulEnd - specifies the ending index of the range
					: SORT_COLUMN nCol - the sorting type
					: int nPos - client position in the shared memory synchronization
					: bool bSortAccend - sort order of the display
					
		 *
		 * @Return	: none
		 */
		void getRangeAlarmID(std::set<std::string>& setAlarmIDs, ULONG ulBegin, ULONG ulEnd, SORT_COLUMN nCol, int nPos, bool bSortAccend);
		
		/* getAvalanchedChildAlarms
		 *
		 * retrieves the alarmIDs of the child avalanche alarms based on the avalachedHeadID.
		 *
		 * @Param	: int nPos - client position in the shared memory synchronization
					: std::string avalanchedHeadID - the alarmID of the head avalanche
					: std::vector<std::string>& vecChildAlarms - vector container for the alarmIDs of the child avalanche alarms
		 *
		 * @Return	: none
		 */
		void getAvalanchedChildAlarms(int csPos, std::string avalanchedHeadID, std::vector<std::string>& vecChildAlarms);
		
		/* getAlarmModifyItem
		 *
		 * retrieves the alarm detail needed for the alarm to be modified.
		 *
		 * @Param	: std::set<std::string >& vecAlarmIDs - vector containing the alarmIDs to query
		 *
		 * @Return	: std::vector<TA_Base_Core::AlarmModifyItem*> - vector container for the alarm details for modification
		 */
		std::vector<TA_Base_Core::AlarmModifyItem*> getAlarmModifyItem(std::set<std::string >& vecAlarmIDs);
		
		/* freeDisplayData
		 *
		 * deletes and clean the vector of alarmDataItem previously allocated in call to getDisplayData
		 *
		 * @Param	: std::vector<AlarmDataItem*>& pDisplayData - vector containing pointers to alarmDataItem to release
		 *
		 * @Return	: none
		 */
		void freeDisplayData(std::vector<AlarmDataItem*>& pDisplayData);

		/* initDetail
		 *
		 * initialize the shared memory internal information and containers
		 *
		 * @Param	: none
		 *
		 * @Return	: none
		 */
		static void initDetail();

		/* getAlarm
		 *
		 * return a pointer to AlarmDataItem matching the alarmID, note that calling function needs to release 
		 * the memory after using it.	
		 *
		 * @Param	: std::string alarmID - alarmID to search
		 *
		 * @Return	: AlarmDataItem*
		 */
		AlarmDataItem* getAlarm(std::string alarmID);
		void setLatestFASAlarm(unsigned long entityKey);
		unsigned long getLatestFASAlarm();
	private:
		// Log details of alarm for debugging purpose
		void logAlarm(TA_Base_Core::AlarmItem* alarm);
		void logAlarm(const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate);

		// Remove all alarm that matches the location key
		unsigned long removeLocationAlarmsInternal(unsigned long locationKey, unsigned long dataSource = ISCS_SUBSYSTEM_ID);

		// Remove all ATS type alarms
		unsigned long removeATSAlarmsInternal();

		// Remove all ISCS type alarms
		unsigned long removeISCSAlarmsInternal();

		// Verify shared memory is still working
		bool checkSharedMemoryInternal();

		// retrieve a vector of alarms from the container based on the relative position of the alarm according to the sorting type and order
		void getIndexWithSortColInternal(AlarmListT* alarmList, SORT_COLUMN nCol, int nPos, bool bSortAccend, ULONG ulIndexBegin, ULONG ulCacheSize);
	
		// deallocate and clean all alarms in the container
		void cleanInternal();

		// trigger event notification to a specific client
		void notifyClient(const char* name)
		{
			HANDLE ev = OpenEvent(EVENT_ALL_ACCESS, FALSE, name);
			if(ev!=NULL)
			{
				SetEvent(ev);
				CloseHandle(ev);
			}
		}

		// Locks the shared memory and check if shared memory is still working
		bool checkSharedMemoryImpl();
		
		// delete/deallocate all alarm items
		void deleteAllAlarmItems();
	
		// delete/deallocate a single alarm item
		int deleteItem(TA_Base_Core::AlarmItem* pItem);

		// insert a new alarm to the multi-index container
		void inserToDutyIndex(TA_Base_Core::AlarmItem* pAlarmItem);
		
		// check if alarm exceeds the maximum capacity and deallocate the oldest alarm if it exceeds
		// inserts new alarm to the multi-index calling insertToDutyIndex
		// function is specific for ISCS alarms
		bool addAlarmItem(TA_Base_Bus::AlarmDataItem* alarmItem);

		// check if alarm exceeds the maximum capacity and deallocate the oldest alarm if it exceeds
		// inserts new alarm to the multi-index calling insertToDutyIndex
		// function is specific for ATS alarms
		void addAlarmItem(const I_ALARM_PROVIDER::AlarmDetail & alartItem, bool isPassPreFilter);

		// check if the alarm is already existing in the container
		// calls addAlarmItem to insert alarm
		bool insertAlarmInternal(TA_Base_Bus::AlarmDataItem* alarmItem);

		// copies contents of AlarmItem to a new AlarmDataItem for client use.
		AlarmDataItem* _newAlarmDataItem(TA_Base_Core::AlarmItem* pAlarm);

		// logs caught exception error codes
		int handle_remap(EXCEPTION_POINTERS *ep);
	public:
		static TA_Base_Core::NonReEntrantProcessRWLockable m_SharedAlarmDutyLock;
		//static ACE_Process_Mutex s_alarmsMutex;              //("Alarms-Mutex");
		//static ACE_Process_Mutex s_alarmIndexWithDutyMutex;  //("alarmIndexWithDuty-Mutex");
		//static ACE_Process_Mutex s_alarmIndexWithFilterMutex;//("alarmIndexWithFilter-Mutex");

	//*//HP++
	};
}
