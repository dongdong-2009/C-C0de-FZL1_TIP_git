#if !defined(AFX_OVERRIDEEXCLUSIVEDUTY_H__D2A15D73_5590_4F78_9DB4_C530D3684F06__INCLUDED_)
#define AFX_OVERRIDEEXCLUSIVEDUTY_H__D2A15D73_5590_4F78_9DB4_C530D3684F06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OverRideExclusiveDuty.h : header file
// added for TD19409

/////////////////////////////////////////////////////////////////////////////
// OverRideExclusiveDuty dialog

class OverRideExclusiveDuty : public CDialog
{
// Construction
public:
	BOOL OnInitDialog();
	OverRideExclusiveDuty(CString subsystemsAtLocations, CWnd* pParent = NULL );   // standard constructor

// Dialog Data
	//{{AFX_DATA(OverRideExclusiveDuty)
	enum { IDD = IDD_DIALOG1 };
	CRichEditCtrl	m_textCtrl;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OverRideExclusiveDuty)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(OverRideExclusiveDuty)
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_detail;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OVERRIDEEXCLUSIVEDUTY_H__D2A15D73_5590_4F78_9DB4_C530D3684F06__INCLUDED_)
