// AlarmMMSJobCommandManager.cpp: implementation of the AlarmMMSJobCommandManager class.
//
//////////////////////////////////////////////////////////////////////

#include "AlarmMMSJobCommandManager.h"

namespace TA_Base_Bus
{
	
	AlarmMMSJobCommandManager::AlarmMMSJobCommandManager(long maxBlockCount,long blockByteSize)
		:CacheDataManager<std::string, AlarmMmsJobCommand>(maxBlockCount,blockByteSize,"AlarmMmsJobCommand")
	{
		
	}
	
	AlarmMMSJobCommandManager::~AlarmMMSJobCommandManager()
	{
		
	}
// 	unsigned long AlarmMMSJobCommandManager::readCachDataKeyFromMem(ShareMemoryHelper* mem, char* pos)
// 	{
// 		return   0;
// 	}
// 	unsigned long AlarmMMSJobCommandManager::getCacheDataKey(AlarmMmsJobCommand* cacheData)
// 	{
// 		return  0;
// 	}
	bool AlarmMMSJobCommandManager::writeCacheDataToMem(AlarmMmsJobCommand* cacheData,ShareMemoryHelper* mem, char* pos)
	{
		//Attention: if the sequence of the following fields is updated, all related component should be re-compile.
		//           and the function, readCacheDataFromMem, should also be updated.
		if (cacheData!=NULL)
		{		 
			mem->writeString(mem->getCurWritePos(),cacheData->alarmId,AlarmIDSize);	 			 	
			mem->writeString(mem->getCurWritePos(),cacheData->assetName,AssetNameSize);	 
			mem->writeString(mem->getCurWritePos(),cacheData->operatorId,OperatorIdSize);	 
			mem->writeULong(mem->getCurWritePos(), cacheData->targetDate);
			mem->writeString(mem->getCurWritePos(),cacheData->problemReport,ProblemReportSize);	  
		}
		else
		{
			//raise error
		}
		return TRUE;
	}
	bool AlarmMMSJobCommandManager::readCacheDataFromMem(AlarmMmsJobCommand* cacheData,ShareMemoryHelper* mem, char* pos)
	{		
		//Attention: if the sequence of the following fields is updated, all related component should be re-compile.
		//           and the function, writeCacheDataToMem, should also be updated.
	  
		 
		cacheData->alarmId=mem->readString(mem->getCurReadPos(),AlarmIDSize).c_str();	 
		 
		cacheData->assetName=mem->readString(mem->getCurReadPos(),AssetNameSize).c_str();	 
		cacheData->operatorId=mem->readString(mem->getCurReadPos(),OperatorIdSize).c_str();	 
		cacheData->targetDate=mem->readULong(mem->getCurReadPos());
		cacheData->problemReport=mem->readString(mem->getCurReadPos(),ProblemReportSize).c_str();	 
	 
		
		
		//cacheData->data	=mem->readString(mem->getCurReadPos(), AlarmMMSJobCommandManager::DataSize);	
		return TRUE;
	}
	
 	
}
