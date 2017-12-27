/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/ProfileLoginDlg.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/07/13 15:31:28 $
  * Last modified by:  $Author: hui.wang $
  *
  * This dialog serves three purposes. It is firstly used as a Login dialog that will 
  * be used each time the user wishes to log in to TransActive. Secondly, it is used 
  * as a  Change Profile dialog that is used by an operator to select a different 
  * profile. Finally, it is used as an Operator Override screen that is used to
  * augment the access rights of the logged in operator.
  */

#pragma warning(disable:4786)
#include "app\system_control\control_station\src\stdafx.h"
#include "app\system_control\control_station\src\ControlStation.h"
#include "app\system_control\control_station\src\ProfileLoginDlg.h"
#include "app\system_control\control_station\src\SessionManager.h"
#include "app\system_control\control_station\src\ControlStationConstants.h"
#include "app\system_control\control_station\src\WinkeysMgr.h"

#include <algorithm>
#include <sstream>

#include "bus\generic_gui\src\HelpLauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus\security\access_control\actions\src\AccessControlledActions.h"
#include "core\exceptions\src\DataException.h"
#include "core\exceptions\src\DatabaseException.h"
#include "core\exceptions\src\SessionException.h"
#include "core\exceptions\src\GraphWorxException.h"
#include "core\data_access_interface\src\IOperator.h"
#include "core\data_access_interface\src\IProfile.h"
#include "core\data_access_interface\src\ILocation.h"
#include "core\data_access_interface\src\ProfileAccessFactory.h"
#include "core\utilities\src\DebugUtil.h"
#include "core\utilities\src\RunParams.h"
#include "core\versioning\src\Version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::SessionException;
using TA_Base_Core::GraphWorxException;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::ILocation;
using TA_Base_App::TA_ControlStation::APPLICATION_NAME;
using TA_Base_App::TA_ControlStation::MAX_PASSWORD_LENGTH;
using TA_Base_App::TA_ControlStation::EPermission;

std::string ProfileLoginDlg::USE_TDS_PROFILE("UseTdsProfile");
boost::regex ProfileLoginDlg::TDS_PATTERN("^[A-Z,0-9]+[[:space:]]*\\(TRA\\)$|^[A-Z]+-TDS[[:space:]]*\\([A-Z,a-z]+\\)$");
/////////////////////////////////////////////////////////////////////////////
// ProfileLoginDlg dialog

IMPLEMENT_DYNAMIC(ProfileLoginDlg, CDialog);


ProfileLoginDlg::ProfileLoginDlg()
    : m_selectedOperator(NULL),
      m_selectedProfile(NULL),
      m_selectedLocation(NULL),
      m_password(""),
	  m_dbConnection(""),
	  m_dbConnectionString(""),
	  m_mode(TA_Base_App::TA_ControlStation::LOGIN_MODE),
      m_isWindowActivated(false),
      m_useExistingData(false),
      m_terminateCode(TA_Base_App::TA_ControlStation::SHUTDOWN), // shutdown by default if the control station couldn't start up properly 
      m_isRestartAlwaysEnabled(false),
      m_isExitAlwaysEnabled(false),
      CDialog(ProfileLoginDlg::IDD, NULL)
{
	//{{AFX_DATA_INIT(ProfileLoginDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


ProfileLoginDlg::~ProfileLoginDlg()
{
}


void ProfileLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ProfileLoginDlg)
	DDX_Control(pDX, IDC_RESTART_BUTTON_AB, m_restartButton);
	DDX_Control(pDX, IDC_EXIT, m_cancelButton);
	DDX_Control(pDX, IDC_ANIMATE_AB, m_introMovie);
	DDX_Control(pDX, IDC_LOGIN_BUTTON_AB, m_loginButton);
	DDX_Control(pDX, IDC_VERSION, m_version);
	DDX_Control(pDX, IDC_PROFILE_COMBO_AB, m_profileCombo);
	DDX_Control(pDX, IDC_PASSWORD_EDIT_AB, m_passwordEdit);
	DDX_Control(pDX, IDC_NAME_COMBO_AB, m_nameCombo);
	DDX_Control(pDX, IDC_LOCATION_COMBO_AB, m_locationCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ProfileLoginDlg, CDialog)
	//{{AFX_MSG_MAP(ProfileLoginDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_NAME_COMBO_AB, onSelchangeNameCombo)
	ON_CBN_SELCHANGE(IDC_PROFILE_COMBO_AB, onSelchangeProfileCombo)
	ON_EN_CHANGE(IDC_PASSWORD_EDIT_AB, onChangePasswordEdit)
	ON_BN_CLICKED(IDC_LOGIN_BUTTON_AB, onLoginButton)
	ON_BN_CLICKED(IDC_HELP_BUTTON_AB, onHelpButton)
	ON_BN_CLICKED(IDC_RESTART_BUTTON_AB, onRestartButton)
	ON_CBN_SELCHANGE(IDC_LOCATION_COMBO_AB, onSelchangeLocationCombo)
	ON_WM_SYSCOMMAND()
	ON_CBN_DROPDOWN(IDC_NAME_COMBO_AB, onDropdownNameCombo)
	ON_CBN_DROPDOWN(IDC_PROFILE_COMBO_AB, onDropdownProfileCombo)
	ON_CBN_DROPDOWN(IDC_LOCATION_COMBO_AB, onDropdownLocationCombo)
	ON_BN_CLICKED(IDC_EXIT, onExit)
    ON_MESSAGE(WM_FORCE_CANCEL, OnCancel)
	//}}AFX_MSG_MAP    
    ON_MESSAGE(WM_SYSTEMCONTROLLER_STATE_CHANGE, onSystemControllerStateChange)    
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// ProfileLoginDlg message handlers
/////////////////////////////////////////////////////////////////////////////

