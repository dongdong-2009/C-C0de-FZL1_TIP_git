/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/ProfileLoginDlg.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This dialog serves three purposes. It is firstly used as a Login dialog that will 
  * be used each time the user wishes to log in to TransActive. Secondly, it is used 
  * as a  Change Profile dialog that is used by an operator to select a different 
  * profile. Finally, it is used as an Operator Override screen that is used to
  * augment the access rights of the logged in operator.
  */

#if !defined(AFX_ProfileLoginDlg_H__2B989BBB_F7AF_4DC7_A1D8_12D396D8F467__INCLUDED_)
#define AFX_ProfileLoginDlg_H__2B989BBB_F7AF_4DC7_A1D8_12D396D8F467__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app\system_control\control_station\src\resource.h"
#include "app/system_control/control_station/src/stdafx.h"
#include "app\system_control\control_station\src\ControlStationConstants.h"

#include <vector>
#include <string>

#include <boost/regex.hpp>

// Forward declare classes
namespace TA_Base_Core
{
    class IOperator;
    class IProfile;
    class ILocation;
}

/////////////////////////////////////////////////////////////////////////////
// ProfileLoginDlg dialog

class ProfileLoginDlg : public CDialog
{
    DECLARE_DYNAMIC(ProfileLoginDlg);

// Construction
public:

    /**
     * Constructor
     */
    ProfileLoginDlg();


    /**
     * Destructor
     */
    virtual ~ProfileLoginDlg();


    /** 
     * setDialogType
     *
     * This sets the dialog to a Login screen.
     *
     * @param EDialogMode - The type of dialog to set up.
     */
    void setDialogType(TA_Base_App::TA_ControlStation::EDialogMode mode)
    {
        m_mode = mode;
    }


    /**
    * activateWindow
    *
    * This activates the fields in the window so the user can click on them.
    * Before this is called all controls will be disabled
    */
    void activateWindow();

    
    /**
     * useExistingDataOnNextDoModal
     *
     * This method should be called if the next time the dialog is shown it should
     * automatically populate itself with the stored operator, profile and location
     * details. This will simply set the member variable m_useExistingData which will
     * be used in OnInitDialog.
     */
    void useExistingDataOnNextDoModal()
    {
        m_useExistingData = true;
    }


    /**
     * getSelectedDetails
     *
     * This is called to retrieve the details the user has entered into the login dialog
     * This should be called after the user has clicked Login or Apply.
     * 
     * @param IOperator*& - A reference to an IOperator pointer. This is because we want to update
     *                      the pointer passed in to be the same as the operator pointer we have
     *                      stored. (This is an OUT parameter)
     * @param ILocation*& - A reference to an ILocation pointer. This is because we want to update
     *                     the pointer passed in to be the same as the location pointer we have
     *                     stored. (This is an OUT parameter)
     * @param IProfile*& - A reference to an IProfile pointer. This is because we want to update
     *                     the pointer passed in to be the same as the profile pointer we have
     *                     stored. (This is an OUT parameter)
     * @param CString& - A reference to a CString so we can store the password that the user
     *                   entered.  (This is an OUT parameter)
     */
    void getSelectedDetails(TA_Base_Core::IOperator*& selectedOperator,
                            TA_Base_Core::IProfile*& selectedProfile,
                            TA_Base_Core::ILocation*& selectedLocation,
                            CString& password) const;


    /** 
      * getTerminateCode
      *
      * Returns the way that the control station should terminate -
      * exit, restart or shutdown (if there's some problem with the control station).
      * The caller should only call this method if the doModal method returns IDCANCEL
      *
      * @return the code for termination
      */
    TA_Base_App::TA_ControlStation::ETerminateMode getTerminateCode() const
    {
        return m_terminateCode;
    }


