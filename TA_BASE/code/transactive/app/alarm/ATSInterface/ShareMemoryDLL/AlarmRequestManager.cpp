// AlarmACKCommandManager.cpp: implementation of the AlarmACKCommandManager class.
//
//////////////////////////////////////////////////////////////////////

#include "AlarmRequestManager.h"
using namespace I_ALARM_PROVIDER;

namespace TA_Base_Bus
{
	
	AlarmRequestManager::AlarmRequestManager(long maxBlockCount,long blockByteSize)
		:CacheDataManager<unsigned long, AlarmRequest>(maxBlockCount,blockByteSize,"AlarmRequestCommand")
	{
		
	}
	
	AlarmRequestManager::~AlarmRequestManager()
	{
		
	}
	unsigned long AlarmRequestManager::readCachDataKeyFromMem(ShareMemoryHelper* mem, char* pos)
	{
		return   0;
	}
	unsigned long AlarmRequestManager::getCacheDataKey(AlarmRequest* cacheData)
	{
		return  0;
	}
	bool AlarmRequestManager::writeCacheDataToMem(AlarmRequest* cacheData,ShareMemoryHelper* mem, char* pos)
	{
		//Attention: if the sequence of the following fields is updated, all related component should be re-compile.
		//           and the function, readCacheDataFromMem, should also be updated.
		if (cacheData!=NULL)
		{			 
			mem->writeString(mem->getCurWritePos(),cacheData->alarmId,AlarmIDSize);	 		 	 
			mem->writeString(mem->getCurWritePos(),cacheData->operatorID,OperatorIDSize);	 
			mem->writeULong(mem->getCurWritePos(), cacheData->requestTime);
			mem->writeUShort(mem->getCurWritePos(), cacheData->state);
		}
		else
		{
			//raise error
		}
		return TRUE;
	}
	bool AlarmRequestManager::readCacheDataFromMem(AlarmRequest* cacheData,ShareMemoryHelper* mem, char* pos)
	{		
		//Attention: if the sequence of the following fields is updated, all related component should be re-compile.
		//           and the function, writeCacheDataToMem, should also be updated.		 
		cacheData->alarmId=mem->readString(mem->getCurReadPos(),AlarmIDSize).c_str();	 		 
		cacheData->operatorID=mem->readString(mem->getCurReadPos(),OperatorIDSize).c_str();	 
	    cacheData->requestTime=mem->readULong(mem->getCurReadPos());		
		cacheData->state=(AlarmState)mem->readUShort(mem->getCurReadPos());		
		return TRUE;
	}
	
 	
}
