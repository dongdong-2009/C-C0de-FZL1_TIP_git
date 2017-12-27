/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/ThreadRWGuard.cpp $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/02/06 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#include "app/event/event_agent/src/ThreadRWGuard.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Core;

namespace TA_Base_App
{
	ThreadRWGuard::ThreadRWGuard( IReadWriteThreadLockable& lockable, READ_WRITE_ACCESS rwAccess /*= WRITE_ACCESS*/, bool log )
		: m_lockable(lockable), m_log (log),m_acquired(false)
	{
		//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugInfo, "ProcessRWGuard lockable");

		if (m_log)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
				"ThreadRWGuard: Attempting lock acquire, address %lx, AccessRight %s", (unsigned long)&m_lockable, rwAccess?"READ":"WRITE");
		}


		acquire(rwAccess);

		if (m_log)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
				"ThreadRWGuard: Acquired lock, address %lx, AccessRight %s", (unsigned long)&m_lockable, rwAccess?"READ":"WRITE");
		}
	}

	ThreadRWGuard::ThreadRWGuard( IReadWriteThreadLockable& lockable, bool log  )
		: m_lockable(lockable), m_log (log),m_acquired(false)
	{

		//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugInfo, "enable Time out");


	}

	bool ThreadRWGuard::acquire(int MillisecondTimeout, READ_WRITE_ACCESS rwAccess/* = WRITE_ACCESS*/)
	{
		if (m_log)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
				"ThreadRWGuard: Attempting lock acquire, address %lx, AccessRight %s", (unsigned long)&m_lockable, rwAccess?"READ":"WRITE");
		}

		bool r = m_lockable.acquire(MillisecondTimeout, rwAccess);

		if (m_log == true && r == true)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
				"ThreadRWGuard: Acquired lock, address %lx, AccessRight %s", (unsigned long)&m_lockable, rwAccess?"READ":"WRITE");
		}
		else if (m_log == true && r == false)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
				"ThreadRWGuard: Failed to Acquired lock, address %lx, AccessRight %s", (unsigned long)&m_lockable, rwAccess?"READ":"WRITE");			
		}

		m_acquired=true;
		return r;
	}
	//
	// Destructor
	//
	ThreadRWGuard::~ThreadRWGuard()
	{
		if (m_log)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace,
				"ThreadRWGuard: Releasing lock, address %lx", (unsigned long)&m_lockable);
		}

		release();

	}
}