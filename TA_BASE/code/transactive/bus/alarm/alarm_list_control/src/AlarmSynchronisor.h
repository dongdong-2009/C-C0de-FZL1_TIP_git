// AlarmSynchronisor.h: interface for the AlarmSynchronisor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALARMSYNCHRONISOR_H__42D0CC27_1E61_447D_8856_59D1B3C3D642__INCLUDED_)
#define AFX_ALARMSYNCHRONISOR_H__42D0CC27_1E61_447D_8856_59D1B3C3D642__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ace/Task.h"
#include "core/utilities/src/RunParams.h"

#include "bus/alarm/alarm_list_control/src/WinMsgDef.h"
//#include "bus/alarm/shared_alarm/src/SharedAlarm.h"
#include "bus/alarm/alarm_store_library/src/SharedAlarmAccessFactory.h"
#include "core/exceptions/src/AlarmListCtrlException.h"

class AlarmSynchronisor:ACE_Task_Base  
{
public:
	AlarmSynchronisor(ACE_Thread_Manager* thr_mgr = ACE_Thread_Manager::instance(), 
		int syn_timeout = 5000);
	~AlarmSynchronisor();
	
	virtual int svc();
	
	inline void setTarget(HWND target)
	{
		m_target = target;		
	}


// 	inline void canNotify()
// 	{
// 		return m_canNotify;		
// 	}

	

	inline void start()
	{
		activate();
		
	}
	
	inline void shutdown()
	{
		thr_mgr_->cancel_task(this);
		SetEvent(m_shutDownEv);
	}

	inline bool isItemselected(TA_Base_Core::AlarmItem* item)
	{
		try
		{
			return item->isSelect(m_csPos);
		}
		catch(TA_Base_Core::AlarmListCtrlException&)
		{

		}
		return false;
	}

	inline void selectItem(TA_Base_Core::AlarmItem* item)
	{
		try
		{
			m_selBuf.push_back(item);
			item->select(m_csPos);
		}
		catch(TA_Base_Core::AlarmListCtrlException&)
		{

		}
	}
	

	inline void unselectAll()
	{
		try
		{
			std::for_each(m_selBuf.begin(), m_selBuf.end(), 
				std::bind1st(std::mem_fun1<int, AlarmSynchronisor, TA_Base_Core::AlarmItem*>
				(&AlarmSynchronisor::unselectItem), this));
			m_selBuf.clear();
		}
		catch(TA_Base_Core::AlarmListCtrlException&)
		{

		}
	}

	inline int unselectItem(TA_Base_Core::AlarmItem* item)
	{
		try
		{
			item->unSelect(m_csPos);
		}
		catch(TA_Base_Core::AlarmListCtrlException&)
		{

		}
		return 0;
	}
	 
	inline void getSelectItems(std::vector<TA_Base_Core::AlarmItem*>& buf)
	{
		buf.assign(m_selBuf.begin(), m_selBuf.end());
	}

	inline void updateDisplay(displayVec& vec, SORT_COLUMN sortCol)
	{
		vec.clear();
		TA_Base_Bus::SharedAlarmAccessFactory::getInstance().getIndexWithSortCol(sortCol, &vec, m_csPos);
	}

	inline bool isUpdating()
	{
		if (m_canNotify==false)
		{
			return true;
		}				 
		return TA_Base_Bus::SharedAlarmAccessFactory::getInstance().isUpdating();		 
	}
	inline TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo* getIndexinfo()
	{
		return &m_info;
	}
	int m_csPos;

	//zyr++: fro map/unmap shared memory
	void notifyAlarmStoreResart();
	//++zyr
private:

	//zyr++: for map/unmap shared memory
	class SharedMemSync : public TA_Base_Core::Thread
	{
	public:
		SharedMemSync(SYN_EVENT& remapEv, SYN_EVENT& unmapEv, AlarmSynchronisor* parent);
		~SharedMemSync();

		void run();
		void terminate();

	private:
		SYN_EVENT m_termiateEv;
		SYN_EVENT& m_remapEv;
		SYN_EVENT& m_unmapEv;

		bool m_terminated;
		AlarmSynchronisor* m_parent;
		//static int m_mapShmemRef;
		int m_mapShmemRef;
	};

	SharedMemSync* m_sharedMemsyn;
	std::string m_remapEvName;
	std::string m_unmapEvName;
	SYN_EVENT m_remapMemEv;
	SYN_EVENT m_unmapMemEv;
	//++zyr

	//index info
	TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo m_info;
	
	//select flag buf
	displayVec m_selBuf;

	// shared alarm
	//TA_Base_Bus::TA_SharedAlarm m_sa;
	
	// used for server synchronise client
	SYN_EVENT m_server;			
	
	// used for client synchronise server
	SYN_EVENT m_client;

	SYN_EVENT m_shutDownEv;

	SYN_EVENT m_rebuildEvent;
	
	//wait time out
	const int m_waitTime;
	
	//target wnd
	HWND m_target;
	
	bool m_canNotify;
	
	std::string m_servername;
	std::string m_clientname;

};

#endif // !defined(AFX_ALARMSYNCHRONISOR_H__42D0CC27_1E61_447D_8856_59D1B3C3D642__INCLUDED_)
