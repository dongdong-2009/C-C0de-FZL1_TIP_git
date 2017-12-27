/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/profile/src/DisplayListCtrl.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the list display used to show the available displays for the profile
  * in the ProfileDetailView class
  */

#if !defined (DISPLAY_LIST_CTRL_H_INCLUDED)
#define DISPLAY_LIST_CTRL_H_INCLUDED


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define IMPORT_EXTENSIONS
#include "app/configuration/config_plugins/operator/src/stdafx.h"
#include "bus\mfc_extensions\src\mfc_extensions.h"


namespace TA_Base_Core
{
    class IConfigProfile;
}

namespace TA_Base_App
{
    class ProfileDatabaseAccessor;

    class DisplayListCtrl : public CEditableListCtrl, public ICellManager
    {
	    public:
            /**
             * Constructor
             */
		    DisplayListCtrl();


            /**
             * Destructor
             */
		    virtual ~DisplayListCtrl();


            /**
             * populateControl
             *
             * This will populate the list control with all the displays for the profile passed in
             *
             * @param IConfigProfile* - The current profile the user has selected
             */
            void populateControl(TA_Base_Core::IConfigProfile* currentProfile);


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


            /** 
             * addBitmapToHeader
             *
             * Adds the Mandatory bitmap to the header of the second column of the 
			 * list control.
             */
	        virtual void addBitmapToHeader();

            void setDatabaseAccessor(ProfileDatabaseAccessor* databaseAccessor)
            {
                m_databaseAccessor = databaseAccessor;
            }

	    // Overrides
		    // ClassWizard generated virtual function overrides
		    //{{AFX_VIRTUAL(DisplayListCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL


            // Generated message map functions
	    protected:
		    //{{AFX_MSG(DisplayListCtrl)
	//}}AFX_MSG

		    DECLARE_MESSAGE_MAP()

		    void setupList();

        private:

            void setUpKnownParameter(std::vector<std::string>& parameterList, const std::string& parameterName);
            static std::string toChinese(const std::string& s);
            static std::string toEnglish(const std::string& s);
            static std::vector<std::string> toChinese(const std::vector<std::string>& vs);
            static std::vector<std::string> toEnglish(const std::vector<std::string>& vs);
            static std::string translate(const std::string& s, const std::map<std::string, std::string>& map);

        private:
            // These are the set up details for the attribute list control
            static const CString        LOCATION_AVAILABLE_COLUMN_NAME;
            static const int            LOCATION_AVAILABLE_COLUMN_WIDTH;
            static const int            LOCATION_AVAILABLE_COLUMN;
            static const CString        LOCATION_NAME_COLUMN_NAME;
            static const int            LOCATION_NAME_COLUMN_WIDTH;
            static const int            LOCATION_NAME_COLUMN;

            static const CString        DISPLAY1_COLUMN_NAME;
            static const CString        DISPLAY2_COLUMN_NAME;
            static const CString        DISPLAY3_COLUMN_NAME;

            static const int            DISPLAY_COLUMN_WIDTH;

            static const std::string    BOOLEAN_TRUE;
            static const std::string    BOOLEAN_FALSE;

            static const CString        LEFT_BANNER_COLUMN_NAME;
            static const CString        CENTRE_BANNER_COLUMN_NAME;
            static const CString        RIGHT_BANNER_COLUMN_NAME;

            TA_Base_Core::IConfigProfile* m_currentProfile;    // This is the current profile we are displaying
                                                          // configuration for

            CImageList m_imageList;                       // This is the image list holding the bitmap for
                                                          // a mandatory item

            ProfileDatabaseAccessor* m_databaseAccessor;

            std::vector<std::string> m_parameterNames;
            CFont m_font;
    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(DISPLAY_LIST_CTRL_H_INCLUDED)
