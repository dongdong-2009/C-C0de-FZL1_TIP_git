/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class controls a dialog that is used to allow the user to change an aspect of an entity. This could
  * be the subsystem, parent or location.
  * 
  */

#if !defined(AFX_CHANGEDIALOG_H__A5D1B8CC_4F75_4F5D_90C8_3805DB134889__INCLUDED_)
#define AFX_CHANGEDIALOG_H__A5D1B8CC_4F75_4F5D_90C8_3805DB134889__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/helperresource.h"

namespace TA_Base_App
{
    class ChangeDialog : public CDialog
    {
    public:
        // A typedef for the map of items the user can select from
        typedef std::map<unsigned long, std::string> ItemsForSelection;

        /**
         * Constructor
         *
         * @param ItemsForSelection& - The list of items to populate the list box with. These are all the
         *                             items the user can choose from.
         * @param const CString& - This is the title for the dialog. This will be something like 'Change Location'.
         * @param const CString& - This is the name of the item the user currently has selected so we can
         *                         set the current selection in the list box to be this string.
         * @param CWnd* - The parent of this dialog
         */
        ChangeDialog(ItemsForSelection& itemsForSelection,
                     const CString& title,
                     const CString& currentItem = "",
                     CWnd* pParent = NULL);


        /**
         * getSelectedName
         *
         * After the user has selected Ok on the dialog and a response has been returned to the 
         * caller then it can call this to retrieve the name of the item the user selected.
         *
         * @return CString - The name of the item the user selected in the list.
         */
        CString getSelectedName() const
        {
            return m_name;
        }


        /**
         * getSelectedKey
         *
         * After the user has selected Ok on the dialog and a response has been returned to the 
         * caller then it can call this to retrieve the associated unsigned long of the item the
         * user selected. This will be one of the unsigned long values passed in the ItemsForSelection
         * map when this class was constructed.
         *
         * @return CString - The unsigned long data value of the item the user selected in the list.
         */
        unsigned long getSelectedKey() const
        {
            return m_key;
        };


        /**
         * setSortItems
         *
         * Specifies whether items in the list control will be sorted alphabetically.
         *
         * @param sorted True if the items should be sorted.
         */
        void setSortItems( const bool sort )
        {
            m_sortItems = sort;
        }


    // Dialog Data
	    //{{AFX_DATA(ChangeDialog)
	    enum { IDD = IDD_CHANGE_DIALOG };
	    CButton	m_okButton;
	    CListBox	m_itemList;
	//}}AFX_DATA


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
	    afx_msg void onDblclkList();
	    virtual BOOL OnInitDialog();
	    afx_msg void onSelchangeList();
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
        // The items to display in the list for the user to choose from.
        ItemsForSelection& m_items;

        // The title for the dialog box.
        CString m_title;

        // The name of the current item when this dialog was created.
        CString m_nameOfCurrentItem;

        // This is the name and key the of the item the user selected. THis will be set when the
        // user selects Ok.
        CString m_name;
        unsigned long m_key;

        // Specifies whether the items will be sorted.
        bool m_sortItems;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_CHANGEDIALOG_H__A5D1B8CC_4F75_4F5D_90C8_3805DB134889__INCLUDED_)
