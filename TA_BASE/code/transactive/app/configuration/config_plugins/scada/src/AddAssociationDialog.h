/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/AddAssociationDialog.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class controls a dialog that is used to allow the user to add input and output association
  * data points.
  */

#if !defined(AFX_ADDASSOCIATIONDIALOG_H__ED4B96F6_827B_4592_BBCB_2AABCFFAADC7__INCLUDED_)
#define AFX_ADDASSOCIATIONDIALOG_H__ED4B96F6_827B_4592_BBCB_2AABCFFAADC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/scada/src/resource.h"

#define IMPORT_EXTENSIONS
#include "bus/scada/scada_tree/src/ScadaTreeCtrl.h"

namespace TA_Base_Bus
{
    class IEntityFactory;
}

namespace TA_Base_App
{
    class AddAssociationDialog : public CDialog
    {
    public:
        /**
         * Constructor
         *
         * @param const int - This is the number of states for this derived data point. If this is 0
         *                    the state selection will be disabled.
         * @param IEntityFactory& - This is the entity factory the scada tree should use to retrieve
         *                          it's entities.
         * @param CString - The title for the dialog
         * @param CString - The name of an initial datapoint to select in the tree
         * @param CWnd* - The parent of this dialog
         */
	    AddAssociationDialog(int numberOfStates, 
                             TA_Base_Bus::IEntityFactory& entityFactoryForScadaTree,
                             const CString& dialogTitle = "Add Association",
                             const CString& initialSelection = "",
                             CWnd* pParent = NULL,
							 bool allSelectionEnabled = false );   // standard constructor


        /**
         * Destructor
         */
        virtual ~AddAssociationDialog(){}


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
         * @return unsigned long - The unsigned long data value of the item the user selected in the list.
         */
        unsigned long getSelectedKey() const
        {
            return m_key;
        };


        /**
         * getSelectedState
         *
         * After the user has selected Ok on the dialog and a response has been returned to the caller
         * then it can call this to retrieve the state the user selected
         *
         * @return int - The state the user has selected. If the state is 0 then no state was selected.
         */
        int getSelectedState() const
        {
            return m_selectedState;
        }


    // Dialog Data
	    //{{AFX_DATA(AddAssociationDialog)
	    enum { IDD = IDD_CHOOSE_DATAPOINT_DIALOG };
	    CComboBox	m_stateCombo;
	    //}}AFX_DATA
	    TA_Base_Bus::CScadaTreeCtrl	m_dataPointTree;


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(AddAssociationDialog)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(AddAssociationDialog)
	    afx_msg void onDblclkDatapointTree(NMHDR* pNMHDR, LRESULT* pResult);
	    afx_msg void onOK();
	    virtual BOOL OnInitDialog();
	afx_msg void onSelchangedDatapointTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void onSelchangeStateCombo();
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:

        /**
         * refresh
         *
         * This will check the current selections in teh dialog to determine if the 
         * Ok button should be enabled or not.
         */
        void refresh();


        // This is the name and key the of the item the user selected. This will be set when the
        // user selects Ok.
        CString m_name;
        unsigned long m_key;
        int m_selectedState;
		bool m_allSelectionEnabled;

        CString m_dialogTitle;      // The title of the dialog.

        CString m_initialSelection; // This is the datapoint to select in the tree when it is first displayed

        int m_numberOfStates;

    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_ADDASSOCIATIONDIALOG_H__ED4B96F6_827B_4592_BBCB_2AABCFFAADC7__INCLUDED_)
