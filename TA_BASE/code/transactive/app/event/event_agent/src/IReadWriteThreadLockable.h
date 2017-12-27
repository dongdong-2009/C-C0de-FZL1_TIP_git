/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/IReadWriteThreadLockable.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/02/06 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */


#ifndef __IREADWRITE_THREAD_LOCKABLE_H_INCLUDED__
#define __IREADWRITE_THREAD_LOCKABLE_H_INCLUDED__

namespace TA_Base_App
{
	enum READ_WRITE_ACCESS {WRITE_ACCESS = 0, READ_ACCESS};
	class IReadWriteThreadLockable  
	{
	public:
	
		virtual ~IReadWriteThreadLockable(){};
		/** 
		* acquire
		*
		* Acquire the lock.
		*
		*/
		virtual void acquire(READ_WRITE_ACCESS rwAccess = WRITE_ACCESS) = 0;	

		virtual bool acquire(int MillisecondTimeout, READ_WRITE_ACCESS rwAccess = WRITE_ACCESS) = 0;	
		
		/** 
		* release
		*
		* Release the lock.
		*
		* PreCondition: This IProcessRWLockable is currently acquired. 
		*/
		virtual void release() = 0;
	};
}

#endif //__IREADWRITE_THREAD_LOCKABLE_H_INCLUDED__


