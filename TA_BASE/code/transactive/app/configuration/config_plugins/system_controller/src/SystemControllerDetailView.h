/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/system_controller/src/SystemControllerDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a system controller's details.
  */

#if !defined(AFX_SystemControllerDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_SystemControllerDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/system_controller/src/resource.h"
#include "app/configuration/config_plugins/system_controller/src/GroupListCtrl.h"
#include "app/configuration/config_plugins/system_controller/src/ProcessListCtrl.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigSystemController;
}


namespace TA_Base_App
{
    class SystemControllerDatabaseAccessor;
    class SystemControllerListView;


    class SystemControllerDetailView : public AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    SystemControllerDetailView();


        /**
         * Destructor
         */
	    virtual ~SystemControllerDetailView();

        
        DECLARE_DYNCREATE(SystemControllerDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(SystemControllerDetailView)
	enum { IDD = IDD_SYSTEMCONTROLLER_VIEW };
	ProcessListCtrl	m_processList;
	GroupListCtrl	m_groupList;
	CEdit	m_debugLogDir;
	CEdit	m_binDir;
	CEdit	m_server;
	CEdit	m_heartbeatSendPeriod;
	CEdit	m_group;
	CEdit	m_agentPollPeriod;
	CEdit	m_agentResponseTimeout;
	CTransparentImage	m_artBitmap;
	CTransparentImage	m_serverBitmap;
	CTransparentImage	m_debugLogDirBitmap;
	CTransparentImage	m_binDirBitmap;
	CTransparentImage	m_hspBitmap;
	CTransparentImage	m_appBitmap;
	CTransparentImage	m_nameBitmap;
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
	    //{{AFX_VIRTUAL(SystemControllerDetailView)
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
	    //{{AFX_MSG(SystemControllerDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
	    afx_msg void onKillfocusEditName();
	    afx_msg void onKillfocusEditAgentPollPeriod();
	    afx_msg void onKillfocusEditLogDirectory();
	    afx_msg void onKillfocusEditBinDirectory();
	    afx_msg void onKillfocusEditHeartbeatSendPeriod();
	    afx_msg void onButtonAddProcess();
	    afx_msg void onButtonRemoveProcess();
	    afx_msg void onButtonServer();
	    afx_msg void onButtonGroup();
	afx_msg void OnKillfocusEditAgentResponseTimeout();
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
            return MessageSender::SYSTEM_CONTROLLER;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        SystemControllerDetailView( const SystemControllerDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        SystemControllerDetailView& operator=(const SystemControllerDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the system controller details. These controls will be
         * hidden until the user selects a system controller from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);


    private:

        // These match the restrictions in the database for how long certain fields can be.
        // Hopefully in the future these could be read from the database.
        static const int NAME_FIELD_LENGTH;
        static const int DEBUG_LOG_DIR_FIELD_LENGTH;
        static const int BIN_DIR_FIELD_LENGTH;
        static const int AGENT_POLL_PERIOD_FIELD_LENGTH;
        static const int HEARTBEAT_SEND_PERIOD_LENGTH;
        static const int AGENT_RESPONSE_TIMEOUT_LENGTH;

        static const CString CHANGE_GROUP_DIALOG_TITLE;
        static const CString CHANGE_SERVER_DIALOG_TITLE;

        static const std::string NO_GROUP;

        TA_Base_Core::IConfigSystemController* m_currentSystemController; // The current system controller the user is configuring.
                                                                     // This is the database object for it.

        SystemControllerListView* m_listView; // This is the view in the left pane of the splitter
                                              // showing the list of all system controllers. We need to use this to
                                              // tell it when system controller details have changed.

        SystemControllerDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve information
                                                               // from the database.


    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_SystemControllerDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
