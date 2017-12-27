
#ifndef IProcessLockable2_H_INCLUDE
#define IProcessLockable2_H_INCLUDE

/**
* @author:  Luo HuiRong
* @version: $Revision: #1 $
*
* Interface definition for an object that allows atomic acquisition of resources
* among process-safe manner. Allows for varying ProcessLockable implementations to 
* be interchanged without modification to client code.
*
*/
 
namespace TA_Base_Core
{
	class IProcessLockable2  
	{
	public:
		 
		virtual ~IProcessLockable2(){};
		/** 
		* acquire
		*
		* Acquire the lock.
		*
		*/
		virtual void acquire() = 0;	

		virtual bool acquire(int MillisecondTimeout) = 0;	
		
		/** 
		* release
		*
		* Release the lock.
		*
		* PreCondition: This IProcessLockable2 is currently acquired. 
		*/
		virtual void release() = 0;
	};
}

#endif //IProcessLockable2_H_INCLUDE