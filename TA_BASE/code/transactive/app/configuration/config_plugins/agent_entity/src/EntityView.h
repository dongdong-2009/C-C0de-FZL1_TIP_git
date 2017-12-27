/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/agent_entity/src/EntityView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an entity's details.
  */

#if !defined(AFX_ENTITYVIEW_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_ENTITYVIEW_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/agent_entity/src/resource.h"
#include "app/configuration/config_plugins/agent_entity/src/ParameterListCtrl.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigEntity;
}


namespace TA_Base_App
{
    class EntityDatabaseAccessor;
    class EntityListView;


    class EntityView : public AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    EntityView();


        /**
         * Destructor
         */
	    virtual ~EntityView();

        
        DECLARE_DYNCREATE(EntityView)


    // Form Data
    public:
	    //{{AFX_DATA(EntityView)
	enum { IDD = IDD_ENTITY_VIEW };
	CTransparentImage	m_addressBitmap;
	CTransparentImage	m_descriptionBitmap;
	CTransparentImage	m_parentBitmap;
	CTransparentImage	m_nameBitmap;
	CTransparentImage	m_locationBitmap;
	ParameterListCtrl	m_parameterList;
	CEdit	m_parent;
	CEdit	m_subsystem;
	CEdit	m_location;
	CStatic	m_modifyDate;
	CStatic	m_createDate;
	CButton	m_type;
	CEdit	m_name;
	CEdit	m_description;
	CEdit	m_address;
	//}}AFX_DATA


        /**
         * populateItemDetails
         *
         * This will populate all fields in the view using the database object passed in.
         *
         * @param IConfigItem* - The database object for the entity we want to display. We will use this to
         *                         set new values and apply. If this is NULL then the details should all be hidden
         */
        void populateItemDetails(TA_Base_Core::IConfigItem* currentEntity);


        /**
         * refreshButtonAvailability
         *
         * This should be called after any modification to the fields. This will enable or disable
         * the reset and apply buttons based on whether or not any data has changed.
         */
        virtual void refreshButtonAvailability();


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(EntityView)
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
	    //{{AFX_MSG(EntityView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
	    afx_msg void onButtonChangeLocation();
	    afx_msg void onButtonChangeParent();
	afx_msg void onKillfocusEditName();
	afx_msg void onKillfocusEditDescription();
	afx_msg void onKillfocusEditAddress();
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
            return MessageSender::ENTITY;
        }

    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        EntityView( const EntityView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        EntityView& operator=(const EntityView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the entity details. These controls will be
         * hidden until the user selects an entity from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);


    private:

        // The titles for the change dialog box
        static const CString CHANGE_PARENT_DIALOG_TITLE;
        static const CString CHANGE_LOCATION_DIALOG_TITLE;

        // These match the restrictions in the database for how long certain fields can be.
        // Hopefully in the future these could be read from the database.
        static const int NAME_FIELD_LENGTH;
        static const int ADDRESS_FIELD_LENGTH;
        static const int DESCRIPTION_FIELD_LENGTH;


        TA_Base_Core::IConfigEntity* m_currentEntity;   // The current entity the user is configuring.
                                                   // This is the database object for it.

        EntityListView* m_listView; // This is the view in the left pane of the splitter
                                    // showing the list of all entities. We need to use this to
                                    // tell it when entity details have changed.

        EntityDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve information
                                                     // from the database.


    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_ENTITYVIEW_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
