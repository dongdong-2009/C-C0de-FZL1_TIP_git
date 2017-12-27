///////////////////////////////////////////////////////////
//  DataLogQueue.h
//  Implementation of the Class DataLogQueue
//  Created on:      13-Ê®¶þÔÂ-2010 11:16:17
//  Original author: luohuirong
///////////////////////////////////////////////////////////

#if !defined(EA_4148EB32_3BC6_45ff_BC03_2304413E375A__INCLUDED_)
#define EA_4148EB32_3BC6_45ff_BC03_2304413E375A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"
#include "DataLogValue.h"
#include "core/data_access_interface/datalog/src/DatalogConfigSettingsData.h"
#include "core/data_access_interface/datalog/src/DatalogRecordingData.h"

#include <list>

namespace TA_Base_Bus
{
	typedef std::list<DataLogValue*> DataLogValues;

	class DataLogQueue : public TA_Base_Core::Thread
	{

	public:
		
		DataLogQueue(unsigned long locationKey, std::string dbLogTableName);
		virtual ~DataLogQueue();

		void insertDataLogToCache(DataLogValues& values);		 
		void writeCacheToDB();

		void setBatchSize(int batchSize);
		void setTimeout(int timeout);

		/**
		* 
		*Implement interface of Thread
		* 
		*/		 
		void run();
		void terminate();

		void freeDatalogRecordingDatas(std::vector<TA_Base_Core::DatalogRecordingData*>& records);
		void freeCache();
		void deleteTimeOutRecordsFromCache();
		time_t getLastSavedTime();
		void setLastSavedTime ( time_t time);
		void deleteLogFromCacheByTime(time_t lastSavedTime);
		void popupABatch ( std::vector<TA_Base_Core::DatalogRecordingData*> & dest);
	private:
		
		
		DataLogValues m_datas;
		TA_Base_Core::ReEntrantThreadLockable m_datasLock;
		time_t m_lastSavedTime;
		TA_Base_Core::ReEntrantThreadLockable  m_lastSavedTimeLock;

		bool m_terminated;
		unsigned long m_locationKey;
		std::string m_dbLogTableName;

		int m_batchUpdateSize;
		int m_dbWriteInterval;
		int m_timeout;
		
		static const int SLEEP_TIME; //ms


	};
}
#endif // !defined(EA_4148EB32_3BC6_45ff_BC03_2304413E375A__INCLUDED_)
