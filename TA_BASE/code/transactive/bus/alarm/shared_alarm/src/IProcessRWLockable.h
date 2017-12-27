
#ifndef IProcessRWLockable_H_INCLUDE
#define IProcessRWLockable_H_INCLUDE

/**
* @author:  Noel R. Tajanlangit
* @version: $Revision: #1 $
*
* Interface definition for an object that allows atomic acquisition of resources
* among process-safe manner. Allows for varying ProcessLockable implementations to 
* be interchanged without modification to client code.
*
*/

namespace TA_Base_Core
{
	enum READ_WRITE_ACCESS {WRITE_ACCESS = 0, READ_ACCESS};
	class IProcessRWLockable  
	{
	public:
	
		virtual ~IProcessRWLockable(){};
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

#endif //IProcessRWLockable_H_INCLUDE