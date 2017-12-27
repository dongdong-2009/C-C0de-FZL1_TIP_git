#include "Thread.h"
#include "ATSListCtrl.h"
#pragma once

class CRequestHandler : public TA_Base_Core::Thread
{
public:
	CRequestHandler(CATSListCtrl* pListCtrl);
	~CRequestHandler(void);
	virtual void run();
	virtual void terminate();

private:
	bool m_bTerminate;
	CATSListCtrl* m_pAtsListCtrl;
};
