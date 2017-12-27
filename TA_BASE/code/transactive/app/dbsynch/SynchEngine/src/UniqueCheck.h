/********************************************************************
//
//	FileName	:	UniqueCheck.h
//	Version		:	1.0
//	Author		:	zhilin,ouyang
//	Date		:	2012-01-06 16:44:41
//	Comment		:	The class used to ensure only one process intance 
//                      exist.
//
*********************************************************************/
#ifndef __UniqueCheck_h_
#define __UniqueCheck_h_
#include "CommonDef.h"

#if defined(_WIN32)
#include <Windows.h>
#else
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <thread.h>
#include<errno.h>
#endif  // __WIN32__

NS_BEGIN(TA_DB_Sync)

// Ensure only one process instance
class CUniqueCheck
{
private:

	#if defined(_WIN32)
    static HANDLE s_hMutex;
	#endif

public:
    CUniqueCheck();
	~CUniqueCheck();

private:
	int  _LockFile(int nfd);
	void _UniqueCheck();
	void _DeleteMutex();
 
};

extern void g_UniqueCheck(void);

NS_END(TA_DB_Sync)

#endif // __UniqueCheck_h_