// AlarmACKCommandManager.h: interface for the AlarmACKCommandManager class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __ALARM_REQUEST_MANAGER_H_INCLUDED__ 
#define __ALARM_REQUEST_MANAGER_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "CacheDataManager.h"
#include "DataStructInfo.h"

namespace TA_Base_Bus
{
	class AlarmRequestManager : public CacheDataManager<unsigned long, I_ALARM_PROVIDER::AlarmRequest>
	{
	public:
		AlarmRequestManager(long maxBlockCount,long blockByteSize);
		virtual ~AlarmRequestManager();	 
		
		virtual unsigned long readCachDataKeyFromMem(ShareMemoryHelper* mem, char* pos);
		virtual unsigned long getCacheDataKey(I_ALARM_PROVIDER::AlarmRequest* cacheData);
		virtual bool writeCacheDataToMem(I_ALARM_PROVIDER::AlarmRequest* cacheData,ShareMemoryHelper* mem, char* pos);
		virtual bool readCacheDataFromMem(I_ALARM_PROVIDER::AlarmRequest* cacheData,ShareMemoryHelper* mem, char* pos);
 	
// 		struct AlarmACKCommand
// 		{
// 			std::string alarmId; 		// max 38 characters.
// 			std::string alarmAcknowledgeBy;  // operatorId, max 10 characters.
// 			unsigned long ackTime;
// 		};

         //the following attribute should be updated based on the "information" related tables in database.
		enum AlarmRequestCommandAttributeSize
		{				 
			AlarmIDSize=I_ALARM_PROVIDER::ALARM_ID_SIZE+LongSharedMemoryData,           
			OperatorIDSize=I_ALARM_PROVIDER::PROFILE_ID_SIZE+LongSharedMemoryData,
			RequestTimeSize=ULongSharedMemoryData,
			AlarmStateSize=UShortSharedMemoryData,
			TotalSizeLength= AlarmIDSize+ OperatorIDSize+ RequestTimeSize+ AlarmStateSize,
			MaxItemCount=200
		};
	};
		
	 
}

#endif //__ALARM_REQUEST_COMMAND_MANAGER_H_INCLUDED__
