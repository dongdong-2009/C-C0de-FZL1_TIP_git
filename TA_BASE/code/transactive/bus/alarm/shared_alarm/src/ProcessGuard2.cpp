/** 
*
* @author:  Luo HuiRong
* @version: $Revision: #1 $
* 
* ProcessGuard2.cpp: implementation of the ProcessGuard2 class.
*
*/
//////////////////////////////////////////////////////////////////////

#include "ProcessGuard2.h"
#include "core/utilities/src/DebugUtil.h"
using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
	ProcessGuard2::ProcessGuard2( IProcessLockable2& lockable, bool log )
		: m_lockable(lockable), m_log (log),m_acquired(false)
	{
		//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugInfo, "ProcessGuard2 lockable");
		
		if (m_log)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
				"ProcessGuard2: Attempting lock acquire, address %lx", (unsigned long)&m_lockable);
		}
		
	    acquire();
		 
		
		if (m_log)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
				"ProcessGuard2: Acquired lock, address %lx", (unsigned long)&m_lockable);
		}
	}
	

 

	ProcessGuard2::ProcessGuard2( IProcessLockable2& lockable,  int enableTimeout, bool log  )
		: m_lockable(lockable), m_log (log),m_acquired(false)
	{
		
		//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugInfo, "enable Time out");
		
		if (m_log)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
				"ProcessGuard2: Attempting lock acquire, address %lx", (unsigned long)&m_lockable);
		}
		
		//m_lockable.acquire();
		
		if (m_log)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
				"ProcessGuard2: Acquired lock, address %lx", (unsigned long)&m_lockable);
		}
	}
	
    //
    // Destructor
    //
	ProcessGuard2::~ProcessGuard2()
	{
		if (m_log)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
				"ProcessGuard2: Releasing lock, address %lx", (unsigned long)&m_lockable);
		}
		release();
		
	}
}
