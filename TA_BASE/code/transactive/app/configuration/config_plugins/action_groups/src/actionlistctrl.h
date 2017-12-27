/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/action_groups/src/actionlistctrl.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the list display used to show the available profiles for the operator
  * in the OperatorDetailView class
  */

#if !defined (ACTION_LIST_CTRL_H_INCLUDED)
#define ACTION_LIST_CTRL_H_INCLUDED


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define IMPORT_EXTENSIONS
#include "app/configuration/config_plugins/action_groups/src/stdafx.h"
#include "bus\mfc_extensions\src\mfc_extensions.h"


namespace TA_Base_Core
{
    class IConfigActionGroup;
    class IAction;
}

namespace TA_Base_App
{
    class ActionListCtrl : public CEditableListCtrl, public ICellManager
    {
	    public:
            /**
             * Constructor
             */
		    ActionListCtrl();


            /**
             * Destructor
             */
		    virtual ~ActionListCtrl();


            /**
             * populateControl
             *
             * This will populate the list control with all the profiles and check those valid for
             * the operator passed in.
             *
             * @param IConfigActionGroup* - The current ActionGroup the user has selected
             */
            void populateControl(TA_Base_Core::IConfigActionGroup* currentActionGroup);


            /**
             * changeSelected
             *
             * This indicates that this control should take all selected items and change their check
             * value
             * 
             * @param bool - True if the items should be ticked, false if they should be un-ticked.
             */
            void changeSelected(bool isSelected);


            //
		    // ICellManager methods
            //

            /**
             * GetCellType
             *
             * This determines the type of cell the list control should display. This will always
             * be a check box.
             *
             * @param CEditableListCtrl* - This is the list control and can be used to retrieve the
             *                             profile name.
             * @param int - This is the currently selected item.
             * @param int - This is the currently selected column. We are only interested if it is the
             *              value column. Otherwise the user cannot edit it.
             *
             * @return CellType - This returns the check box type on all occassions.
             */
            virtual CellType GetCellType(CEditableListCtrl* listCtrl, int item, int subItem);


            /**
             * GetCellMaximumCharacters
             *
             * Return the type of the specified table cell. For unlimited number of characters this method
             * should return 0. This method is only relevant to CT_TEXT and CT_NUMBER field types.
             *
             * @param CEditableListCtrl* - This is the list control and can be used to retrieve the
             *                             parameter name.
             * @param int - This is the currently selected item.
             * @param int - This is the currently selected column. We are only interested if it is the
             *              value column. Otherwise the user cannot edit it.
             */
            virtual unsigned long GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem);

            
            /**
             * GetCellData
             *
             * This is not needed as we do not have any combo entries. Therefore this method will return
             * an emtpy vector.
             *
             * @param CEditableListCtrl* - This is the list control and can be used to retrieve the
             *                             profile name.
             * @param int - This is the currently selected item.
             * @param int - This is the currently selected column. We are only interested if it is the
             *              value column. Otherwise the user cannot edit it.
             *
             * @return CellData - An emtpy vector.
             */
		    virtual CellData GetCellData(CEditableListCtrl* listCtrl, int item, int subItem);


            /**
             * GetCellMask
             *
             * This will determine and return the mask to be used for the specified list control cell.
             *
             * @param CEditableListCtrl* - This is the list control.
             * @param int - This is the currently selected item.
             * @param int - This is the currently selected column.
             *
             * @return CString - The mask for the selected item.
             */
            virtual CString GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem);


            /**
             * GetCellMaskPromptChar
             *
             * This will determine and return the mask prompt character to be used for the specified list control cell.
             *
             * @param CEditableListCtrl* - This is the list control.
             * @param int - This is the currently selected item.
             * @param int - This is the currently selected column.
             *
             * @return char - The prompt character.
             */
            virtual char GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem);


            /**
             * UpdateCellValue
             *
             * Once the user has changed a value then this is called. This must set the value in the
             * IConfigOperator object.
             *
             * @param CEditableListCtrl* - This is the list control and can be used to retrieve the
             *                             profile name.
             * @param int - This is the currently selected item.
             * @param int - This is the currently selected column. We are only interested if it is the
             *              value column. Otherwise the user cannot edit it.
             * @param CString - The new value for the item
             *
             * @return bool - This indicates whether the update worked or not. True indicates everything went
             *                ok and the control can update. False indicates an error occurred and the update
             *                cannot take place.
             */
            virtual bool UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value);


	    // Overrides
		    // ClassWizard generated virtual function overrides
		    //{{AFX_VIRTUAL(ActionListCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL


            // Generated message map functions
	    protected:
		    //{{AFX_MSG(ActionListCtrl)
	        afx_msg void onColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	        //}}AFX_MSG

		    DECLARE_MESSAGE_MAP()

		    void setupList();

        private:

	      /**
		   * updateHeaderSort
		   * 
	       * This method updates the column header when clicked on, to indicate 
		   * the current sort status i.e. ascending/descending.
		   *
		   * @param The column index
		   */
		   void updateHeaderSort(const unsigned int column);


           /**
            * compareItems
            *
            * This compares two items at a time to determine whether they should be swapped or not. This
            * function is passed to the CListCtrl, SortItems() method.
            *
            * @param LPARAM - This is the associated data of the first item. This is an index into the
            *                 m_actions vector
            * @param LPARAM - This is the associated data of the second item. This is an index into the
            *                 m_actions vector
            * @param LPARAM - This is a pointer to this object. ie the ActionListCtrl object.
            */
           static int CALLBACK compareItems(LPARAM index1, LPARAM index2, LPARAM actionListCtrl);

        
            // These are the set up details for the attribute list control
            static const CString ACTION_CHECK_COLUMN_NAME;
            static const int     ACTION_CHECK_COLUMN_WIDTH;
            static const int     ACTION_CHECK_COLUMN;

            static const CString ACTION_NAME_COLUMN_NAME;
            static const int     ACTION_NAME_COLUMN_WIDTH;
            static const int     ACTION_NAME_COLUMN;

            static const CString ACTION_DESCRIPTION_COLUMN_NAME;
            static const int     ACTION_DESCRIPTION_COLUMN_WIDTH;
            static const int     ACTION_DESCRIPTION_COLUMN;

            static const CString BOOLEAN_TRUE;
            static const CString BOOLEAN_FALSE;

            static const bool ASCENDING;
            static const bool DESCENDING;

            TA_Base_Core::IConfigActionGroup* m_currentActionGroup;    // This is the current ActionGroup we are displaying the actions for

            std::vector<TA_Base_Core::IAction*> m_actions; // This is all the actions shown in the list

            bool m_sortOrder;   // This indicates which order we are currently sorting in.
            unsigned int m_sortColumn;  // This indicates which column is currently sorted.
            CFont m_font;
    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(ACTION_LIST_CTRL_H_INCLUDED)