// External versioning variables. See VersionGen.cpp
namespace TA_Base_Core
{
    extern char buildVersion[];
}

BOOL ProfileLoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Play the AVI intro movie.

    // Get the application's path
    char    szAppPath[MAX_PATH] = "";
    ::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

    // Extract directory
    CString aviPath = szAppPath;
    aviPath = aviPath.Left(aviPath.ReverseFind('\\'));

    // Append the AVI name
    aviPath += "\\";
    aviPath += TA_Base_App::TA_ControlStation::AVI_FILE.c_str();

    // Finally open and play the movie
    if( !m_introMovie.Open(aviPath) )
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "AVI specified could not be found: %s", aviPath );
    }
    else
    {
	    m_introMovie.Play(0, -1, -1);
    }

    // Make the Window always on top.
    SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);

    // The window needs to be positioned in a different manner if there is an even number of workstation screens,
    // so it does not appear in the centre between two monitors.
    int numberOfScreens = SessionManager::getInstance().getScreenPositioning().getNumberOfMonitors();
    if ( 0 == (numberOfScreens % 2) )
    {
        // Get the rectangle representing the current dialog position
        CRect windowPos;
        GetWindowRect(&windowPos);
        int windowWidth = windowPos.Width();

        // Need to find out the screen coordinate of the left side of the screen that we want to put this dialog
        // onto
        int screenWidth = SessionManager::getInstance().getScreenPositioning().getEachScreenWidth();
        int screenXPos = (numberOfScreens / 2 - 1) * screenWidth;
        
        // Use the left position and the screen width to work out where to place the log in screen
        windowPos.left = screenXPos + (screenWidth / 2) - (windowWidth / 2);
        windowPos.right = windowPos.left + windowWidth;

        // And move the window
        MoveWindow(&windowPos);
    }

    m_passwordEdit.SetLimitText(MAX_PASSWORD_LENGTH); // limit our password length

    GetDlgItem(IDC_HELP_BUTTON_AB)->EnableWindow(FALSE);

    // Add "About..." menu item to system menu.
    // Remove the "Close" menu item from the system menu
    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
	    if (m_mode == TA_Base_App::TA_ControlStation::LOGIN_MODE)
	    {
            // Disable the 'x' button first so that it could be enabled
            // the same time 'Exit' button is enabled.(PW 628)
            pSysMenu->EnableMenuItem(SC_CLOSE, MF_GRAYED);
        }
        CString strAboutMenu;
	    TA_VERIFY(strAboutMenu.LoadString(IDS_ABOUTBOX)!=0, "Unable to load IDS_ABOUTBOX");
	    if (!strAboutMenu.IsEmpty())
	    {
		    pSysMenu->AppendMenu(MF_SEPARATOR);
		    pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	    }
    }

    // First save the current selections as they will be reset once we start changing things.
    CString operatorEntry("");
    CString profileEntry("");
    CString locationEntry("");
    if (m_useExistingData)
    {
        try
        {
            if (m_selectedOperator !=  NULL)
            {
                operatorEntry = m_selectedOperator->getName().c_str(); //TD15528
                if (m_selectedProfile != NULL)
                {
                    profileEntry =  m_selectedProfile->getName().c_str();
                    if (m_selectedProfile->requiresLocationToBeSelected() && (m_selectedLocation != NULL))
                    {
                        locationEntry = m_selectedLocation->getName().c_str();
                    }
                }
            }
        }
        catch ( const DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Thrown while trying to retrieve details to set the combo box selections in the Login Dialog");
        }
        catch ( const DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Thrown while trying to retrieve details to set the combo box selections in the Login Dialog");
        }
        catch (...)
        {
            // just in case the selected item has been deleted
        }
    }

    // activate and display the controls
    drawControls();

    // It is only the first time the dialogue is loaded that we need to wait for
    // the SessionManager to finish its processing. After this the window will always
    // be active so we can call the activateWindow method to ensure that everything
    // is enabled/disabled correctly.
    if (m_isWindowActivated)
    {
        activateWindow();
    }

    // Select previous data if required
    if (m_useExistingData)
    {
        m_nameCombo.SetCurSel(m_nameCombo.FindStringExact(-1,operatorEntry) );
        onSelchangeNameCombo();

        m_profileCombo.SetCurSel(m_profileCombo.FindStringExact(-1, profileEntry) );
        onSelchangeProfileCombo();

        m_locationCombo.SetCurSel(m_locationCombo.FindStringExact(-1, locationEntry) );
        onSelchangeLocationCombo();

        m_passwordEdit.SetFocus();

        m_useExistingData = false;

        return FALSE;  // we're giving the control to the password edit
    }
	
	//Haipeng Jiang added the codes to disable the CAD keys
	WinkeysMgr::instance()->lockCAD();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void ProfileLoginDlg::OnSysCommand(UINT nID, LPARAM lParam) 
{
    // In WM_SYSCOMMAND messages, the four low-order bits of the nID parameter 
    // are used internally by Windows CE. When an application tests the value of 
    // nID, it must combine the value 0xFFF0 with the nID value by using the 
    // bitwise-AND operator to obtain the correct result.
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
        TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


void ProfileLoginDlg::onSelchangeNameCombo() 
{
    using TA_Base_Core::IProfile;

	// Get the key for the currently selected operator name
	int selected = m_nameCombo.GetCurSel();
    if (selected == CB_ERR )
    {
        m_selectedOperator = NULL;
        m_selectedProfile = NULL;
        m_selectedLocation = NULL;

        // disable the buttons
        m_profileCombo.EnableWindow(FALSE);
        m_locationCombo.EnableWindow(FALSE);
	    m_passwordEdit.EnableWindow(FALSE);

        // clear the selections
        m_profileCombo.SetCurSel(CB_ERR);
	    m_locationCombo.SetCurSel(CB_ERR);
        m_passwordEdit.SetWindowText("");

        m_nameCombo.SetFocus();
   	    refreshButtonAvailability();

        return;
    }

    using TA_Base_Core::IOperator;
    // Update/Set the selected operator member variable
    m_selectedOperator = reinterpret_cast<IOperator*>(m_nameCombo.GetItemData(selected));
    m_selectedProfile = NULL;
    m_selectedLocation = NULL;

    // Because we are going to populate the profile combo boxes regardless
    // of whether the list have been updated, we'll just call these methods so 
    // that they won't be updated again when we give focus to the combos
    // TD 12050
    SessionManager::getInstance().getSessionDetails().resetProfileUpdateFlag();

    // Reload the profile combo box with relevant profiles for this operator
    populateProfileComboBox();

    // enable/disable the buttons
    m_profileCombo.EnableWindow(TRUE);

    // TD1847 - if there's only 1 profile, then just select it
    if (m_profileCombo.GetCount() == 1)
    {
        m_profileCombo.SetCurSel(0);
        onSelchangeProfileCombo();
        return;
    }        

    m_locationCombo.EnableWindow(FALSE);
	m_passwordEdit.EnableWindow(FALSE);

    // clear the selections
    m_profileCombo.SetCurSel(CB_ERR);
	m_locationCombo.SetCurSel(CB_ERR);
    m_passwordEdit.SetWindowText("");

   	refreshButtonAvailability();
}


void ProfileLoginDlg::onSelchangeProfileCombo() 
{
    int selected = m_profileCombo.GetCurSel();

	if (selected == CB_ERR)
	{
        m_selectedProfile = NULL;
        m_selectedLocation = NULL;

		m_locationCombo.SetCurSel(CB_ERR);
		m_locationCombo.EnableWindow(FALSE);
    	m_passwordEdit.EnableWindow(FALSE);
        m_profileCombo.SetFocus();
        return;
	}

    using TA_Base_Core::IProfile;
    // Update/Set the selected profile member variable
    m_selectedProfile = reinterpret_cast<IProfile*>(m_profileCombo.GetItemData(selected));
    m_selectedLocation = NULL;

    // Check if this profile requires a location to be selected.
	if( m_selectedProfile->requiresLocationToBeSelected() )
	{       
        populateLocationComboBox();
        // PW2844, TD1847 - Only enable the combo box if it is not in operator override mode
        if (m_mode == TA_Base_App::TA_ControlStation::OPERATOR_OVERRIDE_MODE)
        {
	        // Set the current login location
	        int index = m_locationCombo.FindStringExact(-1, SessionManager::getInstance().getSessionDetails().getLocationName().c_str());
            if (index == CB_ERR)
            {
				//TD15554++

				//found a local profile
				//to perform an override console location key must equal session location
				if(SessionManager::getInstance().getSessionDetails().getConsoleLocationName()!=SessionManager::getInstance().getSessionDetails().getLocationName())
				{
					index = CB_ERR;
				}
				else
				{
					index = m_locationCombo.FindStringExact(-1, SessionManager::getInstance().getSessionDetails().getConsoleLocationName().c_str());
				}

				//++TD15554

            }
			
            // if we're able to select either the login or console location, then set the selection
            if (index != CB_ERR)
            {
                m_locationCombo.SetCurSel(index);
                // Update/Set the selected location member variable - otherwise 
                // the session manager will think that this is a central profile.
                m_selectedLocation = reinterpret_cast<ILocation*>(m_locationCombo.GetItemData(index));
            	m_passwordEdit.EnableWindow(TRUE);
            }
            else
            {
                m_selectedLocation = NULL;
            	m_passwordEdit.EnableWindow(FALSE); //TD11707 - only enable it if the profile is associated with the login location
            }

            m_locationCombo.EnableWindow(FALSE);
        }
        else
        {
	        // Set the default location
            int index = m_locationCombo.FindStringExact(-1, SessionManager::getInstance().getSessionDetails().getConsoleLocationName().c_str());
            if (index == CB_ERR)
            {
                // just select the first index if it couldn't find the default one
                index = 0;
            }
	        m_locationCombo.SetCurSel(index);
            m_locationCombo.EnableWindow(TRUE);
            onSelchangeLocationCombo();
        }
	}
	else // central profile - so disable location
	{
		m_locationCombo.SetCurSel(CB_ERR);
		m_locationCombo.EnableWindow(FALSE);
    	m_passwordEdit.EnableWindow(TRUE);
        m_selectedLocation = NULL;
	}

   	refreshButtonAvailability();
}


void ProfileLoginDlg::onSelchangeLocationCombo() 
{
    // Don't worry about it if the location combo is disabled - this call
    // is probably called by ourselves explicitly, so just ignore
    if (!m_locationCombo.IsWindowEnabled())
    {
		// TD18944
		//m_selectedLocation = NULL;
        return;
    }

    int selected = m_locationCombo.GetCurSel();
    if (selected == CB_ERR )
    {
        m_selectedLocation = NULL;
        m_passwordEdit.EnableWindow(FALSE);
        m_locationCombo.SetFocus();
        return;
    }

    using TA_Base_Core::ILocation;
    // Update/Set the selected location member variable
    m_selectedLocation = reinterpret_cast<ILocation*>(m_locationCombo.GetItemData(selected));
    m_passwordEdit.EnableWindow(TRUE);
   	refreshButtonAvailability();
}


void ProfileLoginDlg::onChangePasswordEdit() 
{
    m_passwordEdit.GetWindowText(m_password);
   	refreshButtonAvailability();
}


void ProfileLoginDlg::onLoginButton() 
{
    if( (m_selectedOperator == NULL) || (m_selectedProfile == NULL) || (m_password.IsEmpty()) )
    {
        // We are not ready to login yet
        // We should never get in here as the enabling and disabling should prevent us.
        // We have to check anyway though.
        return;
    }

    if (!areDetailsValid())
    {
        return;
    }

	m_introMovie.Stop();
    m_introMovie.Close();

	if( m_selectedLocation != NULL)
	{
	
		if( m_selectedLocation->getKey() != 0 && TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
		{
			std::string loginLocation = m_selectedLocation->getName();
			std::string dbConnectionString = TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE);

			m_dbConnectionString = dbConnectionString;

			std::string::size_type i = dbConnectionString.rfind("\\");
			if( i != std::string::npos )
			{

				// back up the original runtime parameter
				m_dbConnectionString = dbConnectionString;
				dbConnectionString.replace( i+1, dbConnectionString.find("ConnectionString") - 1 - i, loginLocation);
				TA_Base_Core::RunParams::getInstance().set(RPARAM_DBCONNECTIONFILE, dbConnectionString.c_str());
				LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Setting the RunParam RPARAM_DBCONNECTIONFILE as %s", TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE).c_str());
			}
		}

		if( m_selectedLocation->getKey() !=0 && TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DBCONNECTION) )
		{
			std::string loginLocation = m_selectedLocation->getName();
			std::string dbConnection = TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTION);

			std::string::size_type i = dbConnection.find("TRA_");
			if( i != std::string::npos )
			{
				// back up the original runtime parameter
				m_dbConnection = dbConnection;

				dbConnection.replace( i+4, loginLocation.size(), loginLocation);
				TA_Base_Core::RunParams::getInstance().set(RPARAM_DBCONNECTION, dbConnection.c_str());
				LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Setting the RunParam RPARAM_DBCONNECTION as %s", TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTION).c_str());
			}
		}
	}
	else 
	{
		// recover the backup dbconnection and dbconnectionstring when We change profile to central profile.
		if(TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) && m_dbConnectionString.empty() == false)
		{
			TA_Base_Core::RunParams::getInstance().set(RPARAM_DBCONNECTIONFILE, m_dbConnectionString.c_str());
			LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Setting the RunParam RPARAM_DBCONNECTIONFILE as %s", TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE).c_str());			
		}

		if(TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DBCONNECTION) && m_dbConnection.empty() == false )
		{
			TA_Base_Core::RunParams::getInstance().set(RPARAM_DBCONNECTION, m_dbConnection.c_str());
			LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Setting the RunParam RPARAM_DBCONNECTION as %s", TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTION).c_str());			
		}
	}

    CDialog::OnOK();
}


