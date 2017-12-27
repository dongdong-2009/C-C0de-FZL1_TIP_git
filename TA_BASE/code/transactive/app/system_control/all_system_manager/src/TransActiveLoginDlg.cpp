/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/TA_BASE/transactive/app/system_control/system_manager/src/TransActiveLoginDlg.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 12:11:21 $
  * Last modified by:  $Author: builder $
  * 
  * This is the dialog used to allow the user to enter their login details. IT will then attempt to authenticate
  */

#pragma warning(disable:4786)

#include "StdAfx.h"
#include "TransActiveLoginDlg.h"
#include "ISessionChecker.h"

#include <algorithm>

#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"

#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/EntityTypeException.h"
#include "core/exceptions/src/AuthenticationSecurityException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/CannotUseSysControllerException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include <winsock2.h>  // Used by getHostName

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::IOperator;
using TA_Base_Core::IProfile;
using TA_Base_Core::ILocation;
using TA_Base_Core::ProfileAccessFactory;
using TA_Base_Core::LocationAccessFactory;
using TA_Base_Core::OperatorAccessFactory;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;

namespace TA_Base_App
{
    TransActiveLoginDlg::TransActiveLoginDlg(ISessionChecker& sessionChecker)
	    : m_sessionChecker(sessionChecker),
          m_selectedOperator(NULL),
          m_selectedProfile(NULL),
          CDialog(TransActiveLoginDlg::IDD, NULL)
    {
	    //{{AFX_DATA_INIT(TransActiveLoginDlg)
		    // NOTE: the ClassWizard will add member initialization here
	    //}}AFX_DATA_INIT
    	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    }


    TransActiveLoginDlg::~TransActiveLoginDlg()
    {
        try
        {
            //
            // Delete all the operators
            //
            OperatorVector::iterator operatorIter = m_allOperators.begin();
            while (operatorIter != m_allOperators.end())
            {
                if (*operatorIter != NULL)
                {
                    delete *operatorIter;
                    *operatorIter = NULL;
                }
                ++operatorIter;
            }
            m_allOperators.erase( std::remove(m_allOperators.begin(), m_allOperators.end(), static_cast<TA_Base_Core::IOperator*>(NULL)), m_allOperators.end() );

            //
            // Delete all the profiles
            //
            for (ProfileMap::iterator prof = m_profileMap.begin(); prof != m_profileMap.end(); prof++)
            {
                delete prof->second;
                prof->second = NULL;
            }

            //
            // Delete all the locationss
            //
            for (LocationMap::iterator loc = m_locationMap.begin(); loc != m_locationMap.end(); loc++)
            {
                delete loc->second;
                loc->second = NULL;
            }
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Some exception was caught in the destructor. Ignoring");
        }
    }


