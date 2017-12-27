// DataStoreReceiverAlarmACKCommand.h: interface for the DataStoreReceiverAlarmACKCommand class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DATA_STORE_RECEIVER_ALARM_REQUEST_H_INCLUDED__
#define __DATA_STORE_RECEIVER_ALARM_REQUEST_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <map>
//#include  "ace/Synch.h"

//#include "core/threads/src/Thread.h"
//#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "ShareMemoryHelper.h"
#include "CacheDataWraper.h"
#include "AlarmRequestManager.h" 
#include "DataStoreReceiver.h"
#include "DataStructInfo.h"

namespace TA_Base_Bus
{
	
	class DataStoreReceiverAlarmRequest   : public DataStoreReceiver<unsigned long, I_ALARM_PROVIDER::AlarmRequest, AlarmRequestManager>
	{
	public:
		
		virtual ~DataStoreReceiverAlarmRequest();
		//virtual  AlarmACKCommandManager* getDataManager();
		static DataStoreReceiverAlarmRequest* getInstance();
		static void release();		
 
    private:
		DataStoreReceiverAlarmRequest();	
        static DataStoreReceiverAlarmRequest*  m_Instance;
		//static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
	};
}


#endif //__DATA_STORE_RECEIVER_ALARM_REQUEST_H_INCLUDED__
