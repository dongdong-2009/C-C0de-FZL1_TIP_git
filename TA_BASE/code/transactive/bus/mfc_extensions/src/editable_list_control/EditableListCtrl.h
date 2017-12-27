/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/editable_list_control/EditableListCtrl.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/23 16:08:36 $
  * Last modified by:  $Author: limin.zhu $
  *
  */
#if !defined(AFX_EDITABLELISTCTRL_H__A7270AAC_9F4E_4396_8B98_B6C5B75ABA3D__INCLUDED_)
#define AFX_EDITABLELISTCTRL_H__A7270AAC_9F4E_4396_8B98_B6C5B75ABA3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditableListCtrl.h : header file
//

#include "ICellManager.h"
#include "IListCellButtonProcessor.h"


/////////////////////////////////////////////////////////////////////////////
// CEditableListCtrl window

// This guard is so that this MFC extension can be used in another
// MFC extension DLL
//
#ifdef IMPORT_EXTENSIONS
class __declspec(dllimport) CEditableListCtrl : public CListCtrl
#else
class AFX_EXT_CLASS CEditableListCtrl : public CListCtrl
#endif
{
// Construction
public:
		 CEditableListCtrl();
		 CEditableListCtrl(ICellManager *cellManager, bool singleClick = false, int numImages = 9);
		 CEditableListCtrl(bool singleClick, int numImages = 9);

	void SetManager(ICellManager *cellManager);
	void SetComboxCellDataSortable(BOOL sortable);
	BOOL GetComboxCellDataSortable();
// Attributes
protected:
	ICellManager *m_cellManager;
	CWnd *m_cellEditCtrl;
   	bool m_singleClick;

	bool isItemChecked(int nItem, int nSubItem);
	int  addIcon(HICON image);
	void startEdit();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditableListCtrl)
	//}}AFX_VIRTUAL

	void showCheckIcon(int nItem, int nSubItem, bool checked, bool enabled);
	// wenching++ (TD13676)
	void destroyCheckIcon(int nItem, int nSubItem);
	// ++wenching (TD13676)

    void addListCellButtonProcessor(IListCellButtonProcessor* buttonProcessor)
    {
        m_buttonProcessor = buttonProcessor;
    }

// Implementation
public:
	virtual ~CEditableListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditableListCtrl)
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:	
	void loadCheckBoxIcons();
	
	HICON						m_noCheckIcon;
	int							m_noCheckIconIndex;
	HICON						m_checkIcon;
	int							m_checkIconIndex;
	HICON						m_disabledNoCheckIcon;
	int							m_disabledNoCheckIconIndex;
	HICON						m_disabledCheckIcon;
	int							m_disabledCheckIconIndex;

	int							m_numImages;

    CImageList*                 m_imageList;

    IListCellButtonProcessor*   m_buttonProcessor;

	BOOL						m_comboxCellDataSortable;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITABLELISTCTRL_H__A7270AAC_9F4E_4396_8B98_B6C5B75ABA3D__INCLUDED_)
