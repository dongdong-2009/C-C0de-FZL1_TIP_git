/********************************************************************
//
//	FileName	:	SynModeMonitor.h
//	Version		:	1.0
//	Author		:	zhilin,ouyang
//	Date		:	2012-09-26 16:44:41
//	Comment		:	The class used to monitor the running mode for the 
//                  data syn application.
//
*********************************************************************/
#ifndef  SYNMODEMONITOR_H
#define  SYNMODEMONITOR_H
#include "CommonDef.h"
#include "core/threads/src/Thread.h"
#include <string>

#if defined(_WIN32)
#include <Windows.h>
#else
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#endif  // __WIN32__

NS_BEGIN(TA_DB_Sync)

class CSynModeMonitor : public TA_Base_Core::Thread
{
private:
	typedef struct st_shm_buff_tag
	{
		uint32  uiLen;
		char    szMode[defMaxNameLen];
	}SHMBUFF, *PSHMBUFF;

	enum SynModeCode
	{
		SYNMODE_CODE_CHECK_MODE            = 200,
		SYNMODE_CODE_RCV_EVT        
	};

	enum SynEventCode
	{
		SYNEVENT_CODE_CONTROL             = 0,
		SYNEVENT_CODE_MONITOR,
		SYNEVENT_CODE_STOP,

		SYNEVENT_CODE_COUNT
	};

public:
	CSynModeMonitor();
	CSynModeMonitor(const std::string& strSynID);
	~CSynModeMonitor();

public:
	virtual void  run();
	virtual void  terminate();

public:
	void      setModeData(const std::string& strMode);

private:
	void       _ThreadJob();

private:

#if defined(_WIN32)
	void   _WinntJob();
#else
	/*sem_t*              m_pSem;*/	

	void   _SolarisJob();	
#endif  // __WIN32__

private:
	void   _ProcessCheckMode();
	void   _ProcessRcvEvt();

private:
	bool                m_bTerminate;
	int                 m_nModeState;
	std::string         m_strModeData;

#if defined(_WIN32)
	bool                m_bEventExist;
	std::string         m_strSynEventControl;
	std::string         m_strSynEventMonitor;
	std::string         m_strSynEventStop;	
	HANDLE              m_hEventArr[SYNEVENT_CODE_COUNT]; // 0 - Control, 1 - Monitor, 2 - Stop
#else		
	std::string         m_strSynIDShm;	
	PSHMBUFF            m_pShmBuff;
#endif  // __WIN32__

};

NS_END(TA_DB_Sync)

#endif  // SYNMODEMONITOR_H