// DataStoreUpdaterAlarmAlarmACKCommand.h: interface for the DataStoreUpdaterAlarmAlarmACKCommand class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __DATA_STORE_UPDATER_ALARM_REQUEST_H_INCLUDED__
#define __DATA_STORE_UPDATER_ALARM_REQUEST_H_INCLUDED__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include <string> 
#include "CacheDataWraper.h"
#include "AlarmRequestManager.h"
#include "DataStructInfo.h"
#include "DataStoreUpdater.h"
 
namespace TA_Base_Bus
{
	class DataStoreUpdaterAlarmRequest  :public DataStoreUpdater<unsigned long, I_ALARM_PROVIDER::AlarmRequest,AlarmRequestManager>
	{
	public:
		
		virtual ~DataStoreUpdaterAlarmRequest();
		void Initialize(std::vector< boost::shared_ptr<I_ALARM_PROVIDER::AlarmRequest> >& batch);
		static DataStoreUpdaterAlarmRequest* getInstance();
		static void release();		
		 
	private:
		DataStoreUpdaterAlarmRequest();
	 
		bool m_isInitialized;
		TA_Base_Core::NonReEntrantThreadLockable m_initilizedLock;
		
		static DataStoreUpdaterAlarmRequest*  m_Instance;
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
		
		};
}

#endif //__DATA_STORE_UPDATER_ALARM_REQUEST_H_INCLUDED__
