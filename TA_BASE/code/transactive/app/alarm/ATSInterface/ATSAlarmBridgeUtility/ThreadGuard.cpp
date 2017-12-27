// ThreadGuard.cpp: implementation of the ThreadGuard class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ThreadGuard.h"
//#include "core/utilities/src/DebugUtil.h"
//using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	ThreadGuard::ThreadGuard( IThreadLockable& lockable, bool log )
		: m_lockable(lockable), m_log (log)
	{
		
// 		if (m_log)
// 		{
// 			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
// 				"ThreadGuard: Attempting lock acquire, address %lx", (unsigned long)&m_lockable);
//		}
		
		m_lockable.acquire();
		
// 		if (m_log)
// 		{
// 			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
// 				"ThreadGuard: Acquired lock, address %lx", (unsigned long)&m_lockable);
//		}
	}
	
	
    //
    // Destructor
    //
	ThreadGuard::~ThreadGuard()
	{
// 		if (m_log)
// 		{
// 			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
// 				"ThreadGuard: Releasing lock, address %lx", (unsigned long)&m_lockable);
//		}
		
		m_lockable.release();
	}
}
