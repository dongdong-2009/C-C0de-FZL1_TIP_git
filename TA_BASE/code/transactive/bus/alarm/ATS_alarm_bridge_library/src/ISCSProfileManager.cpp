// ISCSProfileManager.cpp: implementation of the ISCSProfileManager class.
//
//////////////////////////////////////////////////////////////////////

#include "ISCSProfileManager.h"

using namespace I_ALARM_PROVIDER;

namespace TA_Base_Bus
{

	ISCSProfileManager::ISCSProfileManager(long maxBlockCount,long blockByteSize)
		:CacheDataManager<unsigned long, ISCSProfileInfo>(maxBlockCount,blockByteSize,"ISCSProfileInfo")
	{

	}

	ISCSProfileManager::~ISCSProfileManager()
	{

	}
// 	unsigned long ISCSProfileManager::readCachDataKeyFromMem(ShareMemoryHelper* mem, char* pos)
// 	{
// 		return   mem->readULong(pos);
// 	}
// 	unsigned long ISCSProfileManager::getCacheDataKey(ISCSProfileInfo* cacheData)
// 	{
// 		return  cacheData->ID;
// 	}
	bool ISCSProfileManager::writeCacheDataToMem(ISCSProfileInfo* cacheData,ShareMemoryHelper* mem, char* pos)
	{
		//Attention: if the sequence of the following fields is updated, all related component should be re-compile.
		//           and the function, readCacheDataFromMem, should also be updated.
		if (cacheData!=NULL)
		{			
			mem->writeULong(pos, cacheData->ID);
			mem->writeString(mem->getCurWritePos(),cacheData->operatorId,OperatorIdSize);	 
			mem->writeULong(mem->getCurWritePos(), cacheData->updateTime);		
			mem->writeString(mem->getCurWritePos(),cacheData->profileId,ProfileIdSize);	 
			mem->writeString(mem->getCurWritePos(),cacheData->loginLocation,LoginLocationSize);	 
			mem->writeBool(mem->getCurWritePos(),cacheData->displayOnly);
		}
		else
		{
			//raise error
		}
		return TRUE;
	}
	bool ISCSProfileManager::readCacheDataFromMem(ISCSProfileInfo* cacheData,ShareMemoryHelper* mem, char* pos)
	{		
		//Attention: if the sequence of the following fields is updated, all related component should be re-compile.
		//           and the function, writeCacheDataToMem, should also be updated.

		cacheData->ID=mem->readULong(pos);

		cacheData->operatorId=mem->readString(mem->getCurReadPos(),OperatorIdSize).c_str();	 
		cacheData->updateTime=mem->readULong(mem->getCurReadPos());		
		cacheData->profileId=mem->readString(mem->getCurReadPos(),ProfileIdSize).c_str();	 
		cacheData->loginLocation=mem->readString(mem->getCurReadPos(),LoginLocationSize).c_str();	
		cacheData->displayOnly=mem->readBool(mem->getCurReadPos());
		 


		//cacheData->data	=mem->readString(mem->getCurReadPos(), ISCSProfileManager::DataSize);	
		return TRUE;
	}


}
