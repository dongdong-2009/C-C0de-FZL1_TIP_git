/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/profile_groups/src/ProfileGroupsDetailView.h $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a profile-group's details.
  */

#if !defined(AFX_ProfileGroupsDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_ProfileGroupsDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/profile_groups/src/resource.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigProfileGroup;
}

namespace TA_Base_App
{
    class ProfileGroupsDatabaseAccessor;
    class ProfileGroupsListView;


    class ProfileGroupsDetailView : public AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    ProfileGroupsDetailView();


        /**
         * Destructor
         */
	    virtual ~ProfileGroupsDetailView();

        
        DECLARE_DYNCREATE(ProfileGroupsDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(ProfileGroupsDetailView)
	enum { IDD = IDD_PROFILE_GROUPS_VIEW };
		CStatic m_type;
		CStatic	m_staticUsed;
	    CStatic	m_modifyDate;
	    CStatic	m_createDate;
	    CEdit	m_name;
	    CTransparentImage	m_nameBitmap;
		CString	m_usedText;
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
	    //{{AFX_VIRTUAL(ProfileGroupsDetailView)
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
	    //{{AFX_MSG(ProfileGroupsDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
	    afx_msg void onKillfocusEditName();
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
            return MessageSender::PROFILE_GROUP;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        ProfileGroupsDetailView( const ProfileGroupsDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        ProfileGroupsDetailView& operator=(const ProfileGroupsDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the profile group details. These controls will be
         * hidden until the user selects a profile group from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);


        /**
         * updateReadOnlyFields
         *
         * This will look at the type of profile group and enable or disable the fields as appropriate.
         * 
         */
        void updateReadOnlyFields();

    private:

        // These match the restrictions in the database for how long certain fields can be.
        // Hopefully in the future these could be read from the database.
        static const int NAME_FIELD_LENGTH;

		// The current profile group viewed / open from the database
		TA_Base_Core::IConfigProfileGroup* m_curProfileGp;

        ProfileGroupsListView* m_listView; // This is the view in the left pane of the splitter
                                     // showing the list of all profile groups. We need to use this to
                                     // tell it when profile group details have changed.

        ProfileGroupsDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve information
                                                      // from the database.


    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_ProfileGroupsDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
