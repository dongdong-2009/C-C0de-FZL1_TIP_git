/** 
*
* @author:  Luo HuiRong
* @version: $Revision: #1 $
* 
* ProcessRWGuard.cpp: implementation of the ProcessRWGuard class.
*
*/
//////////////////////////////////////////////////////////////////////
#include "ProcessRWGuard.h"
#include <iostream>
#include <time.h>
#include <windows.h>
#include "core/utilities/src/DebugUtil.h"
using namespace std;

namespace TA_Base_Core
{
	ProcessRWGuard::ProcessRWGuard( IProcessRWLockable& lockable, READ_WRITE_ACCESS rwAccess /*= WRITE_ACCESS*/, bool log )
		: m_lockable(lockable), m_log (log),m_acquired(false)
	{
		//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugInfo, "ProcessRWGuard lockable");
		
		if (m_log)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
				"ProcessRWGuard: Attempting lock acquire, address %lx, AccessRight %s", (unsigned long)&m_lockable, rwAccess?"READ":"WRITE");
		}

		
	    acquire(rwAccess);
		 
		if (m_log)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
				"ProcessRWGuard: Acquired lock, address %lx, AccessRight %s", (unsigned long)&m_lockable, rwAccess?"READ":"WRITE");
		}
	}
	
 	ProcessRWGuard::ProcessRWGuard( IProcessRWLockable& lockable, bool log  )
 		: m_lockable(lockable), m_log (log),m_acquired(false)
 	{
 		
 		//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugInfo, "enable Time out");
 		

 	}
	
	bool ProcessRWGuard::acquire(int MillisecondTimout, READ_WRITE_ACCESS rwAccess/* = WRITE_ACCESS*/)
	{
 		if (m_log)
 		{
 			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
 				"ProcessRWGuard: Attempting lock acquire, address %lx, AccessRight %s", (unsigned long)&m_lockable, rwAccess?"READ":"WRITE");
 		}
 		
		bool r = m_lockable.acquire(MillisecondTimout, rwAccess);

 		if (m_log == true && r == true)
 		{
 			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
 				"ProcessRWGuard: Acquired lock, address %lx, AccessRight %s", (unsigned long)&m_lockable, rwAccess?"READ":"WRITE");
 		}
		else if (m_log == true && r == false)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
				"ProcessRWGuard: Failed to Acquired lock, address %lx, AccessRight %s", (unsigned long)&m_lockable, rwAccess?"READ":"WRITE");			
		}

		m_acquired=true;
		return r;
	}
    //
    // Destructor
    //
	ProcessRWGuard::~ProcessRWGuard()
	{
		if (m_log)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
				"ProcessRWGuard: Releasing lock, address %lx", (unsigned long)&m_lockable);
		}

		release();

	}
}
