// NonReEntrantProcessLockable.cpp: implementation of the NonReEntrantProcessLockable class.
//
//////////////////////////////////////////////////////////////////////

#include "NonReEntrantProcessLockable.h"
//#include "core/utilities/src/DebugUtil.h"
//using TA_Base_Core::DebugUtil;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus
{
	NonReEntrantProcessLockable::NonReEntrantProcessLockable(std::string name):m_Mutexlock(0),m_isLockCurrentlyAcquired(FALSE)
	{	  
		std::string info;
		info="NonReEntrantProcessLockable create=" + name;
		
		
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
		m_Mutexlock=CreateMutex(&sa,FALSE, name.c_str());

		if (m_Mutexlock==0)
		{
			DWORD errorcode= GetLastError();
			//LOG1(SourceInfo,DebugUtil::DebugError, "SetSecurityDescriptorDacl failed code=%u", errorcode );		
		}		

	}

	NonReEntrantProcessLockable::~NonReEntrantProcessLockable()
	{
		if(m_Mutexlock!=0)
		{
			CloseHandle(m_Mutexlock);
			m_Mutexlock=0;
		}

	}
	void NonReEntrantProcessLockable::acquire()
	{
		if(m_Mutexlock!=0)
		{
		   if (m_isLockCurrentlyAcquired)
		   {
			   return;
		   }		   
		   WaitForSingleObject(m_Mutexlock, INFINITE);
		   m_isLockCurrentlyAcquired=TRUE;
		}
		
	}
	void NonReEntrantProcessLockable::release()
	{
		if(m_Mutexlock!=0)
		{
			ReleaseMutex(m_Mutexlock);
			m_isLockCurrentlyAcquired=FALSE;
		}
		
	}
}
