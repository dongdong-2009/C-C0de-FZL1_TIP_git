/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/rights/src/RightsView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the main view for configuring rights. This is the view that will display all rights information
  * and allow it to be configured.
  */

#if !defined(AFX_RIGHTSVIEW_H__E99415DD_E331_4C0D_A2FF_554CD680D7A1__INCLUDED_)
#define AFX_RIGHTSVIEW_H__E99415DD_E331_4C0D_A2FF_554CD680D7A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/config_plugin_helper/src/IMainView.h"
#include "app/configuration/config_plugins/rights/src/resource.h"

#include "app/configuration/config_plugins/rights/src/RightsGridControl.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_App
{
    class RightsComponent;


    class RightsView : public CFormView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    RightsView();


        /**
         * Destructor
         */
	    virtual ~RightsView();

	    DECLARE_DYNCREATE(RightsView)

    public:

	    //{{AFX_DATA(RightsView)
	    enum { IDD = IDD_RIGHTS_VIEW };
	    CListCtrl	m_subsystemList;
	    CListCtrl	m_applicationList;
        CEdit       m_subsystemState;
        //}}AFX_DATA

    public:

        /**
         * setComponentInterface
         *
         * This is called by the component interface to register itself with this view. This view
         * is the main view so it provides all communication between the various
         * bits and pieces. This method will also perform any final initialisation required on its
         * child views now that it has all the information it requires.
         */
        virtual void setComponentInterface(RightsComponent* component);


        /**
         * refreshButtonAvailability
         *
         * This should be called after any modification to the fields. This will enable or disable
         * the reset and apply buttons based on whether or not any data has changed.
         *
         * @param bool - This indicates if the buttons should be available or not.
         */
        virtual void refreshButtonAvailability(bool isAvailable);


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(RightsView)
	public:
	    virtual void OnInitialUpdate();
	protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL


    // Implementation
    protected:
    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif

	    // Generated message map functions
	    //{{AFX_MSG(RightsView)
	    afx_msg void onButtonApply();
	    afx_msg void onButtonReset();
        afx_msg void onChangeSubsystemState();
        afx_msg void onSelchangeSubsystemAppTab(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnSize(UINT nType, int cx, int cy);
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:

        /**
         * loadItems
         *
         * This will load all the items and populate the grids.
         */
        void loadItems();


        /**
         * loadSubsystemState
         *
         * This will load all the subsystem states edit box.
         */
        void loadSubsystemState();


        /**
         * isValidConfiguration
         *
         * This will check the configuration of the rights and print an appropriate error message
         * to the user if it is invalid
         *
         * @param CString - The name of the task that the user was trying to perform eg "applied" or "loaded"
         *
         * @return bool - Indicates whether the configuration was valid or not. true = valid
         */
        bool isValidConfiguration(const CString& task);


        // These are the control Id's to give the grids we create. These id's are the two last id's in the
        // range specified for this component.
        static const int SUBSYSTEM_GRID_ID;
        static const int APPLICATION_GRID_ID;


        // This is the maximum number of items we should show in a list in an error message
        static const int MAX_NUMBER_OF_ITEMS_TO_LIST;

        RightsComponent* m_component;

        RightsGridControl m_subsystemGrid;
        RightsGridControl m_applicationGrid;

        unsigned long m_subsystemStateKey;

        static const CString CHANGE_SUBSYSTEM_STATE_DIALOG_TITLE;
    
    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // !defined(AFX_RIGHTSVIEW_H__E99415DD_E331_4C0D_A2FF_554CD680D7A1__INCLUDED_)
