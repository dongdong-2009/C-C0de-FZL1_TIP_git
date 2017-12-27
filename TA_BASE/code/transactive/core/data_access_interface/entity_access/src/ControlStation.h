/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/ControlStation.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ControlStation holds the data specific to a ControlStation entity from the
  * database and allows read-only access to that data.
  *
  */

#if !defined(ControlStation_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
#define ControlStation_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_

#include <map>
#include <string>
#include <vector>
#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class IOperator;
    class IProfile;
    class ILocation;

	typedef struct
	{
		long left;
		long top;
		long right;
		long bottom;
	} TA_RECT;

    class ControlStation : public EntityData
    {
    public:

        enum BannerPosition
        {
            NEITHER,    // This is the default and is used if NO_SCREENS is specified for the banner location
            TOP,        // Banner should be placed at the top of the screen
            BOTTOM      // Banner should be placed at the bottom of the screen
        };

        enum LaunchBarPosition
        {
            HIDDEN,  // This is the default and is used if NO_SCREENS is specified for the launch bar location
            LEFT,    // Launch bar should be placed on the left of the screen
            RIGHT    // Launch bar should be placed to the right of the screen
        };

        static const unsigned long NO_SCREENS;  
        static const unsigned long ALL_SCREENS;
        static const unsigned long FULL_SCREENS;
		static const unsigned long INVALID_TIMEOUT;

        /**
         * ControlStation (constructor)
         *
         * Constructs a new ControlStation object that contains only the static values. This
         * includes type, so the getType() method can be called without any further action.
         */
        ControlStation() {};

        /**
         * ControlStation (constructor)
         *
         * Constructs a new ControlStation object with the specified key.
         *
         * @param key The key to this ControlStation in the database.
         */
        ControlStation(unsigned long key);

        /**
         * Destructor
         */
        virtual ~ControlStation();


        /**
         * getDebugLogDir
         *
         * Returns the debug log dir set for the Control Station
         *
         * @return string The debug log dir. If this is not set an empty string will be
         *                returned
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */
        std::string getDebugLogDir();

        /**
         * getStartupDisplay
         *
         * Returns the startup GraphWorX display
         *
         * @return string A path to the startup display.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */
        std::string getStartupDisplay();

        /**
         * getBackgroundDisplay
         *
         * Returns the hidden, unframed background display that is used to launch visible displays
         *
         * @return string A path to the background display.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */
        std::string getBackgroundDisplay();

        /** 
         * getUserPreferences
         *
         * @return bool - Indicates whether user settings should be used when loading and positioning
         *                GUI applications.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */ 
        bool getUserPreferences();

        /** 
         * getGraphWorxScreen
         *
         * @return unsigned long - Returns the number of the screen GraphWorX should be
         *                         displayed on. This can also return the constants NO_SCREENS
         *                         indicating GraphWorX should not be launched at all or
         *                         ALL_SCREENS indication GraphWorX should be launched
         *                         multiple times and positioned on all screens.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */ 
        unsigned long getGraphWorxScreen();

        /** 
         * getGraphWorxHelpfile
         *
         * @return std::string -   Returns the path to the file for graphworx
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */ 
        std::string getGraphWorxHelpfile();

        /** 
         * getLaunchBarScreen
         *
         * @return unsigned long - Returns the number of the screen the launch bar should be
         *                         displayed on. This can also return the constants NO_SCREENS
         *                         indicating the launch bar should not be launched at all.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */ 
        unsigned long getLaunchBarScreen();

        /** 
         * getLaunchBarPosition
         *
         * @return LaunchBarPosition - Returns where the launch bar should be positioned. If the
         *                             LaunchBarScreen is set to NO_SCREEN then this will return HIDDEN.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */ 
        LaunchBarPosition getLaunchBarPosition();

        /** 
         * isRestartAccessControlled
         *
         * Returns true if restart is access controlled.
         *
         * @return true if restart is access controlled.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */
        bool isRestartAccessControlled();

        /** 
         * isExitAccessControlled
         *
         * Returns true if exit is access controlled.
         *
         * @return true if exit is access controlled.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */
        bool isExitAccessControlled();

        /** 
         * getExtSchematic
         *
         * Returns the file path to the schematic that is used by third party product.
         *
         * @return the file path to the schematic used by third party product.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */
        std::string getExtSchematic();

        /** 
         * getAccessControlPopupTimeout
         *
         * Returns the timeout (in seconds) that should be used to automatically close AccessControl popup requests.
         *
         * @return the timeout in seconds.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */
        unsigned long getAccessControlPopupTimeout();

        /** 
         * getApplicationsToTerminate
         *
         * Returns a comma delimited list of application names that the control station will terminate on exit.
         *
         * @return a string containing the comma delimited list of application names.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */
        std::string getApplicationsToTerminate();

        /** 
         * getApplicationsToTerminateOnLogout
         *
         * Returns a comma delimited list of application names that the control station will terminate on logout
         *
         * @return a string containing the comma delimited list of application names.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */
        std::string getApplicationsToTerminateOnLogout();

        /** 
         * getLoginOperator
         *
         * Returns the operator to be used for automatic login.  Returns NULL if not specified.
         *
         * @return the operator to be used for automatic login.  Returns NULL if not specified.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */
        IOperator* getLoginOperator();

        /** 
         * getLoginPassword
         *
         * Returns the password of the operator to be used for automatic login.
         *
         * @return the password of the operator to be used for automatic login.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */
        std::string getLoginPassword();

        /** 
         * getLoginProfile
         *
         * Returns the profile to be used for automatic login.  Returns NULL if not specified.
         *
         * @return the profile to be used for automatic login.  Returns NULL if not specified.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */
        IProfile* getLoginProfile();

        /** 
         * canLoginAutomatically
         *
         * Returns true if the entity contains enough login information to let the Control Station
         * log in automatically.
         *
         * @return true if the entity has sufficient login information.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */
        ILocation* getLoginLocation();
        
        /** 
         * canLoginAutomatically
         *
         * Returns true if the entity contains enough login information to let the Control Station
         * log in automatically.
         *
         * @return true if the entity has sufficient login information.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */
        bool canLoginAutomatically();        


        /** 
         * shouldShowExitButton
         *
         * Returns true the entity parameter has been set to indicate that the exit button
         * should be shown.
         *
         * @return true if the exit button should be shown, false otherwise
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */
        bool shouldShowExitButton();

        
        /** 
         * shouldShowRestartButton
         *
         * Returns true the entity parameter has been set to indicate that the restart button
         * should be shown.
         *
         * @return true if the restart button should be shown, false otherwise
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */
        bool shouldShowRestartButton();


        /** 
         * shouldShowLogoutButton
         *
         * Returns true the entity parameter has been set to indicate that the logout button
         * should be shown.
         *
         * @return true if the logout button should be shown, false otherwise
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */
        bool shouldShowLogoutButton();

        
        /** 
         * getPingTimeoutSeconds
         *
         * Returns the maximum length of time to wait after sending ping before timing out.
         *
         * @return timeout in seconds
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */
        unsigned int getPingTimeoutSeconds();

        
        /** 
         * getPingRetries
         *
         * Returns the number of times to ping a server without success before officially
         * declaring it unpingable.
         *
         * @return number of retries
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */
        unsigned int getPingRetries();

        /** 
         * getCorbaRetries
         *
         * Returns the number of times when corba failures occur before give up.
         *
         * @return number of retries
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular entity parameter cannot be 
         *            converted to the required data type.
         */
        unsigned int getCorbaRetries();

        /**
         * getType
         *
         * Returns the type of this entity. This method must be callable when the object
         * is constructed using the default constructor. The entity type string should be
         * stored as a static const variable in the concrete class. The entity type should
         * not be loaded from the database (note that this method defines no exceptions).
         *
         * @return The entity type of this entity as a std::string
         */
        std::string getType();
		static std::string getStaticType();

        /**
         * clone
         *
         * Returns a pointer to a new ControlStation object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the ControlStation object.
         */
        static IEntityData* clone(unsigned long key);
        
        /**
         * invalidate
         *
         * Mark the data contained by this entity as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

        /** 
          * isDisableCAD
          *
          * description, to check the configuration of value DisableCAD in database
          *
          *
          * @return, 
          */
		bool isDisableCAD();

		//TD16491++
		/**
         * getMmsSubmitterType
         *
         * Returns the mms submitter type to be used for config plugin
         *
         * @return std::string The alarmrule mms ruletype column(i.e. MMS or MIS or other things the client specifies)
         *
         */
		std::string getMmsSubmitterType();
		//++TD16491
		
		// Raymond Pau++ TD16471
		/**
         * isScreenSizeSpecified
		 *
         * @param screen The screen number
		 *
         * Used to check if a custom size has been specified for the specified screen
         *
         * @return true is custom size specified; otherwise false
         */
		bool isScreenSizeSpecified(unsigned int screen);

		/**
         * getScreenSize
		 *
         * @param screen The screen number
		 *
         * Used to retrieve the specified screen size and position
         *
         * @return The specified screen TA_RECT
         */
		TA_RECT getScreenSize(unsigned int screen);
		// ++Raymond Pau TD16471

    protected:

        /**
         * reload
         *
         * (Re)loads the parameter data from the database.
         *
         * @exception DatabaseException If there is a database error.
         * @exception DataException If a parameter value cannot be converted to the required
         *            type
         */
        void reload();

    private:
        /**
         * convertParameterToScreen
         *
         * This will retrieve the specified parameter and convert it to either a screen
         * number or one of the constants NO_SCREENS or ALL_SCREENS
         *
         * @param IData& - The data object to retrieve the parametervalue from.
         * @param int - The index of the item to retrieve from the data object
         *
         * @return unsigned long - This will return the screen number or one of the constants
         *                         NO_SCREENS or ALL_SCREENS.
         *
         * @exception DataException If a parameter value cannot be converted to the required type.
         */
        unsigned long convertParameterToScreen(IData& data, const int index);
		unsigned long convertParameterToScreen(std::string parameter);

        /**
         * convertParameterToLaunchBarPosition
         *
         * This will retrieve the specified parameter and convert it to one of the enumerations
         * in LaunchBarPosition.
         *
         * @param IData& - The data object to retrieve the parametervalue from.
         * @param int - The index of the item to retrieve from the data object
         *
         * @return LaunchBarPosition - This will parse the parametervalue and convert it into the
         *                             equivalent value in this enumerated type.
         *
         * @exception DataException If a parameter value cannot be converted to the required type.
         */
        LaunchBarPosition convertParameterToLaunchBarPosition(IData& data, const int index);
		LaunchBarPosition convertParameterToLaunchBarPosition(std::string parameter);
		// Raymond Pau++ TD16471
        /**
         * convertParameterToScreenSize
         *
         * This will parse the parameter and convert it into a TA_RECT
         *
         * @param parameterValue - a string in the format <<left>>,<<top>,<<right>>,<<bottom>> specifying
		 *                         the screen size
         *
		 * @return TA_RECT - windows equivalent of struct RECT
         */
		TA_RECT convertParameterToScreenSize( const std::string& parameterValue );
		// ++Raymond Pau TD16471

        /**
         * convertStringToUpperCase
         *
         * This will convert the passed in string to all upper case.
         *
         * @param std::string& - This string is converted and then reset. So the string passed in
         *                       will be modified once the method has completed.
         */
        void convertStringToUpperCase(std::string& strToConvert);
		std::map< unsigned int, TA_RECT > m_screenSizes;	// Raymond Pau++ TD16471


    private:
        //
        // Disable copy constructor and assignment operator
        //
        ControlStation( const ControlStation& theControlStation );
        ControlStation& operator=( const ControlStation& theControlStation );

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
        static const std::string DEBUG_LOG_DIR;
        static const std::string STARTUP_DISPLAY;
        static const std::string BACKGROUND_DISPLAY;
        static const std::string GRAPHWORX_SCREEN;
        static const std::string GRAPHWORX_HELP;
        static const std::string LAUNCH_BAR_SCREEN;
        static const std::string LAUNCH_BAR_POSITION;
        static const std::string USER_PREFERENCES;
        static const std::string ACCESS_CONTROL_RESTART;
        static const std::string ACCESS_CONTROL_EXIT;
        static const std::string EXT_SCHEMATIC;
		static const std::string POPUP_TIMEOUT;
        static const std::string TERMINATE_APPLICATIONS;
        static const std::string LOGOUT_APPLICATIONS;
        static const std::string OPERATOR_ID;
        static const std::string PASSWORD;
        static const std::string PROFILE_ID;
        static const std::string LOCATION_ID;
        static const std::string SHOW_RESTART;
        static const std::string SHOW_EXIT;
        static const std::string SHOW_LOGOUT;
        static const std::string PING_TIMEOUT_SECONDS;
        static const std::string PING_RETRIES;
        static const std::string CORBA_RETRIES;
		static const std::string DISABLE_CAD;

		//TD16491++
		static const std::string MMS_SUBMITTER_TYPE;
		static const std::string SCREEN_SIZE_PREFIX;	// Raymond Pau++ TD16471

        static const unsigned long NO_OPERATOR;
        static const unsigned long NO_PROFILE;
        static const unsigned long NO_LOCATION;
    };
    
    typedef boost::shared_ptr<ControlStation> ControlStationPtr;
} //close namespace TA_Base_Core

#endif // !defined(ControlStation_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
