#if !defined(AFX_ALARMLISTCTRL_H__0E4D7E7B_3ABC_4244_9F3D_BDCE16D8770D__INCLUDED_)
#define AFX_ALARMLISTCTRL_H__0E4D7E7B_3ABC_4244_9F3D_BDCE16D8770D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\ShareMemoryDLL\AlarmApi.h"
using namespace I_ALARM_PROVIDER;
#include <vector>
#include <map>

/////////////////////////////////////////////////////////////////////////////
// CAlarmListCtrl window
typedef std::vector<AlarmDetail*> ALARM_LIST;
typedef std::map<std::string, std::string> ALARM_MAP;

class CAlarmListCtrl : public CListCtrl
{
// Construction
public:
	CAlarmListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlarmListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	const char* timeFromater(unsigned long tm);
	int GetAlarmCount();
	void Clear();
	void AddAlarm(AlarmDetail* pItem);
	void Initialize();
	virtual ~CAlarmListCtrl();

	// Generated message map functions
protected:
public:
	AlarmDetail* GetAlarmSelected();
	BOOL PrevPage();
	BOOL NextPage();
	int GetTotalPage();
	int GetCurrentPage();
	//{{AFX_MSG(CAlarmListCtrl)
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	ALARM_LIST m_AlarmList;	
	ALARM_MAP  m_AlarmMap;
    
	std::string getMappingStationCode(std::string& atsCode);

	int m_iCurrentPage;
	int m_iTotalPage;
	int m_iPageSize;

	CString m_formatStr;

	char m_timebuf[32];

	CRITICAL_SECTION m_sync;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALARMLISTCTRL_H__0E4D7E7B_3ABC_4244_9F3D_BDCE16D8770D__INCLUDED_)
