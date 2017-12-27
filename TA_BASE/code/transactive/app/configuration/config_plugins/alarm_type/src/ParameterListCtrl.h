/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_type/src/ParameterListCtrl.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the list display used to show the entity parameters in the AlarmType class
  */


// ParameterListCtrl.h: interface for the ParameterListCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(PARAMETER_LIST_CTRL_H__INCLUDED)
#define PARAMETER_LIST_CTRL_H__INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define IMPORT_EXTENSIONS
#include "app/configuration/config_plugins/generic_entity/src/stdafx.h"
#include "bus\mfc_extensions\src\mfc_extensions.h"

namespace TA_Base_Core
{
    class IConfigAlarmType;
}


namespace TA_Base_App
{
	class ParameterListCtrl : public CEditableListCtrl, public ICellManager  
	{
	    public:
            /**
             * Constructor
             */
		    ParameterListCtrl();


            /**
             * Destructor
             */
		    virtual ~ParameterListCtrl();


            /**
             * populateControl
             *
             * This will populate the list control with the alarm-type parameters of the alarm-type passed 
             * in.
             *
             * @param IConfigAlarmType* - The current alarm-type the user has selected
             */
            void populateControl(TA_Base_Core::IConfigAlarmType* currentAlarmType);

            /**
             * resetParameters
             *
             * This will reset all parameters to empty and clear the list
             *
             */
			void resetParameters();

            /**
             * validateEmptyValues
             *
             * This will check whether all parameter values are empty
			 * A message box will be displayed for any empty parameter value
             *
			 * @ return TRUE if empty and FALSE if not
             */
			bool validateEmptyValues();
            //
		    // ICellManager methods
            //

            /**
             * GetCellType
             *
             * This determines the type of cell the list control should display. This is based on
             * the type of parameter the user has selected.
             *
             * @param CEditableListCtrl* - This is the list control and can be used to retrieve the
             *                             parameter name.
             * @param int - This is the currently selected item.
             * @param int - This is the currently selected column. We are only interested if it is the
             *              value column. Otherwise the user cannot edit it.
             *
             * @return CellType - This returns the string type if this is a STRING entityparameter.
             *                    It will return a combo type for BOOLEAN and ENUMERATION entityparameters.
             *                    It will return number type for REAL and INTEGER entityparameters
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
             *                             parameter name.
             * @param int - This is the currently selected item.
             * @param int - This is the currently selected column. We are only interested if it is the
             *              value column. Otherwise the user cannot edit it.
             *
             * @return CellData - The data for the selected item. If this is a boolean then they will
             *                    be given the two boolean entries plus a blank entry if the parameter
             *                    is not mandatory. For enumeration entries this will return the values
             *                    retrieved from the IConfigEntity object.
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
             *                             parameter name.
             * @param int - This is the currently selected item.
             * @param int - This is the currently selected column. We are only interested if it is the
             *              value column. Otherwise the user cannot edit it.
             * @param CString - THe new value for the item
             *
             * @return bool - This indicates whether the update worked or not. True indicates everything went
             *                ok and the control can update. False indicates an error occurred and the update
             *                cannot take place.
             */
            virtual bool UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value);


	    // Overrides
		    // ClassWizard generated virtual function overrides
		    //{{AFX_VIRTUAL(ParameterListCtrl)
			protected:
			virtual void PreSubclassWindow();
			//}}AFX_VIRTUAL


            // Generated message map functions
	    protected:
		    //{{AFX_MSG(ParameterListCtrl)
			//}}AFX_MSG

		    DECLARE_MESSAGE_MAP()

		    void setupList();

        private:
            // These are the set up details for the attribute list control
            static const int ATTRIBUTE_NAME_COLUMN_WIDTH;
            static const int ATTRIBUTE_NAME_COLUMN;

            static const int ATTRIBUTE_VALUE_COLUMN_WIDTH;
            static const int ATTRIBUTE_VALUE_COLUMN;

            static const int ATTRIBUTE_UPDATE_COLUMN_WIDTH;
            static const int ATTRIBUTE_UPDATE_COLUMN;


            static const int PARAMETER_VALUE_TEXT_LIMIT;
            static const int PARAMETER_VALUE_NUMBER_LIMIT;

            TA_Base_Core::IConfigAlarmType* m_currentAlarmType;    // This is the current alarm type we are displaying
                                                        // parameters for

            CImageList m_imageList;                     // This is the image list holding the bitmap for
                                                        // a mandatory parameter

            unsigned long m_maximumCharactersInField;   // This holds the number of character the currently
                                                        // selected edit field will allow. This is set when
                                                        // the list control retrieves the cell type.
	};
}// end namespace
#endif // !defined(PARAMETER_LIST_CTRL_H__INCLUDED)
