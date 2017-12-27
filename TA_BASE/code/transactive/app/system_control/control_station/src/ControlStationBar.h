/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/ControlStationBar.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class extends a list control. It is a list control where each of the icons
  * is a button. Each of those buttons is linked to an application that can be
  * launched by the user.
  * 
  * This is the class for the Control Station launch bar dialogue. It basically sets
  * up all the controls and then delegates commands when the buttons and controls are
  * used.
  */

#if !defined(AFX_CONTROLSTATIONBAR_H__36C85EB7_1ABF_4D1D_8C5F_E057E139EE1A__INCLUDED_)
#define AFX_CONTROLSTATIONBAR_H__36C85EB7_1ABF_4D1D_8C5F_E057E139EE1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ControlStationBar.h : header file
//

#include "app/system_control/control_station/src/ButtonListCtrl.h"
#include "app/system_control/control_station/src/BtnST.h"
#include "app/system_control/control_station/src/QuickToolComboEx.h"
#include "app/system_control/control_station/src/EditShortcutDialog.h"
#include "app/system_control/control_station/src/resource.h"
#include <vector>
#include "core\configuration_updates\src\IOnlineUpdatable.h"

// This is the message callback received when the user clicks on the Control
// Station icon in the task bar.
#define WM_NOTIFYICON (WM_USER + 112)

/////////////////////////////////////////////////////////////////////////////
// ControlStationBar dialog

namespace TA_Base_Core
{
    class IScadaDisplay;
}

namespace TA_Base_Bus
{
    class SettingsMgr;
}


class ControlStationBar :   public CDialog, 
                            public TA_Base_Core::IOnlineUpdatable
{
// Construction
public:

   /**
    * Constructor
    */
    ControlStationBar(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ControlStationBar)
	enum { IDD = IDD_LAUNCH_BAR };
	CComboBox	m_favouritesComboBox;
	CStatic	            m_dateTimeDetails;
	CButtonST	        m_displayButton5;
	CButtonST       	m_displayButton4;
	CButtonST       	m_displayButton3;
	CButtonST       	m_displayButton2;
	CButtonST         	m_displayButton1;
	CQuickToolComboEx	m_deviceCombo;
	CStatic	            m_userDetails;
	CButtonST	        m_launchCameraView;
	CButtonST	        m_launchSchematicDisplay;
	CButtonST       	m_logoutButton;
	CButtonST	        m_helpButton;
	CButtonST       	m_operatorOverrideButton;
	CButtonST       	m_changePasswordButton;
	CButtonST	        m_changeOperatorButton;
	CButtonST	        m_changeProfileButton;
	CButtonListCtrl	    m_launchList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ControlStationBar)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ControlStationBar)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelpButton();
	afx_msg void OnLogoutButton();
	afx_msg void OnOverrideButton();
	afx_msg void OnClose();
	afx_msg void OnControllerButton();
	afx_msg void OnDisplaySchematicButton();
	afx_msg void OnDisplayMapButton();
	afx_msg void OnViewCameraButton();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDisplayButton1();
	afx_msg void OnDisplayButton2();
	afx_msg void OnDisplayButton3();
	afx_msg void OnDisplayButton4();
	afx_msg void OnDisplayButton5();
	afx_msg void OnOperatorButton();
	afx_msg void OnProfileButton();
	afx_msg void OnPasswordButton();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnEditShortcutButton();
	afx_msg void OnViewShortcutButton();
	afx_msg void OnDblclkShortcutList();
	afx_msg void OnEditchangeFavouritesCombo();
	afx_msg void OnSelchangeFavouritesCombo();
	//}}AFX_MSG
    afx_msg LRESULT OnNotifyIcon(WPARAM,LPARAM);
	DECLARE_MESSAGE_MAP()

public:

    /**
    * loginDetailsChanged
    *
    * This is called when the login details change. This requires the launch bar
    * to refresh what it is displaying because different users and overrides have
    * different security permissions.
    */
    void loginDetailsChanged();


