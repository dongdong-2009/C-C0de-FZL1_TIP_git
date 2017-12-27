/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/agent/src/AgentDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an Agent's details.
  */

#if !defined(AFX_AgentDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_AgentDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>


#include "app/configuration/config_plugins/agent/src/resource.h"
#include "app/configuration/config_plugins/agent/src/ParameterListCtrl.h"

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
    class AgentDatabaseAccessor;
    class AgentTreeView;
    class ConfigAgent;


    class AgentDetailView : public AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
        AgentDetailView();


        /**
         * Destructor
         */
        virtual ~AgentDetailView();

        
        DECLARE_DYNCREATE(AgentDetailView)


    // Form Data
    public:
        //{{AFX_DATA(AgentDetailView)
	enum { IDD = IDD_AGENT_VIEW };
	CEdit	m_password;
	CEdit	m_priority;
	CEdit	m_controlTimeout;
	CEdit	m_monitorTimeout;
	CEdit	m_startupTimeout;
	CTransparentImage	 m_addressBitmap;
    CTransparentImage    m_nameBitmap;
    CTransparentImage    m_descriptionBitmap;
    CTransparentImage    m_executableBitmap;
    CTransparentImage    m_locationBitmap;
    CTransparentImage    m_usernameBitmap;
	CTransparentImage    m_passwordBitmap;
    CTransparentImage    m_lagBitmap;
	CEdit            	 m_subsystem;
    CEdit                m_name;
    CEdit                m_description;
    CEdit                m_executable;
    CEdit                m_address;
    CEdit                m_location;
    CButton              m_checkManaged;
    CButton              m_checkService;
    CButton              m_checkFailback;
    CEdit                m_terminateTimeout;
    CEdit                m_killTimeout;
    CEdit                m_startLag;
    CEdit                m_username;
    ParameterListCtrl    m_attributeList;
    CStatic              m_modifyDate;
    CStatic              m_createDate;
    CButton              m_status;
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


        /**
         * enableChildEntityDetails
         *
         * This will enable or disable all fields of the child entity
         */
        void enableChildEntityDetails(bool isEnabled);



    // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(AgentDetailView)
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
        //{{AFX_MSG(AgentDetailView)
        afx_msg void onKillfocusEditName();
        afx_msg void onKillfocusEditDescription();
        afx_msg void onKillfocusEditExecutable();
        afx_msg void onKillfocusEditAddress();
        afx_msg void onButtonLocation();
        afx_msg void onKillfocusEditUsername();
		afx_msg void onKillfocusEditPassword();
        afx_msg void onCheckManaged();
        afx_msg void onCheckService();
        afx_msg void onCheckFailback();
        afx_msg void onKillfocusEditTerminateTimeout();
        afx_msg void onKillfocusEditKillTimeout();
        afx_msg void onKillfocusEditStartLag();
        afx_msg void onButtonReset();
        afx_msg void onButtonApply();
	    afx_msg void onChangeEditName();
	    afx_msg void onKillfocusEditMonitorTimeout();
	    afx_msg void onKillfocusEditControlTimeout();
	    afx_msg void onKillfocusEditStartupTimeout();
		afx_msg void onKillfocusEditPriority();
		afx_msg void OnButtonPassword();
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
        AgentDetailView( const AgentDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        AgentDetailView& operator=(const AgentDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the Agent details. These controls will be
         * hidden until the user selects a Agent from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);


        /**
         * displayRelatedEntityDetails
         *
         * This retrieves the entity related to this Agent and populates the
         * bottom part of the view.
         *
         * @exception DataException
         * @exception DatabaseException
         */
        void displayRelatedEntityDetails();


    private:

        /**
         * setIsManaged
         *
         * This will disable or enable various fields when a process is changed between managed 
         * and not managed
         *
         * @param bool - This indicates if the process is managed or not
         */
        void setIsManaged(bool isManaged);


        /**
         * setIsService
         *
         * This will disable or enable various fields when a process is changed between a service 
         * and not.
         *
         * @param bool - This indicates if the process is a service or not
         */
        void setIsService(bool isService);


        // These match the restrictions in the database for how long certain fields can be.
        // Hopefully in the future these could be read from the database.
        static const int NAME_FIELD_LENGTH;
        static const int DESCRIPTION_FIELD_LENGTH;
        static const int EXECUTABLE_FIELD_LENGTH;
        static const int ADDRESS_FIELD_LENGTH;
        static const int USERNAME_FIELD_LENGTH;

        // The titles for the change dialog box
        static const CString CHANGE_LOCATION_DIALOG_TITLE;

        // These are the names of the entity parameters that we have fixed fields for
        static const std::string USERNAME;
		static const std::string PASSWORD;
        static const std::string IS_MANAGED;
        static const std::string IS_SERVICE;
        static const std::string AUTO_FAILBACK;
        static const std::string TERMINATE_TIMEOUT;
        static const std::string KILL_TIMEOUT;
        static const std::string START_LAG;
        static const std::string STARTUP_TIMEOUT;
        static const std::string CONTROL_TIMEOUT;
        static const std::string MONITOR_TIMEOUT;
		static const std::string PRIORITY;

        static const std::string TA_TRUE;
        static const std::string TA_FALSE;

		//TD18202 marc_bugfix
		static const std::string RPARAM_HIDEPASSWD;


        ConfigAgent* m_currentAgent; // The current agent the user is configuring.
                                     // This is the database object for it.

        AgentTreeView* m_treeView;  // This is the view in the left pane of the splitter
                                    // showing the list of all agents. We need to use this to
                                    // tell it when Agent details have changed.

        AgentDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve information
                                                     // from the database.


    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_AgentDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
