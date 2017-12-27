// NonReEntrantProcessLockable.h: interface for the NonReEntrantProcessLockable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NONREENTRANTPROCESSLOCKABLE_H__18942A6E_C726_4610_BDDC_BC40D6ADF359__INCLUDED_)
#define AFX_NONREENTRANTPROCESSLOCKABLE_H__18942A6E_C726_4610_BDDC_BC40D6ADF359__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "stdafx.h"
#include <string>
#include "IProcessLockable.h"
#include "Windows.h"
namespace TA_Base_Bus
{
	class NonReEntrantProcessLockable : public IProcessLockable
	{
	public:
		NonReEntrantProcessLockable(std::string name);
		virtual ~NonReEntrantProcessLockable();

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
		NonReEntrantProcessLockable( const NonReEntrantProcessLockable& );
        NonReEntrantProcessLockable& operator=( const NonReEntrantProcessLockable& );

		volatile bool m_isLockCurrentlyAcquired;
		
        // The underlying omni_mutex this class is wrapping.
        HANDLE m_Mutexlock;

	};
}

#endif // !defined(AFX_NONREENTRANTPROCESSLOCKABLE_H__18942A6E_C726_4610_BDDC_BC40D6ADF359__INCLUDED_)
