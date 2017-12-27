/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/SessionDetails.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class holds all session details about the current login as well as the lists of
  * operators, profiles etc.
  */


#if !defined(SessionDetails_B0D5AD53_1126_418b_90BE_D22186AE8DCE__INCLUDED_)
#define SessionDetails_B0D5AD53_1126_418b_90BE_D22186AE8DCE__INCLUDED_

#include "app\system_control\control_station\src\ControlStationConstants.h"

#include <string>
#include <vector>
#include <map>

#include "core\configuration_updates\src\IOnlineUpdatable.h"
#include "core\message\src\NameValuePair.h"
#include "bus/security/authentication_library/src/SessionInfo.h"
#include "bus/security/authentication_agent/idl/src/IAuthenticationAgentCorbaDef.h"


//
// Forward declare classes
//
namespace TA_Base_Core
{
    class IOperator;
    class IConsole;
    class IProfile;
    class ILocation;
}

class SessionDetails : public TA_Base_Core::IOnlineUpdatable
{
public:

    typedef std::vector<TA_Base_Core::IOperator*> OperatorVector;
    typedef std::vector<TA_Base_Core::IProfile*> ProfileVector;
    typedef std::vector<TA_Base_Core::ILocation*> LocationVector;

    typedef std::map<unsigned long, TA_Base_Core::ILocation*> LocationMap;
    typedef std::map<unsigned long, TA_Base_Core::IProfile*> ProfileMap;

    struct SessionInfo
    {
        unsigned long operatorId;               // The user Id
        unsigned long profileId;                // The profile Id
        unsigned long locationId;               // The location Id
        unsigned long consoleId;                // The workstation Id
        std::string password;                   // The password for login operator

        bool isOverriden;                       // true if there is an override
        unsigned long overrideOperatorId;       // The user Id of the overriding operator
        unsigned long overrideProfileId;        // The overriding profile Id 
        std::string overridePassword;           // The password for overriding operator

        SessionInfo()
            : operatorId(0), profileId(0), locationId(0), consoleId(0), password(""), isOverriden(false), overrideOperatorId(0), overrideProfileId(0), overridePassword(""){}
    };



    /**
     * Constructor
     */
    SessionDetails( );


    /**
     * Destructor
     */
    virtual ~SessionDetails();


    /**
     * initialise
     *
     * This will be called once a database connection string has been established and
     * we will now be able to load the necessary data.
     * We must use 2-stage initialisation for this object so that we can create an instance
     * of it straight away (as it will still do stuff) and once we have established a
     * database connection the final initialisation can be done.
     *
     * @param hostName - The host name of the machine this application is running on
     */
    void initialise(const std::string& hostName);


    /**
     * sessionLogin
     *
     * This method sets up the member variables for a new session login with the
     * specified details.
     * PreCondition: The operator and profile are not NULL
     *               The session ID is not ""
     * 
     * @param string - The ID of the new session as a string
     * @param IOperator* - The operator logging in
     * @param IProfile*  - The profile being used to log in as. This can be NULL.
     * @param ILocation*  - The location being used to log in as. This can be NULL.
     *
     * @exception SessionException - Thrown if a session cannot be set up. This will
     *                               most probably be a database error.
     */
    void sessionLogin( std::string sessionId,
		               TA_Base_Core::IOperator* theOperator,
                       TA_Base_Core::IProfile*  theProfile,
                       TA_Base_Core::ILocation*  theLocation);


    /** 
      * renewSession
      *
      * Updates the session Id and runtime parameter
      *
      * @param string - The ID of the new session as a string
      */
    void renewSession(std::string sessionId);


    /**
     * sessionLogout
     *
     * This clears all session information ready to start again
     */
    void sessionLogout();


