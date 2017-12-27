/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/TA_BASE/transactive/app/system_control/system_manager/src/SystemManagerDlg.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 12:11:21 $
  * Last modified by:  $Author: builder $
  *
  * SystemManagerDlg is the class associated with the application's main window.
  * This class inherits from AdminObserver. The main window registers itself 
  * as an observer with the AdminManager.
  *
  */

#if !defined(AFX_SystemManagerDlg_H__4CCDF427_2C84_11D5_B140_0050BAB0C7E8__INCLUDED_)
#define AFX_SystemManagerDlg_H__4CCDF427_2C84_11D5_B140_0050BAB0C7E8__INCLUDED_

#include "StdAfx.h"
#include "Resource.h"
#include "ProcessListCtrl.h"
#include "HostsTabCtrl.h"
#include "StartProcessDlg.h"
#include "OptionsDlg.h"
#include "AdminObserver.h"
#include "SystemControllerDlg.h"

#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/generic_gui/src/IGUIAccess.h"
#include "bus\generic_gui/src/TransActiveDialog.h"
#include "core/utilities/src/RunParams.h"

#include <memory>
#include <string>
#include <vector>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_App
{
    class SystemManagerDlg : public TA_Base_Bus::TransActiveDialog, public AdminObserver, public TA_Base_Core::RunParamUser
    {

    public:

        SystemManagerDlg(TA_Base_Bus::IGUIAccess* gui);

        //{{AFX_DATA(SystemManagerDlg)
	    enum { IDD = IDD_SYSTEMMANAGER_DIALOG };
	    HostsTabCtrl m_tabHosts;
		CStatic	m_menuLine;
	    //}}AFX_DATA

        AdminManager* getActiveAdminManagerPtr();

		//TD18095, jianghp, to fix the performance of showing manager application
		virtual void init();
        /**
          * OnNewData
          * 
          * Calls the getProcesses() function of the AdminManager to retrieve the 
          * up-to-date process data.
          */
        virtual void onNewData( AdminManager* );

        /**
          * onRunParamChange
          *
          * Used to respond to Session ID changes.
          *
          * @param name   The name of the RunParam that has changed.
          * @param value  The new value of the RunParam.
          */
   		virtual void onRunParamChange(const std::string& name, const std::string& value);


    private:

        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(SystemManagerDlg)
	protected:
        virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	    //}}AFX_VIRTUAL

        /**
          * logout
          * 
          * Terminates the current System Manager session.
          */
        void logout();

        /**
          * setupMenu
          * 
          * Enables/disables menu items based on whether an operator is logged in or out.
          *
          * @param loggedIn  True if an operator is logged in, and false otherwise.
          */
        void setupMenu(bool loggedIn);

        /**
          * setupToolbar
          * 
          * Enables/Disables toolbar buttons based on whether an operator is logged in 
          * or out.
          *
          * @param loggedIn  True if an operator is logged in, and false otherwise.
          */
        void setupToolbar(bool loggedIn);

        /**
          * initToolbar
          * 
          * Sets the text for each toolbar button.
          *
          */
        void initToolbar();

        /**
          * setLoggedInMode
          * 
          * Sets the AdminClient mode (Logged in / Logged out) and enables or disables
          * the action buttons depending on that state.
          *
          * @param loggedIn  True if an operator is logged in, and false otherwise.
          */
        void setLoggedInMode(bool loggedIn);
    
        /**
          * resizeChildWindows
          * 
          * Positions and resizes the child windows (process list control, status bar)
          * according to the size of the main client.     
          *
          * @param width   The new width the client area.
          * @param height  The new height the client area.
          */
	    void resizeChildWindows(int width, int height);

        /** 
          * onShutdownSystemController
          *
          * Message handler for the WM_SHUTDOWN_SYSTEM_CONTROLLER message sent
          * when a request to shutdown a System Controller is made.
          *
          */
        afx_msg LRESULT onShutdownSystemController(WPARAM wparam, LPARAM lparam);

		/**
		  * onNewProcessData
		  *
		  * Message handler for the WM_NEW_PROCESS_DATA message sent when there
		  * is new process data.
		  */
		afx_msg LRESULT onNewProcessData(WPARAM wparam, LPARAM lparam);

		/**
		  * onNewProcessData
		  *
		  * Message handler for the WM_NEW_PROCESS_DATA message sent when there
		  * is new process data.
		  */
		afx_msg LRESULT onTabSelChanged(WPARAM wparam, LPARAM lparam);

		/**
		  * onNewProcessData
		  *
		  * Message handler for the WM_NEW_PROCESS_DATA message sent when there
		  * is new process data.
		  */
		afx_msg LRESULT onProcessListSelChanged(WPARAM wparam, LPARAM lparam);

		/**
		  * onNewProcessData
		  *
		  * Message handler for the WM_NEW_PROCESS_DATA message sent when there
		  * is new process data.
		  */
		afx_msg LRESULT onListContexMenu(WPARAM wparam, LPARAM lparam);

		/**
		  * onNewProcessData
		  *
		  * Message handler for the WM_NEW_PROCESS_DATA message sent when there
		  * is new process data.
		  */
		afx_msg LRESULT onHostLoginDone(WPARAM wparam, LPARAM lparam);

        /**
          * getSelectedProcesses
          *
          * Returns the name of all processes that have been selected in the dialog
          * by the operator.
          */
        std::vector<std::string> getSelectedProcesses();

        /**
          * getSelectedProcessesWithState
          *
          * Returns the name of all processes that have been selected in the dialog
          * by the operator and have the specified state. It also checks the managed and
          * services criteria.
          *
          * @param state  The states of the processes to return.
          * @param ignoreServices This indicates if service process should be included or not.
          *                       True indicates services will be excluded. Default is to include them
          * @param ignoreNonManaged This indicates if non-managed process should be included or not.
          *                         True indicates non-managed processes will be excluded. Default
          *                         is to include them.
          */
        std::vector<std::string> getSelectedProcessesWithState(const std::vector<std::string>& state,
                                                               bool ignoreServices = false,
                                                               bool ignoreNonManaged = false);

        /**
          * checkPermissions
          *
          * Sets the m_is...Allowed boolean member variables by checking the available
          * rights from the rights library.
          */
        void checkPermissions();

        void updateStatusMsg( int nLoginResult );

        void setupLoginLogoutToolbarMenu( bool bLogined );

		void onItemchangedMainProcesslist(NMHDR* pNMHDR, LRESULT* pResult);

        void enableRetryAllButton( bool bEnable = true );

public:
		
        static const std::string RUNNING_CONTROL;
        static const std::string RUNNING_MONITOR;
        static const std::string GOING_TO_CONTROL;
        static const std::string GOING_TO_MONITOR;
        static const std::string STARTUP;
        static const std::string STOPPED;
        static const std::string NOT_RUNNING;
        static const std::string TERMINATING;
        static const std::string NOT_STARTED;
        static const std::string RUNNING; // For non managed processes
        static const std::string NOT_APPLICABLE;
        static const std::string CONTROL;
        static const std::string MONITOR;

private:
        // Generated message map functions

        //{{AFX_MSG(SystemManagerDlg)
        virtual BOOL OnInitDialog();
        afx_msg void OnPaint();
        afx_msg HCURSOR OnQueryDragIcon();
        afx_msg void OnLogin();
        afx_msg void OnLogout();
        afx_msg void OnOK() {};
        afx_msg void OnStartProcess();
        afx_msg void OnDebugLevel();
        afx_msg void OnChangeProcessMode();
        afx_msg void OnOptions();
        afx_msg void OnStopProcess();
        afx_msg void OnShutdownMonitor();
        afx_msg void OnRefresh();
        afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	    afx_msg void OnSize(UINT nType, int cx, int cy);
	    afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
        afx_msg void OnAbout();
	    afx_msg void OnButtonLogin();
        afx_msg void OnButtonLogout();
	    afx_msg void OnButtonStartProcess();
	    afx_msg void OnButtonStopProcess();
	    afx_msg void OnButtonRefresh();
		afx_msg void OnClose();
		afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	    afx_msg void OnButtonRetryall();
	//}}AFX_MSG
	    BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);

        DECLARE_MESSAGE_MAP()

        HICON m_hIcon;

        TA_Base_Bus::IGUIAccess* m_gui;

        /**
          * True if the user is logged in.
          */
        bool m_userLoggedIn;

        /**
          * A pointer to the child window created by this class.
          */
        CDialog* m_currentChildWindow;

        /**
          * A pointer to the login dialog.
          */
        SystemControllerDlg* m_sysControllerDialogBox;

        std::auto_ptr<TA_Base_Bus::RightsLibrary> m_rightsLibrary;

        /**
          * The toolbar.
          */
        CToolBar m_toolBar;

        unsigned long m_ulLoginEntityKey;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_SystemManagerDlg_H__4CCDF427_2C84_11D5_B140_0050BAB0C7E8__INCLUDED_)

