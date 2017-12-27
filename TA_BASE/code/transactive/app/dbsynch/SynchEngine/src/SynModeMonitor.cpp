/********************************************************************
//
//	FileName	:	SynModeMonitor.cpp
//	Version		:	1.0
//	Author		:	zhilin,ouyang
//	Date		:	2012-09-26 16:44:41
//	Comment		:	The class used to monitor the running mode for the 
//                  data syn application.
//
*********************************************************************/
#include "SynModeMonitor.h"
#include "core/utilities/src/DebugUtil.h"
#include "UtilityFun.h"
#include "SyncAQMsgManager.h" 

NS_BEGIN(TA_DB_Sync)

static const std::string    s_strDefaultSynName("/ST_DataSyn");
static const std::string    s_strDefaultEventName("Global\\");

CSynModeMonitor::CSynModeMonitor()
: m_bTerminate(false)
, m_nModeState(SYNMODE_CODE_CHECK_MODE)
{
#if defined(_WIN32)
	m_bEventExist = false;
	m_strSynEventControl = (s_strDefaultEventName + "ST_DataSynControl_");
	m_strSynEventMonitor = (s_strDefaultEventName + "ST_DataSynMonitor_");
	m_strSynEventStop = (s_strDefaultEventName + "ST_DataSynStop_");
	
	m_hEventArr[SYNEVENT_CODE_CONTROL] = CreateEvent(NULL, false, false, m_strSynEventControl.c_str());	
	m_hEventArr[SYNEVENT_CODE_MONITOR] = CreateEvent(NULL, false, false, m_strSynEventMonitor.c_str());
	m_hEventArr[SYNEVENT_CODE_STOP]    = CreateEvent(NULL, false, false, m_strSynEventStop.c_str());

	if(GetLastError() == ERROR_ALREADY_EXISTS) m_bEventExist = true;
	if (NULL == m_hEventArr[SYNEVENT_CODE_CONTROL] || 
		NULL == m_hEventArr[SYNEVENT_CODE_MONITOR] || 
		NULL == m_hEventArr[SYNEVENT_CODE_STOP])
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "CreateEvent failed! Error code: %d", GetLastError());
		exit(Ret_Failed);
	}
#else
	m_pShmBuff = NULL;
	m_strSynIDShm = (s_strDefaultSynName + "_Shm");
#endif
}

CSynModeMonitor::CSynModeMonitor(const std::string& strSynID)
: m_bTerminate(false)
, m_nModeState(SYNMODE_CODE_CHECK_MODE)
{
#if defined(_WIN32)
	m_bEventExist = false;
	m_strSynEventControl = (s_strDefaultEventName + "ST_DataSynControl_" + strSynID);
	m_strSynEventMonitor = (s_strDefaultEventName + "ST_DataSynMonitor_" + strSynID);
	m_strSynEventStop = (s_strDefaultEventName + "ST_DataSynStop_" + strSynID);

	m_hEventArr[SYNEVENT_CODE_CONTROL] = CreateEvent(NULL, false, false, m_strSynEventControl.c_str());	
	m_hEventArr[SYNEVENT_CODE_MONITOR] = CreateEvent(NULL, false, false, m_strSynEventMonitor.c_str());
	m_hEventArr[SYNEVENT_CODE_STOP]    = CreateEvent(NULL, false, false, m_strSynEventStop.c_str());

	if(GetLastError() == ERROR_ALREADY_EXISTS) m_bEventExist = true;
	if (NULL == m_hEventArr[SYNEVENT_CODE_CONTROL] || 
		NULL == m_hEventArr[SYNEVENT_CODE_MONITOR] || 
		NULL == m_hEventArr[SYNEVENT_CODE_STOP])
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "CreateEvent failed! Error code: %d", GetLastError());
		exit(Ret_Failed);
	}

#else
	m_pShmBuff = NULL;
	m_strSynIDShm = (s_strDefaultSynName + strSynID + "_Shm");
#endif
}

CSynModeMonitor::~CSynModeMonitor()
{
#if defined(_WIN32)
	CloseHandle(m_hEventArr[SYNEVENT_CODE_CONTROL]);
	CloseHandle(m_hEventArr[SYNEVENT_CODE_MONITOR]);
	CloseHandle(m_hEventArr[SYNEVENT_CODE_STOP]);
#else
	shm_unlink(m_strSynIDShm.c_str());

#endif  // __WIN32__
}

