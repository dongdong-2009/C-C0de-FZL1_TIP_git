// AlarmAccessAdaptor.h: interface for the AlarmAccessAdaptor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AlarmAccessAdaptor_H__D3E3092D_F7C7_4A96_8D7A_9BF11AE5E54F__INCLUDED_)
#define AFX_AlarmAccessAdaptor_H__D3E3092D_F7C7_4A96_8D7A_9BF11AE5E54F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/alarm/ATS_alarm_bridge_library/src/DataStructInfo.h"
#include "DutyAlarmManager.h"
#include "bus/alarm/shared_alarm/src/AllAlarmManager.h"
#include "bus/alarm/alarm_store_library/src/AlarmStoreManager.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Bus
{
	class AlarmAccessAdaptor  
	{
	public:
		// destructor
		virtual ~AlarmAccessAdaptor();
		
		static AlarmAccessAdaptor& getInstance();
		static void freeInstance();

		//create shared duty index 
		void init(ULONG ulMaxAllAlarmCount, bool alwaysRecreateMapfile = false);

		void clean();

		void refreshAlarmbyAgent(unsigned long locationKey, TA_Base_Core::AlarmSequence* alarmList, std::vector<bool>& isPassPreFilters);	
		
		void insertAlarm(TA_Base_Core::AlarmDetailCorbaDef& alarmDetail, bool isPassPreFilter);
		
		void updateAlarm(const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate);
			
		void buildIndexWithDuty(TA_Base_Bus::AlarmStoreManager* pAlarmStoreManager );
         		
		unsigned long removeLocationAlarms(unsigned long locationKey);

		unsigned long removeISCSDutyAlarms();

		// DutyAlarmManager specific methods //         		
		void notifyClientRemap();
		       
        bool RemoveAlarm(std::string strAlarmID);
        
        void buildIndexWithFilter(TA_Base_Bus::AlarmFilterSet* pAlarmFilterSet, fp vfp,int cs_pos,bool isDefault);
        
		bool aliveCheck();
		
		bool isUpdating();
		
		int healthMonitor();

		void UpdateDutyIndex(LocationSubSytemList& changes, TA_Base_Bus::AlarmStoreManager* pAlarmStoreManager);

		TA_Base_Core::AlarmDetailCorbaDef* getAlarm(std::string alarmID);

		void setLatestFASAlarm(unsigned long entityKey);
		unsigned long getLatestFASAlarm();

		void insertOrUpdateAlarm( std::vector<I_ALARM_PROVIDER::AlarmDetail* >& extAlarms, TA_Base_Bus::AlarmStoreManager* pAlarmStoreManager);

		unsigned long removeDataSourceAlarms(unsigned long dataSource);
	private: // functions
		AlarmAccessAdaptor(); 
		AlarmDataItem* _newAlarmItem(TA_Base_Core::AlarmDetailCorbaDef& alarmDetail);
		AlarmDataItem* _newAlarmItem(I_ALARM_PROVIDER::AlarmDetail* alarmDetail);
		void initLocationEntityMap();
	private: // variables
		static AlarmAccessAdaptor* m_pInstance;
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

		AllAlarmManager * m_pAllAlarmManager;
		DutyAlarmManager * m_pDutyAlarmManager;

		TA_Base_Core::NonReEntrantThreadLockable m_lock;

		typedef std::map<unsigned long, TA_Base_Core::IEntityData*> LocationEntityMapT;
		LocationEntityMapT m_locEntityMap;
	};
}

#endif // !defined(AFX_AlarmAccessAdaptor_H__D3E3092D_F7C7_4A96_8D7A_9BF11AE5E54F__INCLUDED_)
