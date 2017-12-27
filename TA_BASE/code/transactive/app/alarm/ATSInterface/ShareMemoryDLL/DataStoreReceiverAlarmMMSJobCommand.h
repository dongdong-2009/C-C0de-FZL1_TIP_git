// DataStoreReceiverAlarmMMSJobCommand.h: interface for the DataStoreReceiverAlarmMMSJobCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DataStoreReceiverAlarmMMSJobCommand_H__87F7FD9A_6FA4_4B19_9885_B3F6166BF608__INCLUDED_)
#define AFX_DataStoreReceiverAlarmMMSJobCommand_H__87F7FD9A_6FA4_4B19_9885_B3F6166BF608__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <map>
//#include  "ace/Synch.h"

//#include "core/threads/src/Thread.h"
//#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "ShareMemoryHelper.h"
#include "CacheDataWraper.h"
#include "AlarmMMSJobCommandManager.h" 
#include "DataStoreReceiver.h"
#include "DataStructInfo.h"

namespace TA_Base_Bus
{
	
	class DataStoreReceiverAlarmMMSJobCommand   : public DataStoreReceiver<std::string , AlarmMmsJobCommand, AlarmMMSJobCommandManager>
	{
	public:
		
		virtual ~DataStoreReceiverAlarmMMSJobCommand();
		//virtual  AlarmMMSJobCommandManager* getDataManager();
		static DataStoreReceiverAlarmMMSJobCommand* getInstance();
		static void release();		
   
    private:
		DataStoreReceiverAlarmMMSJobCommand();	
        static DataStoreReceiverAlarmMMSJobCommand*  m_Instance;
		//static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
	};
}


#endif // !defined(AFX_DataStoreReceiverAlarmMMSJobCommand_H__87F7FD9A_6FA4_4B19_9885_B3F6166BF608__INCLUDED_)
