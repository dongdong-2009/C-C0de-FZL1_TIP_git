#if !defined(AFX_COLOREDSTATUSBARCTRL_H__4D49433F_617D_45A5_A207_5655A0BE779F__INCLUDED_)
#define AFX_COLOREDSTATUSBARCTRL_H__4D49433F_617D_45A5_A207_5655A0BE779F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColoredStatusBarCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ColoredStatusBarCtrl window

class ColoredStatusBarCtrl : public CStatusBar
{
// Construction
public:
	ColoredStatusBarCtrl();

// Attributes
public:
	COLORREF m_crText;
	CString m_sText;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ColoredStatusBarCtrl)
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~ColoredStatusBarCtrl();
	virtual BOOL SetPaneText( int nIndex, LPCTSTR lpszNewText, BOOL bUpdate = TRUE, 
								COLORREF crText = GetSysColor(COLOR_MENUTEXT) );


	// Generated message map functions
protected:
	//{{AFX_MSG(ColoredStatusBarCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLOREDSTATUSBARCTRL_H__4D49433F_617D_45A5_A207_5655A0BE779F__INCLUDED_)