    // Dialog Data
    //{{AFX_DATA(ProfileLoginDlg)
    enum { IDD = IDD_ARIALBOLD_LOGIN_DIALOG };
    CButton             m_restartButton;
    CButton             m_cancelButton;
    CAnimateCtrl        m_introMovie;
    CButton             m_loginButton;
    CStatic             m_version;
    CComboBox           m_profileCombo;
    CEdit               m_passwordEdit;
    CComboBox           m_nameCombo;
    CComboBox           m_locationCombo;
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(ProfileLoginDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
    afx_msg LRESULT OnCancel(WPARAM, LPARAM);

// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    //{{AFX_MSG(ProfileLoginDlg)
    virtual BOOL OnInitDialog();
    virtual void onExit();
    afx_msg void onSelchangeNameCombo();
    afx_msg void onSelchangeProfileCombo();
    afx_msg void onChangePasswordEdit();
    afx_msg void onLoginButton();
    afx_msg void onHelpButton();
    afx_msg void onRestartButton();
    afx_msg void onSelchangeLocationCombo();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void onDropdownNameCombo();
    afx_msg void onDropdownProfileCombo();
    afx_msg void onDropdownLocationCombo();
    //}}AFX_MSG
    afx_msg LRESULT onSystemControllerStateChange(WPARAM, LPARAM);
    DECLARE_MESSAGE_MAP()

private:
	/**
	* check whether the profile_str is tds profile;
	*/
	bool isTdsProfile(std::string profile_str);

    /**
     * populateNameComboBox
     *
     * This will populate the operator name combo box with a list of operator descriptions
     * retrieved from the SessionDetails object.
     */
    void populateNameComboBox();


    /**
     * populateLocationComboBox
     *
     * This will populate the location combo box with a list of location names
     * retrieved from the SessionDetails object.
     */
    void populateLocationComboBox();


    /**
     * populateProfileComboBox
     *
     * This will populate the profile combo box with a list of profile names retrieved
     * from the SessionDetails object. This needs to be called everytime the selected name
     * changes as the list of profiles is based on the operator selected.
     */
    void populateProfileComboBox();


    /**
     * refreshButtonAvailability
     *
     * This will refresh the Login button depending on what the user has entered into the
     * dialogue. The Login button will not be available until the operator has selected an
     * operator, profile and entered a password.
     */
    void refreshButtonAvailability();


    /** 
      * hasFilledAllDetails
      *
      * Returns true if all fields have been filled
      *
      * @return true if all fields have been filled
      */
    bool hasFilledAllDetails();

    
    /** 
     * setToLongDialog
     *
     * This sets the dialog to a Login screen.
     *
     */
    void setToLoginDialog();


    /** 
     * setToChangeProfileDialog
     *
     * This sets the dialog to a Change Profile screen.
     *
     */
    void setToChangeProfileDialog();


    /** 
     * setToOperatorOverrideDialog
     *
     * This sets the dialog to an Operator Override screen.
     *
     */
    void setToOperatorOverrideDialog();


    /** 
     * setToChangeOperatorDialog
     *
     * This sets the dialog to a Change Operator screen.
     *
     */
    void setToChangeOperatorDialog();


    /** 
      * drawControls
      *
      * Draws controls specific to the mode that the dialog is in
      *
      */
    void drawControls();


    /** 
      * areDetailsValid
      *
      * Returns true if the chosen operator, profile and location are still valid
      *
      *
      * @return true if the chosen operator, profile and location are still valid
      */
    bool areDetailsValid();

	static std::string USE_TDS_PROFILE;
	static boost::regex TDS_PATTERN;
    // These member variables hold the user's current selections from the dialog.
    TA_Base_Core::IOperator* m_selectedOperator;
    TA_Base_Core::IProfile*  m_selectedProfile;
    TA_Base_Core::ILocation*  m_selectedLocation;
    CString m_password;

	std::string		m_dbConnectionString;
	std::string		m_dbConnection;
    // The dialog's current mode. ie Is it a login dialog or an operator override dialog.
    TA_Base_App::TA_ControlStation::EDialogMode m_mode;

    // This indicates if the window has been activated. The first time the dialog is
    // displayed to the user it is not active until full initialisation has been
    // completed. After this point this will be set to true.
    bool m_isWindowActivated;

    // When the dialog is displayed to the user it may  need to be displayed using the 
    // existing data stored in the m_selectedOperator and m_selectedSite members. (not password)
    // This may be because the user chose 'Login' and the login failed. We then
    // want to display the dialog to them again with their previously entered details so they can
    // just re-enter their password if necessary.
    bool m_useExistingData;

    // Exit/Restart/Shutdown
    TA_Base_App::TA_ControlStation::ETerminateMode m_terminateCode;

    // controls the restart button - button will always be enabled if this variable is true
    // if false, then all details have to be filled in before operator can restart
    bool m_isRestartAlwaysEnabled;

    // controls the exit button - button will always be enabled if this variable is true
    // if false, then all details have to be filled in before operator can exit
    bool m_isExitAlwaysEnabled;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ProfileLoginDlg_H__2B989BBB_F7AF_4DC7_A1D8_12D396D8F467__INCLUDED_)
