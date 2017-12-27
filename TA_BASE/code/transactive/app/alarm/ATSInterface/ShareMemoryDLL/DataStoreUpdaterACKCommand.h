// DataStoreUpdaterATSAlarmQueueItem.h: interface for the DataStoreUpdaterATSAlarmQueueItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DataStoreUpdatereAlarmACKCommand_H__6199400B_C677_42D0_B3EF_1CD13328F051__INCLUDED_)
#define AFX_DataStoreUpdatereAlarmACKCommand_H__6199400B_C677_42D0_B3EF_1CD13328F051__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string> 
#include "CacheDataWraper.h"
#include "AlarmACKCommandManager.h"
#include "DataStoreUpdater.h"
namespace TA_Base_Bus
{
	class DataStoreUpdaterACKCommand  :public DataStoreUpdater<long , AlarmACKCommand,AlarmACKCommandManager>
	{
	public:

		virtual ~DataStoreUpdaterACKCommand();
		void Initialize(std::vector< boost::shared_ptr<AlarmACKCommand> >& batch);
		static DataStoreUpdaterACKCommand* getInstance();
		static void release();		
		//virtual bool onDispatchObjects(std::vector<boost::shared_ptr<AlarmACKCommand> >&  batch);
	private:
		DataStoreUpdaterACKCommand();
		//AlarmACKCommandManager* m_Manager;
		bool m_isInitialized;
		//TA_Base_Core::NonReEntrantThreadLockable m_initilizedLock;

		static DataStoreUpdaterACKCommand*  m_Instance;
		//static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

	};
}
#endif // !defined(AFX_DataStoreUpdatereAlarmACKCommand_H__6199400B_C677_42D0_B3EF_1CD13328F051__INCLUDED_)
