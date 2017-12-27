// ATSProfileManager.cpp: implementation of the ATSProfileManager class.
//
//////////////////////////////////////////////////////////////////////

#include "ATSProfileManager.h"

namespace TA_Base_Bus
{

	ATSProfileManager::ATSProfileManager(long maxBlockCount,long blockByteSize)
		:CacheDataManager<unsigned long, ExtProfileInfo>(maxBlockCount,blockByteSize,"ExtProfileInfo")
	{

	}

	ATSProfileManager::~ATSProfileManager()
	{

	}
// 	unsigned long ATSProfileManager::readCachDataKeyFromMem(ShareMemoryHelper* mem, char* pos)
// 	{
// 		return   mem->readULong(pos);
// 	}
// 	unsigned long ATSProfileManager::getCacheDataKey(ATSProfileInfo* cacheData)
// 	{
// 		return  cacheData->ID;
// 	}
	bool ATSProfileManager::writeCacheDataToMem(ExtProfileInfo* cacheData,ShareMemoryHelper* mem, char* pos)
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
	bool ATSProfileManager::readCacheDataFromMem(ExtProfileInfo* cacheData,ShareMemoryHelper* mem, char* pos)
	{		
		//Attention: if the sequence of the following fields is updated, all related component should be re-compile.
		//           and the function, writeCacheDataToMem, should also be updated.

		cacheData->ID=mem->readULong(pos);

		cacheData->operatorId=mem->readString(mem->getCurReadPos(),OperatorIdSize).c_str();	 
		cacheData->updateTime=mem->readULong(mem->getCurReadPos());		
		cacheData->profileId=mem->readString(mem->getCurReadPos(),ProfileIdSize).c_str();	 
		cacheData->loginLocation=mem->readString(mem->getCurReadPos(),LoginLocationSize).c_str();	
		cacheData->displayOnly=mem->readBool(mem->getCurReadPos());
		 
		return TRUE;
	}


}
