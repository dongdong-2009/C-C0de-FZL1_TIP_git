/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/QuickFindComboButton.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_QUICKFINDCOMBOBUTTON_H__4CA0753B_737D_4518_837C_E8E4F98E3DAD__INCLUDED_)
#define AFX_QUICKFINDCOMBOBUTTON_H__4CA0753B_737D_4518_837C_E8E4F98E3DAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QuickFindComboButton.h : header file
//

#include "bus/mfc_extensions/src/object_tree_control/ObjectTreeCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// QuickFindComboButton window

namespace TA_Base_Core
{
    class IQuickFindEntity; // forward declaration
    class IEntityData;      // forward declaration
}

class QuickFindComboButton : public CButton
{
// Construction
public:
	QuickFindComboButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(QuickFindComboButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~QuickFindComboButton();

    /**
     * loadEntities
     *
     * Loads the root entities for the dialog. This is a public method because it needs to be called after this
     * button is constructed, but before it is displayed.
     */
    void loadEntities();

    /**
     * hideDropWindow
     *
     * Hides the drop window.
     */
    void hideDropWindow();

    /**
     * getCurrentlySelectedItem
     *
     * Retrieves the currently selected item
     *
     * @return a pointer to the IEntityData that is currently selected. This may be NULL if nothing is selected.
     */
    TA_Base_Core::IEntityData* getCurrentlySelectedItem();

	// Generated message map functions
protected:
	//{{AFX_MSG(QuickFindComboButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG    
    afx_msg BOOL onSelChanged( UINT id, NMHDR * pNotifyStruct, LRESULT * result );
    afx_msg BOOL onItemExpanded( UINT id, NMHDR* pNotifyStruct, LRESULT* result );

	DECLARE_MESSAGE_MAP()

private:

    /**
     * showDropWindow
     *
     * Creates (if necessary) and displays the drop window on a left-mouse click.
     */
    void showDropWindow();

    /**
     * createDropWindow
     *
     * Creates and populates the drop window
     */
    void createDropWindow();
    
    CObjectTreeCtrl m_dropTree;

    std::vector<TA_Base_Core::IQuickFindEntity*> m_rootEntities;

    HTREEITEM m_currentlySelectedItem;

    static const int DROP_BUTTON_WIDTH;
    static const int DROPDOWN_HEIGHT;
    static const int DROPPED_WIDTH_NOT_SET;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUICKFINDCOMBOBUTTON_H__4CA0753B_737D_4518_837C_E8E4F98E3DAD__INCLUDED_)
