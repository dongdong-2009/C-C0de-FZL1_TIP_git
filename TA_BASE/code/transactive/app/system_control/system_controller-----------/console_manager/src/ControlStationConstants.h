/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_GenericGUI/TA_BASE/transactive/app/system_control/control_station/src/ControlStationConstants.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/09/03 14:11:12 $
  * Last modified by:  $Author: CM $
  * 
  * It contains all constants used by the Control Station in general.
  *
  */

#include <string>

#ifndef CONTROL_STATION_CONSTANTS
#define CONTROL_STATION_CONSTANTS

// Custom Windows messages
#define WM_LAUNCH_DISPLAY                   (WM_USER + 200)
#define WM_SERVER_STATE_CHANGE              (WM_USER + 201)
#define WM_SHOW_TASKMANAGER                 (WM_USER + 202)
#define WM_LOGIN_DETAILS_CHANGED            (WM_USER + 203)
#define WM_HIDE_GRAPHWORX                   (WM_USER + 204)
#define WM_CREATE_DIALOGS                   (WM_USER + 205)
#define WM_FORCE_LOGOUT                     (WM_USER + 206)
#define WM_FORCE_END_OVERRIDE               (WM_USER + 207)
#define WM_SYSTEMCONTROLLER_STATE_CHANGE    (WM_USER + 208)
#define WM_SCADA_DISPLAY_CHANGE             (WM_USER + 209)
#define WM_CANCEL_CHILD                     (WM_USER + 210)
#define WM_FORCE_CANCEL                     (WM_USER + 211)
#define WM_TA_TO_GX_MESSAGE                 (WM_USER + 212)
#define WM_GX_TO_TA_MESSAGE                 (WM_USER + 213) //DP-changes

// Messages to display dialog boxes
#define WM_DISPLAY_LOGIN_DIALOG             (WM_USER + 220)
#define WM_DISPLAY_OVERRIDE_DIALOG          (WM_USER + 221)
#define WM_DISPLAY_CHANGE_PROFILE_DIALOG    (WM_USER + 222)
#define WM_LOGOUT                           (WM_USER + 223)
#define WM_DISPLAY_CHANGE_OPERATOR_DIALOG   (WM_USER + 224)
#define WM_DISPLAY_CHANGE_PASSWORD_DIALOG   (WM_USER + 225)
#define WM_DISPLAY_LAUNCH_APPLICATION_ERROR (WM_USER + 226)
#define	WM_DISPLAY_CHANGE_DISPLAY_MODE_DIALOG (WM_USER + 227)
// Duty related messages
#define WM_DUTY_CHANGE                      (WM_USER + 230)
#define WM_DUTY_REQUEST                     (WM_USER + 231)
#define WM_DUTY_RESPONSE                    (WM_USER + 232)



// Since this is a constant file that may be used by other applications we use
// two levels of namespace. This should ensure clashes do not happen.

namespace TA_Base_App
{
	namespace TA_ControlStation
	{
        // The command line to register and deregister the ControlStation and
        // TransActive ControlStation servers
        static const std::string REGISTER_CMD = "--install";
        static const std::string UNREGISTER_CMD = "--remove";

		// The format for the log file names
		static const std::string LOG_PRE_STRING = "Log_";
		static const std::string LOG_POST_STRING = ".log";

		// The separator used when multiple profiles and operators are logged in
		static const std::string NAME_SEPARATOR  = " / ";
		static const std::string LOCATION_PROFILE_SEPARATOR = "@";

		// The name of the application for message boxes
		static const char* APPLICATION_NAME = "Control Station";

		// The name of the AVI to use at startup
		static const std::string AVI_FILE = "TransActiveIntro.avi";

		// RunParams used only by the Control Station
		static const std::string DEBUG_LOG_DIR = "DebugLogDir";         // Online updatable
        static const std::string USER_PREFERENCES = "UserPreferences";  // Online updatable
		static const std::string HOST_NAMES = "HostNames";
		static const std::string PORT_NUMBER = "PortNumber";
		static const std::string ACCESS_CONTROL_POPUP_TIMEOUT = "AccessControlPopupTimeout";
        static const std::string BANNER_ENTITY_TYPE = "Banner";
        static const std::string DLLHOST_ENTITY_NAME = "TADllHost";

        // Minimum length of new password 
        static const int MIN_PASSWORD_LENGTH = 8;

        // Maximum length of password
        static const int MAX_PASSWORD_LENGTH = 50;

        // If the result of compare is equal to this, then the 2 strings used for comparison are the same
		static const int STRINGS_EQUIVALENT = 0;

        // Maximum number of shortcuts
        static const int MAX_SHORTCUT = 10;

        // The prefix of the user setting in the database
        static const char* SHORTCUT_NAME = "Shortcut";

        // The default prefix for the name of the shortcut
        static const char* DEFAULT_SHORTCUT_PREFIX = "New Shortcut ";

        // Maximum length of the shortcut name
        static const unsigned int SHORTCUT_NAME_MAX_LENGTH = 20;

        // Name of sound category for duty notifications
        static const std::string CS_DUTY_SOUND_CATEGORY = "CSDuty";

        // Priority of sound to use for duty notifications
        static const int CS_DUTY_SOUND_PRIORITY = 1;

		// Name of sound category for group online/offline notifications
        static const std::string GROUP_STATUS_SOUND_CATEGORY = "GroupStatus";

        // Priority of sound to use for group online/offline notifications
        static const int GROUP_ONLINE_SOUND_CATEGORY = 2;
		static const int GROUP_OFFLINE_SOUND_CATEGORY = 1;

        // The ProfileLoginDialog is used for getting input for login, change profile and operator override 
        // The enumerated type is used to indicate which mode the dialog should be switched to
		enum EDialogMode
		{
			LOGIN_MODE,			    // Dialog is to be used for logging in operators.
			CHANGE_PROFILE_MODE,	// Dialog is to be used to change the profile of an operator.
			OPERATOR_OVERRIDE_MODE, // Dialog is to be used as an operator override screen.
            CHANGE_OPERATOR_MODE,    // Dialog is to be used to change the operator.
			FORCEDTOCHANGEPASSWORD,
			CHANGE_DISPLAY_MODE,
			CHANGEPASSWORD
		};

        // The different ways that Control Station could terminate
        enum ETerminateMode
        {
            LOGOUT,                 // User is logging out
            EXIT,                   // Control station is exiting as requested by user
            RESTART,                // Restarting workstation
            SHUTDOWN                // Shutting down control station - similar to EXIT but this is done by control station itself
        };

        // Permission levels
        enum EPermission
        {
            DENIED,                 // User does not have the rights to perform an action on certain resource
            PERMITTED,              // User is allowed to perform an action on certain resource
            UNKNOWN,                // Insufficient attributes to determine the rights or a policy applying to the rights request does not exist
            GENERAL_ERROR           // Some error has occurred (the rights library throws some exception other than the RightsException)
        };

        // The operator's response to the Request Duty dialog
        enum EDutyResponse
        {
            ACCEPT,                 // User accepts the transfer of the requested duty
            DENY,                   // User rejects the transfer of the requested duty
            TIMEOUT                 // User fails to respond to the request within time limit
        };
		// The profile type
		enum EProfileShownType
		{
			LOCALONLY,	//only show local profiels
			CENTRALONLY	//only show central profiles
		};
	}
}

#endif // CONTROL_STATION_CONSTANTS