    /**
     * profileChanged
     *
     * This method changes the member variables holding the profile and location
     * to the new values passed in.
     * PreCondition: The profile passed is not NULL.
     *               The session ID is not ""
     *
     * @param string - The ID of the new session as a string
     * @param IProfile*  - The profile being used to log in as
     * @param ILocation*  - The location being used to log in as. This can be NULL.
     *
     * @exception SessionException - Thrown if a new profile could not be created.
     */
    void profileChanged( std::string sessionId, 
                         TA_Base_Core::IProfile* theProfile,
                         TA_Base_Core::ILocation* theLocation);


    /**
     * operatorOverride
     *
     * This method sets up the member variables for a new operator override with the
     * specified details.
     * PreCondition: The operator and profile are not NULL
     * 
     * @param IOperator* - The operator logging in to override
     * @param IProfile*  - The profile being used to override. This can be NULL
     * @param ILocation*  - The location being used to override. This can be NULL.
     *
     * @exception SessionException - Thrown if a new profile could not be created.
     */
    void operatorOverride( TA_Base_Core::IOperator* theOperator,
                           TA_Base_Core::IProfile*  theProfile,
                           TA_Base_Core::ILocation*  theLocation);


    /**
     * removeOperatorOverride
     *
     * This will remove an operator override and reset the member variables to NULL.
     */
    void removeOperatorOverride();


    /**
     * getListOfOperators
     *
     * @return OperatorVector& - This vector contains all the operators in the database.
     */
    OperatorVector& getListOfOperators()
    {
        return m_allOperators;
    }


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
    ProfileVector getListOfProfiles(TA_Base_Core::IOperator* theOperator, const bool onlyOverridable = false);


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
     * getOperatorName
     *
     * Returns the logged in operator's name, including possibly the overridden operator.
     *
     * @return string - Returns "" if no operator is logged in
     *                  Returns "Unknown" if it cannot determine the operator name
     *                  Otherwise returns the operator name(s).
     */
    std::string getOperatorName();

    
    /** 
      * getSessionInfo
      *
      * Returns the session information
      *
      *
      * @return the session information
      */
    SessionInfo getSessionInfo();


    /**
     * getOperatorDescription
     *
     * Returns the logged in operator's description
     *
     * @return string - Returns "" if no operator is logged in
     *                  Returns "Unknown" if it cannot determine the operator name
     *                  Otherwise returns the operator description.
     */
    std::string getOperatorDescription();


    /**
     * getOverrideOperatorName
     *
     * Returns the override operator's name
     *
     * @return string - Returns "" if no operator is logged in
     *                  Returns "Unknown" if it cannot determine the operator name
     *                  Otherwise returns the operator name(s).
     */
    std::string getOverrideOperatorName();


    /**
     * getOverrideOperatorDescription
     *
     * Returns the override operator's description
     *
     * @return string - Returns "" if no operator is logged in
     *                  Returns "Unknown" if it cannot determine the operator name
     *                  Otherwise returns the operator description.
     */
    std::string getOverrideOperatorDescription();


    /**
     * getProfileName
     *
     * Returns the current profile the operator is using.
     *
     * @return string - Returns "" if no profile is specified
     *                  Returns "Unknown" if it cannot determine the profiles
     *                  Otherwise returns the profile(s)
     */
    std::string getProfileName();


    /**
     * getOverrideProfileName
     *
     * Returns the override profile the operator is using
     *
     * @return string - Returns "" if no profile is specified
     *                  Returns "Unknown" if it cannot determine the profiles
     *                  Otherwise returns the profile
     */
	std::string getOverrideProfileName();


    /**
     * getProfileAndLocationDisplayString
     *
     * Returns the current profile and if that profile is associated with a location then
     * this returns the profile and location joined together.
     *
     * @return string - Returns "" if no profile is specified
     *                  Returns "Unknown" if it cannot determine the profiles
     *                  Otherwise returns the profile(s)
     */
    std::string getProfileAndLocationDisplayString();


