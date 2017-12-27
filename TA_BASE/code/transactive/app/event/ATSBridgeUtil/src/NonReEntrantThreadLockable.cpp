// NonReEntrantThreadLockable.cpp: implementation of the NonReEntrantThreadLockable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NonReEntrantThreadLockable.h"

//#include "core/utilities/src/DebugUtil.h"
//using TA_Base_Core::DebugUtil;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus
{
	 

	NonReEntrantThreadLockable::NonReEntrantThreadLockable():m_isLockCurrentlyAcquired(FALSE)
	{
//		InitializeCriticalSection(&cs);
		InitializeCriticalSectionAndSpinCount(&cs,0);
	}

	NonReEntrantThreadLockable::NonReEntrantThreadLockable(std::string name):m_Mutexlock(0),m_isLockCurrentlyAcquired(FALSE)
	{	  
		std::string info;
		info="NonReEntrantThreadLockable create=" + name;
		
		
		//LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, info.c_str());
		SECURITY_DESCRIPTOR sd;        

		if (!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION)) 
		{
			DWORD errorcode;
			errorcode= GetLastError();
			//LOG1(SourceInfo,DebugUtil::DebugError, "InitializeSecurityDescriptor failed code=%u", errorcode );
			
		};
		if (!SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE))
		{
			DWORD errorcode;
			errorcode= GetLastError();
			//LOG1(SourceInfo,DebugUtil::DebugError, "SetSecurityDescriptorDacl failed code=%u", errorcode );			
			
		};	
		SECURITY_ATTRIBUTES sa = { sizeof sa, &sd, TRUE };
		m_Mutexlock=CreateMutex(&sa,FALSE, (name.c_str()));

		if (m_Mutexlock==0)
		{
			DWORD errorcode= GetLastError();
			//LOG1(SourceInfo,DebugUtil::DebugError, "SetSecurityDescriptorDacl failed code=%u", errorcode );		
		}		

	}

	NonReEntrantThreadLockable::~NonReEntrantThreadLockable()
	{
// 		if(m_Mutexlock!=0)
// 		{
// 			CloseHandle(m_Mutexlock);
// 			m_Mutexlock=0;
// 		}

		 
		DeleteCriticalSection(&cs);
		m_isLockCurrentlyAcquired=FALSE;

	}
	void NonReEntrantThreadLockable::acquire()
	{
// 		if(m_Mutexlock!=0)
// 		{
// 		   if (m_isLockCurrentlyAcquired)
// 		   {
// 			   return;
// 		   }		   
// 		   WaitForSingleObject(m_Mutexlock, INFINITE);
// 		   m_isLockCurrentlyAcquired=TRUE;
// 		}

	    	  
		EnterCriticalSection(&cs);
		 
		
	}
	void NonReEntrantThreadLockable::release()
	{
  
	    LeaveCriticalSection(&cs);			
	}
}
