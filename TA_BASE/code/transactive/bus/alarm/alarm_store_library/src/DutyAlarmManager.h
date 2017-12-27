#if !defined(__DUTY_ALARM_MANAGER__)
#define __DUTY_ALARM_MANAGER__ 

#pragma once

#include <vector>
#include <map>
// #include "AlarmQueue.h" 
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/alarm/shared_alarm/src/AlarmDataItem.h"
#include "bus/alarm/shared_alarm/src/IDutyAlarmAccess.h"
#include "bus/alarm/alarm_store_library/src/SharedAlarmAccessFactory.h"

namespace TA_Base_Bus 
{
	class DutyAlarmManager : public IDutyAlarmAccess
	{
	public: // methods
	
		static DutyAlarmManager& getInstance();
		static void freeInstance();

		//interface 
		void init(bool alwaysRecreateMapfile = false);

		bool insertAlarm(AlarmDataItem* alarmData);	
		
		bool deleteAlarm(std::string alarmID);
		
		bool updateAlarm(AlarmDataItem* alarmData);
				
		unsigned long removeLocationAlarms(unsigned long locationKey, unsigned long dataSource = ISCS_SUBSYSTEM_ID);

		void deleteAllAlarmItems();

		~DutyAlarmManager(void);
		
		unsigned long removeATSAlarms();

		unsigned long removeISCSAlarms();

		void notifyClientRemap();
        
        void buildIndexWithFilter(TA_Base_Bus::AlarmFilterSet* pAlarmFilterSet, fp vfp,int cs_pos,bool isDefault);
        
		bool aliveCheck();
		
		bool isUpdating();
		
		int healthMonitor();

		virtual bool insertAlarmlist(std::vector<AlarmDataItem*>& alarmlist);

		virtual long removeLocSubSystemDutyAlarms(unsigned long ulLocationKey, std::list<unsigned long>& subSystemKeyList, unsigned long dataSource =ISCS_SUBSYSTEM_ID);

		AlarmDataItem* getAlarm(std::string alarmID);

		void setLatestFASAlarm(unsigned long entityKey);
		unsigned long getLatestFASAlarm();

	private: // functions
		DutyAlarmManager();
		DutyAlarmManager( const DutyAlarmManager& DutyAlarmManager);
		DutyAlarmManager& operator=(const DutyAlarmManager &);

	private: // variables
		SharedAlarmAccessFactory* m_pSharedAlarm;
		static DutyAlarmManager*  m_pInstance;

		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;	 
		TA_Base_Core::ReEntrantThreadLockable m_lock;
	};
}


#endif // __DUTY_ALARM_MANAGER__