bool ProfileLoginDlg::areDetailsValid()
{
    TA_ASSERT(m_selectedOperator != NULL, "Must select an operator");
    TA_ASSERT(m_selectedProfile != NULL, "Must select a profile");

    if (SessionManager::getInstance().getSessionDetails().hasOperatorUpdated())
    {
        bool found = false;
        try
        {
            SessionDetails::OperatorVector operators = SessionManager::getInstance().getSessionDetails().getListOfOperators();
            
            for (SessionDetails::OperatorVector::iterator iter = operators.begin(); iter != operators.end(); iter++)
            {
                // if we found the operator, then it must have been updated, it's ok
                if ((*iter != NULL) && ((*iter)->getKey() == m_selectedOperator->getKey()))
                {
                    found = true;
                    break;
                }
            }
        }
        catch(...)
        {
            // just in case the actual object got deleted from another thread
        }

        if (!found)
        {
            CString itemName;
            TA_VERIFY(itemName.LoadString(IDS_OPERATOR)!=0, "Unable to load IDS_OPERATOR");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << itemName;
            userMsg.showMsgBox(IDS_UE_020057);
            int sel = CB_ERR;
            onDropdownNameCombo();
            m_nameCombo.SetCurSel(sel);
            onSelchangeNameCombo();
            return false;
        }
    }

    if (SessionManager::getInstance().getSessionDetails().hasProfileUpdated())
    {
        bool found = false;
        try
        {
            SessionDetails::ProfileVector profiles = SessionManager::getInstance().getSessionDetails().getListOfProfiles(m_selectedOperator, 
                (m_mode == TA_Base_App::TA_ControlStation::OPERATOR_OVERRIDE_MODE));

            for (SessionDetails::ProfileVector::iterator iter = profiles.begin(); iter != profiles.end(); iter++)
            {
                // if we found the profile, then it must have been updated, it's ok
                if ((*iter != NULL) && ((*iter)->getKey() == m_selectedProfile->getKey()))
                {
                    found = true;
                    break;
                }
            }
        }
        catch(...)
        {
            // just in case the actual object got deleted from another thread
        }

        if (!found)
        {
            CString itemName;
            TA_VERIFY(itemName.LoadString(IDS_PROFILE)!=0, "Unable to load IDS_PROFILE");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << itemName;
            userMsg.showMsgBox(IDS_UE_020057);
            int sel = CB_ERR;
            onDropdownProfileCombo();
            m_profileCombo.SetCurSel(sel);
            onSelchangeProfileCombo();
            return false;
        }
    }

    if (SessionManager::getInstance().getSessionDetails().hasLocationUpdated() && m_selectedProfile->requiresLocationToBeSelected())
    {
        TA_ASSERT(m_selectedLocation != NULL, "Must select an operator");
        bool found = false;
        try
        {
            SessionDetails::LocationVector locations = SessionManager::getInstance().getSessionDetails().getListOfLocations(m_selectedProfile);

            for (SessionDetails::LocationVector::iterator iter = locations.begin(); iter != locations.end(); iter++)
            {
                // if we found the profile, then it must have been updated, it's ok
                if ((*iter != NULL) && ((*iter)->getKey() == m_selectedLocation->getKey()))
                {
                    found = true;
                    break;
                }
            }
        }
        catch(...)
        {
            // just in case the actual object got deleted from another thread
        }

        if (!found)
        {
            CString itemName;
            TA_VERIFY(itemName.LoadString(IDS_LOCATION)!=0, "Unable to load IDS_LOCATION");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << itemName;
            userMsg.showMsgBox(IDS_UE_020057);
            int sel = CB_ERR;
            onDropdownLocationCombo();
            m_locationCombo.SetCurSel(sel);
            onSelchangeLocationCombo();
            return false;
        }
    }
    return true;
}


