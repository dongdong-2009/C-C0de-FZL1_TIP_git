// DataStoreReceiverATSProfile.h: interface for the DataStoreReceiverATSProfile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DataStoreReceiverATSProfile_H__87F7FD9A_6FA4_4B19_9885_B3F6166BF608__INCLUDED_)
#define AFX_DataStoreReceiverATSProfile_H__87F7FD9A_6FA4_4B19_9885_B3F6166BF608__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <map>
//#include  "ace/Synch.h"

//#include "core/threads/src/Thread.h"
//#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "ShareMemoryHelper.h"
#include "CacheDataWraper.h"
#include "ATSProfileManager.h" 
#include "DataStoreReceiver.h"
#include "DataStructInfo.h"

namespace TA_Base_Bus
{

	class DataStoreReceiverATSProfile   : public DataStoreReceiver<unsigned long, I_ALARM_PROVIDER::ExtProfileInfo, ATSProfileManager>
	{
	public:

		virtual ~DataStoreReceiverATSProfile();
		//virtual  AlarmACKCommandManager* getDataManager();
		static DataStoreReceiverATSProfile* getInstance();
		static void release();		

		boost::shared_ptr<I_ALARM_PROVIDER::ExtProfileInfo> getProfile();
 
	private:
		DataStoreReceiverATSProfile();	
		static DataStoreReceiverATSProfile*  m_Instance;
		//static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
	};
}


#endif // !defined(AFX_DataStoreReceiverATSProfile_H__87F7FD9A_6FA4_4B19_9885_B3F6166BF608__INCLUDED_)