void  CSynModeMonitor::run()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Begin run CSynModeMonitor::run()");

	while (!m_bTerminate)
	{
		_ThreadJob();

		sleep(TIME_BASE_SECOND);
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End run CSynModeMonitor::run()");
}

void  CSynModeMonitor::terminate()
{
	m_bTerminate = true;

#if defined(_WIN32)
	SetEvent(m_hEventArr[SYNEVENT_CODE_STOP]);
#endif
}

void   CSynModeMonitor::setModeData(const std::string& strMode)
{
#if defined(_WIN32)
	if (m_bEventExist)
	{
		if(!CUtilityFun::strnicmp_(strMode.c_str(), defCmdLine_Mode_Active, strMode.size()))
		{
			SetEvent(m_hEventArr[SYNEVENT_CODE_CONTROL]);
		}
		else if(!CUtilityFun::strnicmp_(strMode.c_str(), defCmdLine_Mode_Passive, strMode.size()))
		{
			SetEvent(m_hEventArr[SYNEVENT_CODE_MONITOR]);
		}
		else if(!CUtilityFun::strnicmp_(strMode.c_str(), defCmdLine_Mode_Stop, strMode.size()))
		{
			SetEvent(m_hEventArr[SYNEVENT_CODE_STOP]);
		}

		exit(Ret_Success);
	}
	else
	{
		m_strModeData = strMode;
	}

#else
	int  nFd = 0;
	int  nRetCode = 0;
	int  nParaChk = 0;
	errno = 0;
	PSHMBUFF pShmBuff = NULL;

	nParaChk = CUtilityFun::stricmp_(defCmdLine_StatusSynch, strMode.c_str());
	nFd = shm_open(m_strSynIDShm.c_str(), O_CREAT | O_EXCL | O_RDWR, 0777);
	switch(errno)
	{
	case EEXIST:
		nFd = shm_open(m_strSynIDShm.c_str(), O_RDWR, 0777);
		if ( -1 == nFd) 
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s", errno, "open the shm failed.");

			shm_unlink(m_strSynIDShm.c_str());

			exit(Ret_Failed);
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"New mode: %s, size of shared buffer: %d",  strMode.c_str(), sizeof(SHMBUFF));
		
		pShmBuff = (PSHMBUFF)mmap(NULL, sizeof(SHMBUFF),
			PROT_READ | PROT_WRITE, MAP_SHARED, nFd, 0);
		if (MAP_FAILED == pShmBuff || !pShmBuff)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s", errno,  "map the shm failed.");

			exit(Ret_Failed);
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"Existing shm mode: %s, len: %u",  pShmBuff->szMode, pShmBuff->uiLen);

		nRetCode = CUtilityFun::strnicmp_(strMode.c_str(), pShmBuff->szMode, strMode.size());
		if (nRetCode && nParaChk)
		{
			pShmBuff->uiLen = strMode.size();
			memset(pShmBuff->szMode, 0, defMaxNameLen);
			memcpy(pShmBuff->szMode, strMode.c_str(), strMode.size());			
		}

		if (!CUtilityFun::stricmp_(defCmdLine_Mode_Stop, strMode.c_str()))
		{
			//	printf("stop successfully...");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Will notify SynchEngine to stop...");

			shm_unlink(m_strSynIDShm.c_str());
		}

		if (!nParaChk)
		{
			printf("current running mode: %s\n", pShmBuff->szMode);						
		}

		exit(Ret_Success);
		break;
	case EACCES:
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "The permissions  specified by oflag are denied.");
		exit(Ret_Success);
		break;
	case EINTR:
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "The shm_open() operation was interrupted.");
		exit(Ret_Success);
		break;
	case EINVAL:
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "The shm_open() operation is not supported for the given name.");
		exit(Ret_Success);
		break;
	case EMFILE:
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Too many file descriptors are currently in use by this process..");
		shm_unlink(m_strSynIDShm.c_str());
		exit(Ret_Success);
		break;
	case ENAMETOOLONG:
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "The length of the name string exceeds PATH_MAX.");
		exit(Ret_Success);
		break;
	case ENFILE:
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Too many shared memory objects are currently open in the system..");
		shm_unlink(m_strSynIDShm.c_str());
		exit(Ret_Success);
		break;
	default:
		m_strModeData = strMode;	

		if (!CUtilityFun::stricmp_(defCmdLine_Mode_Stop, m_strModeData.c_str()))
		{
			//		printf("stop successfully...");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SynchEngine stop already.");

			shm_unlink(m_strSynIDShm.c_str());
			exit(Ret_Success);
		}

		if (!nParaChk)
		{
			printf("SynchEngine not running...\n");	

			shm_unlink(m_strSynIDShm.c_str());
			exit(Ret_Success);
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"Current running mode: %s, size of shared buffer: %d",  strMode.c_str(), sizeof(SHMBUFF));

		nRetCode =  ftruncate(nFd, sizeof(SHMBUFF));
		if (Ret_Success != nRetCode)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s", errno,  "truncate the shm failed at first time.");
			return;
		}

		m_pShmBuff = (PSHMBUFF)mmap(NULL, sizeof(SHMBUFF),
			PROT_READ | PROT_WRITE, MAP_SHARED, nFd, 0);
		if (MAP_FAILED == m_pShmBuff || !m_pShmBuff)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error Code: %d, Error message: %s", errno,  "map the shm failed.");
			return;
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"Share memory address: %p",  m_pShmBuff);		

		m_pShmBuff->uiLen = m_strModeData.size();
		memcpy(m_pShmBuff->szMode, m_strModeData.c_str(), m_strModeData.size());

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"share memory mode data: %s, size: %d",  m_pShmBuff->szMode, m_pShmBuff->uiLen);
	}

