// ATSAlarmObserver.h: interface for the ATSAlarmObserver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATSAlarmObserver_H__14578BAA_F458_4FB5_AF0E_B30CFD50D1A3__INCLUDED_)
#define AFX_ATSAlarmObserver_H__14578BAA_F458_4FB5_AF0E_B30CFD50D1A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/threads/src/Thread.h"
#include "bus/alarm/ATS_alarm_bridge_library/src/ICacheDataUpdateObserver.h"
#include "bus/alarm/ATS_alarm_bridge_library/src/DataStructInfo.h"
#include "bus/alarm/alarm_store_library/src/AlarmStoreManager.h"

 
 
namespace TA_Base_Bus
{
    
	class ATSAlarmObserver: //public ICacheDataUpdateObserver<ATSAlarmQueueItem>,
		virtual public TA_Base_Core::Thread
	{
	public:
		typedef std::map<std::string, unsigned long> LocationMapT;

		ATSAlarmObserver(AlarmStoreManager* _alarmStoreManager, unsigned long ullocationKey);
		virtual ~ATSAlarmObserver();
		void notifyStoreRestart();
		void initLocationsMap();
	   // virtual void processData(boost::shared_ptr<CacheDataWraper <ATSAlarmQueueItem> > datas);	
    private:
	    time_t   m_lastUpdateTime;			 
		virtual void run();		 
		virtual void terminate();
		bool m_Running;
		
		// convert locationID -> locationKey
		LocationMapT m_locationMap;
		AlarmStoreManager* m_alarmStoreManager;
		unsigned long m_ulSubSystemID;
		unsigned long m_ullocationKey;
	};
}

#endif // !defined(AFX_ATSAlarmObserver_H__14578BAA_F458_4FB5_AF0E_B30CFD50D1A3__INCLUDED_)
