/********************************************************************
//
//	FileName	:	UniqueCheck.cpp
//	Version		:	1.0
//	Author		:	zhilin,ouyang
//	Date		:	2012-01-06 16:44:41
//	Comment		:	The class used to ensure only one process intance 
//                      exist.
//
*********************************************************************/
#include "UniqueCheck.h"
#include "UtilityFun.h"
#include "core/utilities/src/DebugUtil.h"

NS_BEGIN(TA_DB_Sync)

#if defined(_WIN32)
#define PROCESS_MUTEX       "450AD03A-AE43-40e3-BC3B-9F6DB4877983"
HANDLE CUniqueCheck::s_hMutex = NULL;
#else
#define PROCESS_MUTEX       "SynchEngineClient.pid"
#define LOCKMODE           (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
#endif

CUniqueCheck::CUniqueCheck()
{
	_UniqueCheck();
}

CUniqueCheck::~CUniqueCheck()
{
	_DeleteMutex();
}

void CUniqueCheck::_DeleteMutex()
{
#if defined(_WIN32)
	if (s_hMutex)
	{
		CloseHandle(s_hMutex);
		s_hMutex = NULL;
	}
#endif
}

int CUniqueCheck::_LockFile(int nfd)
{
#if defined(_WIN32)
	return Ret_Success;
#else
	struct flock fl;

	// the lock type£ºF_RDLCK-- read lock£»F_WRLCK -- write lock£»F_UNLCK -- delete existing lock.
	fl.l_type = F_WRLCK;

	// l_start -- set the lock start position
	fl.l_start = 0;

	// l_whenec set l_start position¡£state£ºSEEK_SET -- file begin£»SEEK_CUR -- current position£»SEEK_END -- file end.
	fl.l_whence = SEEK_SET;

	// l_len -- set the lock memory size
	fl.l_len = 0;

	// l_pid -- the lock process
	// F_SETLK£ºwhen l_whence¡¢l_start¡¢l_len have been set, and if l_type set to F_RDLCK or F_WRLCK then the system allocate a lock;
	// if l_type set to F_UNLCK then free a existing lock.
	// return -1 when failed; errno: EACCES or EAGAIN¡£
	return (fcntl(nfd,F_SETLK,&fl));

#endif
}

void CUniqueCheck::_UniqueCheck()
{
#if defined(_WIN32)
	s_hMutex = NULL;

	s_hMutex = CreateMutex(NULL, TRUE, PROCESS_MUTEX);
	DEF_ASSERT_EXIT(s_hMutex);
	DEF_ERROR_EXIT(GetLastError() != ERROR_ALREADY_EXISTS);
	return;

Exit0:        
	if (s_hMutex)
	{
		CloseHandle(s_hMutex);
		s_hMutex = NULL;
	}
	ExitProcess(Ret_Failed);

#else
	int fd = Ret_Success;
	char buf[16] = {0};

    fd = open(PROCESS_MUTEX,O_RDWR|O_CREAT,LOCKMODE);

    if (fd < Ret_Success)
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_CreateFile_Failed, defErrMsg(Err_CreateFile_Failed));
        exit(Ret_Failed);
    }

    if (_LockFile(fd) < Ret_Success)
    {
        if (errno == EACCES || errno == EAGAIN)
        {
            close(fd);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"Message: %s", "Another instance is running.");
            exit(Ret_Success);
        }
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error Code: %d, Error message: %s", Err_LockFile_Failed, defErrMsg(Err_LockFile_Failed));
        exit(Ret_Failed);
    }

    ftruncate(fd,0);
	CUtilityFun::int2str(getpid(), buf);
    write(fd,buf,strlen(buf));

#endif
}


void g_UniqueCheck(void)
{
	static CUniqueCheck uniqueCheck;
}

NS_END(TA_DB_Sync)