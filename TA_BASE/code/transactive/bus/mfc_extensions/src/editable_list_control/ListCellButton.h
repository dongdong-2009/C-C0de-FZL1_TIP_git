/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/editable_list_control/ListCellButton.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
#if !defined(AFX_LISTCELLBUTTON_H__D7A66435_C142_11D5_B649_0050BAB1D92B__INCLUDED_)
#define AFX_LISTCELLBUTTON_H__D7A66435_C142_11D5_B649_0050BAB1D92B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ListCellButton.h : header file
//

#include "ICellManager.h"
#include "IListCellButtonProcessor.h"


/////////////////////////////////////////////////////////////////////////////
// CListCellButton window

class CListCellButton : public CButton
{
// Construction
public:
    CListCellButton(CEditableListCtrl* parent, ICellManager* cellManager, CRect cellRect, int item, int subItem, CString text, ICellManager::CellData data, CString caption, int buttonWidth, IListCellButtonProcessor* listCellButtonProcessor);

// Attributes
public:

protected:
	bool					    m_esc;	
	int 					    m_item;
	int 					    m_subItem;
	CEditableListCtrl* 		    m_parent; 
	ICellManager*			    m_cellManager;
    IListCellButtonProcessor*   m_listCellButtonProcessor;
    
// Operations
public:
	void Update();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCellButton)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListCellButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListCellButton)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCELLBUTTON_H__D7A66435_C142_11D5_B649_0050BAB1D92B__INCLUDED_)
