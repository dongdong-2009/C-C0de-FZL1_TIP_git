/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/EditShortcutDialog.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides the dialog for user to edit the shortcuts associated with the session
  * 
  */

#if !defined(AFX_EDITSHORTCUTDIALOG_H__1C423390_694E_4189_8B96_0ADA1FC1029B__INCLUDED_)
#define AFX_EDITSHORTCUTDIALOG_H__1C423390_694E_4189_8B96_0ADA1FC1029B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditShortcutDialog.h : header file
//

#include "app/system_control/control_station/src/resource.h"
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// EditShortcutDialog dialog

namespace TA_Base_Core
{
    class IScadaDisplay;
}

class EditShortcutDialog : public CDialog
{
// Construction
public:
    struct ShortcutInformation
    {
        std::string name;                   // The name of the shortcut
        TA_Base_Core::IScadaDisplay* display;    // Pointer to the display data
    };

    typedef std::vector<ShortcutInformation*> ShortcutVector;
 

    EditShortcutDialog(CWnd* pParent = NULL);   // standard constructor

    /** 
      * initialise
      *
      * Initialise the shortcut and scada displays lists
      *
      * @param shortcuts The list of shortcuts available to the current session
      * @param allDisplays The list of scada displays
      * @param selection The index of the selected shortcut
      *
      */
    void initialise(ShortcutVector& shortcuts, 
                    std::vector<TA_Base_Core::IScadaDisplay*>& allDisplays, 
                    const int selection = CB_ERR);


    /** 
      * getShortcuts
      *
      * Returns the reference to the list of shortcuts
      *
      *
      * @return The reference to the list of shortcuts
      */
    ShortcutVector& getShortcuts()
    {
        return m_shortcuts;
    }


    /** 
      * cleanUp
      *
      * Clean up the internal pointers and vectors
      *
      */
    void cleanUp();


// Dialog Data
	//{{AFX_DATA(EditShortcutDialog)
	enum { IDD = IDD_EDIT_SHORTCUT_DIALOG };
	CButton	m_removeShortcutCtrl;
	CButton	m_moveUpCtrl;
	CButton	m_moveDownCtrl;
	CButton	m_setShortcutCtrl;
	CComboBox       	    m_display;
	CEdit	                m_name;
	CListCtrl	            m_listCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(EditShortcutDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

    /** 
      * initialiseDisplay
      *
      * Sets up the display combo
      *
      */
    void initialiseDisplayCombo();


    /** 
      * populateShortcutDetails
      *
      * Sets the name and display associated with the shortcut
      *
      */
    void populateShortcutDetails();


    /** 
      * swapItems
      *
      * Swap the specified item with the next or previous item on the list
      *
      * @param nItem1 index of item 1
      * @param moveDown True if the item is moving down by 1 position, otherwise it is moving up 
      *        by 1 position
      *
      */
    void swapItems(int nItem1, bool moveDown);


    /** 
      * getDefaultShortcutName
      *
      * Formulates the default shortcut name and returns it.  The method will
      * look up the default names that are already in the list and generate a string
      * with the next number up.
      * The default names are in the form on "New Shorcut 1", "New Shortcut 2", ....  
      *
      * @return The new default name
      */
    std::string getDefaultShortcutName();


    /** 
      * activateShortcutControls
      *
      * Enable or disable all controls related to a shortcut.
      *
      * @param enable If true, the Remove, Move Up, Move Down, Set, name and display 
      *               controls will be enabled.  Will be disabled otherwise.
      */
    void activateShortcutControls(bool enable);


    /** 
      * onScadaDisplayChange
      *
      * The scada displays have been updated.  This method resets list of displays
      * listed in the display combo
      *
      */
    LRESULT onScadaDisplayChange(WPARAM, LPARAM);


	// Generated message map functions
	//{{AFX_MSG(EditShortcutDialog)
	afx_msg void OnMoveUp();
	afx_msg void OnMoveDown();
	afx_msg void OnAddShortcut();
	afx_msg void OnRemoveShortcut();
	afx_msg void OnHelpButton();
	afx_msg void OnGetdispinfoShortcutList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickShortcutList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnSetShortcut();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    HICON m_hIcon;

private:
    // pointer to the scada display vector
    std::vector<TA_Base_Core::IScadaDisplay*>* m_allDisplays;

    // using vector of pointers so that it's easier to swap the values
    ShortcutVector m_shortcuts;
    int m_selection;

    static const int INDEX_COLUMN;
    static const int NAME_COLUMN;
    static const int DISPLAY_COLUMN;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITSHORTCUTDIALOG_H__1C423390_694E_4189_8B96_0ADA1FC1029B__INCLUDED_)
