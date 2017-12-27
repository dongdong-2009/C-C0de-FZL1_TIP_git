/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/synchronisation/src/ThreadGuard.cpp $
  * @author:  B Fountain
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Implementation of the ThreadGuard class
  *
  */

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
    //
    // Constructor
    //
    ThreadGuard::ThreadGuard( IThreadLockable& lockable, bool log )
		: m_lockable(lockable), m_log (log)
	{
		
		if (m_log)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
				"ThreadGuard: Attempting lock acquire, address %lx", (unsigned long)&m_lockable);
		}

		m_lockable.acquire();
					
		if (m_log)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
				"ThreadGuard: Acquired lock, address %lx", (unsigned long)&m_lockable);
		}
	}


    //
    // Destructor
    //
	ThreadGuard::~ThreadGuard()
	{
		if (m_log)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
				"ThreadGuard: Releasing lock, address %lx", (unsigned long)&m_lockable);
		}

		m_lockable.release();
	}

} // namespace TA_Base_Core
