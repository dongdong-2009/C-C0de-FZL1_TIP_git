/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/profile/src/ProfileListView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all profiles available to be configured. This view must communicate
  * with the ProfileDetailView as this will show the details of the item selected in this view.
  */

#if !defined(AFX_ProfileListView_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_)
#define AFX_ProfileListView_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_

#include <afxcview.h>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractListView.h"

#include "bus/security/authentication_library/src/AuthenticationLibrary.h" //TD15556

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_App
{
    class ProfileListView : public AbstractListView
    {
    protected:
        /**
         * Constructor
         *
         * Protected constructor used by dynamic creation
         */
	    ProfileListView();

	    virtual ~ProfileListView();

	    DECLARE_DYNCREATE(ProfileListView)


    // Operations
    public:

        /**
         * deleteItem
         *
         * This requires an item to be selected. This warns the user about what they are deleting and then
         * calls the base class version of this method. It also checks that the current profile is not a system
         * profile as these cannot be deleted.
         */
        virtual void deleteItem();


        /**
         * copyItem
         *
         * This requires an item to be selected. This method also checks to see if the current profile
         * is a system only profile before copying. It then calls the base class version of this method.
         */
        virtual void copyItem();


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ProfileListView)
	    public:
	    protected:
	    virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	    //}}AFX_VIRTUAL

    // Implementation
    protected:
    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif

	    // Generated message map functions
    protected:
	    //{{AFX_MSG(ProfileListView)
    	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    protected:

        /**
         * getComponentIcon
         *
         * This should return the component icon for items to be shown in the list
         *
         * @return HICON - The icon to be shown
         */
        virtual HICON getComponentIcon()
        {
            return m_componentIcon;
        }


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
        HICON m_componentIcon;

		bool isActiveProfile(unsigned long profileKey); //TD15556
		TA_Base_Bus::AuthenticationLibrary m_authLibrary; //TD15556
    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_ProfileListView_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_)
