// PopulateEventLevelsThread.h: interface for the CPopulateEventLevelsThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POPULATEEVENTLEVELSTHREAD_H__56552D3B_BEEA_4F32_9738_572174C59CAE__INCLUDED_)
#define AFX_POPULATEEVENTLEVELSTHREAD_H__56552D3B_BEEA_4F32_9738_572174C59CAE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/threads/src/Thread.h"

class CPopulateEventLevelsThread : public TA_Base_Core::Thread  
{
public:
	CPopulateEventLevelsThread();
	virtual ~CPopulateEventLevelsThread();

	virtual void run();
	virtual void terminate();
};

#endif // !defined(AFX_POPULATEEVENTLEVELSTHREAD_H__56552D3B_BEEA_4F32_9738_572174C59CAE__INCLUDED_)
