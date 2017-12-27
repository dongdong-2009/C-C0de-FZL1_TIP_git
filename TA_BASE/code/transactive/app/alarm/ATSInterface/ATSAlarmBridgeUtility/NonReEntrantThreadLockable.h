// NonReEntrantThreadLockable.h: interface for the NonReEntrantThreadLockable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NONREENTRANTPROCESSLOCKABLE_H__18942A6E_C726_4610_BDDC_BC40D6ADF359__INCLUDED_)
#define AFX_NONREENTRANTPROCESSLOCKABLE_H__18942A6E_C726_4610_BDDC_BC40D6ADF359__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "stdafx.h"
#include <string>
#include "IThreadLockable.h"
#include "Windows.h"
namespace TA_Base_Bus
{
	class NonReEntrantThreadLockable : public IThreadLockable
	{
	public:
		NonReEntrantThreadLockable(std::string name);
		NonReEntrantThreadLockable();
		virtual ~NonReEntrantThreadLockable();

		/** 
		* acquire
		*
		* Acquire the lock.
		*
		* PreCondition: The lock is not already acquired by the current process.
		*/
        virtual void acquire();
        
        
        /** 
		* release
		*
		* Release the lock.
		*
		* PreCondition: The lock is currently acquired.  
		*/
        virtual void release();
	private:
		//
        // Disable the default copy and assignment constructors as shouldn't used.
        //
		NonReEntrantThreadLockable( const NonReEntrantThreadLockable& );
        NonReEntrantThreadLockable& operator=( const NonReEntrantThreadLockable& );

		volatile bool m_isLockCurrentlyAcquired;
		
        // The underlying omni_mutex this class is wrapping.
        HANDLE m_Mutexlock;

		CRITICAL_SECTION cs;
		bool m_bIsUsingMutex;

	};
}

#endif // !defined(AFX_NONREENTRANTPROCESSLOCKABLE_H__18942A6E_C726_4610_BDDC_BC40D6ADF359__INCLUDED_)
