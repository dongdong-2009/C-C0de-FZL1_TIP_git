#if !defined(AFX_HOSTSTABCTRL_H__DB483856_C759_4AEF_9707_68A8EDB32645__INCLUDED_)
#define AFX_HOSTSTABCTRL_H__DB483856_C759_4AEF_9707_68A8EDB32645__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HostsTabCtrl.h : header file
//

#include <map>
#include <string>

/////////////////////////////////////////////////////////////////////////////
// HostsTabCtrl window
namespace TA_Base_App
{
class ProcessListCtrl;
class AdminManager;

struct ProcessListAdmin 
{
    ProcessListCtrl* pProcessListCtrl;
    AdminManager* pAdminManager;

    ProcessListAdmin::ProcessListAdmin() :
    pProcessListCtrl( NULL ),
    pAdminManager( NULL )
    {
    };
};

class HostsTabCtrl : public CTabCtrl
{
// Construction
public:
	HostsTabCtrl();

// Attributes
public:

// Operations
public:
    void initTabs( const std::vector<std::string>& vecHostNames );

    ProcessListCtrl* getActiveProcessListCtrl();
    AdminManager* getActiveAdminManager();

    ProcessListCtrl* getAdminBindProcessList( const AdminManager* const pAdminManaer );

    void clearProcessList();

    int login( const std::string& portNumber, const std::string& sessionId, const std::string& operatorName );

    void logout();

    void retryLoginAllFailed();

    void checkPermissions();

    void highlightAdminBindTab( const AdminManager* const pAdminManaer, bool bHightlight = true );

    void unHighlightAllTabs();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(HostsTabCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~HostsTabCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(HostsTabCtrl)
	afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
    std::map<int, ProcessListAdmin*> m_processLists;

    ProcessListCtrl* m_pCurActiveProcessList;
    AdminManager* m_pCurActiveAdminManager;
};

}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOSTSTABCTRL_H__DB483856_C759_4AEF_9707_68A8EDB32645__INCLUDED_)
