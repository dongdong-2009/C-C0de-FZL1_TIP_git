/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/TA_BASE/transactive/app/system_control/system_manager/src/SystemManager.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 12:11:21 $
  * Last modified by:  $Author: builder $
  *
  * This is the main class for the System Manager.
  * It does not inherit from TransactiveWinApp, as
  * the System Manager must be able to run without
  * a Control Station. Instead it inherits directly
  * from CWinApp and re-implements some of the
  * TransactiveWinApp code.
  *
  */

#if !defined(AFX_SYSTEMMANAGER_H__DDF277E9_975E_4C52_8E47_A00562AE406D__INCLUDED_)
#define AFX_SYSTEMMANAGER_H__DDF277E9_975E_4C52_8E47_A00562AE406D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "Resource.h"		// Main symbols
#include "bus/generic_gui/src/TransActiveWinApp.h"
#include "SystemManagerGUI.h"
#include "SessionHandler.h"

namespace TA_Base_Bus
{
    class GenericGUI; // Forward declaration
}

namespace TA_Base_App
{

    class SystemManager : public TA_Base_Bus::TransActiveWinApp
    {
    public:
	    SystemManager();

	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(SystemManager)
	    public:
	    virtual BOOL InitInstance();
	    //}}AFX_VIRTUAL
		
        /**
         * ExitInstance
         *
         * This method shuts down the application and provides final cleanup. This will call the
         * base member to perform any TransActive cleanup required.
         */
        virtual int ExitInstance();

	    //{{AFX_MSG(SystemManager)
		    // NOTE - the ClassWizard will add and remove member functions here.
		    //    DO NOT EDIT what you see in these blocks of generated code !
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

		virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);

    private:

        // Copy constructor and operator= made private to prevent them being used.
        SystemManager(const SystemManager&);
        SystemManager& operator=(const SystemManager&);

        /**
          * runWithoutControlStation
          *
          * If the System Manager cannot connect to the Control Station then it must attempt to
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
          * retrieveConsoleName
          *
          * Retrieves the console name from the database, if possible. If it isn't, returns the hostname
          * of the machine instead.
          *
          * @return  The console name as a string.
          */
        std::string retrieveConsoleName();


        /**
          * The instance of GenericGUI we use to attempt to connect to 
          * the Control Station.
          */
        TA_Base_Bus::GenericGUI* m_genericGUI;       

        /**
          * This is used to startup the GUI components.
          */
        SystemManagerGUI m_gui;              

        /**
          * The instance of the session handler we use to connect
          * to the system controller and/or database
          */
        SessionHandler* m_sessionHandler;  
 
        /**
          * This is used temporarily when setting the application
          * name in RunParams.
          */
        CString m_applicationName;  

        std::string m_consoleName;

    };

} // namespace TA_Base_App

#endif // !defined(AFX_SYSTEMMANAGER_H__DDF277E9_975E_4C52_8E47_A00562AE406D__INCLUDED_)