LRESULT ProfileLoginDlg::OnCancel(WPARAM, LPARAM) 
{
    // TD11102 - Do nothing when the operator hits the 'ESC' button
    if (m_mode != TA_Base_App::TA_ControlStation::LOGIN_MODE)
    {
        onExit();
    }
	return S_OK;
}


void ProfileLoginDlg::onExit() 
{
    // If this is not the login dialog, then it's just a normal
    // cancel action, just call CDialog's OnCancel
    if (m_mode == TA_Base_App::TA_ControlStation::LOGIN_MODE)
    {
        if (!areDetailsValid())
        {
            return;
        }

        m_terminateCode = TA_Base_App::TA_ControlStation::EXIT;
    }
    m_introMovie.Stop();
    m_introMovie.Close();
    
    CDialog::OnCancel();
}


void ProfileLoginDlg::onHelpButton() 
{
	TA_Base_Bus::HelpLauncher::getInstance().displayHelp(true);
}


void ProfileLoginDlg::onRestartButton() 
{
    if (!areDetailsValid())
    {
        return;
    }

    m_terminateCode = TA_Base_App::TA_ControlStation::RESTART;
    m_introMovie.Stop();
    m_introMovie.Close();
    
    // We'll close the dialog box just as though we had a Cancel event.
    // Calling CDialog::OnCancel() will make DoModal return IDCANCEL
    CDialog::OnCancel();
}


