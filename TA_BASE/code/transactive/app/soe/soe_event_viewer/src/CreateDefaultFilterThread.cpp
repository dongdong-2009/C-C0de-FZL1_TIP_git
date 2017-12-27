// CreateDefaultFilterThread.cpp: implementation of the CCreateDefaultFilterThread class.
//
//////////////////////////////////////////////////////////////////////

#include "app/soe/soe_event_viewer/src/CreateDefaultFilterThread.h"
#include "app\soe\soe_event_viewer\src\CombinedEventFilter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCreateDefaultFilterThread::CCreateDefaultFilterThread(CombinedEventFilter *pFilter) : m_pFilter(pFilter)
{

}

CCreateDefaultFilterThread::~CCreateDefaultFilterThread()
{

}

void CCreateDefaultFilterThread::run()
{
	m_pFilter->CreateDefaultFilter();
}

void CCreateDefaultFilterThread::terminate()
{
}
