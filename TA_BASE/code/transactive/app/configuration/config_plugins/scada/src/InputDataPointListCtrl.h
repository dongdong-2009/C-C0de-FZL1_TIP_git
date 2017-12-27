/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/InputDataPointListCtrl.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the list display used to show the input association data points.
  */

#if !defined (INPUT_DATAPOINT_LIST_CTRL_H_INCLUDED)
#define INPUT_DATAPOINT_LIST_CTRL_H_INCLUDED


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define IMPORT_EXTENSIONS
#include "app/configuration/config_plugins/scada/src/stdafx.h"
#include "bus/mfc_extensions/src/editable_list_control/EditableListCtrl.h"

namespace TA_Base_Core
{
    class IConfigDerivedDataPoint;
}


namespace TA_Base_App
{
    class InputDataPointListCtrl : public CEditableListCtrl, public ICellManager
    {
	    public:
            /**
             * Constructor
             */
		    InputDataPointListCtrl();


            /**
             * Destructor
             */
		    virtual ~InputDataPointListCtrl();


            /**
             * populateControl
             *
             * This will populate the list control with the processes of the system controller passed in.
             *
             * @param IConfigDerivedDataPoint& - The current derived datapoint the user has selected
             */
            void populateControl(TA_Base_Core::IConfigDerivedDataPoint& derivedDataPoint);


            /**
             * addNewItem
             *
             * This is called when the user has selected the option to add new data points to the list.
             * This should display a dialog to the user allowing them to select a new datapoint to add.
             */
            void addNewItem();


            /**
             * removeSelectedItem
             *
             * This is called when the user has selected the option to remove a datapoint from the list.
             * This should remove the item from the list and from the data point object.
             */
            void removeSelectedItem();


            //
		    // ICellManager methods
            //

            /**
             * GetCellType
             *
             * This determines the type of cell the list control should display. This will be either a
             * normal field (for the datapoint name) or a numerical edit box (for the other columns).
             *
             * @param CEditableListCtrl* - This is the list control and can be used to retrieve the
             *                             datapoint name or key
             * @param int - This is the currently selected item.
             * @param int - This is the currently selected column. We are only interested if it is the
             *              calculation order or predefined input column. Otherwise the user cannot edit it.
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
             *                             datapoint name.
             * @param int - This is the currently selected item.
             * @param int - This is the currently selected column. 
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
             *                             datapoint name and key
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
		    //{{AFX_VIRTUAL(InputDataPointListCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL


            // Generated message map functions
	    protected:
		    //{{AFX_MSG(InputDataPointListCtrl)
	//}}AFX_MSG

		    DECLARE_MESSAGE_MAP()

		    void setupList();

        public:
            // These are the set up details for the attribute list control
            static const CString DATAPOINT_COLUMN_NAME;
            static const int DATAPOINT_COLUMN_WIDTH;
            static const int DATAPOINT_COLUMN;

            static const CString CALC_ORDER_COLUMN_NAME;
            static const int CALC_ORDER_COLUMN_WIDTH;
            static const int CALC_ORDER_COLUMN;

            static const CString PREDEFINED_VALUE_COLUMN_NAME;
            static const int PREDEFINED_VALUE_COLUMN_WIDTH;
            static const int PREDEFINED_VALUE_COLUMN;

            static const int PARAMETER_VALUE_NUMBER_LIMIT;

        private:
            // This is the current derived data point we are displaying input associations for
            TA_Base_Core::IConfigDerivedDataPoint* m_derivedDataPoint;
    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(INPUT_DATAPOINT_LIST_CTRL_H_INCLUDED)
