// ATSAlarmInfoManager.cpp: implementation of the ATSAlarmInfoManager class.
//
//////////////////////////////////////////////////////////////////////

#include "ATSAlarmInfoManager.h"
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus
{
	


	ATSAlarmInfoManager::ATSAlarmInfoManager(long maxBlockCount,long blockByteSize)
		:CacheDataManager<std::string , ExAlarmQueueItem>(maxBlockCount,blockByteSize,"ATSAlarmInfo")
	{

	}

	ATSAlarmInfoManager::~ATSAlarmInfoManager()
	{

	}
//     std::string ATSAlarmInfoManager::readCachDataKeyFromMem(ShareMemoryHelper* mem, char* pos)
// 	{
// 		return  mem->readString(pos,AlarmIDSize).c_str();
// 	}
// 	std::string ATSAlarmInfoManager::getCacheDataKey(ATSAlarmQueueItem* cacheData)
// 	{
// 		return  cacheData->detail.alarmID;
//	}
	bool ATSAlarmInfoManager::writeCacheDataToMem(ExAlarmQueueItem* cacheData,ShareMemoryHelper* mem, char* pos)
	{
		//Attention: if the sequence of the following fields is updated, all related component should be re-compile.
		//           and the function, readCacheDataFromMem, should also be updated.
		 
		if (cacheData!=NULL)
		{			
			mem->writeString(pos, cacheData->detail.alarmID,AlarmIDSize);
            mem->writeULong(mem->getCurWritePos(),cacheData->detail.ackTime);
			mem->writeULong(mem->getCurWritePos(),cacheData->detail.sourceTime.time);
			mem->writeULong(mem->getCurWritePos(),cacheData->detail.sourceTime.millitm);
			mem->writeULong(mem->getCurWritePos(),cacheData->detail.closeTime);
			mem->writeString(mem->getCurWritePos(),cacheData->detail.assetName,AssetNameSize);
			mem->writeULong(mem->getCurWritePos(),cacheData->detail.alarmSeverity);
			mem->writeString(mem->getCurWritePos(),cacheData->detail.alarmDescription,AlarmDescriptionSize);
			mem->writeString(mem->getCurWritePos(),cacheData->detail.alarmAcknowledgeBy,AlarmAcknowledgeBySize);
			mem->writeLong(mem->getCurWritePos(),cacheData->detail.state);
			mem->writeString(mem->getCurWritePos(),cacheData->detail.locationId,LocationIdSize);
			mem->writeString(mem->getCurWritePos(),cacheData->detail.parentAlarmID,parentAlarmIDSize);
			mem->writeString(mem->getCurWritePos(),cacheData->detail.avalancheHeadID,AvalancheHeadIDSize);
			mem->writeLong(mem->getCurWritePos(),cacheData->detail.isHeadOfAvalanche);
			mem->writeLong(mem->getCurWritePos(),cacheData->detail.isChildOfAvalanche);
			mem->writeLong(mem->getCurWritePos(),cacheData->detail.mmsState);
			mem->writeString(mem->getCurWritePos(),cacheData->detail.alarmValue,AlarmValueSize);
			unsigned char uChar=(unsigned char)cacheData->detail.omAlarm;
			mem->writeChar(mem->getCurWritePos(),&(uChar),omAlarmSize);		 
			mem->writeString(mem->getCurWritePos(),cacheData->detail.alarmType,AlarmTypeSize);
			mem->writeString(mem->getCurWritePos(),cacheData->detail.systemkey,SystemKeySize);
			mem->writeString(mem->getCurWritePos(),cacheData->detail.subsytemkey,SubSystemKeySize);
			mem->writeULong(mem->getCurWritePos(),cacheData->detail.alarmSource);
		}
		else
		{
			//raise error
		}
		return TRUE;
	}
	bool ATSAlarmInfoManager::readCacheDataFromMem(ExAlarmQueueItem* cacheData,ShareMemoryHelper* mem, char* pos)
	{		
		//Attention: if the sequence of the following fields is updated, all related component should be re-compile.
		//           and the function, writeCacheDataToMem, should also be updated.
		 
		cacheData->detail.alarmID=mem->readString(pos,AlarmIDSize).c_str();		 
		cacheData->detail.ackTime=mem->readULong(mem->getCurReadPos());
		cacheData->detail.sourceTime.time=mem->readULong(mem->getCurReadPos());
		cacheData->detail.sourceTime.millitm=mem->readULong(mem->getCurReadPos());
		cacheData->detail.closeTime=mem->readULong(mem->getCurReadPos());
		cacheData->detail.assetName=mem->readString(mem->getCurReadPos(),AssetNameSize).c_str();
		cacheData->detail.alarmSeverity=mem->readULong(mem->getCurReadPos());
		cacheData->detail.alarmDescription=mem->readString(mem->getCurReadPos(),AlarmDescriptionSize).c_str();
		cacheData->detail.alarmAcknowledgeBy=mem->readString(mem->getCurReadPos(),AlarmAcknowledgeBySize).c_str();

		
		cacheData->detail.state=(AlarmState)mem->readLong(mem->getCurReadPos());
		cacheData->detail.locationId=mem->readString(mem->getCurReadPos(),LocationIdSize).c_str();
		cacheData->detail.parentAlarmID=mem->readString(mem->getCurReadPos(),parentAlarmIDSize).c_str();
		cacheData->detail.avalancheHeadID=mem->readString(mem->getCurReadPos(),AvalancheHeadIDSize).c_str();
		cacheData->detail.isHeadOfAvalanche=mem->readLong(mem->getCurReadPos());
		cacheData->detail.isChildOfAvalanche=mem->readLong(mem->getCurReadPos());
		cacheData->detail.mmsState=(MmsStateType)mem->readLong(mem->getCurReadPos());
		cacheData->detail.alarmValue=mem->readString(mem->getCurReadPos(),AlarmValueSize).c_str();	
		unsigned char uChar=0;
		mem->readChar(mem->getCurReadPos(),&uChar,omAlarmSize);		
		cacheData->detail.omAlarm=(char)uChar;
		cacheData->detail.alarmType=mem->readString(mem->getCurReadPos(),AlarmTypeSize).c_str();
		cacheData->detail.systemkey=mem->readString(mem->getCurReadPos(),SystemKeySize).c_str();
		cacheData->detail.subsytemkey=mem->readString(mem->getCurReadPos(),SubSystemKeySize).c_str();
		cacheData->detail.alarmSource=mem->readULong(mem->getCurReadPos());

		return TRUE;
	} 

}
