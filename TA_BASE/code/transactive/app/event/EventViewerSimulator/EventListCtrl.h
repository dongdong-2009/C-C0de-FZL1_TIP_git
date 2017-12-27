#pragma once

#include "core/event/idl/src/EventDetailCorbaDef.h"
#include "app/event/EventViewerSimulator/DataRetriever.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

// CEventListCtrl

typedef enum {
	COL_Date = 0,
	COL_EventID,
	COL_Severity,
	COL_EquipmentID,
	COL_Description,
	COL_Value
}LIST_COLUMNS;

class CEventListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CEventListCtrl)

public:
	CEventListCtrl();
	virtual ~CEventListCtrl();
	void setUpEventList();
	void startAgentRefresh(int nQueryLocation, unsigned long _ulQueryInterval, bool _bNeedLog);
	void stopStopRefresh();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void onGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void onOdCacheHint(NMHDR* pNMHDR, LRESULT* pResult);

private: //Functions
	std::string _formatTime(long ulTime);
	void _releaseCacheData();
	void _refreshCacheData();
	void _clearEntries();
private:
	// The timer id for updates and flashing
	static const int UPDATE_TIMER;
	static const int UPDATE_INTERVAL;

	T_VecEvents m_eventCache;
	CDataRetriever* m_pObjDataRetriever;

	TA_Base_Core::ReEntrantThreadLockable m_cacheLock;
};


