/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/config_plugin_helper/src/ChangeEntityDialog.h $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class controls a dialog that is used to allow the user to select an entity
  * from a SCADA tree.
  */

#if !defined(AFX_CHANGEENTITYDIALOG_H__10044F2D_3AF3_42AD_9CEB_D9A71A5B2927__INCLUDED_)
#define AFX_CHANGEENTITYDIALOG_H__10044F2D_3AF3_42AD_9CEB_D9A71A5B2927__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define IMPORT_EXTENSIONS
#include "bus/scada/scada_tree/src/ScadaTreeCtrl.h"
#include "bus/scada/scada_tree/src/IEntityFactory.h"

namespace TA_Base_Core
{
    class IEntityData;
}

namespace TA_Base_Bus
{
	class IEntityFactory;
}

namespace TA_Base_App
{
    class ChangeEntityDialog : public CDialog
    {
    public:
        /** 
          * ChangeEntityDialog constructor
          *
          * @param const CString& currentSelection - This is the name of the item the user 
          *                                          currently has selected so we can set the 
          *                                          current selection in the tree control to
          *                                          be this string.
          * @param CWnd* pParent                   - The parent of this dialog
          * @param IEntityFactory* entityRetrievalFactory - This is the factory that should be
          *                                          used to retrieve the entity data.
          *                                          If this is NULL then the EntityAccessFactory
          *                                          will be used instead.
          * @param bool shouldDeleteFactory        - This indicates whether this class should delete
          *                                          the IEntityFactory or whether the caller will 
          *                                          do it.
          *                                          true = this class will delete.
          *                                          false = caller must delete
          *                                          Default is for the caller to delete.
          */
        ChangeEntityDialog(const CString& currentSelection, CWnd* pParent = NULL, TA_Base_Bus::IEntityFactory* entityRetrievalFactory = NULL, bool shouldDeleteFactory = false);


        /**
         * Destructor
         */
        virtual ~ChangeEntityDialog(){}


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
        //{{AFX_DATA(ChangeEntityDialog)
        enum { IDD = IDD_CHANGE_ENTITY_DIALOG };
        //}}AFX_DATA
        TA_Base_Bus::CScadaTreeCtrl	m_entityTree;
        
        
        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(ChangeEntityDialog)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL
        
        // Implementation
    protected:
        
        // Generated message map functions
        //{{AFX_MSG(ChangeEntityDialog)
        virtual BOOL OnInitDialog();
        afx_msg void onSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void onDblclkTree(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void onOK();
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
#endif // !defined(AFX_CHANGEENTITYDIALOG_H__10044F2D_3AF3_42AD_9CEB_D9A71A5B2927__INCLUDED_)
