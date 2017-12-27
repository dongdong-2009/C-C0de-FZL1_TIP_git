/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/authentication_agent/src/UserCache.cpp $
 * @author:   Nick Jardine
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * This class is a "lazy cache" of the user table. That is, it reads users on
 * demand, and once read, stores them for future retreval.
 */

#if defined(_MSC_VER)
#pragma warning(disable:4786)   // Decorated name too long
#endif // defined(_MSC_VER)


#include "app/security/authentication_agent/src/UserCache.h"

#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/AuthenticationSecurityException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/data_access_interface/src/IConfigOperator.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/message/src/ConfigUpdateMessageSender.h"
#include "core/message/types/ConfigUpdate_MessageTypes.h"
#include "core/message/src/MessagePublicationManager.h"

using TA_Base_Core::OperatorAccessFactory;
using TA_Base_Core::ProfileAccessFactory;
using TA_Base_Core::LocationAccessFactory;
using TA_Base_Core::IConfigOperator;
using TA_Base_Core::IOperator;
using TA_Base_Core::IProfile;
using TA_Base_Core::ILocation;
using TA_Base_Core::AuthenticationAgentException;
using TA_Base_Core::AuthenticationSecurityException;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataConfigurationException;
using TA_Base_Core::OnlineUpdateListener;
using TA_Base_Core::ConfigUpdateDetails;
using TA_Base_Core::ThreadGuard;
using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{


    UserCache::UserCache() : m_configMessageSender(NULL)
    {
        FUNCTION_ENTRY("UserCache::UserCache() constructor");

        // start by building up the Operator, Profile and Location caches so that
        // we don't have to fetch from the database each time
        // We'll just put them into maps so that access to any item in those containers
        // is going to be fast
        // since we're using values from the cache, it is possible for the values to
        // go out of date when there is an online update, so we need to register for 
        // online updates

        OnlineUpdateListener::getInstance().registerAllInterests(TA_Base_Core::OPERATOR, *this);
        OnlineUpdateListener::getInstance().registerAllInterests(TA_Base_Core::PROFILE, *this);
        OnlineUpdateListener::getInstance().registerAllInterests(TA_Base_Core::LOCATION, *this);

        // we want operator objects to be writeable as an operator might want to change password 
        // via this this agent
        std::vector<IOperator*> operators = OperatorAccessFactory::getInstance().getAllOperators(true);
        {// scope for guarding the operator map
            ThreadGuard guard(m_operatorMapLock);
            for (unsigned int i = 0; i < operators.size(); i++)
            {
                m_operatorMap.insert(OperatorMap::value_type(operators[i]->getKey(), dynamic_cast<IConfigOperator*>(operators[i])));
            }
        }

        std::vector<IProfile*> profiles = ProfileAccessFactory::getInstance().getAllProfiles(false);
        {// scope for guarding the profile map
            ThreadGuard guard(m_profileMapLock);
            for (unsigned int i = 0; i < profiles.size(); i++)
            {
                m_profileMap.insert(ProfileMap::value_type(profiles[i]->getKey(), profiles[i]));
            }
        }

        std::vector<ILocation*> locations = LocationAccessFactory::getInstance().getAllLocations(false);
        {// scope for guarding the location map
            ThreadGuard guard(m_locationMapLock);
            for (unsigned int i = 0; i < locations.size(); i++)
            {
                m_locationMap.insert(LocationMap::value_type(locations[i]->getKey(), locations[i]));
            }
        }
        m_configMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getConfigUpdateMessageSender(TA_Base_Core::ConfigUpdate::Context);
        FUNCTION_EXIT;
    }


    UserCache::~UserCache()
    {
        FUNCTION_ENTRY("destructor");
        // stop listening to updates since we're closing down anyway
        OnlineUpdateListener::getInstance().deregisterAllInterests(TA_Base_Core::OPERATOR, *this);
        OnlineUpdateListener::getInstance().deregisterAllInterests(TA_Base_Core::PROFILE, *this);
        OnlineUpdateListener::getInstance().deregisterAllInterests(TA_Base_Core::LOCATION, *this);

        ThreadGuard guard1(m_operatorMapLock);
        for (OperatorMap::iterator op = m_operatorMap.begin(); op != m_operatorMap.end(); op++)
        {
            delete op->second;
            op->second = NULL;
        }

        ThreadGuard guard2(m_profileMapLock);
        for (ProfileMap::iterator prof = m_profileMap.begin(); prof != m_profileMap.end(); prof++)
        {
            delete prof->second;
            prof->second = NULL;
        }

        ThreadGuard guard3(m_locationMapLock);
        for (LocationMap::iterator loc = m_locationMap.begin(); loc != m_locationMap.end(); loc++)
        {
            delete loc->second;
            loc->second = NULL;
        }

        FUNCTION_EXIT;
    }


    UserCache::UserDetails UserCache::getUserDetails(const unsigned long userId)
    {
        FUNCTION_ENTRY("getUserDetails");

        UserDetails user;
        // Using more database interaction, so need to wrapper this in try/catch too
        try
        {
            std::vector<unsigned long> profiles;
            {// scope for guarding the operator map
                ThreadGuard guard(m_operatorMapLock);
                // The operator object we'll be working on.
                OperatorMap::iterator opIter = m_operatorMap.find(userId);
                if (opIter == m_operatorMap.end())
                {
                    TA_THROW(AuthenticationAgentException("The specified operator is not recognised by the application") );
                }

                IConfigOperator* op = opIter->second;
		        // Create a UserDetails object

                // store the basic details
                user.password = op->getPassword();
                user.userId = op->getKey();
                user.userName = op->getName();
                LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
                    "Operator details: User ID = %d; Name = %s", userId, user.userName.c_str());

                // get the profiles and locations and store them
                profiles = (op->getAssociatedProfileKeys());
            }

            ThreadGuard guard(m_profileMapLock);

			for (unsigned int i = 0; i < profiles.size(); i++)
            {
                // Now map the keys of the location to another vector
                std::vector<LocationDetails> locationDetails;
                ProfileMap::iterator prfIter = m_profileMap.find(profiles[i]);

                if (prfIter == m_profileMap.end())
                {
                    LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
                        "Unable to find the profile, %d, that is associated with operator, %d.  Ignoring", profiles[i], userId);
                    // if we can't find the profile, just ignore it.  This shouldn't really happen though.
                    continue;
                }
                IProfile* prof = prfIter->second;

                LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
                    "Profile: ID = %d, Name = %s", prof->getKey(), prof->getName().c_str());
                // But only if this profile is associated with locations, otherwise leave the vector empty
				if ( prof->requiresLocationToBeSelected() )
                {
                    // Retrieve the locations for this profile, and store them in a vector
					std::vector<unsigned long> locations = prof->getAssociatedLocationKeys();

                    // Loop through the locations vector pulling out the details.
					for (unsigned int j = 0; j < locations.size(); j++)
                    {
                        ThreadGuard guard(m_locationMapLock);
                        LocationMap::iterator locIter = m_locationMap.find(locations[j]);
                        if (locIter == m_locationMap.end())
                        {
                            // if we can't find the location, just ignore it.  This shouldn't really happen though.
                            LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
                                "Unable to find the location, %d, that is associated with profile, %d.  Ignoring", locations[j], profiles[i]);
                            continue;
                        }
                        ILocation* loc = locIter->second;

                        // Create the LocationDetails object that will be inserted into the vector
						LocationDetails location;
						location.locationId = loc->getKey();
						location.locationName = loc->getName();
                        // And put it in the vector
                        locationDetails.push_back(location);
                        LOGMORE(SourceInfo, "Location: ID = %d; Name = %s", location.locationId, location.locationName.c_str());
                    }
                }

                // Attempt to find this profile in the map of profiles to locations
                ProfileLocationMap::iterator profileIter(user.profileLocationMap.find( prof->getKey()));

                // If the profile exists (it shouldn't but need to check just in case), just append the vector
                if (profileIter != user.profileLocationMap.end())
                {
                    profileIter->second.associatedLocations.insert(profileIter->second.associatedLocations.end(),
                        locationDetails.begin(),locationDetails.end() );
                }
                // otherwise create a new ProfileLocationDetails object and insert it into the map.
                else
                {
                    // Create the ProfileLocationDetails object
                    ProfileLocationDetails pld;
                    pld.associatedLocations = locationDetails;
                    pld.profileId = prof->getKey();
                    pld.profileName = prof->getName();

                    // And insert it into the map
                    user.profileLocationMap.insert(ProfileLocationMap::value_type(
                        prof->getKey(), pld ) );                    
                }
            }
        }
        catch(DataException&)
        {
            TA_THROW(AuthenticationAgentException("A database error occured while attempting to load the user details") );
        }
        catch(DatabaseException&)
        {
            TA_THROW(AuthenticationAgentException("A database error occured while attempting to load the user details") );
        }
        catch(AuthenticationAgentException&)
        {
            throw;
        }
        catch(...)
        {
            TA_THROW(AuthenticationAgentException("An unknown error occured while attempting to load the user details") );
        }
        
        FUNCTION_EXIT;
        return user;

    }


    void UserCache::changeUserPassword(const unsigned long userId, const std::string& password)
    {
        FUNCTION_ENTRY("changeUserPassword");

        // Test to ensure the password is not empty
        if (0 == password.length())
        {
            // password is empty - throw an exception
            TA_THROW(AuthenticationSecurityException("The new password was empty. Passwords cannot be empty",
                AuthenticationSecurityException::INVALID_PASSWORD) );
        }

        // The operator object we'll be working on.
        OperatorMap::iterator opIter = m_operatorMap.find(userId);
        if (opIter == m_operatorMap.end())
        {
            TA_THROW(AuthenticationAgentException("The specified operator is not recognised by the application") );
        }
        IConfigOperator* op = opIter->second;

        try
        {
            op->setPassword(password);
            TA_Base_Core::ItemChanges itemChanges = op->getAllItemChanges();
            // Apply the changes to the database
            op->applyChanges();

            std::vector<std::string> changes;
            for(TA_Base_Core::ItemChanges::const_iterator iter = itemChanges.begin(); iter != itemChanges.end();++iter)
            {
                changes.push_back( iter->first );
            }
            m_configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigOperator,
                                                           userId,
                                                           TA_Base_Core::Update,
                                                           changes);

        }
        catch(DataException&)
        {
            TA_THROW(AuthenticationAgentException("A database error occured while attempting to change password") );
        }
        catch(DatabaseException&)
        {
            TA_THROW(AuthenticationAgentException("A database error occured while attempting to change password") );
        }
        catch(...)
        {
            TA_THROW(AuthenticationAgentException("An unknown error occured while attempting to change password") );
        }

        FUNCTION_EXIT;
    }


    void UserCache::processUpdate(const ConfigUpdateDetails& updateEvent)
    {
        FUNCTION_ENTRY("processUpdate");
        switch (updateEvent.getType())
        {
        case TA_Base_Core::OPERATOR:
            updateOperator(updateEvent);
            break;
        case TA_Base_Core::PROFILE:
            updateProfile(updateEvent);
            break;
        case TA_Base_Core::LOCATION:
            updateLocation(updateEvent);
            break;
        default:
            // ignoring
            LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
                "Did not register for this online update.  Ignoring");
            break;
        }
        FUNCTION_EXIT;
    }


    void UserCache::updateOperator(const ConfigUpdateDetails& updateEvent)
    {
        FUNCTION_ENTRY("updateOperator");
        try
        {
            unsigned long key = updateEvent.getKey();
            OperatorMap::iterator op = m_operatorMap.find(key);
            switch (updateEvent.getModifications())
            {
            case TA_Base_Core::Create:
            case TA_Base_Core::Update:
                if (op == m_operatorMap.end())
                {
                    // we haven't got it yet, so just reload
                    LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
                        "Got online update for Operator.  Adding new operator, %d, into map", key);
                    IOperator* newOp = OperatorAccessFactory::getInstance().getOperator(key, true);
                    m_operatorMap.insert(OperatorMap::value_type(key, dynamic_cast<IConfigOperator*>(newOp)));
                }
                else
                {
                    // already got it so just update
                    LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
                        "Got online update for Operator.  Updating operator, %d", key);
                    op->second->invalidate();
                }
                break;
            case TA_Base_Core::Delete:
                {
					LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
                    "Got online update for Operator.  Removing operator, %d", key);
					if(op != m_operatorMap.end())
					{
					delete op->second;
	                op->second = NULL;
	                m_operatorMap.erase(key);
					}
            	}
                break;
            default:
                // this is not going to happen. ignoring
                break;
            }
        }
        catch (...)
        {
            LOG_GENERIC(SourceInfo,DebugUtil::DebugError, "Caught some exception when processing online update for operator");
        }
        FUNCTION_EXIT;
    }


    void UserCache::updateProfile(const ConfigUpdateDetails& updateEvent)
    {
        FUNCTION_ENTRY("updateProfile");
        try
        {
            unsigned long key = updateEvent.getKey();
            ProfileMap::iterator prof = m_profileMap.find(key);
            switch (updateEvent.getModifications())
            {
            case TA_Base_Core::Create:
            case TA_Base_Core::Update:
                if (prof == m_profileMap.end())
                {
                    // we haven't got it yet, so just reload
                    LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
                        "Got online update for Profile.  Adding new profile, %d, into map", key);
                    IProfile* newOp = ProfileAccessFactory::getInstance().getProfile(key);
                    m_profileMap.insert(ProfileMap::value_type(key, newOp));
                }
                else
                {
                    // already got it so just update
                    LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
                        "Got online update for Profile.  Updating profile, %d", key);
                    prof->second->invalidate();
                }
                break;
            case TA_Base_Core::Delete:
               {
					LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
                    "Got online update for Profile.  Removing profile, %d", key);
					if(prof != m_profileMap.end())
					{
					delete prof->second;
	                prof->second = NULL;
	                m_profileMap.erase(key);
					}
                }
                break;
            default:
                // this is not going to happen. ignoring
                break;
            }
        }
        catch (...)
        {
            LOG_GENERIC(SourceInfo,DebugUtil::DebugError, "Caught some exception when processing online update for profile");
        }
        FUNCTION_EXIT;
    }


    void UserCache::updateLocation(const ConfigUpdateDetails& updateEvent)
    {
        FUNCTION_ENTRY("updateLocation");
        try
        {
            unsigned long key = updateEvent.getKey();
            LocationMap::iterator loc = m_locationMap.find(key);
            switch (updateEvent.getModifications())
            {
            case TA_Base_Core::Create:
            case TA_Base_Core::Update:
                if (loc == m_locationMap.end())
                {
                    // we haven't got it yet, so just reload
                    LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
                        "Got online update for Location.  Adding new location, %d, into map", key);
                    ILocation* newOp = LocationAccessFactory::getInstance().getLocationByKey(key);
                    m_locationMap.insert(LocationMap::value_type(key, newOp));
                }
                else
                {
                    // already got it so just update
                    LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
                        "Got online update for Location.  Updating location, %d", key);
                    loc->second->invalidate();
                }
                break;
            case TA_Base_Core::Delete:
                {
					LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
                    "Got online update for Location.  Removing location, %d", key);
					if(loc!=m_locationMap.end())
					{
					delete loc->second;
	                loc->second = NULL;
	                m_locationMap.erase(key);
					}
            	}
                break;
            default:
                // this is not going to happen. ignoring
                break;
            }
        }
        catch (...)
        {
            LOG_GENERIC(SourceInfo,DebugUtil::DebugError, "Caught some exception when processing online update for location");
        }
        FUNCTION_EXIT;
    }
}


