/** 
*
* @author:  Luo HuiRong
* @version: $Revision: #1 $
* 
* ProcessGuard.cpp: implementation of the ProcessGuard class.
*
*/
//////////////////////////////////////////////////////////////////////

#include "ProcessGuard.h"
#include "core/utilities/src/DebugUtil.h"
using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
	ProcessGuard::ProcessGuard( IProcessLockable& lockable, bool log )
		: m_lockable(lockable), m_log (log)
	{
		
		if (m_log)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
				"ProcessGuard: Attempting lock acquire, address %lx", (unsigned long)&m_lockable);
		}
		
		m_lockable.acquire();
		
		if (m_log)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
				"ProcessGuard: Acquired lock, address %lx", (unsigned long)&m_lockable);
		}
	}
	
	
    //
    // Destructor
    //
	ProcessGuard::~ProcessGuard()
	{
		if (m_log)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
				"ProcessGuard: Releasing lock, address %lx", (unsigned long)&m_lockable);
		}
		
		m_lockable.release();
	}
}
