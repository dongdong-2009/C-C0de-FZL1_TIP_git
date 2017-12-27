/** 
*
* @author:  Luo HuiRong
* @version: $Revision: #1 $ 
*
* implementation of the NonReEntrantProcessRWLockable class.
*/
#include "NonReEntrantProcessRWLockable.h"
#include <string>
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
	NonReEntrantProcessRWLockable::NonReEntrantProcessRWLockable(std::string name)
#ifdef __WIN32__
		:m_isLockCurrentlyAcquired(FALSE), m_hNamedMutex(0)//m_aceRW_ProcLock(0)
#else
		:m_isLockCurrentlyAcquired(false), m_hNamedMutex(0)//m_aceRW_ProcLock(0)
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
		m_hNamedMutex=CreateMutex(&sa,FALSE, name.c_str());
		
		if (m_hNamedMutex==0)
		{
			DWORD errorcode= GetLastError();
			LOG1(SourceInfo,DebugUtil::DebugError, "SetSecurityDescriptorDacl failed code=%u", errorcode );		
		}	

#endif
			
	}

	NonReEntrantProcessRWLockable::~NonReEntrantProcessRWLockable()
	{
#ifdef __WIN32__
		if(m_hNamedMutex!=0)
		{
			CloseHandle(m_hNamedMutex);
			m_hNamedMutex=0;
		}
#endif

	}
	void NonReEntrantProcessRWLockable::acquire(READ_WRITE_ACCESS rwAccess /*= WRITE_ACCESS*/)
	{
		
#ifdef __WIN32__
		if(m_hNamedMutex!=0)
		{
			if (m_isLockCurrentlyAcquired)
			{
				return;
			}
			WaitForSingleObject(m_hNamedMutex, INFINITE);
			m_isLockCurrentlyAcquired=TRUE;
		}
#endif
		
	}

	bool NonReEntrantProcessRWLockable::acquire(int MillisecondTimeout, READ_WRITE_ACCESS rwAccess /*= WRITE_ACCESS*/)
	{
#ifdef __WIN32__
		if(m_hNamedMutex!=0)
		{
			if (m_isLockCurrentlyAcquired)
			{
				return true;
			}
			if (MillisecondTimeout>0 )
			{
				DWORD  r=WaitForSingleObject(m_hNamedMutex, MillisecondTimeout);
				if (r==WAIT_TIMEOUT)
				{
					LOG1(SourceInfo, DebugUtil::DebugError, "acquire timeout %d",MillisecondTimeout);
					return false;
				}
				
			}
			else
			{
				WaitForSingleObject(m_hNamedMutex, INFINITE);
			}
			m_isLockCurrentlyAcquired=TRUE;
		}
#endif
		return true;	
	}


	void NonReEntrantProcessRWLockable::release()
	{
#ifdef __WIN32__
		if(m_hNamedMutex!=0)
		{
			ReleaseMutex(m_hNamedMutex);
			m_isLockCurrentlyAcquired=FALSE;
		}
#endif		
	}
}
