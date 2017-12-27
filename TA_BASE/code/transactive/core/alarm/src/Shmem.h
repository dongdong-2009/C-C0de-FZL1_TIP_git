	// Shmem.h: interface for the Shmem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHMEM_H__5E852CBA_6FBC_4858_9551_D35071F1C4F7__INCLUDED_)
#define AFX_SHMEM_H__5E852CBA_6FBC_4858_9551_D35071F1C4F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <ace/Memory_Pool.h>
#include <ace/Malloc_T.h>
#include <ace/Null_Mutex.h>
//#include <ace/PI_Malloc.h>
//#include <ace/Process_Mutex.h>
namespace TA_Base_Core
{
	//typedef ACE_Allocator_Adapter< ACE_Malloc_T< ACE_MMAP_MEMORY_POOL, ACE_Process_Mutex, ACE_PI_Control_Block> > ALLOCATOR;
	typedef ACE_Allocator_Adapter< ACE_Malloc_T< ACE_MMAP_MEMORY_POOL, ACE_Null_Mutex, ACE_Control_Block> > ALLOCATOR;

	class Shmem  
	{
		//typedef ACE_Malloc_T<ACE_MMAP_MEMORY_POOL, ACE_Null_Mutex, ACE_Control_Block> Allocator;
	public:
		static void* alloc(size_t numBytes, const void *memoryBlockToBeNear = NULL)
		{
			if(m_allocator != NULL)
			{
				return m_allocator->malloc(numBytes);
			}

			return NULL;
		}

		static void dealloc(void *ptr)
		{
			if(m_allocator != NULL)
			{
				m_allocator->free(ptr);
			}
		}

		static int remove()
		{
			int ret = 0;
			if(m_allocator != NULL)
			{
				ret = m_allocator->remove();
			}

			return ret;
		}

 		static int release()
 		{
			int ret = 0;
			if(m_allocator != NULL)
			{
 				ret = m_allocator->alloc().release();
			}

			return ret;
 		}

//HP++
		static inline ALLOCATOR* getAllocator() 
		{
			return m_allocator;
		}

		static inline int sync()
		{
			if(m_allocator != NULL)
			{
				return m_allocator->sync();
			}

			return -1;
		}
//HP++

		int static find(const char* name, void*& pointer)
		{
			if(m_allocator != NULL)
			{
				return m_allocator->find(name, pointer);
			}

			return -1;
		};

		int static bind(const char* name, void* pointer)
		{
			if(m_allocator != NULL)
			{
				return m_allocator->bind(name, pointer);
			}

			return -1;
		};

		int static unbind(const char* name)
		{
			if(m_allocator != NULL)
			{
				return m_allocator->unbind(name);
			}

			return -1;
		};

		static void* initializeLoger();
		//zyr++: for map/unmap shared memory
		// return the reference counter
		static int refCounter()
		{
			if(m_allocator != NULL)
			{
				return m_allocator->alloc().ref_counter();
			}

			return 0;
		}
		// return the reference counter
		static int mapSharedMem(const char* mapfile = "C:/Shared.map", bool attempDeleteOldFile = true);
		// return the reference counter
		static int unmapSharedMem();
		//++zyr
		 
	private:
		static ALLOCATOR* create(const char* mapfile = "C:/Shared.map", bool attempDeleteOldFile = true);
		static void* m_logInitializer;
		static ALLOCATOR* m_allocator;
	};
}
#endif // !defined(AFX_SHMEM_H__5E852CBA_6FBC_4858_9551_D35071F1C4F7__INCLUDED_)
