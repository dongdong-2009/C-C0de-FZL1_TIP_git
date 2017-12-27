// DataStoreReceiverISCSProfile.h: interface for the DataStoreReceiverISCSProfile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DataStoreReceiverISCSProfile1_H__87F7FD9A_6FA4_4B19_9885_B3F6166BF608__INCLUDED_)
#define AFX_DataStoreReceiverISCSProfile1_H__87F7FD9A_6FA4_4B19_9885_B3F6166BF608__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <map>
//#include  "ace/Synch.h"

//#include "core/threads/src/Thread.h"
//#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "ShareMemoryHelper.h"
#include "CacheDataWraper.h"
#include "ISCSProfileManager.h" 
#include "DataStoreReceiver.h"
#include "DataStructInfo.h"

namespace TA_Base_Bus
{

	class DataStoreReceiverISCSProfile   : public DataStoreReceiver<unsigned long, I_ALARM_PROVIDER::ISCSProfileInfo, ISCSProfileManager>
	{
	public:

		virtual ~DataStoreReceiverISCSProfile();
		//virtual  AlarmACKCommandManager* getDataManager();
		static DataStoreReceiverISCSProfile* getInstance();
		static void release();		

		boost::shared_ptr<I_ALARM_PROVIDER::ISCSProfileInfo> getProfileWithoutLock();
		boost::shared_ptr<I_ALARM_PROVIDER::ISCSProfileInfo> getProfile();
 
	private:
		DataStoreReceiverISCSProfile();	
		static DataStoreReceiverISCSProfile*  m_Instance;
		//static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
	};
}


#endif // !defined(AFX_DataStoreReceiverISCSProfile_H__87F7FD9A_6FA4_4B19_9885_B3F6166BF608__INCLUDED_)
