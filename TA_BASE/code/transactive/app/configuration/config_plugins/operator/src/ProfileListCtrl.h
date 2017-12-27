/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/operator/src/ProfileListCtrl.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the list display used to show the available profiles for the operator
  * in the OperatorDetailView class
  */

#if !defined (PROFILE_LIST_CTRL_H_INCLUDED)
#define PROFILE_LIST_CTRL_H_INCLUDED


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define IMPORT_EXTENSIONS
#include "app/configuration/config_plugins/operator/src/stdafx.h"
#include "bus\mfc_extensions\src\mfc_extensions.h"


namespace TA_Base_Core
{
    class IConfigOperator;
}

namespace TA_Base_App
{
    class ProfileListCtrl : public CEditableListCtrl, public ICellManager
    {
	    public:
            /**
             * Constructor
             */
		    ProfileListCtrl();


            /**
             * Destructor
             */
		    virtual ~ProfileListCtrl();


            /**
             * populateControl
             *
             * This will populate the list control with all the profiles and check those valid for
             * the operator passed in.
             *
             * @param IConfigOperator* - The current operator the user has selected
             * @param bool - This indicates if the control will be enabled or disabled so we know what
             *               to do with the check-boxes.
             */
            void populateControl(TA_Base_Core::IConfigOperator* currentOperator, bool isEnabled);


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
		    //{{AFX_VIRTUAL(ProfileListCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL


            // Generated message map functions
	    protected:
		    //{{AFX_MSG(ProfileListCtrl)
	//}}AFX_MSG

		    DECLARE_MESSAGE_MAP()

		    void setupList();

        private:

            // These are the set up details for the attribute list control
            static const CString PROFILE_NAME_COLUMN_NAME;
            static const int     PROFILE_NAME_COLUMN_WIDTH;
            static const int     PROFILE_NAME_COLUMN;

            static const CString PROFILE_CHECK_COLUMN_NAME;
            static const int     PROFILE_CHECK_COLUMN_WIDTH;
            static const int     PROFILE_CHECK_COLUMN;

            static const std::string BOOLEAN_TRUE;
            static const std::string BOOLEAN_FALSE;
			CFont                   m_font;

            TA_Base_Core::IConfigOperator* m_currentOperator;    // This is the current operator we are displaying
                                                            // parameters for

    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(PROFILE_LIST_CTRL_H_INCLUDED)
