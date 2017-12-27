/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_rules/src/SelectDataNodeDialog.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class controls a dialog that is used to allow the user to select
  * a datapoint or data node.
  */

#ifndef CONFIG_EDITOR_ALARM_RULES_CHANGE_PARENT_DIALOG
#define CONFIG_EDITOR_ALARM_RULES_CHANGE_PARENT_DIALOG

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/alarm_rules/src/resource.h"
#include "app/configuration/config_plugins/alarm_rules/src/AlarmRulesDatabaseAccessor.h"

#define IMPORT_EXTENSIONS
#include "bus/scada/scada_tree/src/ScadaTreeCtrl.h"

namespace TA_Base_App
{
    class SelectDataNodeDialog : public CDialog
    {
    public:
        /**
         * Constructor
         *
         * @param accessor - The alarm rules database accessor
         * @param CWnd* - The parent of this dialog
         */
	    SelectDataNodeDialog(
            AlarmRulesDatabaseAccessor* accessor, CWnd* pParent = NULL);   // standard constructor


        /**
         * Destructor
         */
        virtual ~SelectDataNodeDialog(){}


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
          * getAlarmType
          *
          * Gets the alarm type the user has chosen.
          *
          * @return the alarm type
          */
        unsigned long getAlarmTypeKey();
        /*{
            return m_alarmTypeKey;
        }*/


    // Dialog Data
	    //{{AFX_DATA(SelectDataNodeDialog)
	    enum { IDD = IDD_SELECTDATANODE_DIALOG };
        CComboBox       m_alarmTypeCombo;
	    //}}AFX_DATA
	    TA_Base_Bus::CScadaTreeCtrl	m_scadaTree;


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(SelectDataNodeDialog)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(SelectDataNodeDialog)
	    afx_msg void onOK();
	virtual BOOL OnInitDialog();
	afx_msg void onDblclkTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void onSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void onSelectionChangeComboAlarmType();
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
        // This is the name and key the of the item the user selected. This will be set when the
        // user selects Ok.
        CString m_name;
        unsigned long m_key;
        std::string m_alarmType;

        std::map<unsigned long, std::string> m_alarmTypeKeyDispNameMap;

        AlarmRulesDatabaseAccessor* m_databaseAccessor;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // CONFIG_EDITOR_ALARM_RULES_CHANGE_PARENT_DIALOG
