/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ISessionChecker.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This interface provides the methods necessary for a login dialog to authenticate and determine if the
  * application should be run.
  */

#include <string>

#ifndef ISESSION_CHECKER_H
#define ISESSION_CHECKER_H

namespace TA_Base_App
{
	class ISessionChecker
	{
    public:
        virtual ~ISessionChecker(){};


        struct SessionDetails
        {
            unsigned long operatorKey;
            unsigned long profileKey;
            unsigned long locationKey;
            unsigned long consoleKey;
            std::string password;
        };


        /**
          * checkUserPermissions
          *
          * This will check if this user has permission to run this application. If they do not then this
          * will throw an exception
          *
          * @exception GenericGUIException - This indicates that either the user did not have permission to run
          *                                  the application or there was an error attempting to retrieve the rights.
          */
        virtual void checkUserPermissions() =0;


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
        virtual void authenticate(const SessionDetails& sessionDetails) =0;

	};
}

#endif //ISESSION_CHECKER_H