#endif  // __WIN32__

}

void  CSynModeMonitor::_ThreadJob()
{
#if defined(_WIN32)
	 _WinntJob();
#else
	_SolarisJob();
#endif  // __WIN32__
}

#if defined(_WIN32)

void   CSynModeMonitor::_WinntJob()
{
	DWORD dwWaitRet = WaitForMultipleObjects(SYNEVENT_CODE_COUNT, m_hEventArr, false, INFINITE);
	switch(dwWaitRet)
	{
	case WAIT_OBJECT_0 + SYNEVENT_CODE_CONTROL:
		m_strModeData = defCmdLine_Mode_Active;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"SynMode will be change to state: %s",  m_strModeData.c_str());
		_ProcessRcvEvt();
		break;
	case WAIT_OBJECT_0 + SYNEVENT_CODE_MONITOR:
		m_strModeData = defCmdLine_Mode_Passive;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"SynMode will be change to state: %s",  m_strModeData.c_str());
		_ProcessRcvEvt();
		break;
	case WAIT_OBJECT_0 + SYNEVENT_CODE_STOP:
		m_strModeData = defCmdLine_Mode_Stop;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"SynMode will be change to state: %s",  m_strModeData.c_str());
		_ProcessRcvEvt();
		break;
	}
}
#else

void   CSynModeMonitor::_SolarisJob()
{
	switch (m_nModeState)
	{
	case SYNMODE_CODE_CHECK_MODE:
		_ProcessCheckMode();
		break;
	case SYNMODE_CODE_RCV_EVT:
		_ProcessRcvEvt();
		break;
	default:
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error message: %s", "the SynMode state is not support.");
		break;
	}
}

#endif  // __WIN32__

void   CSynModeMonitor::_ProcessCheckMode()
{
#if defined(_WIN32)

#else
	if (m_pShmBuff)
	{
		if (CUtilityFun::stricmp_(m_pShmBuff->szMode, m_strModeData.c_str()))
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"SynMode will be change to state: %s",  m_pShmBuff->szMode);
			m_strModeData = m_pShmBuff->szMode;
			m_nModeState = SYNMODE_CODE_RCV_EVT;			
		}
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal,
			"Error message: %s", "the shm address is null.");
		exit(Ret_Success);
	}

#endif  // __WIN32__
}

void  CSynModeMonitor::_ProcessRcvEvt()
{
	CSyncAQMsgManager::getInstance().setSynchMode(m_strModeData);

	m_nModeState = SYNMODE_CODE_CHECK_MODE;
}

NS_END(TA_DB_Sync)