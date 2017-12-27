// IProcessLockable.h: interface for the IProcessLockable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPROCESSLOCKABLE_H__71B186AB_44DB_4772_896E_DC507C435459__INCLUDED_)
#define AFX_IPROCESSLOCKABLE_H__71B186AB_44DB_4772_896E_DC507C435459__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_Bus
{
	class IProcessLockable  
	{
	public:
		IProcessLockable(){};
		virtual ~IProcessLockable(){};
		/** 
		* acquire
		*
		* Acquire the lock.
		*
		*/
		virtual void acquire() = 0;	
		
		/** 
		* release
		*
		* Release the lock.
		*
		* PreCondition: This IProcessLockable is currently acquired. 
		*/
		virtual void release() = 0;
	};
}

#endif // !defined(AFX_IPROCESSLOCKABLE_H__71B186AB_44DB_4772_896E_DC507C435459__INCLUDED_)
