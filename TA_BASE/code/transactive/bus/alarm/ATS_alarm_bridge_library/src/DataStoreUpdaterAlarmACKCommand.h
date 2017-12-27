// DataStoreUpdaterAlarmAlarmACKCommand.h: interface for the DataStoreUpdaterAlarmAlarmACKCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DataStoreUpdaterAlarmAlarmACKCommand_H__6199400B_C677_42D0_B3EF_1CD13328F051__INCLUDED_)
#define AFX_DataStoreUpdaterAlarmAlarmACKCommand_H__6199400B_C677_42D0_B3EF_1CD13328F051__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include <string> 
#include "CacheDataWraper.h"
#include "AlarmACKCommandManager.h"
#include "DataStructInfo.h"
#include "DataStoreUpdater.h"
 
namespace TA_Base_Bus
{
	class DataStoreUpdaterAlarmACKCommand  :public DataStoreUpdater<unsigned long, I_ALARM_PROVIDER::AlarmACKCommand,AlarmACKCommandManager>
	{
	public:
		
		virtual ~DataStoreUpdaterAlarmACKCommand();
		void Initialize(std::vector< boost::shared_ptr<I_ALARM_PROVIDER::AlarmACKCommand> >& batch);
		static DataStoreUpdaterAlarmACKCommand* getInstance();
		static void release();		
		 
	private:
		DataStoreUpdaterAlarmACKCommand();
	 
		bool m_isInitialized;
		TA_Base_Core::NonReEntrantThreadLockable m_initilizedLock;
		
		static DataStoreUpdaterAlarmACKCommand*  m_Instance;
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
		
		};
}
#endif // !defined(AFX_DataStoreUpdaterAlarmAlarmACKCommand_H__6199400B_C677_42D0_B3EF_1CD13328F051__INCLUDED_)