/////////////////////////////////////////////////////////////////////////////
// Other Methods
/////////////////////////////////////////////////////////////////////////////

void ProfileLoginDlg::getSelectedDetails(TA_Base_Core::IOperator*& selectedOperator,
                                  TA_Base_Core::IProfile*& selectedProfile,
                                  TA_Base_Core::ILocation*& selectedLocation,
                                  CString& password) const
{
    selectedOperator = m_selectedOperator;
    selectedProfile  = m_selectedProfile;
    selectedLocation  = m_selectedLocation;
    password = m_password;
}


bool ProfileLoginDlg::hasFilledAllDetails()
{
    m_passwordEdit.GetWindowText(m_password);
    // Because of the way we enable and disable our fields, the only 
    // way all fields could be filled in is when the user has filled
    // the password field. 
	// TD18944
    return (m_passwordEdit.IsWindowEnabled() && !m_password.IsEmpty() && m_password.GetLength() >= 8 );
}


void ProfileLoginDlg::refreshButtonAvailability()
{
    bool enable = hasFilledAllDetails();
	m_loginButton.EnableWindow(enable);

    try
    {
        if (SessionManager::getInstance().getControlStationEntity().shouldShowRestartButton())
        {
            // enable the restart button if it's supposed to be enabled always
            // or when all the details are filled in
            m_restartButton.EnableWindow(m_isRestartAlwaysEnabled || enable);
        }
    }
    catch( ... )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DAI Exception", "Could not determine if restart button should be shown so it will be shown");
        // enable the restart button if it's supposed to be enabled always
        // or when all the details are filled in
        m_restartButton.EnableWindow(m_isRestartAlwaysEnabled || enable);
    }
    
    if (m_mode == TA_Base_App::TA_ControlStation::LOGIN_MODE)
    {
        m_cancelButton.EnableWindow(m_isExitAlwaysEnabled || enable);
        CMenu* pSysMenu = GetSystemMenu(FALSE);
        if (pSysMenu != NULL)
        {
            // Enable/disable the 'x' button at the same time as Cancel (Exit) button (PW 628)
            pSysMenu->EnableMenuItem(SC_CLOSE, enable?MF_ENABLED:MF_GRAYED);
        }
    }
}


