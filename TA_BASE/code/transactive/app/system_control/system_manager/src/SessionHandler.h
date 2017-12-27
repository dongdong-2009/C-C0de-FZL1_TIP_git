/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_manager/src/SessionHandler.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is a replacement for GenericGUI. If the Control Station is not running
  * then this class performs similar functionality to GenericGUI (minus the
  * requirement on the Control Station).
  */


#if !defined(SessionHandler_0900475D_E824_4a2e_8F26_0E2336137178__INCLUDED_)
#define SessionHandler_0900475D_E824_4a2e_8F26_0E2336137178__INCLUDED_

#include "ISessionChecker.h"

#include "core/process_management/IDL/src/IProcessManagerCorbaDef.h"

#include "core/utilities/src/RunParams.h"

namespace TA_Base_Core
{
    class IEntityData;
}

namespace TA_Base_Bus
{
    class IGUIApplication;
	class AuthenticationLibrary;
    class RightsLibrary;
}

namespace TA_Base_App
{
    static const std::string RPARAM_HOSTNAME = "HostName";
    static const std::string RPARAM_PORTNUMBER = "PortNumber";

    class SessionHandler : public virtual TA_Base_Core::RunParamUser, public virtual ISessionChecker
    {

    public:
        /** 
         * Constructor
         *
         * @param IGUIApplication& guiApp - This is a reference to a constructed GUI application
         *                                  object for GenericGUI to call.
         * @exception GenericGUIException - If this exception is thrown the caller should check
         *                                  the type and 'advertise' to the user in an appropriate
         *                                  manner.
         */
        SessionHandler(TA_Base_Bus::IGUIApplication& gui);


        /**
         * Destructor
         */
	    virtual ~SessionHandler();


        /**
         * run
         * 
         * This starts the application and launches the GUI.
         *
         * @return This returns a boolean indicating whether the application should enter
         * the application message pump:
         *    True for Yes
         *    False for No
         *
         * @exception GenericGUIException
         */
        bool run();

      
        /**
         * onRunParamChange
         *
         * This tells the application when a RunParam has changed. This should check the
         * name to see that the parameter that has changed is the SESSIONID.
         *
         * @param string& name -  The name of the changed parameter
         * @param string& value - The new value for the parameter
         */
        void onRunParamChange(const std::string& name, const std::string& value);


        /**
         * checkUserPermissions
         *
         * This will check if this user has permission to run this application. If they do not then this
         * will throw an exception
         *
         * @exception GenericGUIException - This indicates that either the user did not have permission to run
         *                                  the application or there was an error attempting to retrieve the rights.
         */
        virtual void checkUserPermissions();


        /**
         * authenticate
         *
         * This attempts to authenticate the user with the TransActive system.
         *
         * @param SessionDetails - The details the user has entered for the session
         *
         * @exception AuthenticationSecurityException - The user name or password etc the user has entered were
         *                                              invalid
         * @exception AuthenticationAgentException - The authentication agent encountered strange errors
         * @exception ObjectResolutionException - The authentication agent could not be found
         */
        virtual void authenticate(const ISessionChecker::SessionDetails& sessionDetails);


        /**
         * getEntityKey
         *
         * This is called to retrieve the entity key from the IEntityData member held by this object.
         * If the IEntityData member is NULL, the entity key returned will be 0.
         *
         * @return The entity key of this applicaitons entity, or 0 if the entity was not succesfully retireved
         * from the database.
         */
        unsigned long getEntityKey();

    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        SessionHandler( const SessionHandler&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        SessionHandler& operator=(const SessionHandler&);


        /**
        * checkCommandLine
        *
        * This is a helper method for the constructor.
        * Check the command line passed was correct. We check entity separately so
        * this is just for any extra params.
        *
        * @exception GenericGUIException
        */
        void checkCommandLine();


        /**
        * setUpGUIEntity
        *
        * This is a helper method for the constructor.
        * This will retrieve the GUI Entity, ensure it exists and load it from the database
        *
        * @exception GenericGUIException
        */
        void setUpGUIEntity();
        

        /**
         * connectToSystemController
         *
         * This attempts to initialise and connect to the system using the system controller. It provides the
         * user the opportunity to enter System Controller details and login.
         *
         * @exception UserCancelException - This means the user wants to exit the application
         * @exception CannotUseSysControllerException - This means we failed trying to connect using the
         *                                              System Controller.
         * @exception GenericGUIException - An error occurred
         */
        void connectToSystemController();


        /**
         * locateSystemController
         *
         * This attempts to connect to the System Controller using the host name and port
		 * number specified.
         *
		 * @param hostName    The host name of the System Controller to connect to.
		 * @param portNumber  The port number to connect on.
		 *
         * @exception CannotUseSysControllerException - This means we failed trying to connect using the
         *                                              System Controller.
         */
        void locateSystemController(std::string hostName, unsigned long portNumber);


        /** 
         * getPortNumber
         *
         * Checks if the RPARAM_PORTNUMBER run param is set, and if it contains
		 * a valid port number. If so, it is returned - else the default port
		 * number is returned.
         *
         * @return The port number to use to connect to the System Controller.
         */
		unsigned long getPortNumber();


        /**
         * retrieveRunParams
         *
         * This attempts to retrieve the runtime parameters from the System Controller
         *
         * @exception GenericGUIException - There is no reason why we shouldn't be able to
         *                                  get the RunParams if this method is called because
         *                                  the System Controller has been successfully contacted. So this
         *                                  is an initialisation error.
         */
        void retrieveRunParams();


        /**
         * configureHelpFile
         *
         * When not connected to the Control Station the application does not get given its help
         * file. This method sees if the helpfile is set and if not, retrieves it from the database.
         */
        void configureHelpFile();

        
        //
        // This is the GUI application all the calls will be delegated to. This is set
        // in the constructor.
        //
        TA_Base_Bus::IGUIApplication& m_guiApplication;

        //
        // This holds the database information for the GUI entity. This class is responsible
        // for deleting the object when finished.
        //
        TA_Base_Core::IEntityData* m_guiEntity;

        //
        // This is a pointer to the actual system controller.
        //
        TA_Base_Core::IProcessManagerCorbaDef_var m_systemController;

        //
        // This will be used to authenticate if we contact the system controller
        //
        TA_Base_Bus::AuthenticationLibrary* m_authentication;

        //
        // This will be used to see if this user has permission to run the Configuration Editor
        //
        TA_Base_Bus::RightsLibrary* m_rights;

    };
    
}

#endif // !defined(SessionHandler_0900475D_E824_4a2e_8F26_0E2336137178__INCLUDED_)
