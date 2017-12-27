/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/config_plugin_helper/src/MultipleChangeDialog.h $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Class is very similar (and based upon) ChangeDialog, except that it
  * uses a list control instead of a list box, and works with multiple selections
  * 
  */

#if !defined(AFX_MULTIPLECHANGEDIALOG_H__A5D1B8CC_4F75_4F5D_90C8_3805DB134889__INCLUDED_)
#define AFX_MULTIPLECHANGEDIALOG_H__A5D1B8CC_4F75_4F5D_90C8_3805DB134889__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <string>
#include <vector>
#include <list>

#include "app/configuration/config_plugins/config_plugin_helper/src/helperresource.h"

namespace TA_Base_App
{
    class MultipleChangeDialog : public CDialog
    {
    public:

        // A typedef for the map of items the user can select from
        typedef std::map<unsigned long, std::vector<std::string> > ItemsForSelection;
        typedef std::list<unsigned long> KeySelection;
        
        struct HeaderProperties
        {
            HeaderProperties() : label(""), format(LVCFMT_LEFT), width(-1) {}
            std::string label;
            int format; // LVCFMT_LEFT
            int width;  // -1: note LVSCW_AUTOSIZE (-1) or LVSCW_AUTOSIZE_USEHEADER(-2)
        };

        /**
         * Constructor
         *
         * @param ItemsForSelection& - The list of items to populate the list box with. These are all the
         *                             items the user can choose from.
         * @param const CString& - This is the title for the dialog. This will be something like 'Change Location'.
         * @param columnHeaders - an array defining the column header properties
         * @param initialSelection - Array of keys defining the initial selection (if any)
         * @param CWnd* - The parent of this dialog
         */
        MultipleChangeDialog(ItemsForSelection& itemsForSelection,
                     const CString& title,
                     const std::vector<HeaderProperties>& columnHeaders,
                     const KeySelection& initialSelection,
                     CWnd* pParent = NULL,
                     bool useSingleSel = false,
                     DWORD initialStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);


        /**
         * getSelectedKeys
         *
         * After the user has selected Ok on the dialog and a response has been returned to the 
         * caller then it can call this to retrieve the array of selected keys
         *
         * @return an array defining the current selection, may be empty
         */
        const KeySelection& getSelectedKeys() const;

        /**
         * onSelchangedList
         *
         * Called each time the selection changes in the list control
         */
        void    onSelchangedList();
              
        /**
         * setSortingFunction
         *
         * Applies a sorting callback to this dialog (calling before DoModal)
         *
         * @param compareFunc the function that should be called to compare items in
         *          the list control, null to unset compare function
         *
         * @param data the data passed to the compare function
         *
         */
        void setSortingFunction(PFNLVCOMPARE compareFunc, DWORD data) { m_compareFunc = compareFunc; m_compareData = data; }

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ChangeDialog)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(ChangeDialog)
	    afx_msg void onOK();
	    afx_msg void onDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	    virtual BOOL OnInitDialog();
	    afx_msg void onSelchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
  
        // Dialog Data
	        //{{AFX_DATA(ChangeDialog)
	        enum { IDD = IDD_MULTIPLE_CHANGE_DIALOG };
	        CButton	m_okButton;
	        CListCtrl	m_itemList;
	    //}}AFX_DATA


        // The items to display in the list for the user to choose from.
        ItemsForSelection& m_items;

        // The title for the dialog box.
        CString m_title;

        // Cache of constructor parameters
        std::vector<HeaderProperties> m_headerProperties;
        KeySelection m_initialSelection;

        // This is the name and key the of the item the user selected. THis will be set when the
        // user selects Ok.
        KeySelection m_keySelection;

        DWORD   m_initialStyle;
        bool m_useSingleSel;

        PFNLVCOMPARE m_compareFunc;
        DWORD       m_compareData;
        CFont m_fntListCtrl;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_MULTIPLECHANGEDIALOG_H__A5D1B8CC_4F75_4F5D_90C8_3805DB134889__INCLUDED_)
