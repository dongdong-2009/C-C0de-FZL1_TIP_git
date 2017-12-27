/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/editable_list_control/ListCellDateTime.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
#if !defined(AFX_LISTCELLDATETIME_H)
#define AFX_LISTCELLDATETIME_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ListCellDateTime.h : header file
//

#include "ICellManager.h"


/////////////////////////////////////////////////////////////////////////////
// CListCellDateTime window

class CListCellDateTime : public CDateTimeCtrl
{
// Construction
public:
    CListCellDateTime(CEditableListCtrl* parent, ICellManager* cellManager, CRect& cellRect, int item,
		int subItem, CString text, ICellManager::CellType cellType);

// Attributes
public:

protected:
	bool					m_esc,
							m_bMouseDown;
	int 					m_item;
	int 					m_subItem;
	CEditableListCtrl* 		m_parent; 
	ICellManager*			m_cellManager;
    bool                    m_parentUpdated;
    CString                 m_text;

// Operations
public:
	void Update();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCellDateTime)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListCellDateTime();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListCellDateTime)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCloseup(NMHDR * pNotifyStruct, LRESULT* result);
	afx_msg void OnDropDown(NMHDR * pNotifyStruct, LRESULT* result);
	afx_msg LRESULT OnWmuKillFocus(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCELLDATETIME_H)