    /**
     * getOverrideProfileAndLocationDisplayString
     *
     * Returns the override profile and if that profile is associated with a location then
     * this returns the profile and location joined together.
     *
     * @return string - Returns "" if no profile is specified
     *                  Returns "Unknown" if it cannot determine the profiles
     *                  Otherwise returns the profile(s)
     */
    std::string getOverrideProfileAndLocationDisplayString();

    
    /**
     * getProfileDisplay
     *
     * Returns the display associated with the current profile
     *
     * @param unsigned long - This indicates which screen the display is for.
     *                        This starts at screen 1 and increases from there.
     *
     * @return string - Returns "" if no profile is specified
     *                  Returns "Unknown" if if cannot determine the display
     *                  Otherwise returns the path for the display
     */
    std::string getProfileDisplay(unsigned long screenNumber);


	/**
     * getProfileDisplayAtLocation
     *
     * Returns the display associated with the current profile at the specified location
     *
     * @param unsigned long - This indicates which screen the display is for.
     *                        This starts at screen 1 and increases from there.
	 * @param const std::string - This indicates the location name.                        
     *
     * @return string - Returns "" if no profile is specified
     *                  Returns "Unknown" if if cannot determine the display
     *                  Otherwise returns the path for the display
     */
    std::string getProfileDisplayAtLocation(unsigned long screenNumber, const std::string& locationName);


    /**
     * getLocationName
     *
     * Returns the location the operator is working at. This is the location the user picked when they
     * logged in.
     *
     * @return string - Returns "" if no location is specified
     *                  Returns "Unknown" if it cannot determine the location
     *                  Otherwise returns the location name
     */
    std::string getLocationName();


    /**
     * getLocationKey
     *
     * Returns the location the operator is working at. This is the location the user picked when they
     * logged in.
     *
     * @return unsigned long  - The key of the location at which this session is logged in.
     */
    unsigned long getLocationKey();


    /**
     * getConsoleName
     *
     * Returns the name of the console the user is working on.
     *
     * @return string - Returns "" if no console is specified
     *                  Returns "Unknown" if it cannot determine the console
     *                  Otherwise returns the console name
     */
    std::string getConsoleName();
    

	/**
	 * getConsoleId
	 *
	 * Returns unsigned long - The current session Id.
	 *
	 * @return unsigned long - The current session Id.
	 */
	unsigned long getConsoleId();


	 /**
     * getServerLocation
     *
     * Returns the key of the location configured for the server.
     *
     * @return unsigned long The location key.
     */
	unsigned long getServerLocation();
    /**
     * getConsoleLocationId
     *
     * Returns the key of the location configured for the console.
     *
     * @return unsigned long The location key.
     */
    unsigned long getConsoleLocationId();

    
    /**
     * getConsoleLocationName
     *
     * Returns the location the operator is logged into. This is the physical location of the user and
     * is retrieved from the console
     *
     * @return string - Returns "" if no location is specified
     *                  Returns "Unknown" if it cannot determine the location
     *                  Otherwise returns the location name
     */
    std::string getConsoleLocationName();


    /**
	 * isOperatorOverridden
	 *
	 * Returns true if in operator overridden mode.
	 *
	 * @return bool true if in operator overridden mode, and false otherwise.
	 */
	bool isOperatorOverridden() const
	{
		return m_isOverrideValid;
	}


	/**
	 * getSessionId
	 *
	 * @return std::string - The current session Id. "" if there is no current session.
	 *
	 * @return std::string - The current session Id. "" if there is no current session.
	 */
	std::string getSessionId();


