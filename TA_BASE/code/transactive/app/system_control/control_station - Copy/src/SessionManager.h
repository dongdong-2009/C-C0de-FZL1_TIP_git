/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/SessionManager.h $
  * @author:  Karen Graham
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/06/03 17:28:56 $
  * Last modified by:  $Author: limin.zhu $
  * 
  * This class manages all session related tasks such as logging in, logging out
  * and communicating with the System Controller.
  */


#if !defined(SessionManager_B0D5AD53_1126_418b_90BE_D22186AE8DCE__INCLUDED_)
#define SessionManager_B0D5AD53_1126_418b_90BE_D22186AE8DCE__INCLUDED_

#include "app\system_control\control_station\src\ProfileLoginDlg.h"
#include "app\system_control\control_station\src\SessionDetails.h"
#include "app\system_control\control_station\src\ScreenPositioning.h"
#include "app\system_control\control_station\src\ControlStationConstants.h"
#include "app\system_control\control_station\src\DutyManager.h"

#include <string>
#include <list>
#include <vector>

#include "core\process_management\IDL\src\IProcessManagerCorbaDef.h"
#include "core\threads\src\Thread.h"
#include "core\configuration_updates\src\IOnlineUpdatable.h"
#include "core\message\src\SpecialisedMessageSubscriber.h"
#include "core\message\IDL\src\CommsMessageCorbaDef.h"
//
// Forward declare classes
//
class GraphWorxManager;
class IGraphWorxManager;
class ProcessManager;
class IProcessManager;
class ControlStationCorbaImpl;
class ProfileLoginDlg;
class CPasswordDlg;

namespace TA_Base_Core
{
    class IOperator;
    class IProfile;
    class ILocation;
    class ControlStation;
}

namespace TA_Base_Bus
{
	class AuthenticationLibrary;
	class RightsLibrary;
}

class SessionManager : public TA_Base_Core::Thread,
                       public TA_Base_Core::IOnlineUpdatable,
                       public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>

