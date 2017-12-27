/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ConfigurationEditor.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the application starting point. This is the first class that is used (as far as
  * we are concerned). This sets up GenericGUI etc. This does not inherit from
  * TransActiveWinApp as it must handle exceptions differently.
  */


#if !defined(AFX_CONFIGURATION_EDITOR_H__A05580F7_39A8_4098_9B49_9E27CC0F3949__INCLUDED_)
#define AFX_CONFIGURATION_EDITOR_H__A05580F7_39A8_4098_9B49_9E27CC0F3949__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "bus/generic_gui/src/TransActiveWinApp.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorGUI.h"

namespace TA_Base_Bus
{
    class GenericGUI;
}

namespace TA_Base_App
{
    class SessionHandler;

    class CConfigurationEditorApp : public TA_Base_Bus::TransActiveWinApp
    {
    public:
        /**
          * Constructor
          */
	    CConfigurationEditorApp();

        /**
          * Destructor
          */
        virtual ~CConfigurationEditorApp(){ };

        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CConfigurationEditorApp)
	    public:
	    //}}AFX_VIRTUAL


        /**
          * WinHelp
          *
          * This overrides the TransActiveWinApp version. For this application different help needs
          * to be displayed based on the current component view. This is not handled by the standard
          * command context specific help so we must retrieve configured help files and use them instead.
          */
		virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);


        /**
          * InitInstance
          *
          * This method starts up the application and all the windows. This does NOT call a base version
          * of this method.
          */
        virtual BOOL InitInstance();


        /**
          * ExitInstance
          *
          * This method shuts down the application and provides final cleanup. This will call the
          * base member to perform any TransActive cleanup required.
          */
        virtual int ExitInstance();


    // Implementation
	    //{{AFX_MSG(CConfigurationEditorApp)
		    // NOTE - the ClassWizard will add and remove member functions here.
		    //    DO NOT EDIT what you see in these blocks of generated code !
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    private:

        /**
          * Copy Constructor
          *
          * Hidden as a private method so it is not used.
          */
        CConfigurationEditorApp( const CConfigurationEditorApp&);


        /**
          * Operator =
          *
          * Hidden as a private method so it is not used.
          */
        CConfigurationEditorApp& operator=(const CConfigurationEditorApp&);


        /**
          * runWithoutControlStation
          *
          * If the Configuration Editor cannot connect to the Control Station then it must attempt to
          * run via other means. This method attempts to achieve this.
          *
          * @return This returns a boolean indicating whether the application should enter
          * the application message pump:
          *    True for Yes
          *    False for No
          */
        bool runWithoutControlStation();


        /** 
          * submitAppStartedAuditMessage
          *
          * Submit an audit message with the name of the console on which this application is running
          *
          */
        void submitAppStartedAuditMessage();


        /** 
          * setDBConnectionString
          *
          * Set the database connection string to be used by Configuration Editor 
          *
          */
		void setDBConnectionString();
        std::string getOperatorNameFromSession();
        bool localTest();

    private:

		static const std::string CONFIGURATIONEDITOR_APPNAME;
		static const std::string CONFIGCONNECTIONFILE_NAME;



        ConfigurationEditorGUI   m_gui;              // The GUI class used to interface between GenericGUI and our
                                                     // application.

        TA_Base_Bus::GenericGUI* m_genericGUI;       // The instance of GenericGUI we use to attempt
                                                     // to connect to the Control Station
        SessionHandler*          m_sessionHandler;   // The instance of the session handler we use to connect
                                                     // to the system controller and/or database

        CString                  m_applicationName;  // This is used temporarily when setting the application
                                                     // name in RunParams.
        bool                     m_isAlreadyRunning; // True if the Config Editor is already running.
    };
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGURATION_EDITOR_H__A05580F7_39A8_4098_9B49_9E27CC0F3949__INCLUDED_)
