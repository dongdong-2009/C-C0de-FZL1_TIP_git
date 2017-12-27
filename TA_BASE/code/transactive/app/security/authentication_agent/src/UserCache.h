/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/authentication_agent/src/UserCache.h $
 * @author:   Nick Jardine
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * This class is a "lazy cache" of the user table. That is, it reads users on
 * demand, and once read, stores them for future retreval.
 */


#if !defined(UserCache_72ED6440_AAE6_4c96_A421_D490B05A6BCA__INCLUDED_)
#define UserCache_72ED6440_AAE6_4c96_A421_D490B05A6BCA__INCLUDED_

#include <string>
#include <vector>
#include <map>
#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

// forward declaration
namespace TA_Base_Core
{
    class IConfigOperator; 
    class ILocation;
    class IProfile;
    class ConfigUpdateMessageSender;
}


namespace TA_Base_App
{
    class UserCache : virtual public TA_Base_Core::IOnlineUpdatable
    {
    public:

        struct LocationDetails
        {
            unsigned long locationId;       // The key to the location in the database
            std::string locationName;       // The name of the location
        };

        struct ProfileLocationDetails
        {
            std::string profileName;                            // The name of the profile in the database
            unsigned long profileId;                            // The key to the profile in the database
            std::vector<LocationDetails> associatedLocations;   // The list of locations associated with this profile
        };

        typedef std::map<unsigned long,ProfileLocationDetails > ProfileLocationMap;
        //typedef std::map<unsigned long,std::vector<unsigned long> > ProfileLocationMap;

        struct UserDetails
        {
	        std::string userName;                       // The name of the user
	        unsigned long userId;                       // The key to the user in the database
	        std::string password;                       // The users password
            ProfileLocationMap profileLocationMap;      // The map of profiles and their associated locations.
        };


        /**
         * constructor
         *
         * Constructs a new instance of the user cache
         */
	    UserCache();

        /**
         * destructor
         *
         * Destructs the user cache
         */
	    virtual ~UserCache();


	    /**
         * getUserDetails
         *
	     * Returns the log-in details for the specified user
         *
	     * @param    userId The ID of the user for which to retrieve the details
         *
         * @exception AuthenticationSecurityException Thrown if the userId is not valid
         * @exception AuthenticationAgentException Thrown if there is any other problem retrieving the user details
	     */
	    UserDetails getUserDetails(const unsigned long userId);


        /**
         * changeUserPassword
         *
         * Changes the password for the current user, and send a config-update message to that effect.
         *
         * @param userId The ID of the users for which to change the password
         * @param password The new password to set for the user
         *
         * @exception AuthenticationSecurityException Thrown if the userId is not known to the system
         * @exception AuthenticationAgentException Thrown if there are any other errors while change the password (e.g. Database).
         */
	    void changeUserPassword(const unsigned long userId, const std::string& password);


        /**
         * processUpdate
         *
         * When there is a configuration update of the type and key matching
         * one registered by this class, this method will be invoked
         * to process the update accordingly.
         */
        virtual void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

    private:
        /**
         * Copy Constructor
         *
         * Hidden to avoid accidental use
         */
	    UserCache( const UserCache& theUserCache);


        /**
         * operator=
         *
         * Hidden to avoid accidental use
         */
        UserCache& operator=(const UserCache& rhs);


        /** 
          * updateOperator
          *
          * Updates the operator map accordingly
          *
          * @param updateEvent The online update event
          */
        void updateOperator(const TA_Base_Core::ConfigUpdateDetails& updateEvent);


        /** 
          * updateProfile
          *
          * Updates the profile map accordingly
          *
          * @param updateEvent The online update event
          */
        void updateProfile(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

    
        /** 
          * updateLocation
          *
          * Updates the location map accordingly
          *
          * @param updateEvent The online update event
          */
        void updateLocation(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

    
    private:
        // the following containers cache the data required for authentication
        typedef std::map<unsigned long, TA_Base_Core::IConfigOperator*> OperatorMap;
        typedef std::map<unsigned long, TA_Base_Core::IProfile*> ProfileMap;
        typedef std::map<unsigned long, TA_Base_Core::ILocation*> LocationMap;

        // caches configurable operator objects
        OperatorMap m_operatorMap;

        // caches non-configurable profile objects
        ProfileMap m_profileMap;

        // caches non-configurable location objects
        LocationMap m_locationMap;

        // all the locks that protects the associated maps
	    mutable TA_Base_Core::NonReEntrantThreadLockable m_operatorMapLock;
	    mutable TA_Base_Core::NonReEntrantThreadLockable m_profileMapLock;
	    mutable TA_Base_Core::NonReEntrantThreadLockable m_locationMapLock;

        // This is used to send configuration update messages indicating changes to the configuration data.
        TA_Base_Core::ConfigUpdateMessageSender* m_configMessageSender;
    };
}

#endif // !defined(UserCache_72ED6440_AAE6_4c96_A421_D490B05A6BCA__INCLUDED_)
