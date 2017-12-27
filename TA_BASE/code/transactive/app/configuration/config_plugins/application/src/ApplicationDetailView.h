/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/application/src/ApplicationDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a Application's details.
  */

#if !defined(AFX_ApplicationDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_ApplicationDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/application/src/resource.h"
#include "app/configuration/config_plugins/application/src/ParameterListCtrl.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigGui;
}


namespace TA_Base_App
{
    class ApplicationDatabaseAccessor;
    class ApplicationListView;


    class ApplicationDetailView : public AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    ApplicationDetailView();


        /**
         * Destructor
         */
	    virtual ~ApplicationDetailView();

        
        DECLARE_DYNCREATE(ApplicationDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(ApplicationDetailView)
	enum { IDD = IDD_APPLICATION_VIEW };
	CButton	m_checkReposition;
	CEdit	m_helpPath;
	CTransparentImage	m_shortnameBitmap;
	CTransparentImage	m_nameBitmap;
	CTransparentImage	m_locationBitmap;
	CTransparentImage	m_descriptionBitmap;
	CTransparentImage	m_addressBitmap;
	CTransparentImage	m_fullnameBitmap;
	CTransparentImage	m_executableBitmap;
	ParameterListCtrl	m_attributeList;
	CEdit	m_subsystem;
	CEdit	m_shortname;
	CEdit	m_name;
	CEdit	m_location;
	CEdit	m_fullname;
	CEdit	m_executable;
	CEdit	m_description;
	CEdit	m_address;
	CStatic	m_modifyDate;
	CStatic	m_createDate;
	CButton	m_type;
	CButton	m_status;
	CButton	m_checkLimitInstances;
	CButton	m_checkPassParams;
	CButton	m_checkDisplayIcon;
	CButton	m_checkClose;
	//}}AFX_DATA


        /**
         * populateItemDetails
         *
         * This will populate all fields in the view using the database object passed in.
         *
         * @param IConfigItem* - The database object for the item we want to display. We will use this to
         *                       set new values and apply. If this is NULL then the details should all be hidden
         */
        void populateItemDetails(TA_Base_Core::IConfigItem* currentItem);


        /**
         * refreshButtonAvailability
         *
         * This should be called after any modification to the fields. This will enable or disable
         * the reset and apply buttons based on whether or not any data has changed.
         */
        virtual void refreshButtonAvailability();


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ApplicationDetailView)
	public:
	virtual void OnInitialUpdate();
	protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

    protected:

    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif


	    // Generated message map functions
	    //{{AFX_MSG(ApplicationDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
	afx_msg void onButtonLocation();
	afx_msg void onCheckClose();
	afx_msg void onCheckDisplayIcon();
	afx_msg void onCheckMultiple();
	afx_msg void onCheckPassparams();
	afx_msg void onKillfocusEditAddress();
	afx_msg void onKillfocusEditDescription();
	afx_msg void onKillfocusEditExecutable();
	afx_msg void onKillfocusEditFullname();
	afx_msg void onKillfocusEditName();
	afx_msg void onKillfocusEditShortname();
	afx_msg void onChangeEditName();
	afx_msg void onKillfocusEditHelp();
	afx_msg void onCheckReposition();
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    protected:

        /**
         * getConfigMessageType
         *
         * This should return the type of configuration update. ie ENTITY, OPERATOR etc.
         *
         * @return ConfigUpdateType - The type of component this update is for
         */
        virtual MessageSender::ConfigUpdateType getConfigMessageType() const
        {
            return MessageSender::APPLICATION;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        ApplicationDetailView( const ApplicationDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        ApplicationDetailView& operator=(const ApplicationDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the Application details. These controls will be
         * hidden until the user selects a Application from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);


        /**
         * enableEntitySection
         *
         * This will enable or disable all the controls for the entity details. These controls will be
         * disabled for applications that do not have entity configuration
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void enableEntitySection(bool isEnabled);


        /**
         * displayEntityDetails
         *
         * This retrieves the entity related to this Gui and populates the bottom part of the view
         *
         * @exception DataException
         * @exception DatabaseException
         */
        void displayEntityDetails();


    private:

        // These match the restrictions in the database for how long certain fields can be.
        // Hopefully in the future these could be read from the database.
        static const int SHORT_NAME_FIELD_LENGTH;
        static const int FULL_NAME_FIELD_LENGTH;
        static const int EXECUTABLE_FIELD_LENGTH;
        static const int NAME_FIELD_LENGTH;
        static const int DESCRIPTION_FIELD_LENGTH;
        static const int ADDRESS_FIELD_LENGTH;

        // The titles for the change dialog box
        static const CString CHANGE_LOCATION_DIALOG_TITLE;

        static const CString ENTITYNAME;

        TA_Base_Core::IConfigGui* m_currentApplication; // The current Application the user is configuring.
                                                   // This is the database object for it.

        ApplicationListView* m_listView; // This is the view in the left pane of the splitter
                                    // showing the list of all applications. We need to use this to
                                    // tell it when Application details have changed.

        ApplicationDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve information
                                                     // from the database.


    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_ApplicationDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