void ProfileLoginDlg::populateNameComboBox()
{
    m_nameCombo.ResetContent();

    SessionDetails& details = SessionManager::getInstance().getSessionDetails();

    SessionDetails::OperatorVector& operators = details.getListOfOperators();

    SessionDetails::OperatorVector::iterator iter = operators.begin();
    while ( iter != operators.end() )
    {
        if (*iter != NULL)
        {
            CString /*description*/ name(""); //TD15528
            try
            {
                /*description*/ name = (*iter)->getName().c_str(); //TD15528
                // Add each operator name to the combo box and store the IOperator* as the
                // associated data.
                int index = m_nameCombo.AddString(/*description*/ name); //TD15528
                m_nameCombo.SetItemData(index, reinterpret_cast<DWORD>(*iter) ); 
            }
            catch ( DataException& )
            {
            }
            catch ( DatabaseException& )
            {
            }
            catch (...)
            {
            }
        }
        ++iter;
    }
}


void ProfileLoginDlg::populateLocationComboBox()
{
    m_locationCombo.ResetContent();

    bool isOnline = SessionManager::getInstance().isSystemControllerGroupOnline();
    CString consoleLocation = SessionManager::getInstance().getSessionDetails().getConsoleLocationName().c_str();
    
    SessionDetails::LocationVector locations;
    try
    {
        if (m_selectedProfile != NULL)
        {
            SessionDetails& details = SessionManager::getInstance().getSessionDetails();
            locations = details.getListOfLocations(m_selectedProfile);
        }
    }
    catch(...)
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not load the list of available operators or locations.");
    }

    SessionDetails::LocationVector::iterator iter = locations.begin();
    while ( iter != locations.end() )
    {
        if (*iter != NULL && (*iter)->getKey() != 0 )
        {
            CString name("");
            try
            {
                name = (*iter)->getName().c_str();
                // Add each location name to the combo box and store the ILocation* as the
                // associated data.

                // if in degraded mode only add the console location
                // if not in degraded mode, add all locations
                if (isOnline || (!isOnline && consoleLocation.Compare(name) == 0))
                {
                    int index = m_locationCombo.AddString(name);
                    m_locationCombo.SetItemData(index, reinterpret_cast<DWORD>(*iter) );
                }
            }
            catch ( DataException& )
            {
            }
            catch ( DatabaseException& )
            {
            }
            catch (...)
            {
            }
        }
        ++iter;
    }
}


