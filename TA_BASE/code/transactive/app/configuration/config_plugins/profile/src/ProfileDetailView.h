/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/profile/src/ProfileDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a profile's details.
  */

#if !defined(AFX_ProfileDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_ProfileDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/profile/src/resource.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "app/configuration/config_plugins/profile/src/DisplayListCtrl.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigProfile;
}


namespace TA_Base_App
{
    class ProfileDatabaseAccessor;
    class ProfileListView;


    class ProfileDetailView : public AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    ProfileDetailView();


        /**
         * Destructor
         */
	    virtual ~ProfileDetailView();

        
        DECLARE_DYNCREATE(ProfileDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(ProfileDetailView)
	enum { IDD = IDD_PROFILE_VIEW };
	CButton	m_isSystemProfile;
	    CButton	m_type;
	    CStatic	m_modifyDate;
	    CStatic	m_createDate;
	    CEdit	m_name;
	    DisplayListCtrl	m_displayList;
	    CTransparentImage	m_typeBitmap;
	    CTransparentImage	m_nameBitmap;
	    CTransparentImage	m_groupBitmap;
	    CButton m_controlRadio;
	    CButton m_localRadio;
        CEdit m_accessGroup;
        CEdit m_radioGroup;
        CEdit m_paPriorityGroup;
        CButton m_changeAccessGroup;
        CButton m_changeRadioGroup;
        CButton m_changePaPriorityGroup;
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
	    //{{AFX_VIRTUAL(ProfileDetailView)
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
	    //{{AFX_MSG(ProfileDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
	    afx_msg void onKillfocusEditName();
	afx_msg void onCentralRadio();
	afx_msg void onLocalRadio();
    afx_msg void onChangeAccessGroup();
    afx_msg void onChangeRadioGroup();
    afx_msg void onChangePaPriorityGroup();
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
            return MessageSender::PROFILE;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        ProfileDetailView( const ProfileDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        ProfileDetailView& operator=(const ProfileDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the profile details. These controls will be
         * hidden until the user selects a profile from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);


        /**
         * updateReadOnlyFields
         *
         * This will look at the type of profile and enable or disable the fields as appropriate.
         * This is based on whether the profile is a system profile or not.
         */
        void updateReadOnlyFields();

    private:

        // These match the restrictions in the database for how long certain fields can be.
        // Hopefully in the future these could be read from the database.
        static const int NAME_FIELD_LENGTH;

        // These are the keys used for local and central profiles. At the moment we have to 
        // hard-code them.
        static const int LOCAL_PROFILE;
        static const int CENTRAL_PROFILE;

        TA_Base_Core::IConfigProfile* m_currentProfile;   // The current profile the user is configuring.
                                                     // This is the database object for it.

        ProfileListView* m_listView; // This is the view in the left pane of the splitter
                                     // showing the list of all profiles. We need to use this to
                                     // tell it when profile details have changed.

        ProfileDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve information
                                                      // from the database.

        static const CString CHANGE_ACCESS_CONTROL_GROUP_DIALOG_TITLE;
        static const CString CHANGE_RADIO_LOGIN_GROUP_DIALOG_TITLE;
        static const CString CHANGE_PA_PRIORITY_GROUP_DIALOG_TITLE;

    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_ProfileDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
