/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/ControlStationRootDialog.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is an extension of CDialog and is the root dialog and is responsible
  * for displaying the appropriate dialog boxes, messages and GraphWorX displays.
  */

#if !defined(AFX_CONTROLSTATIONROOTDIALOG_H__8A7CBCC8_9B51_4AA8_B329_0602AA61207A__INCLUDED_)
#define AFX_CONTROLSTATIONROOTDIALOG_H__8A7CBCC8_9B51_4AA8_B329_0602AA61207A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ControlStationRootDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ControlStationRootDialog dialog

#include "app/system_control/control_station/src/stdafx.h"
#include <winuser.h>

#include "app/system_control/control_Station/src/ProfileLoginDlg.h"
#include "app/system_control/control_Station/src/PasswordDlg.h"
#include "app/system_control/control_Station/src/LogoutDlg.h"

// TD19075 ++
#include "app\system_control\control_station\src\DutyChangeNoteDlg.h"
// TD19075 ++

#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/threads/src/IWorkItem.h"
#include <map>

class CTaskManagerDlg;
class CControlStation2;
class CSchematicDisplay;

// Forward declarations
namespace TA_Base_Core
{
    class NonReEntrantThreadLockable;
	class ReEntrantThreadLockable;
}

namespace TA_Base_Bus
{
    class SoundManagerProxy;    
}

class ControlStationBar; // Forward declaration


// structure for display related information
struct DisplayInformation
{
    CString display;     // The name of the display
    CString asset;       // The identifier located on the display that is centred on the the screen
    long position;       // The position on the screen that this display is displayed on 
                         // (the display will take up the entire screen area where this point is located)
};

// Structure for a GX target command message
struct TAToGxMessageInformation
{
	CString targetSessionID;
	CString targetSchematicID;
	CString sourceEntityName;
	CString message;
};

typedef std::map<std::string,DWORD> TerminationResults;

class ControlStationRootDialog : public CDialog
{
public:

    /**
     * getInstance
     *
     * This method makes this class a singleton. This should be called to retrieve an instance
     * of the class.
     */
    static ControlStationRootDialog& getInstance();


    /**
     * Destructor
     */
    virtual ~ControlStationRootDialog();


    /** 
     * setControlStationComObject
     *
     * This sets the Control Station COM object that can be used
     * to send events. Because this dialog is the main thread of the application
     * this must be used to send the COM events. Therefore we need a pointer to
     * the object that can send the events so we can initiate it.
     *
     */
    void setControlStationComObject(CControlStation2* obj)
    {
        m_comControlStation = obj;
    }


    /** 
     * setRippleControlStationComObject
     *
     * This sets the second Control Station COM object that will be used
     * by the third party clients.  The control station will only send login
     * details changed event to these clients.
     *
     */
    void setRippleControlStationComObject(CSchematicDisplay* obj)
    {
        m_comRippleControlStation = obj;
    }


    /**
     * initialisationComplete
     *
     * This should be called once the Session Manager has finished all initialisation
     * so that the login dialogue can be activated for the user.
     *
     * @param bool - This indicates if the initialisation was successful. It if was not
     *               successful then this will be false.
     */
    void initialisationComplete(bool wasSuccessful);

    bool hasInitDialog() { return m_hasInitDialog; }

	void fireDutyChanged();
	inline void postDutyChanged();

	//{{AFX_DATA(ControlStationRootDialog)
	enum { IDD = IDD_DUMMY_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


    // Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ControlStationRootDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

    /**
     * OnOK
     *
     * Override this with empty method as we don't want the control station
     * to end whenever the user hits the enter key
     */
    virtual void OnOK() {}


    /**
     * OnCancel
     *
     * Override this with empty method as we don't want the control station
     * to end whenever the user hits the escape key
     */
    virtual void OnCancel() {}


    /** 
      * referenceDialog
      *
      * References the child dialog
      *
      * @param dialog Child dialog
      */
    void referenceDialog(CWnd& dialog);


    /** 
      * cancelChildDialogs
      *
      * Calls the OnCancel method of all child dialogs
      *
      */
    void cancelChildDialogs();


    unsigned long m_numberOfMonitors;    // This is the number of monitors the machine has   