	/**
      * processUpdate
      *
	  * This tells the application that one of the objects it was interested in has
      * been updated and should be processed.
      *
	  * @param    updateEvent - This event contains all the information about the update
	  */
    virtual void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);


	/**
      * processOperatorUpdate
      *
	  * This method updates the operator list.
      *
	  * @param    updateEvent - This event contains all the information about the update
	  */
    void processOperatorUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);


	/**
      * processLocationUpdate
      *
	  * This method updates the location list.
      *
	  * @param    updateEvent - This event contains all the information about the update
	  */
    void processLocationUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);


	/**
      * processProfileUpdate
      *
	  * This method updates the profile list.
      *
	  * @param    updateEvent - This event contains all the information about the update
	  */
    void processProfileUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);


    /** 
      * hasLocationUpdated
      *
      * Returns true if the location has been updated.  
      *
      * @return true if location has been updated
      */
    bool hasLocationUpdated() const
    {
        return m_hasLocationUpdated;
    }


    /** 
      * hasOperatorUpdated
      *
      * Returns true if the operator has been updated.  
      *
      * @return true if operator has been updated
      */
    bool hasOperatorUpdated() const
    {
        return m_hasOperatorUpdated;
    }


    /** 
      * hasProfileUpdated
      *
      * Returns true if the profile has been updated.  
      *
      * @return true if profile has been updated
      */
    bool hasProfileUpdated() const
    {
        return m_hasProfileUpdated;
    }


    /** 
      * resetOperatorUpdateFlag
      *
      * This method should be called to reset the flag after the corresponding UI 
      * has been updated (to prevent the UI being updated unnecessarily)
      */
    void resetOperatorUpdateFlag()
    {
        m_hasOperatorUpdated = false;
    }   


    /** 
      * resetLocationUpdateFlag
      *
      * This method should be called to reset the flag after the corresponding UI 
      * has been updated (to prevent the UI being updated unnecessarily)
      */
    void resetLocationUpdateFlag()
    {
        m_hasLocationUpdated = false;
    }   


    /** 
      * resetProfileUpdateFlag
      *
      * This method should be called to reset the flag after the corresponding UI 
      * has been updated (to prevent the UI being updated unnecessarily)
      */
    void resetProfileUpdateFlag()
    {
        m_hasProfileUpdated = false;
    }   


    /** 
      * isSessionValid
      *
      * Returns true if there is a valid and active session running.
      *
      * @return true if there is a valid and active session running.
      */
    bool isSessionValid() const
    {
        return m_isSessionValid;
    }


	/**
	 * convertULtoString
	 *
	 * Returns string - The string representation of the unsigned long
	 *
     * @param unsigned long - The number to be converted
     *
	 * @return string - The string representation of the unsigned long
	 */
    static std::string convertULtoString(unsigned long num);

	TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef toCorbaSessionInfo();

private:

    //
    // These are made private as we do not need them.
    //
    SessionDetails( const SessionDetails& sd){ };
    SessionDetails& operator=(const SessionDetails&){ };

    
    // This is a list of all operators, profiles and locations. 
    OperatorVector      m_allOperators;
    ProfileMap          m_profileMap;
    LocationMap         m_locationMap;

    //
    // This is all the information about the current session
    //
	std::string          m_sessionId;
    TA_Base_Core::IOperator*  m_operator;
	TA_Base_Core::IConsole*   m_console;
    TA_Base_Core::IProfile*   m_profile;
    TA_Base_Core::ILocation*  m_location;
    TA_Base_Core::ILocation*  m_consoleLocation;

	// The operator and profile if in operator override mode. If not in 
	// operator override mode, these are set to NULL.  
    // The location always has to be the same as login location, so it is not required here
	TA_Base_Core::IOperator*  m_operatorOverride;
	TA_Base_Core::IProfile*   m_profileOverride;

    // boolean to indicate if the operator/location/profile has been online updated 
    bool m_hasOperatorUpdated;
    bool m_hasLocationUpdated;
    bool m_hasProfileUpdated;

    bool m_isSessionValid;
    bool m_isOverrideValid;

    // Boolean to indicate whether the default display should be updated.  Should update
    // the display when there is a change in operator or profile.
    bool m_shouldChangeDisplay;
};

#endif // !defined(SessionDetails_B0D5AD53_1126_418b_90BE_D22186AE8DCE__INCLUDED_)