private:

    /**
    * initialiseViewDisplayButtons
    *
    * This sets up the 'View display on' buttons
    * It assigns icons to them and sets up their positions based on the number of
    * monitors attached to the console.
    */
    void initialiseViewDisplayButtons();

    
    /**
    * initialiseToolButton
    *
    * This sets up the tool buttons (such as help, logout, operator override).
    * It assigns icons to them and sets up their colour etc.
    */
    void initialiseToolButtons();


    /**
    * initialiseQuickLaunchButtons
    *
    * This sets up the quick launch buttons associated with the drop down
    * list of entities. 
    * It assigns icons to them and sets up their colour etc.
    */
    void initialiseQuickLaunchButtons();


    /**
    * initialiseShortcutButtons
    *
    * This sets up the shortcut buttons
    * 
    */
    void initialiseShortcutButtons();


    /**
    * refreshQuickToolsButtons
    *
    * Enables/disables the buttons in the quick tools base on rights of 
    * the current operator
    */
    void refreshQuickToolsButtons();

    /**
    * calculateDialogLayout
    *
    * The banner of the Control station must sit in a certain position on the screen.
    * This calculates where it must sit, an appropriate size and then moves and resizes
    * the window as appropriate.
    */
    void calculateDialogLayout();


    /**
    * addTaskBarIcon
    *
    * This will add an icon into the task bar for the Control Station. It will only add it
    * if it isn't already there (using the m_isTaskBarIconShowing member variable). It will
    * set the callback for this icon to be the WM_NOTIFYICON message
    */
    void addTaskBarIcon();


    /**
    * deleteTaskBarIcon
    *
    * This will delete the icon for the task bar for the Control Station. It will only delete it
    * if it is already there (using the m_isTaskBarIconShowing member variable).
    */
    void deleteTaskBarIcon();


    /**
    * launchDisplay
    *
    * This will call the GraphWorxManager and tell it to duplicate the GraphWorx display
    * on the specified screen.
    *
    * @param unsigned long - The screen number to launch on. This may be an invalid screen
    *                        so must be checked for validity.
    */
    void launchDisplay(unsigned long screenNumber);


    /**
     * loadShortcuts
     *
     * Gets the list of shortcuts from the user settings and add them to the
     * list box.
     * 
     */
    void loadShortcuts();


    /** 
      * unloadShortcuts
      *
      * Saves changes to the shortcut to the database and cleans up the shortcut lists.
      *
      */
    void unloadShortcuts();


    /** 
      * saveShortcuts
      *
      * Saves changes to the shortcut to the database
      *
      */
    void saveShortcuts();


    /** 
      * synchroniseShortcuts
      *
      * Synchronise the internal shortcut list with the one in m_editShortcutDialog
      *
      */
    void synchroniseShortcuts();


    /** 
      * refreshShortcutListBox
      *
      * Refreshes the shortcut list box
      *
      */
    void refreshShortcutListBox();


	/**
      * processUpdate
      *
	  * This tells the application that one of the objects it was interested in has
      * been updated and should be processed.
      *
	  * @param    updateEvent - This event contains all the information about the update
	  */
    virtual void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);


    // This indicates whether there is currently an icon in the task bar for the Control Station.
    bool m_isTaskBarIconShowing;

    // Dialog for editing the display shortcuts
    EditShortcutDialog m_editShortcutDlg; 

    // The shortcuts for the current session
    typedef std::vector<std::string> StringVector;      
    StringVector m_oldShortcuts;                        // shortcuts at the start of the session
    EditShortcutDialog::ShortcutVector m_shortcuts;     // shortcuts at the end of the session

    // The vector storing all displays
    typedef std::vector<TA_Base_Core::IScadaDisplay*> ScadaDisplayVector;
    ScadaDisplayVector m_allDisplays;

    TA_Base_Bus::SettingsMgr* m_settingsMgr;

    // This indicates how much the button colour should change when the mouse is moved over
    // them (ie they become hot).
    static const int BUTTON_COLOUR_OFFSET_WHEN_HOT;

    // This is the text to be used for the Operator Override tool button.
    static const CString APPLY_OVERRIDE_BUTTON_TEXT;
    static const CString APPLY_OVERRIDE_TOOLTIP_TEXT;
    static const CString REMOVE_OVERRIDE_BUTTON_TEXT;
    static const CString REMOVE_OVERRIDE_TOOLTIP_TEXT;

    // These are the formats of the text that will describe who is logged in. If the format is
    // changed and the placeholders (ie % items) are changed in any way then the place where this
    // constant is used must be changed as well.
    static const CString LOGIN_DETAILS_WHEN_OVERRIDE_FORMAT;
    static const CString LOGIN_DETAILS_WHEN_NO_OVERRIDE_FORMAT;

    // Format of the text to save into the USER_SETTINGS table.
    // Shortcut names are in the form of "Shortcut00", "Shortcut01", ..., "Shortcut19", "Shortcut<MAX_SHORTCUTS-1>"
    // We just need 2 digits for the number as having 100 shortcuts is more than enough
    // Shortcut values are in the form of "My new shortcut name\shortcut"
    static const CString SHORTCUT_SETTING_NAME_FORMAT;
    static const CString SHORTCUT_SETTING_VALUE_FORMAT;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLSTATIONBAR_H__36C85EB7_1ABF_4D1D_8C5F_E057E139EE1A__INCLUDED_)
