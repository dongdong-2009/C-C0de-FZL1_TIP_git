/** 
*
* @author:  Luo HuiRong
* @version: $Revision: #1 $ 
*
* implementation of the NonReEntrantProcessLockable class.
*/
#include "NonReEntrantProcessLockable.h"
#include "core/utilities/src/DebugUtil.h"
using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
	NonReEntrantProcessLockable::NonReEntrantProcessLockable(std::string name)
#ifdef __WIN32__
		:m_Mutexlock(0),
		m_isLockCurrentlyAcquired(FALSE)
#else
		:m_isLockCurrentlyAcquired(false)
#endif

	{	  
		#ifdef __WIN32__
		    //give the mutex a default security token to make sure everyone can access it.
			std::string info;
			info="NonReEntrantProcessLockable create=" + name;
			
			
			LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, info.c_str());
			SECURITY_DESCRIPTOR sd;        

			if (!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION)) 
			{
				DWORD errorcode;
				errorcode= GetLastError();
				LOG1(SourceInfo,DebugUtil::DebugError, "InitializeSecurityDescriptor failed code=%u", errorcode );
				
			};
			if (!SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE))
			{
				DWORD errorcode;
				errorcode= GetLastError();
				LOG1(SourceInfo,DebugUtil::DebugError, "SetSecurityDescriptorDacl failed code=%u", errorcode );			
				
			};	
			SECURITY_ATTRIBUTES sa = { sizeof sa, &sd, TRUE };
			m_Mutexlock=CreateMutex(&sa,FALSE, name.c_str());

			if (m_Mutexlock==0)
			{
				DWORD errorcode= GetLastError();
				LOG1(SourceInfo,DebugUtil::DebugError, "SetSecurityDescriptorDacl failed code=%u", errorcode );		
			}	
			
      #endif

	}

	NonReEntrantProcessLockable::~NonReEntrantProcessLockable()
	{
		#ifdef __WIN32__
			if(m_Mutexlock!=0)
			{
				CloseHandle(m_Mutexlock);
				m_Mutexlock=0;
			}
		#endif

	}
	void NonReEntrantProcessLockable::acquire()
	{
		#ifdef __WIN32__
			if(m_Mutexlock!=0)
			{
			   if (m_isLockCurrentlyAcquired)
			   {
				   return;
			   }
			   WaitForSingleObject(m_Mutexlock, INFINITE);
			   m_isLockCurrentlyAcquired=TRUE;
			}
		#endif
		
	}
	void NonReEntrantProcessLockable::release()
	{
		#ifdef __WIN32__
			if(m_Mutexlock!=0)
			{
				ReleaseMutex(m_Mutexlock);
				m_isLockCurrentlyAcquired=FALSE;
			}
		#endif
		
	}
}
