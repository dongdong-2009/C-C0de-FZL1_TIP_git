/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/tree_list_control/TreeListCtrl.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
/*
 * Definition of the TreeListCtrl
 *
 * Copyright (c)1999 David Carballo, All Right Reserved
 *
 * Created June 1999
 * Revised: August 1999
 * Written by David Carballo
 * Internet: fibeto@redestb.es
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 */

#if !defined(AFX_TREELISTCTRL_H__36C21A6B_051D_11D3_96E7_000000000000__INCLUDED_)
#define AFX_TREELISTCTRL_H__36C21A6B_051D_11D3_96E7_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl window
#include "../list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"

#include <vector>

class AFX_EXT_CLASS tree_list_info
{
public:
	int				m_nLevel;
	int				m_nImage;
	std::vector<CString>	m_lstCols;
	bool			m_bCollapsed;
	DWORD           m_itemData;
};

/// A virtual table holds all items (imagine they were all expanded), called virtual items
/// The actual visible items in the tree are termed actual items (or just items)
class AFX_EXT_CLASS CTreeListCtrl: public TA_Base_Bus::ListCtrlSelNoFocus
{
// Construction
public:
					CTreeListCtrl();
	BOOL			Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes
public:

// Operations
public:

    /**
     * Returns an array defining the GetVItem data component for each item in this tree
     *  list that is currently in an expanded state
     */
    std::vector<DWORD> fetchExpandedNodeItemData();

    /**
     * @return the level of the specified virtual item (dependent on what
     *      level pass passed into AddItem function)
     */
    int     getVItemLevel(int vItem);

	int				AddItem(LPCTSTR lpszItem, int nImage, int nLevel);

    // Sets the text of a virtual item (updates actual items text - provided it's currently visible)
	BOOL			SetVItemText( int vItem, int nSubItem, LPCTSTR lpszText );
	BOOL			DeleteAllItems();

    /// @return the number of virtual items (use GetItemCount to return number of visible items)
    virtual int GetVItemCount() const { return m_tree.size(); }
    /// SetVirtualItemData
    virtual BOOL    SetVItemData(int nItem,DWORD dwData) { return SetItemData(nItem, dwData); }
    /// GetVirtualItemData
	virtual DWORD   GetVItemData(int nItem) const { return GetItemData(nItem); }

    // It is not recommended you use these function names (use SetVItemData, GetVItemData) - as they are not
    // virtual within the CListCtrl class, so if you cast them to type CListCtrl, they
    // won't get called..  Do not remove virtual keyword, else these functions cannot be called in normal manner
	virtual BOOL            SetItemData(int nItem,DWORD dwData);
	virtual DWORD           GetItemData(int nItem) const;
	
    /**
     * GetVirtualIndexFromActual
     *
     * @return the virtual index for use with GetVItemData
     * @param nItem the actual item index, as defined by the current (expanded or otherwise) state
     *      of the list control
     * [the virtual index is contained within the (actual) item data param of list items]
     */
    DWORD GetVirtualIndexFromActual(int nItem) { return CListCtrl::GetItemData(nItem); }

	/**
     * GetActualIndexFromVirtual
     *
     * Inverse of GetVirtualIndexFromActual..
     * @return -1 if not found (note: vItem may not have an actual index, if it's an unexpanded child node..)
     */
    int GetActualIndexFromVirtual(int vItem);

    /**
     * Expand
     *
     * Expands an item in the tree
     *
     * @pre item is currently collapsed
     * @param nItem the actual item index (not the virtual!)
     */
    void Expand(int nItem);

    /**
     * isVItemCollapsed
     *
     * @return true if the specified item is collapsed
     * @param vItem the virtual item index
     */
    bool isVItemCollapsed(int vItem);

    /**
     * setupIndentWithoutIcons
     *
     * Usually the level of indendation is dependent on the icon size.  If we
     *  don't want to use icons, call this function, to set up a 1 pixel (clear) icon
     *  allowing us to effectively indent the text without (visible) use of an icon
     *
     * @param indentPixels Defines level of indentation in pixels
     */
    void setupIndentWithoutIcons(DWORD indentPixels = 16);
			
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeListCtrl)
	protected:
	virtual void	PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual			~CTreeListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeListCtrl)
	afx_msg void	OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	int		InsertItem(int nItem, int nPosArray, tree_list_info& info);
	virtual void	Collapse(int nPos, int nItem);
	int		Expand(int nItem, bool bRecursive, int& nCounterInsert);
	int		findItem(int nItem);
	
protected:
	CImageList				m_ilState;
	CImageList				m_dummyImageList;
    DWORD                   m_indentMultiplier;
	std::vector<tree_list_info>	m_tree;

private:

    /**
     * getStateImageState 
     *
     * Internal helper function to determine what kind of state image should
     * be used at the input location
     *
     */
    DWORD getStateImageState(DWORD indexIntoTree, bool showCollapsed);

};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREELISTCTRL_H__36C21A6B_051D_11D3_96E7_000000000000__INCLUDED_)
