// CreateDefaultFilterThread.h: interface for the CCreateDefaultFilterThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CREATEDEFAULTFILTERTHREAD_H__517536EC_709E_4A63_9776_9901FF99067C__INCLUDED_)
#define AFX_CREATEDEFAULTFILTERTHREAD_H__517536EC_709E_4A63_9776_9901FF99067C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CombinedEventFilter;

#include "core\threads\src\Thread.h"

class CCreateDefaultFilterThread : public TA_Base_Core::Thread  
{
public:
	CCreateDefaultFilterThread(CombinedEventFilter *pFilter);
	virtual ~CCreateDefaultFilterThread();

	virtual void run();
	virtual void terminate();

private:
	CombinedEventFilter *m_pFilter;
};

#endif // !defined(AFX_CREATEDEFAULTFILTERTHREAD_H__517536EC_709E_4A63_9776_9901FF99067C__INCLUDED_)
