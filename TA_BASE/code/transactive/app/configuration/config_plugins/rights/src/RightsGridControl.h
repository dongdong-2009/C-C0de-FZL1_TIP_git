/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/rights/src/RightsGridControl.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is a wrapper around the CGridCtrl (from mfc_extensions) to modify it to display and allow
  * configuration of rights.
  */

#if !defined(AFX_RIGHTSGRIDCONTROL_H__715F497C_7179_4956_90F9_6313A0F35620__INCLUDED_)
#define AFX_RIGHTSGRIDCONTROL_H__715F497C_7179_4956_90F9_6313A0F35620__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <string>

#define IMPORT_EXTENSIONS
#include "bus/mfc_extensions/src/mfc_extensions.h"
#include "app/configuration/config_plugins/rights/src/RightsDatabaseAccessor.h"

namespace TA_Base_App
{
    class RightsDatabaseAccessor;

    class RightsGridControl : public CGridCtrl
    {
    public:
        /**
         * Constructor
         */
	    RightsGridControl();


        /**
         * Destructor
         */
	    virtual ~RightsGridControl();


        /**
         * loadItems
         *
         * This is called when the rest of the view is ready for the grid to be loaded with details
         *
         * @param RightsDatabaseAccessor* - This is used to retrieve the rights from the database
         * @param RightsType - This indicates what rights this grid should display
         * @param CProgressCtrl& - This is used to indicate how we are going with the loading
         */
        void loadItems(RightsDatabaseAccessor* dbAccessor,
                       RightsDatabaseAccessor::RightsType gridType,
                       CProgressCtrl& progress,
                       unsigned long subsystemStateKey);


        /**
         * isConfigurationValid
         *
         * This looks at the current configuration and determines if it is valid or not.
         *
         * @param vector<unsigned long> - If the configuration is invalid this will be populated with all the
         *                                subsystem pkeys with invalid configuration.
         *
         * @return bool - Indicates whether the configuration is valid. Returns true if valid, false otherwise
         */
        bool isConfigurationValid(std::vector<unsigned long>& subsystems);


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(RightsGridControl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	    // Generated message map functions
    protected:
	    //{{AFX_MSG(RightsGridControl)
		    // NOTE - the ClassWizard will add and remove member functions here.
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


        /**
         * OnEndEditCell
         *
         * This is an override that is called when the user has finished editing a cell. In here
         * we will update our database objects and store the changes ready for applying later.
         *
         * @param int - The row the user has modified
         * @param int - The column the user has modified
         * @param CString - This is the new value the user has entered for the cell
         */
        virtual void OnEndEditCell(int nRow, int nCol, CString str);


        /**
         * CreateInPlaceEditControl
         *
         * This is an override that is called when the user begins editing a cell. We must override
         * it because we want to create a drop down list rather then just let the user type text in.
         *
         * @param Parameters just need to be passed onto the base implementation of this method
         */
        virtual void CreateInPlaceEditControl(CRect& rect, DWORD dwStyle, UINT nID,
                                                     int nRow, int nCol,
                                                     LPCTSTR szText, int nChar);

    private:
        /**
         * setUpHeaders
         * 
         * This sets up the row and column headers after retrieving all the required information
         * from the database
         */
        void setUpHeaders();


        /** 
         * loadActionGroups
         *
         * THis loads the details about the ActionGroups and stores it for later use
         */
        void loadActionGroups();


        /**
         * populateWithData
         *
         * This loads all the righst information and adds it into the grid
         */
        void populateWithData(unsigned long subsystemStateKey);

        /**
         * resetGrid
         *
         * Resets all the fields to blank
         */
        void resetGrid();


        static const int LOCAL_PROFILE;
        static const int CENTRAL_PROFILE;

        // This is used to retrieve rights etc from the database
        RightsDatabaseAccessor* m_databaseAccessor;

        // This is a key to hold the ActionGroup keys and names. THe names will be displayed to the user
        // and the keys will be set in the Rights database objects.
        std::map<unsigned long, std::string> m_actionGroups;

        // This indicates what type of information this grid is holding
        RightsDatabaseAccessor::RightsType m_gridType;

        // This contains the icons to indicate if the profiles are local or central icons
        CImageList m_profileIcons;

        unsigned long m_subsystemStateKey;
    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_RIGHTSGRIDCONTROL_H__715F497C_7179_4956_90F9_6313A0F35620__INCLUDED_)
