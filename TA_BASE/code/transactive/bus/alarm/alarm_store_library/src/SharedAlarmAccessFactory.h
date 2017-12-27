/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_LM/CODE_TA_BASE_4669_T01271300/transactive/core/data_access_interface/src/ActionAccessFactory.h $
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

#ifndef __SHARED_ALARM_ACCESS_FACTORY_H_INCLUDED__
#define __SHARED_ALARM_ACCESS_FACTORY_H_INCLUDED__

#include <string>
#include <vector>
#include <set>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/alarm/shared_alarm/src/SharedAlarm.h"
#include "bus/alarm/ATS_alarm_bridge_library/src/DataStructInfo.h"

namespace TA_Base_Bus
{
	class SharedAlarmAccessFactory
	{

	public:
        virtual ~SharedAlarmAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an SharedAlarmAccessFactory object.
         */
		static SharedAlarmAccessFactory& getInstance();
		static void freeInstance();

		//zyr++: for map/unmap shared memory.
        void init(bool alwaysRecreateMapfile = false);     
        void updateIndexInfo(TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo& info, int cs_pos);
        
		//zyr++: for map/unmap shared memory (adding two parameters).
        int addClient(const char* server, const char* client, const char* mapEvName, const char* unmapEvName, int& csPos, bool mayExist = true);	//return non zero indicate that the alarm index is ready    
        bool removeClient(const char* server, const char* client, const char* mapEvName, const char* unmapEvName);

		bool unmapSharedMem();
		bool remapSharedMem();
		void notifyClientRemap();
		//++zyr
		void insertOrUpdateAlarm(std::vector<I_ALARM_PROVIDER::AlarmDetail*>& alarmDetails, bool isPassPreFilter);
        bool RemoveAlarm(const char* alarmID);        
        void buildIndexWithFilter(TA_Base_Bus::AlarmFilterSet* pAlarmFilterSet, fp vfp,int cs_pos,bool isDefault);
        
        void clean();
		unsigned long removeLocationAlarms(unsigned long locationKey, unsigned long dataSource =ISCS_SUBSYSTEM_ID);
		unsigned long removeATSAlarms();
		unsigned long removeISCSAlarms();
		bool aliveCheck();

		void setLatestFASAlarm(unsigned long entityKey);
        unsigned long getLatestFASAlarm();

		bool isUpdating();
		int healthMonitor();

		//++ Added for the new AlarmStore Modification
		bool insertAlarm(AlarmDataItem* alarmUpdate);			
		bool deleteAlarm(std::string alarmID);
		bool updateAlarm(AlarmDataItem* alarmData);		
		bool insertAlarmlist(std::vector<AlarmDataItem*>& alarmlist);
		//++ added for dutychange
		long removeLocSubSystemDutyAlarms(unsigned long ulLocationKey, std::list<unsigned long>& subSystemKeyList, unsigned long dataSource =ISCS_SUBSYSTEM_ID);
		AlarmDataItem* getAlarm(std::string alarmID);

		//++ Added for display modification
		void getDisplayData(TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo& info, std::vector<AlarmDataItem*>& pDisplayData, SORT_COLUMN nCol, int nPos, bool bSortAccend, ULONG ulIndexBegin, ULONG ulCacheSize, bool bUpdateIndexInfo);
		AlarmDataItem* getAlarmItemByID(std::string strAlarmID);
		AlarmDataItem* getAlarmItemByIndex(ULONG ulIndex, SORT_COLUMN nCol, int csPos, bool bSortAccend);
		std::vector<TA_Base_Core::AlarmModifyItem*> getAllModifyItems(SORT_COLUMN nCol, int csPos);
		long findItemPosition(std::string strAlarmID, SORT_COLUMN nCol, int nPos, bool bSortAccend);
		void getRangeAlarmID(std::set<std::string>& setAlarmIDs, ULONG ulBegin, ULONG ulEnd, SORT_COLUMN nCol, int nPos, bool bSortAccend);
		void getAvalanchedChildAlarms(int csPos, std::string avalanchedHeadID, std::vector<std::string>& vecChildAlarms);
		std::vector<TA_Base_Core::AlarmModifyItem*> getAlarmModifyItem(std::set<std::string >& vecAlarmIDs);

    private:
        //
        // These are private as this method is a singleton
        //
        SharedAlarmAccessFactory() : m_sa(NULL){m_sa = new TA_Base_Bus::TA_SharedAlarm;};

		
		SharedAlarmAccessFactory( const SharedAlarmAccessFactory& SharedAlarmAccessFactory);
		SharedAlarmAccessFactory& operator=(const SharedAlarmAccessFactory &);

		static SharedAlarmAccessFactory* m_instance;
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

		TA_Base_Bus::TA_SharedAlarm* m_sa;
        TA_Base_Core::ReEntrantThreadLockable m_lock;
    };

} // closes TA_Base_Bus

#endif //__SHARED_ALARM_ACCESS_FACTORY_H_INCLUDED__
