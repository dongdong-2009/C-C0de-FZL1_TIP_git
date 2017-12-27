// EventListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "EventViewerSimulator.h"
#include "EventListCtrl.h"
#include "core/synchronisation/src/ThreadGuard.h"

const int CEventListCtrl::UPDATE_TIMER = 1;
const int CEventListCtrl::UPDATE_INTERVAL = 100;
// CEventListCtrl

IMPLEMENT_DYNAMIC(CEventListCtrl, CListCtrl)

CEventListCtrl::CEventListCtrl()
	: m_pObjDataRetriever(0)
{
}

CEventListCtrl::~CEventListCtrl()
{
}


BEGIN_MESSAGE_MAP(CEventListCtrl, CListCtrl)
	ON_WM_TIMER()
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, onGetdispinfo)
	ON_NOTIFY_REFLECT(LVN_ODCACHEHINT, onOdCacheHint)
END_MESSAGE_MAP()



// CEventListCtrl message handlers
void CEventListCtrl::setUpEventList()
{
	// SetUp Headers
	InsertColumn(COL_Date, "Date", LVCFMT_CENTER, 100);
	InsertColumn(COL_EventID, "Event ID", LVCFMT_CENTER, 150);
	InsertColumn(COL_Severity, "Severity", LVCFMT_CENTER, 100);
	InsertColumn(COL_EquipmentID, "Equipment ID", LVCFMT_CENTER, 200);
	InsertColumn(COL_Description, "Description", LVCFMT_CENTER, 200);
	InsertColumn(COL_Value, "Value", LVCFMT_LEFT, 100);

	SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	
}

void CEventListCtrl::startAgentRefresh(int nQueryLocation, unsigned long _ulQueryInterval, bool _bNeedLog)
{

	if (m_pObjDataRetriever == 0)
	{
		_clearEntries();

		m_pObjDataRetriever = new CDataRetriever(nQueryLocation, _ulQueryInterval, _bNeedLog);
		m_pObjDataRetriever->start();

		// Start refresh
		SetTimer(UPDATE_TIMER, UPDATE_INTERVAL, NULL);
	}
}

void CEventListCtrl::stopStopRefresh()
{
	KillTimer(UPDATE_TIMER);

	if (m_pObjDataRetriever != 0)
	{
		m_pObjDataRetriever->terminateAndWait();
		delete m_pObjDataRetriever;
		m_pObjDataRetriever = NULL;
	}
	
}

void CEventListCtrl::OnTimer(UINT nIDEvent) 
{
	switch(nIDEvent)
	{
	case  CEventListCtrl::UPDATE_TIMER:
		_refreshCacheData();
		break;
	default:
		break;
	}
}

void CEventListCtrl::onGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	LV_ITEM* pItem = &(pDispInfo)->item;

	int iItem = pItem->iItem;

	if (pItem->mask & LVIF_TEXT) //valid text buffer?
	{
		TA_Base_Core::ThreadGuard guard(m_cacheLock);

		TA_Base_Core::EventItem* pDetails = m_eventCache[iItem];
		switch(pItem->iSubItem)
		{
		case COL_Date: //fill in main text
			_tcscpy_s(pItem->pszText, pItem->cchTextMax,
				_formatTime(pDetails->createTime.time).c_str());
			break;
		case COL_EventID: //fill in sub item 1 text
			_tcscpy_s(pItem->pszText, pItem->cchTextMax,
				pDetails->eventID);
			break;
		case COL_Severity: //fill in sub item 2 text
			_tcscpy_s(pItem->pszText, pItem->cchTextMax,
				pDetails->severityname);
			break;
		case COL_EquipmentID:
			_tcscpy_s(pItem->pszText, pItem->cchTextMax,
				pDetails->assetname);
			break;
		case COL_Description:
			strncpy(pItem->pszText, pDetails->description, pItem->cchTextMax);
			break;
		case COL_Value:
			_tcscpy_s(pItem->pszText, pItem->cchTextMax,
				pDetails->value);
			break;
		}
	}

	*pResult = 0;
}

void CEventListCtrl::onOdCacheHint(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCACHEHINT pCacheHint = reinterpret_cast<LPNMLVCACHEHINT>(pNMHDR);
	// TODO: Add your control notification handler code here

	_refreshCacheData();

	*pResult = 0;
}

std::string CEventListCtrl::_formatTime(long ulTime)
{
	char szTime[256];
	CTime time(ulTime);
	CString str = time.Format("%H:%M:%S - %d-%b-%y");
	strncpy(szTime, str.GetBuffer(0), sizeof(szTime) - 1);
	return std::string(szTime);
}

void CEventListCtrl::_releaseCacheData()
{
	TA_Base_Core::ThreadGuard guard(m_cacheLock);
	T_VecEvents::iterator itr = m_eventCache.begin();
	for(itr; itr!=m_eventCache.end(); itr++)
	{
		delete (*itr);
	}
	m_eventCache.clear();

	SetItemCountEx(m_eventCache.size(), LVSICF_NOSCROLL | LVSICF_NOINVALIDATEALL);
}

void CEventListCtrl::_refreshCacheData()
{
	T_VecEvents events;

	if(m_pObjDataRetriever->getDataForDisplay(events))
	{	//Need reload
		_releaseCacheData();
	}

	if (events.size() > 0)
	{
		TA_Base_Core::ThreadGuard guard(m_cacheLock);
		T_VecEvents::iterator itr = events.begin();
		
		for(itr; itr!=events.end(); itr++)
		{

			if (m_eventCache.size() > 20000)
			{
				TA_Base_Core::EventItem* pDelItem = *(m_eventCache.begin());
				delete pDelItem;
				pDelItem =NULL;

				m_eventCache.erase(m_eventCache.begin());
			}

			m_eventCache.push_back((*itr));
		}

		//m_eventCache.insert(m_eventCache.end(), events.begin(), events.end());
		SetItemCountEx(m_eventCache.size(), LVSICF_NOSCROLL | LVSICF_NOINVALIDATEALL);
		AfxGetApp()->GetMainWnd()->PostMessage(WMU_UPDATE_COUNTERS, (WPARAM)m_eventCache.size(), 0);
		
		Invalidate();
	}	
}

void CEventListCtrl::_clearEntries()
{
	_releaseCacheData();
	Invalidate();
}