/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/NonReentrantRWThreadLocable.cpp $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/02/06 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#include "NonReentrantRWThreadLockable.h"


namespace TA_Base_App
{
	/** 
	* acquire
	*/
	void NonReentrantRWThreadLocable::acquire(READ_WRITE_ACCESS rwAccess /*= WRITE_ACCESS*/)
	{
		if (rwAccess == READ_ACCESS)
		{
			m_sharedLock.lock_shared();
		}
		else if (rwAccess == WRITE_ACCESS)
		{
			m_sharedLock.lock();
		}

		m_rwAccess = rwAccess;
	}
    
	bool NonReentrantRWThreadLocable::acquire(int MillisecondTimeout, READ_WRITE_ACCESS rwAccess /*= WRITE_ACCESS*/)
	{
		boost::system_time const timeout=boost::get_system_time() + boost::posix_time::milliseconds(MillisecondTimeout);
		
		if (rwAccess == READ_ACCESS)
		{
			if(m_sharedLock.timed_lock_shared(timeout))
			{
				m_rwAccess = rwAccess;
				return true;
			}
		}
		else if (rwAccess == WRITE_ACCESS)
		{
			if(m_sharedLock.timed_lock(timeout))
			{
				m_rwAccess = rwAccess;
				return true;
			}
		}

		return false;
	}
    
	/** 
	* release
	*/
	void NonReentrantRWThreadLocable::release()
	{
		if (m_rwAccess == READ_ACCESS)
		{
			m_sharedLock.unlock_shared();
		}
		else if (m_rwAccess == WRITE_ACCESS)
		{
			m_sharedLock.unlock();
		}
	}
}