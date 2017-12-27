/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/alarm/src/SharedMem.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2011/12/29 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This class contains the shared memory utility to manage the shared memory objects.
  */

#if !defined(__SHARED_MEMORY__)
#define __SHARED_MEMORY__
#pragma once

#include <boost/interprocess/managed_windows_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/named_mutex.hpp> // in replacement of NonReEntrantProcessLockable
#include <boost/interprocess/permissions.hpp>

using namespace boost::interprocess;

const char* SHARED_MEM_NAME	= "Global\\TestSharedMemory";

typedef basic_managed_windows_shared_memory 
	<char
	,rbtree_best_fit<mutex_family>
	,iset_index>
fixed_managed_winShmem;

namespace TA_Base_Core
{
	class CSharedMem
	{
	public:
		typedef enum create_flag {_create = 0, _open, _open_create }CREATE_FLAG;

		/**
         * getSegmentInstance
         *
         * returns the only instance of the segment manager.
         *
         * @param: none
         *
         * @return: pointer to the segment manager.
         */
		static fixed_managed_winShmem * getSegmentInstance()
		{
			FUNCTION_ENTRY("CSharedMem::getSegmentInstance");
			if (m_pSegment == NULL)
			{
				if(create(SHARED_MEM_NAME, _open) == false)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Unable to open shared memory object, try to create object...");
					if(create(SHARED_MEM_NAME, _open_create) == false)
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"Fatal error. Unable to open or create shared memory object.");
						return NULL;
					}
				}
			}
			FUNCTION_EXIT;
			return m_pSegment;
		}

		/**
         * mapSharedMem
         *
         * create and map the shared memory.
         *
         * @param: CREATE_FLAG - specifies how to create the shared memory default is _open_create
         *
         * @return: bool - true on success otherwise false.
         */
		static bool mapSharedMem(CREATE_FLAG create_flag = _open_create)
		{
			FUNCTION_ENTRY("CSharedMem::mapSharedMem");
			bool bRet = create(SHARED_MEM_NAME, create_flag);
			if (false == bRet && create_flag == _create)
			{
				// Shared memory maybe existing try opening the shared memory
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo," Shared memory may already be existing, try opening shared memory");
				bRet = create(SHARED_MEM_NAME, _open_create);
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Return %s", bRet?"TRUE":"FALSE");
			FUNCTION_EXIT;
			return bRet;
		}

		/**
         * unmapSharedMem
         *
         * unmap and releases the shared memory.
         *
         * @param: none
         *
         * @return: bool - true on success otherwise false.
         */
		static bool unmapSharedMem()
		{
			FUNCTION_ENTRY("CSharedMem::unmapSharedMem");
			bool bRetVal = false;
			if (m_pSegment != NULL)
			{
				m_pSegment->~basic_managed_windows_shared_memory();
				delete m_pSegment;
				m_pSegment = NULL;
				bRetVal = true;
			}
			FUNCTION_EXIT;
			return bRetVal;
		}

		/**
         * alloc
         *
         * allocates a shared memory according to the specified size.
         *
         * @param: size_t - size of the memory requested
         *
         * @return: void* - a pointer to shared memory allocated on success otherwise NULL
         */
		static void * alloc(size_t numBytes)
		{
			FUNCTION_ENTRY("CSharedMem::alloc");
			void * ptr = 0;
			if(m_pSegment == NULL)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Segment manager is not initialize.");
				return ptr;
			}
			try 
			{
				ptr = m_pSegment->allocate(numBytes);
			}
			catch(...)
			{
				ptr = 0;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught an unknown exception while allocating memory.");
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Allocated address = %p", ptr);
			FUNCTION_EXIT;
			return ptr;
		}

		/**
         * dealloc
         *
         * deallocates a shared memory based on the pointer address.
         *
         * @param: void * - pointer to the shared memory
         *
         * @return: none
         */
		static void dealloc(void *ptr)
		{
			FUNCTION_ENTRY("CSharedMem::dealloc");
			if(m_pSegment == NULL)
				return;

			m_pSegment->deallocate(ptr);
			FUNCTION_EXIT;
		}

		/**
         * destroy
         *
         * destroys the name object in the shared memory and release the memory allocated.
         *
         * @param: const char* - the name of the shared object to destroy
         *
         * @return: bool - true if object is found and destroyed otherwise false
         */
		template<class T>
		static bool destroy(const char* object_name)
		{
			if (std::strlen(object_name) == 0)
			{
				return false;
			}
			return m_pSegment->destroy<T>(object_name);
		}

		/**
         * find
         *
         * finds the name object in the shared memory
         *
         * @param: const char* - the name of the shared object to find
         *
         * @return: pointer to the named object in the shared memory otherwise NULL
         */
		template<class T>
		static T* find(const char* object_name)
		{
			FUNCTION_ENTRY("CSharedMem::find");
			if (std::strlen(object_name) == 0)
			{
				return NULL;
			}
			std::pair<T*, std::size_t> Result = m_pSegment->find<T>(object_name);

			FUNCTION_EXIT;
			return Result.first;
		}

	private:
		// Segment manager
		static fixed_managed_winShmem *m_pSegment;

		/**
         * create
         *
         * finds the name object in the shared memory
         *
         * @param: const char* - the name of the shared object to create
		 *		   CREATE_FLAG - specifies how to create the shared memory default is _open_create
         *
         * @return: bool - true on success otherwise false
         */
		static bool create(const char* strSharedName, CREATE_FLAG create_flag)
		{
			FUNCTION_ENTRY("CSharedMem::create");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Create flag = %d", (int)create_flag);
			bool nRet = true;

			try
			{	
				boost::interprocess::permissions objPermision;
				objPermision.set_unrestricted();
				switch (create_flag)
				{
				case _create:
					m_pSegment = new fixed_managed_winShmem(create_only, strSharedName, 128*1024*1024, (void*)(1024*1024*1024), objPermision);
					break;
				case _open:
					m_pSegment = new fixed_managed_winShmem(open_only, strSharedName, (void*)(1024*1024*1024)); 
					break;
				case _open_create:
					m_pSegment = new fixed_managed_winShmem(open_or_create, strSharedName, 128*1024*1024, (void*)(1024*1024*1024), objPermision);
					break;
				}
				if (m_pSegment == NULL)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo," Unable to create windows manage shared memory segment");
					nRet = false;
				}
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"Caught an unknown exception in TA_Base_Core::CSharedmem::create");
				nRet = false;
			}

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Result = %s", nRet?"TRUE":"FALSE");
			FUNCTION_EXIT;
			return nRet;
		}
	};
}

fixed_managed_winShmem* TA_Base_Core::CSharedMem::m_pSegment = NULL;

#endif // __SHARED_MEMORY__