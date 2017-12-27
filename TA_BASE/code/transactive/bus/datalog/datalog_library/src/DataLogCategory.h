///////////////////////////////////////////////////////////
//  DataLogCategory.h
//  Implementation of the Class DataLogCategory
//  Created on:      13-Ê®¶þÔÂ-2010 11:16:15
//  Original author: luohuirong
///////////////////////////////////////////////////////////

#if !defined(EA_AB71C281_4F4A_4fbe_BD8F_6DE946BEFFEA__INCLUDED_)
#define EA_AB71C281_4F4A_4fbe_BD8F_6DE946BEFFEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include "core/threads/src/Thread.h"
#include "core/data_access_interface/datalog/src/DatalogConfigSettingsData.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"

#include "DataLogQueue.h"
#include "DataLogDPItem.h"


namespace TA_Base_Bus
{
	class DataLogCategory : public TA_Base_Core::Thread
	{

	public:
	 
		virtual ~DataLogCategory();
		 

		DataLogCategory(unsigned long pkey,unsigned long locationKey, TA_Base_Core::ConfigType configType, 
			std::string& dpTableName,std::string& dpGrpTableName, std::string& dpLogTableName,
			unsigned long versionNum,std::string& dpViewTableName, int maxDelayTimes, int timeoutDelay );
			

		bool Init();
		bool reInit();
		time_t getLastSavedTimeFromQueue ();
		void setLastSavedTime( time_t time);
		void deleteLogFromCacheByTime(time_t lastSavedTime);

		/**
		* 
		*Start and Stop logging data point value
		* 
		*/
		void StartLog();
		void StopLog();

		/**
		* 
		*Implement interface of Thread
		* 
		*/		 
		void run();
		void terminate();

		void freeDataLogItemMap ();
		void freeDatalogValues( DataLogValues & datalogValues);

		unsigned long getPKey();
		unsigned long getLocationKey();
		TA_Base_Core::ConfigType getConfigType();
		std::string getDPTableName();
		std::string getDPGrpTableName();
		std::string getDPLogTableName();
		unsigned long getVersionNum();
		std::string getDPViewTableName();
		void setVersionNum(unsigned long verNum);
		void pullDataLogValuesForEachDP ( std::list<DataLogValue*>& datalogValues );
		void addNewRecordToDatalogValues( std::list<DataLogValue*>& datalogValues, 
			time_t planLogTime_timet ,unsigned long entityKey, double value);

	
		std::map<unsigned long, DataLogDPItem*> getDPItemMap();

		std::vector<unsigned long> getEntityKeys();
		bool entityKeyExits(unsigned long entityKey);
		bool CheckDPChanged();

		
	private:
		unsigned long m_pkey;
		unsigned long m_locationKey;
		TA_Base_Core::ConfigType m_configType;
		std::string m_dpTableName;
		std::string m_dpGrpTableName;
		std::string m_dpLogTableName;
		unsigned long m_versionNum;
		std::string m_dpViewTableName;
		time_t m_lastSavedTime;
		

		TA_Base_Core::ReEntrantThreadLockable m_DataLogItemMapLock;

		typedef std::map<unsigned long, DataLogDPItem*> DataLogDPItemMap;
		DataLogDPItemMap m_DPItemMap;

		bool m_terminated;

		DataLogQueue* m_dataLogQueue;

		int m_timeoutDelay;
		int m_maxDealyTimes;
		int m_sleepTime;
		 
// 		static const int TIME_OUT_DELAY_DEFAULT;
// 		static const int MAX_DELAY_TIMES_DEFAULT;
// 		static const int SLEEP_TIME;
		



	};
}
#endif // !defined(EA_AB71C281_4F4A_4fbe_BD8F_6DE946BEFFEA__INCLUDED_)
