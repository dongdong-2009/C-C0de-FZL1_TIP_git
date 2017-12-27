//////////////////////////////////////////////////////////////////////
///		@file		MonitorAQData.h
///		@author		zhilin,ouyang
///		@date		2012-01-10 14:44:41
///
///		@brief	    the main thread to synch data for the system. 
///                 it gather the data changes from the source database
///                 in its queue table.
///                 
//////////////////////////////////////////////////////////////////////
#ifndef  MONITORAQDATA_H
#define  MONITORAQDATA_H
#include "CommonData.h"
#include "core/threads/src/Thread.h"
#include <map>

NS_BEGIN(TA_DB_Sync)

using namespace TA_Base_Core;
class CPropagateMsg;
class CDeqMsgJob;
class CConfigInfoHelper;
class CTcpConnectionListen;

class CAQMsgThreadsPool : public Thread
{
private:
    // propagate thread info
	typedef  std::map<uint32, CPropagateMsg*>                        mapQThreadPoolT;
	typedef  std::map<uint32, CPropagateMsg*>::iterator              mapQThreadPoolIterT;
	typedef  std::map<uint32, CPropagateMsg*>::value_type            mapQThreadPoolValueT;
    
    //map one queue and all propagatemsgS of the queue.
	typedef  std::map<std::string, mapQThreadPoolT*>				 mapAllQThreadPoolT;
	typedef  std::map<std::string, mapQThreadPoolT*>::iterator		 mapAllQThreadPoolIterT;	
	typedef  std::map<std::string, mapQThreadPoolT*>::value_type	 mapAllQThreadPoolValueT;

    // queue recipient info, come from S table, PRO_THD( map to one record in s table)
	typedef  std::map<uint32, PRO_THD*>								 mapQRecipientInfoT;
	typedef  std::map<uint32, PRO_THD*>::iterator				     mapQRecipientInfoIterT;
	typedef  std::map<uint32, PRO_THD*>::value_type                  mapQRecipientInfoValueT;	

    //map queue and recipients, queue name, recipient info
	typedef  std::map<std::string, mapQRecipientInfoT*>			    	 mapAllQRecipientInfoT;
	typedef  std::map<std::string, mapQRecipientInfoT*>::iterator		 mapAllQRecipientInfoIterT;
	typedef  std::map<std::string, mapQRecipientInfoT*>::value_type		 mapAllQRecipientInfoValueT;	
	
	//Dequeue job, come from m table
	//CDeqMsgJob is a thread object and responsible for dequeuing data to local
	typedef  std::map<std::string, CDeqMsgJob*>			    	     mapAllDeqJobT;
	typedef  std::map<std::string, CDeqMsgJob*>::iterator	   	     mapAllDeqJobIterT;
	typedef  std::map<std::string, CDeqMsgJob*>::value_type		     mapAllDeqJobValueT;

private:
	enum eumPoolState
	{
		STATE_CODE_INITIAL_ENV               = 0,
		STATE_CODE_INITIAL_POOL,
		STATE_CODE_MONITOR_AQ_SYS_EVENT,
		STATE_CODE_START_UP_NEW_JOBS,
		STATE_CODE_STOP_JOBS,
		STATE_CODE_WAITFOR_ACTIVE_EVENT
	};

public:
	CAQMsgThreadsPool(void);
	~CAQMsgThreadsPool(void);

public:
	virtual void  run();
	virtual void  terminate();

private:
	void  _ThreadJob();
	
private:
	void  _ProcessInitialEnv();
	void  _ProcessInitialPool();
	void  _ProcessMonitorAQEvent();
	void  _ProcessStartNewJob();
	void  _ProcessStopJobs();
	void  _ProcessWaitAQActiveEvent();

private:
	void  _ClearAll();
	void  _CheckDeqJob(mapAllDeqJobT& rExistDeqJobPool, mapAllQRecipientInfoT& rNew);
	int   _TransformRecipientInfos(mapAllQRecipientInfoT& rNew, mapAllQRecipientInfoT& rAll);
	int   _TransformRecipient(mapQRecipientInfoT* rOri, mapQRecipientInfoT*& rDes);
	void  _StartUpThreads();
	void  _StartUpNewJobs(mapQRecipientInfoT* rOri, mapQThreadPoolT*& rPoolJobs);
	
	void  _ClearRecipientInfos(mapAllQRecipientInfoT& rMapThread);	
	void  _FreeRecipientInfos(mapAllQRecipientInfoT& rMapThread);
	void  _FreeRecipientInfos(mapAllQThreadPoolT& rMapPool);
	void  _FreeRecipientInfos(mapAllDeqJobT& rMapDeqJobPool);

private:
	bool					 m_bTerminate;
	bool					 m_bThreadPoolFlag;
	mapAllDeqJobT            m_mapAllDeqJobs; //Queue Dequeue Job
	mapAllQRecipientInfoT	 m_mapNewRecipients;
	mapAllQRecipientInfoT    m_mapAllThreads;
	mapAllQThreadPoolT		 m_mapAllJobs;
	CConfigInfoHelper*       m_pConfigHelper;	
	eumPoolState		     m_nPoolState;
	CTcpConnectionListen*	 m_pMysqlTcpServer;
};

NS_END(TA_DB_Sync)

#endif  // MONITORAQDATA_H
