/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/system_controller/src/ProcessListCtrl.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the list display used to show the system controller's managed processes
  */

#if !defined (PROCESS_LIST_CTRL_H_INCLUDED)
#define PROCESS_LIST_CTRL_H_INCLUDED


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define IMPORT_EXTENSIONS
#include "app/configuration/config_plugins/system_controller/src/stdafx.h"
#include "bus\mfc_extensions\src\mfc_extensions.h"


namespace TA_Base_Core
{
    class IConfigSystemController;
}

namespace TA_Base_App
{
    class ProcessListCtrl : public CEditableListCtrl, public ICellManager
    {
	    public:
            /**
             * Constructor
             */
		    ProcessListCtrl();


            /**
             * Destructor
             */
		    virtual ~ProcessListCtrl();


            /**
             * populateControl
             *
             * This will populate the list control with the processes of the system controller passed 
             * in.
             *
             * @param IConfigEntity& - The current system controller the user has selected
             */
            void populateControl(TA_Base_Core::IConfigSystemController& currentSystemController);


            /**
             * addNewItems
             *
             * This is called when the user has selected the option to add new processes to the list.
             * This should show a dialog to the user to allow them to add new processes and then add them
             * to the list and the system controller object
             */
            void addNewItems();


            /**
             * removeSelectedItem
             *
             * This is called when the user has selected the option to remove a process from the list.
             * This should remove the item from the list and from the System Controller object.
             */
            void removeSelectedItem();


            //
		    // ICellManager methods
            //

            /**
             * GetCellType
             *
             * This determines the type of cell the list control should display. This will be either a
             * normal field (for the managed process name) or a combo box (for the running type).
             *
             * @param CEditableListCtrl* - This is the list control and can be used to retrieve the
             *                             process name or key
             * @param int - This is the currently selected item.
             * @param int - This is the currently selected column. We are only interested if it is the
             *              running type column. Otherwise the user cannot edit it.
             *
             * @return CellType - This returns the combo type
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
             * This will determine the values to display in the combo box and return them.
             *
             * @param CEditableListCtrl* - This is the list control and can be used to retrieve the
             *                             process name.
             * @param int - This is the currently selected item.
             * @param int - This is the currently selected column. We are only interested if it is the
             *              value column. Otherwise the user cannot edit it.
             *
             * @return CellData - This will return 'Control' and 'Monitor'
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
             * IConfigEntity object.
             *
             * @param CEditableListCtrl* - This is the list control and can be used to retrieve the
             *                             process name and key
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
		    //{{AFX_VIRTUAL(ProcessListCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL


            // Generated message map functions
	    protected:
		    //{{AFX_MSG(ProcessListCtrl)
	//}}AFX_MSG

		    DECLARE_MESSAGE_MAP()

		    void setupList();

        private:
            // These are the set up details for the attribute list control
            static const CString PROCESS_NAME_COLUMN_NAME;
            static const int PROCESS_NAME_COLUMN_WIDTH;
            static const int PROCESS_NAME_COLUMN;

            static const CString PROCESS_OPERATING_MODE_COLUMN_NAME;
            static const int PROCESS_OPERATING_MODE_COLUMN_WIDTH;
            static const int PROCESS_OPERATING_MODE_COLUMN;

            static const std::string CONTROL_MODE;
            static const std::string MONITOR_MODE;

            // This is the current system controller we are displaying processes for
            TA_Base_Core::IConfigSystemController* m_currentSystemController;
    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(PROCESS_LIST_CTRL_H_INCLUDED)
