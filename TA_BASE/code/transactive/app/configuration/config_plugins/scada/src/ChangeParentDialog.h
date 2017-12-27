/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/ChangeParentDialog.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class controls a dialog that is used to allow the user to change the parent of a datapoint or
  * data node.
  */

#if !defined(AFX_CHANGEPARENTDIALOG_H__80CBB492_84C1_4B50_B568_5658B02064D8__INCLUDED_)
#define AFX_CHANGEPARENTDIALOG_H__80CBB492_84C1_4B50_B568_5658B02064D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/scada/src/resource.h"

#define IMPORT_EXTENSIONS
#include "bus/scada/scada_tree/src/ScadaTreeCtrl.h"

namespace TA_Base_App
{
    class ChangeParentDialog : public CDialog
    {
    public:
        /**
         * Constructor
         *
         * @param const CString& - This is the name of the item the user currently has selected so we can
         *                         set the current selection in the tree control to be this string.
         * @param CWnd* - The parent of this dialog
         */
	    ChangeParentDialog(const CString& currentSelection, CWnd* pParent = NULL);   // standard constructor


        /**
         * Destructor
         */
        virtual ~ChangeParentDialog(){}


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


    // Dialog Data
	    //{{AFX_DATA(ChangeParentDialog)
	    enum { IDD = IDD_CHANGEPARENT_DIALOG };
	    //}}AFX_DATA
	    TA_Base_Bus::CScadaTreeCtrl	m_scadaTree;


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ChangeParentDialog)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(ChangeParentDialog)
	    afx_msg void onOK();
	virtual BOOL OnInitDialog();
	afx_msg void onDblclkTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void onSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
        // This is the name and key the of the item the user selected. This will be set when the
        // user selects Ok.
        CString m_name;
        unsigned long m_key;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_CHANGEPARENTDIALOG_H__80CBB492_84C1_4B50_B568_5658B02064D8__INCLUDED_)
