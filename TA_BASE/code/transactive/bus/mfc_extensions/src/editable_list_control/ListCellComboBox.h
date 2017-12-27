/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/editable_list_control/ListCellComboBox.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/23 16:08:36 $
  * Last modified by:  $Author: limin.zhu $
  *
  */
#if !defined(AFX_LISTCELLCOMBOBOX_H__D7A66435_C142_11D5_B649_0050BAB1D92B__INCLUDED_)
#define AFX_LISTCELLCOMBOBOX_H__D7A66435_C142_11D5_B649_0050BAB1D92B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ListCellComboBox.h : header file
//

#include "ICellManager.h"
#include "XTipListBox.h" //TD18272


/////////////////////////////////////////////////////////////////////////////
// CListCellComboBox window

class CListCellComboBox : public CComboBox
{
// Construction
public:
    CListCellComboBox(CEditableListCtrl* parent, ICellManager* cellManager, CRect cellRect, int item, int subItem, CString text, ICellManager::CellData data, BOOL sortable);

// Attributes
public:

protected:
	bool					m_esc;	
	int 					m_item;
	int 					m_subItem;
	CEditableListCtrl* 		m_parent; 
	ICellManager*			m_cellManager;
    bool                    m_parentUpdated;
	CString					m_strCurrentValue;
	BOOL					m_sortable;
	//TD18272_jinhua++
	CXTipListBox	m_listbox;
	//TD18272_jinhua++

// Operations
public:
	void Update();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCellComboBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//TD18272_jinhua++
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	//++TD18272_jinhua
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListCellComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListCellComboBox)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCloseup();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCELLCOMBOBOX_H__D7A66435_C142_11D5_B649_0050BAB1D92B__INCLUDED_)
