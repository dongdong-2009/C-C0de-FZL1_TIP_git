// DataStoreUpdaterATSAlarmQueueItem.h: interface for the DataStoreUpdaterATSAlarmQueueItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DataStoreUpdatereATSProfile_H__6199400B_C677_42D0_B3EF_1CD13328F051__INCLUDED_)
#define AFX_DataStoreUpdatereATSProfile_H__6199400B_C677_42D0_B3EF_1CD13328F051__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string> 
#include "CacheDataWraper.h"
#include "ATSProfileManager.h"
#include "DataStoreUpdater.h"
namespace TA_Base_Bus
{
	class DataStoreUpdaterATSProfile  :public DataStoreUpdater<std::string , ExtProfileInfo, ATSProfileManager>
	{
	public:

		virtual ~DataStoreUpdaterATSProfile();
		void Initialize(std::vector< boost::shared_ptr<ExtProfileInfo> >& batch);
		static DataStoreUpdaterATSProfile* getInstance();
		static void release();		
		 
	private:
		DataStoreUpdaterATSProfile();
	 
		bool m_isInitialized;
		//TA_Base_Core::NonReEntrantThreadLockable m_initilizedLock;

		static DataStoreUpdaterATSProfile*  m_Instance;
		//static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

	};
}
#endif // !defined(AFX_DATASTOREUPDATERATSAlarmQueueItem_H__6199400B_C677_42D0_B3EF_1CD13328F051__INCLUDED_)
