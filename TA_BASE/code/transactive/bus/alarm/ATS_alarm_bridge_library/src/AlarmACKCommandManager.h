// AlarmACKCommandManager.h: interface for the AlarmACKCommandManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AlarmAlarmACKCommandManager_H__2BA8A561_4EB6_4F8C_A003_B1CC88A86160__INCLUDED_)
#define AFX_AlarmAlarmACKCommandManager_H__2BA8A561_4EB6_4F8C_A003_B1CC88A86160__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "CacheDataManager.h"
#include "DataStructInfo.h"

namespace TA_Base_Bus
{
	class AlarmACKCommandManager : public CacheDataManager<unsigned long, I_ALARM_PROVIDER::AlarmACKCommand>
	{
	public:
		AlarmACKCommandManager(long maxBlockCount,long blockByteSize);
		virtual ~AlarmACKCommandManager();	 
		
		virtual unsigned long readCachDataKeyFromMem(ShareMemoryHelper* mem, char* pos);
		virtual unsigned long getCacheDataKey(I_ALARM_PROVIDER::AlarmACKCommand* cacheData);
		virtual bool writeCacheDataToMem(I_ALARM_PROVIDER::AlarmACKCommand* cacheData,ShareMemoryHelper* mem, char* pos);
		virtual bool readCacheDataFromMem(I_ALARM_PROVIDER::AlarmACKCommand* cacheData,ShareMemoryHelper* mem, char* pos);
 	
// 		struct AlarmACKCommand
// 		{
// 			std::string alarmId; 		// max 38 characters.
// 			std::string alarmAcknowledgeBy;  // operatorId, max 10 characters.
// 			unsigned long ackTime;
// 		};

         //the following attribute should be updated based on the "information" related tables in database.
		enum AlarmACKCommandAttributeSize
		{				 
			AlarmIDSize=I_ALARM_PROVIDER::ALARM_ID_SIZE+LongSharedMemoryData,           
			AlarmAcknowledgeBySize=I_ALARM_PROVIDER::PROFILE_ID_SIZE+LongSharedMemoryData,
			AckTimeSize=ULongSharedMemoryData,
			TotalSizeLength= AlarmIDSize+ AlarmAcknowledgeBySize+ AckTimeSize,
			MaxItemCount=200
		};
	};
		
	 
}

#endif // !defined(AFX_AlarmAlarmACKCommandManager_H__2BA8A561_4EB6_4F8C_A003_B1CC88A86160__INCLUDED_)