void ProfileLoginDlg::populateProfileComboBox()
{
    m_profileCombo.ResetContent();
	bool usetds = TA_Base_Core::RunParams::getInstance().isSet(USE_TDS_PROFILE.c_str());
    if (m_selectedOperator == NULL)
    {
        return;
    }

    SessionDetails& details = SessionManager::getInstance().getSessionDetails();
    SessionDetails::ProfileVector& profiles = details.getListOfProfiles(m_selectedOperator,
        (m_mode == TA_Base_App::TA_ControlStation::OPERATOR_OVERRIDE_MODE));

    SessionDetails::ProfileVector::iterator iter = profiles.begin();
    while ( iter != profiles.end() )
    {
        if (*iter != NULL)
        {
            CString name("");
            try
            {
                name = (*iter)->getName().c_str();
                // Add each profile name to the combo box and store the IProfile* as the
                // associated data.
				if(isTdsProfile((*iter)->getName()) && !usetds)
				{
					++ iter;
					continue;
				}
                int index = m_profileCombo.AddString(name);
                m_profileCombo.SetItemData(index, reinterpret_cast<DWORD>(*iter) );
            }
            catch ( DataException& )
            {
            }
            catch ( DatabaseException& )
            {
            }
            catch (...)
            {
            }
        }
        ++iter;
    }

    // TD1146, TD1572 - Remove the current profile from profile list
    if ((m_mode == TA_Base_App::TA_ControlStation::CHANGE_PROFILE_MODE) ||
        (m_mode == TA_Base_App::TA_ControlStation::OPERATOR_OVERRIDE_MODE))
    {
        // Remove the profile from the list of profiles available
        m_profileCombo.DeleteString(m_profileCombo.FindStringExact(-1, 
						 SessionManager::getInstance().getSessionDetails().getProfileName().c_str()));
    }
}


void ProfileLoginDlg::drawControls()
{
    static bool hasRestart = false;

    try
    {
        if (SessionManager::getInstance().getControlStationEntity().shouldShowRestartButton())
        {
            hasRestart = true;
        }
    }
    catch(...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DAI Exception", "Could not determine if restart button should be shown so it will be shown");
        hasRestart = true;
    }

    // set the version number
    CString version("");
    if (m_mode == TA_Base_App::TA_ControlStation::LOGIN_MODE)
    {
        version.Format("版本 %s", TA_Base_Core::buildVersion);
    }
    m_version.SetWindowText(version);

	if ((m_mode == TA_Base_App::TA_ControlStation::LOGIN_MODE) && hasRestart)
	{
	    // We don't have to move any button, just show the restart button
	    m_restartButton.ShowWindow(SW_SHOW);
	}
	else
	{
        // Move the Cancel/Exit button across between the side buttons
        // to make them look equally distributed
		CRect window;
		GetWindowRect(&window);
        CRect rect;
		m_cancelButton.GetWindowRect(&rect);
		ScreenToClient(&rect);
		int width = rect.Width();
		rect.left = (window.Width() - width) / 2;
		rect.right = rect.left + width;
    	m_cancelButton.MoveWindow(&rect);
	}
}


