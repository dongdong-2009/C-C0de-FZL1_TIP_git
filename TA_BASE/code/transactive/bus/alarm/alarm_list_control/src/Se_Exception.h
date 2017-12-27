// Se_Exception.h: interface for the Se_Exception class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SE_EXCEPTION_H__DA65AF2C_CC03_4C9E_8B26_871E48EF2286__INCLUDED_)
#define AFX_SE_EXCEPTION_H__DA65AF2C_CC03_4C9E_8B26_871E48EF2286__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <windows.h>
#include <eh.h>
#include "core/exceptions/src/AlarmListCtrlException.h"
class SE_Exception
{
private:
    unsigned int nSE;
public:
    SE_Exception() {}
    SE_Exception( unsigned int n ) : nSE( n ) {}
    ~SE_Exception() {}
    unsigned int getSeNumber() { return nSE; }
};
void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	if(EXCEPTION_ACCESS_VIOLATION == u)
	{
		throw TA_Base_Core::AlarmListCtrlException(); 
	}
}

#endif // !defined(AFX_SE_EXCEPTION_H__DA65AF2C_CC03_4C9E_8B26_871E48EF2286__INCLUDED_)
