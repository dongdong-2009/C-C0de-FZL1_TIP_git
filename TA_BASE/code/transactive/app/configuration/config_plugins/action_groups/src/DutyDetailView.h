/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/action_groups/src/DutyDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a Duty's details.
  */

#if !defined(AFX_DutyDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_DutyDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/action_groups/src/resource.h"
#include "app/configuration/config_plugins/action_groups/src/ActionListCtrl.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigActionGroup;
}


namespace TA_Base_App
{
    class DutyDatabaseAccessor;
    class DutyListView;


    class DutyDetailView : public AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    DutyDetailView();


        /**
         * Destructor
         */
	    virtual ~DutyDetailView();

        
        DECLARE_DYNCREATE(DutyDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(DutyDetailView)
	enum { IDD = IDD_DUTY_VIEW };
	CButton	m_controlCheck;
	CTransparentImage	m_nameBitmap;
	ActionListCtrl	m_actionList;
	CButton	m_type;
	CStatic	m_modifyDate;
	CStatic	m_createDate;
	CEdit	m_name;
	//}}AFX_DATA


        /**
         * populateItemDetails
         *
         * This will populate all fields in the view using the database object passed in.
         *
         * @param IConfigItem* - The database object for the Duty we want to display. We will use this to
         *                         set new values and apply. If this is NULL then the details should all be hidden
         */
        void populateItemDetails(TA_Base_Core::IConfigItem* currentDuty);


        /**
         * refreshButtonAvailability
         *
         * This should be called after any modification to the fields. This will enable or disable
         * the reset and apply buttons based on whether or not any data has changed.
         */
        virtual void refreshButtonAvailability();


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(DutyDetailView)
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
	    //{{AFX_MSG(DutyDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
	afx_msg void onKillfocusEditName();
	afx_msg void onCheckControl();
	afx_msg void onButtonAddSelected();
	afx_msg void onButtonRemoveSelected();
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
            return MessageSender::ACTION_GROUP;
        }

    
    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        DutyDetailView( const DutyDetailView&);


        /**
         * Duty =
         *
         * Hidden as a private method so it is not used.
         */
        DutyDetailView& operator=(const DutyDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the Duty details. These controls will be
         * hidden until the user selects a Duty from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);


    private:

        // These match the restrictions in the database for how long certain fields can be.
        // Hopefully in the future these could be read from the database.
        static const int NAME_FIELD_LENGTH;


        TA_Base_Core::IConfigActionGroup* m_currentDuty;   // The current Duty the user is configuring.
                                                   // This is the database object for it.

        DutyListView* m_listView; // This is the view in the left pane of the splitter
                                    // showing the list of all dutys. We need to use this to
                                    // tell it when Duty details have changed.

        DutyDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve information
                                                     // from the database.


    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_DutyDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
