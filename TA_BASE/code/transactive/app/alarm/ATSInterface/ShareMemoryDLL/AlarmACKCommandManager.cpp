// AlarmACKCommandManager.cpp: implementation of the AlarmACKCommandManager class.
//
//////////////////////////////////////////////////////////////////////

#include "AlarmACKCommandManager.h"

namespace TA_Base_Bus
{
	
	AlarmACKCommandManager::AlarmACKCommandManager(long maxBlockCount,long blockByteSize)
		:CacheDataManager<unsigned long, AlarmACKCommand>(maxBlockCount,blockByteSize,"AlarmACKCommand")
	{
		
	}
	
	AlarmACKCommandManager::~AlarmACKCommandManager()
	{
		
	}
	unsigned long AlarmACKCommandManager::readCachDataKeyFromMem(ShareMemoryHelper* mem, char* pos)
	{
		return   0;
	}
	unsigned long AlarmACKCommandManager::getCacheDataKey(AlarmACKCommand* cacheData)
	{
		return  0;
	}
	bool AlarmACKCommandManager::writeCacheDataToMem(AlarmACKCommand* cacheData,ShareMemoryHelper* mem, char* pos)
	{
		//Attention: if the sequence of the following fields is updated, all related component should be re-compile.
		//           and the function, readCacheDataFromMem, should also be updated.
		if (cacheData!=NULL)
		{			 
			mem->writeString(mem->getCurWritePos(),cacheData->alarmId,AlarmIDSize);	 		 	 
			mem->writeString(mem->getCurWritePos(),cacheData->alarmAcknowledgeBy,AlarmAcknowledgeBySize);	 
			mem->writeULong(mem->getCurWritePos(), cacheData->ackTime);
		}
		else
		{
			//raise error
		}
		return TRUE;
	}
	bool AlarmACKCommandManager::readCacheDataFromMem(AlarmACKCommand* cacheData,ShareMemoryHelper* mem, char* pos)
	{		
		//Attention: if the sequence of the following fields is updated, all related component should be re-compile.
		//           and the function, writeCacheDataToMem, should also be updated.		 
		cacheData->alarmId=mem->readString(mem->getCurReadPos(),AlarmIDSize).c_str();	 		 
		cacheData->alarmAcknowledgeBy=mem->readString(mem->getCurReadPos(),AlarmAcknowledgeBySize).c_str();	 
	    cacheData->ackTime=mem->readULong(mem->getCurReadPos());				
		return TRUE;
	}
	
 	
}
