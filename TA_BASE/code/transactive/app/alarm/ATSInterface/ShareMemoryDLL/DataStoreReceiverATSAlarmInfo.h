// DataStoreReceiverATSAlarmInfo.h: interface for the DataStoreReceiverATSAlarmInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DataStoreReceiverATSAlarmInfo_H__9A110C71_A475_4A53_9A5A_06792983C24C__INCLUDED_)
#define AFX_DataStoreReceiverATSAlarmInfo_H__9A110C71_A475_4A53_9A5A_06792983C24C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <map>
 

 
 
#include "ShareMemoryHelper.h"
#include "CacheDataWraper.h"
#include "ATSAlarmInfoManager.h" 
#include "DataStoreReceiver.h"
#include "DataStructInfo.h"

namespace TA_Base_Bus
{

	class DataStoreReceiverATSAlarmInfo  : public DataStoreReceiver<std::string, ExAlarmQueueItem, ATSAlarmInfoManager>
	{
	public:
		
		virtual ~DataStoreReceiverATSAlarmInfo();
		//virtual  SessionCacheManager* getDataManager();
		static DataStoreReceiverATSAlarmInfo* getInstance();
		static void release();		
    
    private:
		DataStoreReceiverATSAlarmInfo();	
        static DataStoreReceiverATSAlarmInfo*  m_Instance;
		 
	};
}

#endif // !defined(AFX_DataStoreReceiverATSAlarmInfo_H__9A110C71_A475_4A53_9A5A_06792983C24C__INCLUDED_)