    void TransActiveLoginDlg::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(TransActiveLoginDlg)
	    DDX_Control(pDX, IDC_PROFILE_COMBO, m_profileCombo);
	    DDX_Control(pDX, IDC_PASSWORD_EDIT, m_passwordEdit);
	    DDX_Control(pDX, IDC_NAME_COMBO, m_nameCombo);
	    DDX_Control(pDX, IDC_LOCATION_COMBO, m_locationCombo);
	    //}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(TransActiveLoginDlg, CDialog)
	//{{AFX_MSG_MAP(TransActiveLoginDlg)
	ON_BN_CLICKED(IDOK, onOK)
	ON_CBN_SELCHANGE(IDC_NAME_COMBO, onSelchangeNameCombo)
	ON_CBN_SELCHANGE(IDC_PROFILE_COMBO, onSelchangeProfileCombo)
	ON_CBN_SELCHANGE(IDC_LOCATION_COMBO, onSelchangeLocationCombo)
	ON_EN_CHANGE(IDC_PASSWORD_EDIT, onChangePasswordEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    BOOL TransActiveLoginDlg::OnInitDialog() 
    {
	    CDialog::OnInitDialog();
	    
	    // Set the icon for this dialog.  The framework does this automatically
	    //  when the application's main window is not a dialog
	    SetIcon(m_hIcon, TRUE);			// Set big icon
	    SetIcon(m_hIcon, FALSE);		// Set small icon

        retrieveConsoleInfo();

		// Passwords can only be a maximum of 50 characters.
		m_passwordEdit.SetLimitText(50); 

        try
        {
            m_allOperators = OperatorAccessFactory::getInstance().getAllNonSystemOperators(false, false); 

            ProfileVector profiles = ProfileAccessFactory::getInstance().getAllNonSystemProfiles(false);
            for (unsigned int i = 0; i < profiles.size(); i++)
            {
                m_profileMap.insert(ProfileMap::value_type(profiles[i]->getKey(), profiles[i]));
            }

            LocationVector locations = LocationAccessFactory::getInstance().getAllLocations(false);
            for (unsigned int i = 0; i < locations.size(); i++)
            {
                m_locationMap.insert(LocationMap::value_type(locations[i]->getKey(), locations[i]));
            }
        }
        catch ( const DataException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not retrieve the operators to add to the combo box");
        }
        catch ( const DatabaseException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", "Could not retrieve the operators to add to the combo box");
        }

        populateOperatorCombo();

        refresh();
	    
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void TransActiveLoginDlg::populateOperatorCombo()
    {
        FUNCTION_ENTRY("populateOperatorCombo");

        m_nameCombo.ResetContent();

        for (OperatorVector::iterator iter = m_allOperators.begin(); iter != m_allOperators.end(); ++iter)
        {
            try
            {
                int index = m_nameCombo.AddString( (*iter)->getName().c_str() );
                m_nameCombo.SetItemData(index, reinterpret_cast<DWORD>(*iter));
            }
            catch ( const DataException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not retrieve the operators to add to the combo box");
            }
            catch ( const DatabaseException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not retrieve the operators to add to the combo box");
            }
        }

        FUNCTION_EXIT;
    }


    void TransActiveLoginDlg::populateLocationCombo()
    {
        FUNCTION_ENTRY("populateLocationCombo");
        TA_ASSERT(m_selectedProfile != NULL, "Must have selected a profile");

        m_locationCombo.ResetContent();

        LocationVector locations = getListOfLocations(m_selectedProfile);

        for (LocationVector::iterator iter = locations.begin(); iter != locations.end(); ++iter)
        {
            try
            {
                int index = m_locationCombo.AddString( (*iter)->getDescription().c_str() );
                m_locationCombo.SetItemData(index, (*iter)->getKey());
            }
            catch ( const DataException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not retrieve the locations to add to the combo box");
            }
            catch ( const DatabaseException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not retrieve the locations to add to the combo box");
            }
        }

        FUNCTION_EXIT;
    }


    void TransActiveLoginDlg::populateProfileCombo()
    {
        FUNCTION_ENTRY("populateProfileCombo");

        TA_ASSERT(m_selectedOperator != NULL, "Must have selected an operator");
        m_profileCombo.ResetContent();

        ProfileVector profiles = getListOfProfiles(m_selectedOperator);

        for (ProfileVector::iterator iter = profiles.begin(); iter != profiles.end(); ++iter)
        {
            try
            {
                int index = m_profileCombo.AddString( (*iter)->getName().c_str() );
                m_profileCombo.SetItemData(index, reinterpret_cast<DWORD>(*iter));
            }
            catch ( const DataException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not retrieve the profiles to add to the combo box");
            }
            catch ( const DatabaseException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not retrieve the profiles to add to the combo box");
            }
        }

        FUNCTION_EXIT;
    }


    void TransActiveLoginDlg::onSelchangeNameCombo() 
    {
        m_profileCombo.SetCurSel(-1);
        m_locationCombo.SetCurSel(-1);

        // Passwords are based on operator so if the operator is changed the password should be removed
        m_passwordEdit.SetWindowText("");

        int selected = m_nameCombo.GetCurSel();

        if (m_nameCombo.GetCurSel() != CB_ERR)
        {
            m_selectedOperator = reinterpret_cast<IOperator*>(m_nameCombo.GetItemData(selected));
            populateProfileCombo();
        }
        else
        {
            m_selectedOperator = NULL;
        }
        m_selectedProfile = NULL;
	    refresh();
    }


    void TransActiveLoginDlg::onSelchangeProfileCombo() 
    {
        m_locationCombo.SetCurSel(-1);

        int selected = m_profileCombo.GetCurSel();
        if (selected == CB_ERR)
        {
            m_selectedProfile = NULL;
            refresh();
            return;
        }

        m_selectedProfile = reinterpret_cast<IProfile*>(m_profileCombo.GetItemData(selected));

        try
        {
		    if (m_selectedProfile->requiresLocationToBeSelected())
		    {
                populateLocationCombo();
			    // Select a default location.
			    m_locationCombo.SetCurSel(m_locationCombo.FindStringExact(-1, 
							    m_locationDescription.c_str()));
		    }
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not determine if a location is required for this profile");
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", "Could not determine if a location is required for this profile");
        }

	    refresh();
    }


    void TransActiveLoginDlg::onSelchangeLocationCombo() 
    {
	    refresh();
    }


    void TransActiveLoginDlg::onChangePasswordEdit() 
    {
	    refresh();
    }


    void TransActiveLoginDlg::onOK() 
    {
        FUNCTION_ENTRY("onOK");

        TA_ASSERT(m_selectedOperator != NULL, "Must select an operator before OK is activated");
        TA_ASSERT(m_selectedProfile!= NULL, "Must select an operator before OK is activated");

        using TA_Base_Core::RunParams;
        using TA_Base_Core::AuthenticationSecurityException;

        ISessionChecker::SessionDetails details;
        details.operatorKey = m_selectedOperator->getKey();
        details.profileKey = m_selectedProfile->getKey();
        details.locationKey = 0;
        if (m_selectedProfile->requiresLocationToBeSelected())
        {
            details.locationKey = m_locationCombo.GetItemData( m_locationCombo.GetCurSel() );
        }
        CString password;
        m_passwordEdit.GetWindowText(password);
        details.password = std::string(password);

        details.consoleKey = m_consoleKey;
        try
        {
            m_sessionChecker.authenticate(details);
            m_sessionChecker.checkUserPermissions();
        }
        catch(const AuthenticationSecurityException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AuthenticationSecurityException","The details entered did not have permission to login");

            unsigned int errorId;        // The resource Id of the string
            switch( ex.getReason() )
            {
                case(AuthenticationSecurityException::INVALID_USER):           // The supplied user is not known to the system
                    errorId = IDS_INVALID_USER;
                    break;

                case(AuthenticationSecurityException::INVALID_PASSWORD):       // The supplied password is incorrect for the user
                    errorId = IDS_INVALID_PASSWORD;
                    break;

                case(AuthenticationSecurityException::INVALID_PROFILE):        // The supplied profile is not known to the system
                    errorId = IDS_INVALID_PROFILE;
                    break;

                case(AuthenticationSecurityException::INVALID_LOCATION):       // The supplied location is not known to the system
                    errorId = IDS_INVALID_LOCATION;
                    break;

                case(AuthenticationSecurityException::INVALID_WORKSTATION):    // The supplied workstation is not known to the system
                    errorId = IDS_INVALID_WORKSTATION;
                    break;

                case(AuthenticationSecurityException::PROFILE_NOT_PERMITTED):  // The profile is not allowed for the supplied user
                    errorId = IDS_PROFILE_NOT_PERMITTED;
                    break;

                case(AuthenticationSecurityException::LOCATION_NOT_PERMITTED): // The location is not allowed for the supplied profile
                    errorId = IDS_LOCATION_NOT_PERMITTED;
                    break;

                default:
                    errorId = IDS_UNKNOWN_ERROR;
                    break;
            }

            CString action, error, actionName;
            action.LoadString(IDS_LOGIN);
            error.LoadString(errorId);
            actionName = action + error;
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_020046);

            FUNCTION_EXIT;
            return;
        }
        catch (const TA_Base_Core::AuthenticationAgentException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AuthenticationAgentException","This is an unknown exception from the Authentication Agent. It could mean the Authentication agent can't contact the database");

            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UI_020018);            

            // Try to validate password with database directly.
            try
            {
                performDatabaseVerification(details);
            }
            catch(const AuthenticationSecurityException&)
            {
                FUNCTION_EXIT;
                return;
            }
            
        }
        catch (const TA_Base_Core::ObjectResolutionException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException","The Authentication Agent may not be running or may be running incorrectly.");

            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UI_020018);            
    
            // Try to validate password with database directly.
            try
            {
                performDatabaseVerification(details);
            }
            catch(const AuthenticationSecurityException&)
            {
                FUNCTION_EXIT;
                return;
            }
        }
        catch ( const TA_Base_Core::GenericGUIException& ex)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "GenericGUIException", "Caught but can only deal with the ACCESS_DENIED problem. This will be thrown on if this is not the type");

            switch(ex.getFailType())
            {
                case(TA_Base_Core::GenericGUIException::ACCESS_DENIED):
						 {
							 TA_Base_Bus::TransActiveMessage userMsg;
							 std::string userName;
							 std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
							 TA_Base_Bus::AuthenticationLibrary authLibrary;
							 TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
							 std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
							 userName = iOperator->getName();
							 std::string actionName = "start this application";
							 userMsg << userName;
							 userMsg << actionName;
							 UINT selectedButton = userMsg.showMsgBox(IDS_UE_010021);
							 FUNCTION_EXIT;
							 return;
						 }
                default:
                    throw;
            }
        }

        CDialog::OnOK();
    }


    void TransActiveLoginDlg::refresh()
    {
        if (m_selectedOperator == NULL)
        {
            m_profileCombo.EnableWindow(false);
            m_locationCombo.EnableWindow(false);
            m_passwordEdit.EnableWindow(false);
            GetDlgItem(IDOK)->EnableWindow(false);
            return;
        }

        m_profileCombo.EnableWindow(true);
        
        if (m_selectedProfile == NULL)
        {
            m_locationCombo.EnableWindow(false);
            m_passwordEdit.EnableWindow(false);
            GetDlgItem(IDOK)->EnableWindow(false);
            return;
        }

        if (m_selectedProfile->requiresLocationToBeSelected())
        {
            m_locationCombo.EnableWindow(true);			

	        int selectedLocation = m_locationCombo.GetCurSel();
	        if (selectedLocation == CB_ERR)
	        {
                m_passwordEdit.EnableWindow(false);
                GetDlgItem(IDOK)->EnableWindow(false);
                return;
	        }
        }
        else
        {
            m_locationCombo.EnableWindow(false);
        }

        m_passwordEdit.EnableWindow(true);

        CString password;
        m_passwordEdit.GetWindowText(password);
        if (password.IsEmpty())
        {
            GetDlgItem(IDOK)->EnableWindow(false);
		    return;
        }

        GetDlgItem(IDOK)->EnableWindow(true);
    }


    void TransActiveLoginDlg::retrieveConsoleInfo()
    {
        // 
        // Retrieve the console name and check it is in the database
        // 
        char hostName[512];
	    int hNameLength = 512;

	    if( gethostname(hostName, hNameLength) != 0 )
        {
            using TA_Base_Core::GenericGUIException;
            // This is a big error so we'll throw it all the way back up to the top
            TA_THROW( GenericGUIException( "Host name could not be retrieved so we could not determine the console" , GenericGUIException::INITIALISATION_ERRORS) );
        }

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "This console is: %s",hostName );

        try
        {
	        TA_Base_Core::IConsole* console = 
				TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromAddress(hostName);
			unsigned long locationKey = console->getLocation();
			m_consoleKey = console->getKey();

			TA_Base_Core::ILocation* location = 
				TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(locationKey);
			m_locationDescription = location->getDescription();

			delete console;
			delete location;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Error occurred while attempting to look up console in the database");
            TA_THROW( TA_Base_Core::CannotUseSysControllerException("Console has not been configured.") );
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Error occurred while attempting to look up console in the database");
            TA_THROW( TA_Base_Core::CannotUseSysControllerException("Console has not been configured.") );
        }
        catch ( const TA_Base_Core::EntityTypeException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "EntityTypeException", "Error occurred while attempting to look up console in the database");
            TA_THROW( TA_Base_Core::CannotUseSysControllerException("Console has not been configured.") );
        }
    }


    void TransActiveLoginDlg::verifyDetailsWithDatabase(const ISessionChecker::SessionDetails& details)
    {
        unsigned long operatorKey = details.operatorKey;
        std::string enteredPassword = details.password;

        // Get the real password.
        TA_Base_Core::IOperator* operatorData = TA_Base_Core::OperatorAccessFactory::
                                    getInstance().getOperator(operatorKey, false);
        std::string actualPassword = operatorData->getPassword();
        delete operatorData;

        // Check that the passwords match.
        if (enteredPassword.compare(actualPassword) != 0)
        {
            TA_THROW(TA_Base_Core::AuthenticationSecurityException("The password is incorrect", 
                TA_Base_Core::AuthenticationSecurityException::ESecurityReason::INVALID_PASSWORD));
        }
    }


    void TransActiveLoginDlg::performDatabaseVerification(const ISessionChecker::SessionDetails& details)
    {
        FUNCTION_ENTRY("performDatabaseVerification");

        // Try to validate password with database directly.
        try
        {
            verifyDetailsWithDatabase(details);
        }
        catch(const TA_Base_Core::DataException&)
        {
            CString action, error, actionName;
            action.LoadString(IDS_LOGIN);
            error.LoadString(IDS_INVALID_USER);
            actionName = action + error;
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            userMsg.showMsgBox(IDS_UE_020046);

            // Convert exception to an AuthenticationSecurityException
            TA_THROW(TA_Base_Core::AuthenticationSecurityException("The user is unknown", 
                TA_Base_Core::AuthenticationSecurityException::ESecurityReason::INVALID_USER));

        }
        catch(const TA_Base_Core::DatabaseException&)
        {
            CString action, error, actionName;
            action.LoadString(IDS_LOGIN);
            error.LoadString(IDS_DATABASE_ERROR);
            actionName = action + error;
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            userMsg.showMsgBox(IDS_UE_020046);

            // Convert exception to an AuthenticationSecurityException
            TA_THROW(TA_Base_Core::AuthenticationSecurityException("Database errors occurred.", 
                TA_Base_Core::AuthenticationSecurityException::ESecurityReason::INVALID_USER));

        }
        catch(const TA_Base_Core::AuthenticationSecurityException& ex)
        {
            CString action, error, actionName;

            switch( ex.getReason() )
            {
                case(TA_Base_Core::AuthenticationSecurityException::INVALID_PASSWORD):
                    error.LoadString(IDS_INVALID_PASSWORD);
                    break;
                default:
                    error.LoadString(IDS_UNKNOWN_ERROR);
                    break;
            }

            action.LoadString(IDS_LOGIN);
            actionName = action + error;
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            userMsg.showMsgBox(IDS_UE_020046);

            throw;
        }

        FUNCTION_EXIT;
    }


    TransActiveLoginDlg::ProfileVector TransActiveLoginDlg::getListOfProfiles(TA_Base_Core::IOperator* theOperator)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry,"getListOfProfiles");

        TA_ASSERT(theOperator != NULL, "The operator passed was equal to NULL");

        ProfileVector associatedProfiles;

        // Load the new profiles
        try
        {
            std::vector<unsigned long> profiles = theOperator->getAssociatedProfileKeys();
            for (unsigned int i = 0 ; i < profiles.size(); i++)
            {
                ProfileMap::iterator profIter = m_profileMap.find(profiles[i]);
                if (profIter == m_profileMap.end())
                {
                    LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
                        "Unable to find the profile, %d, that is associated with operator, %d.  Ignoring", profiles[i], theOperator->getKey());
                }
                else
                {
                    associatedProfiles.push_back(profIter->second);
                }
            }
        }
        catch ( DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Error occurred while attempting to retrieve a list of profiles from the database");
        }
        catch ( DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Error occurred while attempting to retrieve a list of profiles from the database");
        }

        // returning an array which points to our internal copies
        return associatedProfiles;
    }


    TransActiveLoginDlg::LocationVector TransActiveLoginDlg::getListOfLocations(TA_Base_Core::IProfile* theProfile)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry,"getListOfLocations");

        TA_ASSERT(theProfile != NULL, "The profile passed was equal to NULL");

        LocationVector associatedLocations;

        // Load the new locations
        try
        {
            std::vector<unsigned long> locations = theProfile->getAssociatedLocationKeys();
            for (unsigned int i = 0 ; i < locations.size(); i++)
            {
                LocationMap::iterator locIter = m_locationMap.find(locations[i]);
                if (locIter == m_locationMap.end())
                {
                    LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
                        "Unable to find the location, %d, that is associated with profile, %d.  Ignoring", locations[i], theProfile->getKey());
                }
                else
                {
                    associatedLocations.push_back(locIter->second);
                }
            }
        }
        catch ( DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Error occurred while attempting to retrieve a list of locations from the database");
        }
        catch ( DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Error occurred while attempting to retrieve a list of locations from the database");
        }

        // returning an array which points to our internal copies
        return associatedLocations;
    }


}
