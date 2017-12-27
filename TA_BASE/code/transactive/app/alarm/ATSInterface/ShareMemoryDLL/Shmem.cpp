// Shmem.cpp: implementation of the Shmem class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Shmem.h"
//## #include "core/utilities/src/DebugUtil.h"

#include <fstream>

const char* CREATE_ALLOCATOR_LOCK_NAME = "TA_CreateAllocatorLock";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*
TA_Base_Core::ALLOCATOR TA_Base_Core::Shmem::m_allocator("C:/Shared.map", "Shared_Lock", 
									&ACE_MMAP_Memory_Pool_Options(ACE_DEFAULT_BASE_ADDR,
							 ACE_MMAP_Memory_Pool_Options::ALWAYS_FIXED, 1, 64*1024*1024
							 ));
							 */
void* TA_Base_Core::Shmem::m_logInitializer = TA_Base_Core::Shmem::initializeLoger();
//TA_Base_Core::ALLOCATOR* TA_Base_Core::Shmem::m_allocator = TA_Base_Core::Shmem::create();
TA_Base_Core::ALLOCATOR* TA_Base_Core::Shmem::m_allocator = NULL;

void* TA_Base_Core::Shmem::initializeLoger()
{
	 //TA_Base_Core::DebugUtil::getInstance().setFile("c:/shmem.log");
	 return NULL;
};


TA_Base_Core::ALLOCATOR* TA_Base_Core::Shmem::create(const char* mapfile /*= "C:/ATSShared.map"*/, bool attempDeleteOldFile /*= true*/)
{
	//###¡¡LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "======Shmem::create() enter ======");
	TA_Base_Core::ALLOCATOR* tempAlloc = NULL;
	try
	{
#ifdef WIN32
		if(attempDeleteOldFile)
		{
			// zyr: if all processes which used the shared memory have unmapped, ACE Allocator will delete the mapfile automaticly,
			// here just in case of... ( ?? double assurance )
			DeleteFile(mapfile);
		}
#endif	
		tempAlloc = new ALLOCATOR(mapfile, "Shared_Lock", 
			&ACE_MMAP_Memory_Pool_Options(ACE_DEFAULT_BASE_ADDR,
			ACE_MMAP_Memory_Pool_Options::ALWAYS_FIXED, 1, 10*1024*1024
			));
	}
	catch(...)
	{
		//### LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"Caught an unknown exception in TA_Base_Core::Shmem::create");
	}	

	//### LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "======Shmem::create() exit ======");
	return tempAlloc;
};

//zyr++: for map/unmap shared memory
int TA_Base_Core::Shmem::mapSharedMem(const char* mapfile /*= "C:/ATSShared.map"*/, bool attempDeleteOldFile /*= true*/)
{
	int ret = -1;
#ifdef WIN32
	HANDLE glock = CreateMutex(NULL, FALSE, CREATE_ALLOCATOR_LOCK_NAME);
	try
	{
		//### LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TA_Base_Core::Shmem::mapSharedMem entry ...");
		if(m_allocator == NULL)
		{
			m_allocator = create(mapfile, attempDeleteOldFile);
		}

		if(m_allocator != NULL)
		{
			ret = m_allocator->alloc().ref_counter();
			//### LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "call ref_counter in TA_Base_Core::Shmem::mapSharedMem, ref=%d", ret);
		}
	}
	catch(...)
	{
		//### LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"Caught an unknown exception in TA_Base_Core::Shmem::mapSharedMem");
	}
	
	ReleaseMutex(glock);
	CloseHandle(glock);
//### 	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TA_Base_Core::Shmem::mapSharedMem exit, ret=%d", ret);
#endif //Win32

	return ret;
}

int TA_Base_Core::Shmem::unmapSharedMem()
{
	int ret = 0;
#ifdef WIN32
	HANDLE glock = CreateMutex(NULL, FALSE, CREATE_ALLOCATOR_LOCK_NAME);
	try
	{
//### 		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TA_Base_Core::Shmem::unmapSharedMem entry ...");
		if(m_allocator != NULL)
		{
			ret = m_allocator->alloc().release();
			//### LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "call ref_counter in TA_Base_Core::Shmem::unmapSharedMem, ref=%d", ret);
			delete m_allocator;
			m_allocator = NULL;
		}
	}
	catch(...)
	{
		//### LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"Caught an unknown exception in TA_Base_Core::Shmem::unmapSharedMem");
	}	
	
	ReleaseMutex(glock);
	CloseHandle(glock);
//### 	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TA_Base_Core::Shmem::unmapSharedMem exit, ret=%d", ret);
#endif // Win32
	return ret;
}
//++zyr

/*
new ALLOCATOR("C:/Shared.map", "Shared_Lock", 
												&ACE_MMAP_Memory_Pool_Options(ACE_DEFAULT_BASE_ADDR,
												ACE_MMAP_Memory_Pool_Options::ALWAYS_FIXED, 1, 128*1024*1024));
*/