void ProfileLoginDlg::activateWindow()
{
    m_isWindowActivated = true;
    m_introMovie.Play(0, -1, -1);

    // Populate the combo boxes.
    populateNameComboBox();

    switch (m_mode)
    {
        case TA_Base_App::TA_ControlStation::CHANGE_PROFILE_MODE:
            setToChangeProfileDialog();
            break;

        case TA_Base_App::TA_ControlStation::OPERATOR_OVERRIDE_MODE:
            setToOperatorOverrideDialog();
            break;

        case TA_Base_App::TA_ControlStation::CHANGE_OPERATOR_MODE:
            setToChangeOperatorDialog();
            break;

        case TA_Base_App::TA_ControlStation::LOGIN_MODE:
        default:
            setToLoginDialog();
            break;
    }

    GetDlgItem(IDC_HELP_BUTTON_AB)->EnableWindow(TRUE);

    if (m_mode == TA_Base_App::TA_ControlStation::LOGIN_MODE)
	{
        m_isRestartAlwaysEnabled = !SessionManager::getInstance().isRestartAccessControlled();
        m_isExitAlwaysEnabled = !SessionManager::getInstance().isExitAccessControlled();

        // Enable the required controls
        m_cancelButton.EnableWindow(m_isExitAlwaysEnabled);
        m_restartButton.EnableWindow(m_isRestartAlwaysEnabled);

        CMenu* pSysMenu = GetSystemMenu(FALSE);
        if (pSysMenu != NULL)
        {
            // Disable the 'x' button first so that it could be enabled
            // the same time 'Exit' button is enabled.(PW 628)
            pSysMenu->EnableMenuItem(SC_CLOSE, MF_GRAYED);
        }
	}
	else
	{
        m_cancelButton.EnableWindow(TRUE);
	}
    SetActiveWindow();
}


void ProfileLoginDlg::setToLoginDialog()
{
	// Change the mode of the dialog, now that it will be used for logging in.

    m_nameCombo.EnableWindow(TRUE);

	// Update the button captions and the window title.
	m_loginButton.SetWindowText("登录");
	m_cancelButton.SetWindowText("退出");
	SetWindowText("操作员登录");
}


void ProfileLoginDlg::setToChangeProfileDialog()
{
	// Change the mode of the dialog, now that it will be used to change profiles.

	// Make the dialog display the correct name and profile.
	m_nameCombo.SetCurSel(m_nameCombo.FindStringExact(-1,
						  SessionManager::getInstance().getSessionDetails().getOperatorName().c_str())); //TD15528
    onSelchangeNameCombo();
	// Disable the name combo as this cannot be changed.
	m_nameCombo.EnableWindow(FALSE);

   	refreshButtonAvailability();

	// Update the button captions and the window title.
	m_loginButton.SetWindowText("确认");
	m_cancelButton.SetWindowText("取消");
	SetWindowText("更改角色");
}


void ProfileLoginDlg::setToOperatorOverrideDialog()
{
	// Change the mode of the dialog, now that it will be used to override operators.
    m_nameCombo.EnableWindow(TRUE);

   	refreshButtonAvailability();

	// Update the button captions and the window title.
	m_loginButton.SetWindowText("确认");
	m_cancelButton.SetWindowText("取消");
	SetWindowText("Operator Override");
}


void ProfileLoginDlg::setToChangeOperatorDialog()
{
	// Change the mode of the dialog, now that it will be used to change operator.

	// Make the dialog display the correct name and profile.
	m_nameCombo.DeleteString(m_nameCombo.FindStringExact(-1,
						  SessionManager::getInstance().getSessionDetails().getOperatorName().c_str())); //TD15528

    m_nameCombo.EnableWindow(TRUE);

   	refreshButtonAvailability();

	// Update the button captions and the window title.
	m_loginButton.SetWindowText("确认");
	m_cancelButton.SetWindowText("取消");
	SetWindowText("切换用户");
}


void ProfileLoginDlg::onDropdownNameCombo() 
{
    if (SessionManager::getInstance().getSessionDetails().hasOperatorUpdated())
    {
        populateNameComboBox();
        SessionManager::getInstance().getSessionDetails().resetOperatorUpdateFlag();
    }

}


void ProfileLoginDlg::onDropdownProfileCombo() 
{
    bool profileHasUpdated = SessionManager::getInstance().getSessionDetails().hasProfileUpdated();

    // there's either a profile change or operator has associated/dissociated
    // from a profile, then update the profile list.
    if (SessionManager::getInstance().getSessionDetails().hasOperatorUpdated() ||
        profileHasUpdated)
    {
        populateProfileComboBox();
        SessionManager::getInstance().getSessionDetails().resetProfileUpdateFlag();
    }
}


void ProfileLoginDlg::onDropdownLocationCombo() 
{
    // there's either a location change or profile has associated/dissociated
    // from a location, then update the location list.
    if (SessionManager::getInstance().getSessionDetails().hasLocationUpdated() ||
        SessionManager::getInstance().getSessionDetails().hasProfileUpdated())
    {
        populateLocationComboBox();
        SessionManager::getInstance().getSessionDetails().resetLocationUpdateFlag();
    }
}


LRESULT ProfileLoginDlg::onSystemControllerStateChange(WPARAM, LPARAM)
{
    // If there is a system controller state change, simply need to re-populate the locations drop-down
    populateLocationComboBox();
	return S_OK;
}
bool ProfileLoginDlg::isTdsProfile(std::string profile_str)
{
	boost::match_results<std::string::const_iterator> what;
	if(0 == boost::regex_match(profile_str, what, TDS_PATTERN, boost::match_default))
	{
		return false;
	}
	if(what[0].matched)
	{
		return true;
	}
	return false;
}
