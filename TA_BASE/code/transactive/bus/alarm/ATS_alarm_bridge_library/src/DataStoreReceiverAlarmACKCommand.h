// DataStoreReceiverAlarmACKCommand.h: interface for the DataStoreReceiverAlarmACKCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DataStoreReceiverAlarmACKCommand_H__87F7FD9A_6FA4_4B19_9885_B3F6166BF608__INCLUDED_)
#define AFX_DataStoreReceiverAlarmACKCommand_H__87F7FD9A_6FA4_4B19_9885_B3F6166BF608__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <map>
//#include  "ace/Synch.h"

//#include "core/threads/src/Thread.h"
//#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "ShareMemoryHelper.h"
#include "CacheDataWraper.h"
#include "AlarmACKCommandManager.h" 
#include "DataStoreReceiver.h"
#include "DataStructInfo.h"

namespace TA_Base_Bus
{
	
	class DataStoreReceiverAlarmACKCommand   : public DataStoreReceiver<unsigned long, AlarmACKCommand, AlarmACKCommandManager>
	{
	public:
		
		virtual ~DataStoreReceiverAlarmACKCommand();
		//virtual  AlarmACKCommandManager* getDataManager();
		static DataStoreReceiverAlarmACKCommand* getInstance();
		static void release();		
 
    private:
		DataStoreReceiverAlarmACKCommand();	
        static DataStoreReceiverAlarmACKCommand*  m_Instance;
		//static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
	};
}


#endif // !defined(AFX_DataStoreReceiverAlarmACKCommand_H__87F7FD9A_6FA4_4B19_9885_B3F6166BF608__INCLUDED_)

//AFX_DataStoreReceiverAlarmCommand_H__87F7FD9A_6FA4_4B19_9885_B3F6166BF608__INCLUDED_