{
    friend class LoadBannerThread; // limin++ CL-21031
	typedef std::list<unsigned long> ResourceList;
public:

    /**
     * Destructor
     */
    virtual ~SessionManager();


    /**
     * getInstance
     *
     * This method makes this class a singleton. This should be called to retrieve an instance
     * of the class.
     */
	static SessionManager& getInstance();


	//TD17205,jianghp
	inline bool getDBUserPwdValid()
	{
		return m_dbUserPwdValid;
	};
	//TD17205,jianghp
    /**
     * cleanUp
     *
     * This method must be called to tidy up the Session Manager so everything is deleted
     * correctly. Because it is a singleton it cannot be destructed and this is the alternative
     *
     */
    static void cleanUp();


    /**
    * run
    *
	* This will block until terminate() is called.
    * NOTE: do not call this method directly. It needs to be public so that it can be called from
    * the thread-spawning function
	*/
	void run();


	/**
    * terminate
    *
	* The terminate() method will cause run() to return. 
    * NOTE: probably not a good idea to call this method directly - call terminateAndWait() instead
	*/
	void terminate();


    /**
     * login
     *
     * This method attempts to log an operator into the system. If the login is successful
     * it will then launch the required startup applications. (including GraphWorx).
     *
     * @param loginDlg - This instance of the login dialogue is used to retrieve the selected
     *                   details for the login from the actual dialogue
     *
     * @exception SessionException - This will be thrown if an error occurs or if authentication
     *                               fails. Check the fail type.
     */
    void login(const ProfileLoginDlg& loginDlg);


    /**
     * changeProfile
     *
     * This method attempts to change an operator's profile and propagate the change through to
     * all applications.
     *
     * @param loginDlg - This instance of the login dialogue is used to retrieve the selected
     *                   details for the login from the actual dialogue
     *
     * @exception SessionException - This will be thrown if an error occurs or if authentication
     *                               fails. Check the fail type.
     */
    void changeProfile(const ProfileLoginDlg& loginDlg);


    /**
     * changeOperator
     *
     * This method attempts to change an operator and propagate the change through to
     * all applications.
     *
     * @param loginDlg - This instance of the login dialogue is used to retrieve the selected
     *                   details for the login from the actual dialogue
     *
     * @exception SessionException - This will be thrown if an error occurs or if authentication
     *                               fails. Check the fail type.
     */
    void changeOperator(const ProfileLoginDlg& loginDlg);


    /**
     * operatorOverride
     *
     * This method attempts to override the current operator's login and profile, so 
	 * that the access rights of both operators are accessible.
     *
     * @param loginDlg - This instance of the login dialogue is used to retrieve the selected
     *                   details for the login from the actual dialogue
     *
     * @exception SessionException - This will be thrown if an error occurs or if authentication
     *                               fails. Check the fail type.
     */
	void operatorOverride(const ProfileLoginDlg& loginDlg);


    /**
     * changePassword
     *
     * This method attempts to modify the current operator's password with a new password
     *
     * @param passwordDlg - This instance of the change password dialogue 
     *                   is used to retrieve the new and old password from 
	 *                   the actual dialogue
     *
     * @exception SessionException - This will be thrown if an error occurs or if authentication
     *                               fails. Check the fail type.
     */
	void changePassword(const CPasswordDlg& passwordDlg);


    /**
     * removeOperatorOverride
     *
     * This method will remove any operator overrides currently in place.
     *
     */
    void removeOperatorOverride();


    /**
     * logout
     *
     * This method will log the user out of the Control Station. It just sets the member
     * variable m_isLoggedIn to false. The applications are terminated from the run() method.
     *
     */
    void logout();


    /**
     * isShutdownPermitted
     * 
     * This method returns true if the current operator is allowed to exit or shutdown
     * from the control station, terminate all TransActive GUI applications and
     * exit to the Windows explorer
     *
     * @param loginDlg - This instance of the login dialogue is used to retrieve the selected
     *                   details for the login from the actual dialogue
     *
     * @return EPermission Returns TA_Base_App::TA_ControlStation::DENIED if permission is denied; 
     *         TA_Base_App::TA_ControlStation::PERMITTED if permitted;
     *         TA_Base_App::TA_ControlStation::UNKNOWN if it's either indeterminate or not applicable
     *
     * @exception SessionException - This will be thrown if an error occurs or if authentication
     *                               fails. Check the fail type.
     */
    TA_Base_App::TA_ControlStation::EPermission isShutdownPermitted(const ProfileLoginDlg& loginDlg);


    /**
     * getProcessManager
     *
     * This returns a pointer to an IProcessManager object. It will return NULL if no
     * operator is logged on.
     *
     * @return IProcessManager* - Returns NULL if noone is logged on.
     *                            Returns a pointer to the object if it exists and a user is
     *                            logged in.
     */
    IProcessManager* getProcessManager();


    /**
     * getGraphWorxManager
     *
     * This returns a pointer to an IGraphWorxManager object. It will return NULL if no
     * operator is logged on.
     *
     * @return IGraphWorxManager* - Returns NULL if noone is logged on.
     *                            Returns a pointer to the object if it exists and a user is
     *                            logged in.
     */
    IGraphWorxManager* getGraphWorxManager();

    
    /**
     * getSessionDetails
     *
     * @return SessionDetails& - The session details object.
     */
    SessionDetails& getSessionDetails()
    {
        return m_sessionDetails;
    }


    DutyManager& getDutyManager()
    {
        return *m_dutyManager;
    }


    /**
     * getScreenPositioning
     *
     * @return ScreenPositioning& - The screen positioning object to retrieve set-up details.
     */
    ScreenPositioning& getScreenPositioning()
    {
        return m_screenPositions;
    }


    /**
     * getControlStationEntity
     *
     * This returns a reference to the Control Station entity
     *
     * @return ControlStation& - The entity holding the configuration information for this Control Station
     */
    TA_Base_Core::ControlStation& getControlStationEntity()
    {
        TA_ASSERT(m_controlStationEntity != NULL, "Something is trying to retrieve the Control Station entity before it has been loaded");
        
        return *m_controlStationEntity;
    }
	

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
     * isControlPermitted
     *
     * This method is used to validate operator's permission to perform the specified control station actions
     *
     * @param unsigned long The action constant
     *
     * @return EPermission Returns TA_Base_App::TA_ControlStation::DENIED if permission is denied; 
     *         TA_Base_App::TA_ControlStation::PERMITTED if permitted;
     *         TA_Base_App::TA_ControlStation::UNKNOWN if it's either indeterminate or not applicable
     */
    TA_Base_App::TA_ControlStation::EPermission isControlPermitted(unsigned long action);


    /**
     * isActionPermitted
     *
     * This method is used to validate operator's permission to perform the specified action
     *
     * @param unsigned long The action constant
     * @param unsigned long The resource key of the item that operator wishes to perform action on
     *
     * @return EPermission Returns TA_Base_App::TA_ControlStation::DENIED if permission is denied; 
     *         TA_Base_App::TA_ControlStation::PERMITTED if permitted;
     *         TA_Base_App::TA_ControlStation::UNKNOWN if it's either indeterminate or not applicable
     */
    TA_Base_App::TA_ControlStation::EPermission isActionPermitted(unsigned long action, unsigned long resourceKey);


    /** 
     * isRestartAccessControlled
     *
     * Returns true if restart should be access controlled
     *
     * @return true if restart should be access controlled
     */
    bool isRestartAccessControlled() const;


    /** 
     * isExitAccessControlled
     *
     * Returns true if exit should be access controlled
     *
     * @return true if exit should be access controlled
     */
    bool isExitAccessControlled() const;


    /** 
      * isSystemControllerGroupOnline
      *
      * Returns true if the system controller group is online i.e. system
      * is not in degraded mode.
      *
      * @return true if the system controller group is online 
      */
    bool isSystemControllerGroupOnline() const
    {
        return m_sCGroupOnline;
    }


    /** 
      * getGraphWorxHelpfile
      *
      * Returns the path graphworx's help file
      *
      * @return the path graphworx's help file
      */
    std::string getGraphWorxHelpfile() const
    {
        return m_graphWorxHelp;
    }


	/**
	* receiveSpecialisedMessage
	*
	* This is overriden by the client to receive their specialise message
	* 
	* @param	T	The narrowed Message
	*/
    virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

	/**
	* processTAToGxMessage
	*
	* This processes the TA_Base_Core::ControlStationComms::TAToGXMessageNotification messages
	* 
	* @param	T	The narrowed Message
	*/
	virtual void processTAToGxMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

	/**
	* processGxToTAMessage
	*
	* This processes the TA_Base_Core::ControlStationComms::GXToTAMessageNotification messages
	* 
	* @param	T	The narrowed Message
	*/
	virtual void processGxToTAMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

	virtual void processLaunchApplicationMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

	void sendLaunchApplicationACK(unsigned long locationKey, unsigned long appType, bool bSuccess);
    /**
     * exit
     *
     * This method is called when the Session Manager is shut down after a normal session (i.e.
     * not when initialisation fails) so the required notifications can be sent.
     *
     * @param isRestart Set this to true if this is exiting due to restart
     */
    void exit(const bool isRestart = false);


    /** 
      * canLoginAutomatically
      *
      * Returns true if the control station could login automatically using the control 
      * station entity.
      *
      * @return true if it can auto login
      */
    bool canLoginAutomatically()
    {
        if (m_controlStationEntity != NULL)
        {
            return m_controlStationEntity->canLoginAutomatically();
        }
        return false;
    }


	/**
	  * processGroupStateRunParam
	  * 
	  * Sets the m_sCGroupOnline flag when updated via RunParams
	  *
	  * @param isGroupOnline True is group is online
	  */
	void processGroupStateRunParam(const bool isGroupOnline);


    /** 
      * renewSession
      *
      * Request a new session using the details from the last active session before the network goes down.
      * Does not do anything if no operator was logged in before the network disconnection.
      *
      */
    void renewSession();


    /** 
      * exitNeedsRestart
      *
      * Returns true if the workstation needs to be restarted
      *
      *
      * @return true if the workstation needs to be restarted
      */
    static bool exitNeedsRestart()
    {
        return s_exitNeedsRestart;
    }
    
	TA_Base_Bus::SessionInfo getSessionInfo(const std::string & sessionId);

	void areActionsPermitted(unsigned long action, ResourceList& resourceKeyList,ResourceList& permittedResourceKeyList);
	
private:

    //
    // All constructors etc are made private as this is a singleton
    //
    SessionManager( );
    SessionManager( const SessionManager& theSessionManager){ };
    SessionManager& operator=(const SessionManager&){ };


    /** 
      * login
      *
      * Logs with the specified values.
      *
      * @param selectedOperator
      * @param selectedProfile
      * @param selectedLocation
      * @param password
      *
      * @exception SessionException - This will be thrown if an error occurs or if authentication
      *                               fails. Check the fail type.
      */
    void login(TA_Base_Core::IOperator* selectedOperator,
              TA_Base_Core::IProfile* selectedProfile,
              TA_Base_Core::ILocation* selectedLocation,
              CString& password);


    /**
     * locateSystemController
     *
     * This attempts to connect to the System Controller using the list of host names and the
     * port specified on the command line.
     *
     * NOTE: This should be changed so that it continues to try
     *
     * @exception SessionException - This will be thrown if the application find the System Controller
     *                               or cannot narrow it correctly.
     */
	void locateSystemController();

    
    /**
     * retrieveRunParams
     *
     * This attempts to retrieve the runtime parameters from the System Controller
     *
     * @param hostName - The name of the host this application is running on. This is needed for the
     *                   call to getParams on the system controller.
     *
     * @exception SessionException - This will be thrown if the application cannot retrieve the parameters.
     */
    void retrieveRunParams(const std::string& hostName);


    /**
     * loadBanners
     *
     * This will load the banners for the top of the screen. This is currently hardcoded but should later
     * be changed to be configurable. (TODO)
     */
    void loadBanners();
    void loadDllHost();


    /**
     * requestSession
     *
     * This method is used to request a session from the authentication library
     */
    void requestSession(TA_Base_Core::IOperator*& selectedOperator,
                          TA_Base_Core::IProfile*& selectedProfile,
                          TA_Base_Core::ILocation*& selectedLocation,
                          CString& password);

private:

    // This indicates if a user is currently logged in. If they are logged in then all the
    // session information below is valid. This is modified in the methods login() and logout().
    bool m_isLoggedIn;

    // This holds all details about the current session such as operator, profile etc.
    SessionDetails m_sessionDetails;

    // This calculates and holds all the screen positioning and layout information.
    ScreenPositioning m_screenPositions;

    // The resource key of the control station
    unsigned int m_resourceKey;

    // Indicate whether the system controller group is online.
    volatile bool m_sCGroupOnline;

    // The host name of the console
    std::string m_hostName;

    // The name of the system controller that we're connected to
    std::string m_systemControllerName;

    // Path to the context sensitive graphworx helpfile
    std::string m_graphWorxHelp;

    // This is a pointer to the actual system controller.
    TA_Base_Core::IProcessManagerCorbaDef_var m_systemController;

    // This is the list of banners the Control Station will need to load and position around.
    std::vector<TA_Base_Core::IEntityData*> m_bannerEntities;
    TA_Base_Core::IEntityData* m_dllHostEntity;

    // The entity for this Control Station.
    TA_Base_Core::ControlStation* m_controlStationEntity;

    // The entity for system controller that we're connected to
    TA_Base_Core::IEntityData* m_systemControllerEntity;

    // Control Station servant that allows applications to talk to this application via corba
    ControlStationCorbaImpl* m_controlStationImpl;

    // Responsible for managing the applications
	ProcessManager* m_processManager;

    // Responsible for managing the graphworx instances
    GraphWorxManager* m_graphWorxManager;

    // This is used to perform all authentication
	TA_Base_Bus::AuthenticationLibrary* m_authenticationMgr;

    // This is used to check operator's rights to perform certain actions
    TA_Base_Bus::RightsLibrary* m_rightsMgr;

    // This object handles all duty logics
    DutyManager* m_dutyManager;

    static SessionManager* s_instance;

	// protect singleton creation
	static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

    static const std::string CONSOLE_NAME;
	static const std::string PROFILE_NAME;
    static const std::string USER_NAME;
	static const std::string LOGIC_LOCATION;
    static const std::string GROUP_ONLINE; // This string is used in the RunParams to indicate when no group is offline

    // True if the workstation needs to be restarted when control station exits.  
    // Defaults to false.
    static bool s_exitNeedsRestart;

	//TD17205,jianghp
	static bool  m_dbUserPwdValid;
	//TD17205,jianghp
};

#endif // !defined(SessionManager_B0D5AD53_1126_418b_90BE_D22186AE8DCE__INCLUDED_)
