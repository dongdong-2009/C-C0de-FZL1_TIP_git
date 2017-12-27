/** 
*
* @author:  Luo HuiRong
* @version: $Revision: #1 $ 
*
* implementation of the NonReEntrantProcessLockable2 class.
*/
#include "NonReEntrantProcessLockable2.h"
#include "core/utilities/src/DebugUtil.h"
using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
	NonReEntrantProcessLockable2::NonReEntrantProcessLockable2(std::string name)
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
			info="NonReEntrantProcessLockable2 create=" + name;
			
			
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

	NonReEntrantProcessLockable2::~NonReEntrantProcessLockable2()
	{
		#ifdef __WIN32__
			if(m_Mutexlock!=0)
			{
				CloseHandle(m_Mutexlock);
				m_Mutexlock=0;
			}
		#endif

	}
	void NonReEntrantProcessLockable2::acquire()
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

	bool NonReEntrantProcessLockable2::acquire(int MillisecondTimeout)
	{
#ifdef __WIN32__
		if(m_Mutexlock!=0)
		{
			if (m_isLockCurrentlyAcquired)
			{
				return true;
			}
			if (MillisecondTimeout>0 )
			{
			    DWORD  r=WaitForSingleObject(m_Mutexlock, MillisecondTimeout);
				if (r==WAIT_TIMEOUT)
				{
					LOG1(SourceInfo, DebugUtil::DebugError, "acquire timeout %d",MillisecondTimeout);
					return false;
				}
				
			}
			else
			{
				WaitForSingleObject(m_Mutexlock, INFINITE);
			}
			m_isLockCurrentlyAcquired=TRUE;
		}
#endif
		return true;
		
	}


		
	void NonReEntrantProcessLockable2::release()
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