	// Generated message map functions
	//{{AFX_MSG(ControlStationRootDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnClose();
	//}}AFX_MSG
    afx_msg LRESULT onLaunchDisplay(WPARAM display, LPARAM);
    afx_msg LRESULT onServerStateChange(WPARAM isContactable, LPARAM);
    afx_msg LRESULT onSystemControllerStateChange(WPARAM isOnline, LPARAM group);
    afx_msg LRESULT onDutyChange(WPARAM dutyNotificationDetail, LPARAM);
    afx_msg LRESULT onDutyRequest(WPARAM dutyRequestDetail, LPARAM);
    afx_msg LRESULT onDutyResponse(WPARAM uniqueId, LPARAM response);
    afx_msg LRESULT onShowTaskManager(WPARAM, LPARAM);
    afx_msg LRESULT onHideGraphWorxDisplays(WPARAM, LPARAM);
    afx_msg LRESULT onCreateDialogs(WPARAM,LPARAM);
    afx_msg LRESULT onDisplayLoginDialogue(WPARAM, LPARAM);
    afx_msg LRESULT onDisplayChangeProfileDialogue(WPARAM, LPARAM);
    afx_msg LRESULT onDisplayOperatorOverrideDialogue(WPARAM, LPARAM);
    afx_msg LRESULT onDisplayChangeOperatorDialogue(WPARAM, LPARAM);
    afx_msg LRESULT onDisplayChangePasswordDialogue(WPARAM, LPARAM);
    afx_msg LRESULT onLogout(WPARAM,LPARAM);
    afx_msg LRESULT onForceLogout(WPARAM,LPARAM);
    afx_msg LRESULT onForceEndOverride(WPARAM,LPARAM);
    afx_msg LRESULT onLoginDetailsChanged(WPARAM,LPARAM);
    afx_msg LRESULT dereferenceDialog(WPARAM dialog,LPARAM);
    afx_msg LRESULT displayLaunchApplicationError(WPARAM,LPARAM);
    afx_msg LRESULT onTAToGXMessage(WPARAM pMsg,LPARAM);
	afx_msg LRESULT onGXToTAMessage(WPARAM pMsg,LPARAM); //DP-changes
	DECLARE_MESSAGE_MAP()

private:

    //
    //Hide constructors and operator= as this is a singleton
    //
	ControlStationRootDialog();
    ControlStationRootDialog(ControlStationRootDialog& );
    ControlStationRootDialog& operator=(const ControlStationRootDialog&);


    /**
     * loginDetailsChanged
     *
     * This is called to update all necessary components (ie ControlStationBar, GraphWorX)
     * that the login details have changed a refresh is required.
     */
    void loginDetailsChanged();


    /** 
      * exit
      *
      * Exits from the control station.
      * 
      * @param isRestart True if the control station needs to restart the workstation upon termination
      *
      */
    void exit(bool isRestart = false);


    /** 
      * logout
      *
      * Terminates the current session
      *
      */
    void logout();


private:

    // This is the task manager that developers can use to get greater control over
    // their applications that are launched by the Control Station.
    CTaskManagerDlg*  m_taskManager;

    // This is the Control Station COM object that will be used to fire events
    // to the clients.
    CControlStation2* m_comControlStation;

    // This is a Control Station COM object that will be used mainly by third party
    // clients to get relevant information to display the right schematics.  
    // It will only get the login details change event.
    CSchematicDisplay* m_comRippleControlStation;

    // This is the Control Station launch bar that is displayed down the side of the screen.
    // This will only be created if the launch bar is requird.
    ControlStationBar* m_launchBar;

    // The main dialog for logging in and changing profile. This will need to be displayed
    // several times through the life of the application and so must be stored.
    ProfileLoginDlg m_loginDialog;

	// dialog for changing password for the current operator
    CPasswordDlg m_passwordDialog;

    // dialog for logging out
    LogoutDlg m_logoutDialog;

    // vector for storing the references to child dialogs
    std::vector<CWnd*> m_childDialog;

    static ControlStationRootDialog* s_instance;

	// protect singleton creation
	static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

    // Message that is sent to GraphWorx to tell it to disable itself to prevent 
    // operator from selecting from the GraphWorx menu when the logout dialog
    // is being displayed
    static const CString MENU_SUSPEND_MESG;

    // Message that is sent to GraphWorx to tell it to enable itself as operator
    // has just cancelled the logout dialog
    static const CString MENU_RESUME_MESG;

    // Message that is sent to GraphWorx to tell it to change the colour of the
    // operator/profile panel to indicate the system is in degraded mode.
    static const CString SYSTEM_DEGRADED_MESG;

    // Message that is sent to GraphWorx to tell it to change the colour of the 
    // operator/profile panel to indicate the system is in normal mode.
    static const CString SYSTEM_NORMAL_MESG;

	//run param to enable or disable the screensaver
	static const std::string RPARAM_USE_SCREENSAVER;

	// Pointer to the sound interface    
	TA_Base_Bus::SoundManagerProxy* m_soundInterface;

	volatile bool m_hasInitDialog; //whether call onInitDialog

	TA_Base_Core::ThreadPoolSingletonManager * m_threadPool;
};

struct DutyChangeWorkItem: public TA_Base_Core::IWorkItem
{
    DutyChangeWorkItem(ControlStationRootDialog* dialog) : m_dialog(dialog) {}
    virtual void executeWorkItem() { m_dialog->fireDutyChanged(); }
	ControlStationRootDialog* m_dialog;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLSTATIONROOTDIALOG_H__8A7CBCC8_9B51_4AA8_B329_0602AA61207A__INCLUDED_)
