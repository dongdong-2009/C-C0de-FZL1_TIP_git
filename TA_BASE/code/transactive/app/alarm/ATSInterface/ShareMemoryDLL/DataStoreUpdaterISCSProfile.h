// DataStoreUpdaterISCSAlarmQueueItem.h: interface for the DataStoreUpdaterISCSAlarmQueueItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DataStoreUpdatereISCSProfile)
#define AFX_DataStoreUpdatereISCSProfile

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string> 
#include "CacheDataWraper.h"
#include "ISCSProfileManager.h"
#include "DataStoreUpdater.h"
namespace TA_Base_Bus
{
	class DataStoreUpdaterISCSProfile  :public DataStoreUpdater<std::string , ISCSProfileInfo,ISCSProfileManager>
	{
	public:

		virtual ~DataStoreUpdaterISCSProfile();
		void Initialize(std::vector< boost::shared_ptr<ISCSProfileInfo> >& batch);
		static DataStoreUpdaterISCSProfile* getInstance();
		static void release();		
		 
	private:
		DataStoreUpdaterISCSProfile();
		 
		bool m_isInitialized;
		//TA_Base_Core::NonReEntrantThreadLockable m_initilizedLock;

		static DataStoreUpdaterISCSProfile*  m_Instance;
		//static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

	};
}
#endif // !defined(AFX_DATASTOREUPDATERISCSAlarmQueueItem_H__6199400B_C677_42D0_B3EF_1CD13328F051__INCLUDED_)
