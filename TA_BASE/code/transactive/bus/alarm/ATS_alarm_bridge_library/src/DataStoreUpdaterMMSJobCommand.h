// DataStoreUpdaterATSAlarmQueueItem.h: interface for the DataStoreUpdaterATSAlarmQueueItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DataStoreUpdatereMMSJobCommand_H__6199400B_C677_42D0_B3EF_1CD13328F051__INCLUDED_)
#define AFX_DataStoreUpdatereMMSJobCommand_H__6199400B_C677_42D0_B3EF_1CD13328F051__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string> 
#include "CacheDataWraper.h"
#include "AlarmMMSJobCommandManager.h"
#include "DataStoreUpdater.h"
 
namespace TA_Base_Bus
{
	class DataStoreUpdaterMMSJobCommand  :public DataStoreUpdater<std::string , I_ALARM_PROVIDER::AlarmMmsJobCommand,AlarmMMSJobCommandManager>
	{
	public:

		virtual ~DataStoreUpdaterMMSJobCommand();
		void Initialize(std::vector< boost::shared_ptr<I_ALARM_PROVIDER::AlarmMmsJobCommand> >& batch);
		static DataStoreUpdaterMMSJobCommand* getInstance();
		static void release();		
		
	private:
		DataStoreUpdaterMMSJobCommand();
		
		bool m_isInitialized;
		//TA_Base_Core::NonReEntrantThreadLockable m_initilizedLock;

		static DataStoreUpdaterMMSJobCommand*  m_Instance;
		//static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

	};
}
#endif // !defined(AFX_DataStoreUpdatereMMSJobCommand_H__6199400B_C677_42D0_B3EF_1CD13328F051__INCLUDED_)
