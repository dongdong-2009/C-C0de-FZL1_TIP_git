/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/TransActiveLoginDlg.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This is the dialog used to allow the user to enter their login details. IT will then attempt to authenticate
  */

#if !defined(AFX_TRANSACTIVELOGINDLG_H__E4B1DCC7_7AC0_48B4_91D3_8D3C199ED05F__INCLUDED_)
#define AFX_TRANSACTIVELOGINDLG_H__E4B1DCC7_7AC0_48B4_91D3_8D3C199ED05F__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <map>
#include "app/configuration/configuration_editor/src/resource.h"

namespace TA_Base_Core
{
    class IItem;
    class AuthenticationLibrary;
    class IOperator;
    class IProfile;
    class ILocation;
}

namespace TA_Base_App
{
    class ISessionChecker;

    class TransActiveLoginDlg : public CDialog
    {
        // Construction
    public:
        /**
          * Constructor
          *
          * @param ISessionChecker* - The object to use to create the session with and check permissions
          */
        TransActiveLoginDlg(ISessionChecker& sessionChecker);   // standard constructor

        /**
          * ~TransActiveLoginDlg
          *
          * Deletes the cached operator, profile and location objects
          *
          */
        ~TransActiveLoginDlg();

        // Dialog Data
        //{{AFX_DATA(TransActiveLoginDlg)
        enum { IDD = IDD_LOGIN_DIALOG };
        CComboBox	m_profileCombo;
        CEdit	m_passwordEdit;
        CComboBox	m_nameCombo;
        CComboBox	m_locationCombo;
        //}}AFX_DATA

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(TransActiveLoginDlg)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

        // Implementation
    protected:
        HICON m_hIcon;

        // Generated message map functions
        //{{AFX_MSG(TransActiveLoginDlg)
        afx_msg void onOK();
        afx_msg void onSelchangeNameCombo();
        afx_msg void onSelchangeProfileCombo();
        afx_msg void onSelchangeLocationCombo();
        virtual BOOL OnInitDialog();
        afx_msg void onChangePasswordEdit();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        typedef std::vector<TA_Base_Core::IOperator*> OperatorVector;
        typedef std::vector<TA_Base_Core::IProfile*> ProfileVector;
        typedef std::vector<TA_Base_Core::ILocation*> LocationVector;

        /**
          * refresh
          *
          * This checks the status of the controls and enables or disables them
          */
        void refresh();

        /**
          * populateOperatorCombo
          *
          * This will retrieve all operators from the database and populate the combo box
          * with their descriptions.
          */
        void populateOperatorCombo();

        /**
          * populateLocationCombo
          *
          * This will retrieve all locations from the database and populate the combo box
          * with their descriptions.
          */
        void populateLocationCombo();

        /**
          * populateProfileCombo
          *
          * This will retrieve all profiles for the currently selected operator from the database
          * and populate the combo box with their names.
          */
        void populateProfileCombo();

        /**
          * getListOfProfiles
          *
          * PreCondition: The operator passed is not NULL.
          *
          * @param IOperator* - The list of profiles is based on the chosen operator. This is the
          *                     operator to load the profiles for.
          * @param onlyOverridable - If this is true, it will only return the ones that are overridable
          *                    by the current login profile.  Otherwise, it will return all profiles.
          *
          * @return ProfileVector - This vector contains all the profiles in the database available
          *                          for the specified operator.  Only returns the ones that are overridable
          *                          by the operator if the onlyOverridable is set to true.
          */
        ProfileVector getListOfProfiles(TA_Base_Core::IOperator* theOperator);

        /**
          * getListOfLocations
          *
          * PreCondition: The profile passed is not NULL.
          *
          * @param IPorilfe* - The list of locations is based on the chosen profile. This is the
          *                    profile to load the locations for.
          * @return ProfileVector - This vector contains all the locations in the database available
          *                          for the specified profile.
          */
        LocationVector getListOfLocations(TA_Base_Core::IProfile* theProfile);

        /**
          * retrieveConsoleInfo
          *
          * This will retrieve the console key and location description for this
          * machine from the database.
          *
          * @exception CannotUseSysControllerException - This indicates that the configuration for the console
          *                                              could not be found in the database
          * @exception GenericGUIException - This indicates that the actual hostname of the machine could not
          *                                  be found. This exception is unlikely
          */
        void retrieveConsoleInfo();

        ISessionChecker& m_sessionChecker; // This will be used to create a session for the user and check they
        // have permission to run the application

        unsigned long m_consoleKey; // The key for this console. We retrieve this early as there is no
        // point us continuing if it is not in the database.

        std::string m_locationDescription; // The description of this console's location.

        // These member variables hold the user's current selections from the dialog.
        TA_Base_Core::IOperator* m_selectedOperator;
        TA_Base_Core::IProfile*  m_selectedProfile;

        typedef std::map<unsigned long, TA_Base_Core::ILocation*> LocationMap;
        typedef std::map<unsigned long, TA_Base_Core::IProfile*> ProfileMap;

        // This is a list of all operators, profiles and locations.
        OperatorVector      m_allOperators;
        ProfileMap          m_profileMap;
        LocationMap         m_locationMap;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_TRANSACTIVELOGINDLG_H__E4B1DCC7_7AC0_48B4_91D3_8D3C199ED05F__INCLUDED_)
