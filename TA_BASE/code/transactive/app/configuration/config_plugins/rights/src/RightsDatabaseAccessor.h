/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/rights/src/RightsDatabaseAccessor.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve rights information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#if !defined(RightsDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
#define RightsDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_

#include <map>
#include <vector>
#include <string>


// Forward declarations
namespace TA_Base_Core
{
    class IConfigRights;
    class IProfile;
    class ISubsystem;
}


namespace TA_Base_App
{
    class RightsDatabaseAccessor
    {

    public:

        // This is a map of IConfigRights* objects stored by their rights key (unsigned long)
        typedef std::map<unsigned long, TA_Base_Core::IConfigRights*> LoadedRights;


        enum RightsType
        {
            APPLICATION,    // Non physical subsystems
            SUBSYSTEM,      // Physical subsystems
            ALL             // All subsystems (used when requesting information. This should not be stored
        };


        struct ProfileDetails
        {
            std::string name;      // The name of the profile
            unsigned long typeKey; // Indicates whether the profile is a central or local type
        };


        /**
         * Constructor
         */
        RightsDatabaseAccessor();


        /**
         * Destructor
         */
        virtual ~RightsDatabaseAccessor();


        /**
         * loadItems
         *
         * This will retrieve all the rights from the database and store them in m_appliationRights
         * and m_subsystemRights.
         *
         * @param subsystemStateKey - pkey of the subsystem state
         *
         * @exception DataException - This will be thrown if there is an error with the rights data and it
         *                             cannot be retrieved for some reason.
         * @exception DatabaseException - This will be thrown if there is an error communicating with the
         *                                database.
         */
        void loadItems(unsigned long subsystemStateKey, const RightsType& type);

        
        /**
         * getRights
         *
         * This returns all rights to the caller based on the type they pass in
         * Precondition: loadEntities() must be called before this method or nothing will be returned
         *
         * @param RightsType - This indicates the type of rights the user wants to retrieve
         *
         * @return const LoadedRights& - This returns a map of all the rights keyed on the unique identifier
         *
         */
        const LoadedRights& getRights(RightsType type)
        {
            switch(type)
            {
                case(APPLICATION):
                    return m_applicationRights;
                case(SUBSYSTEM):
                    return m_subsystemRights;
            }

            return m_subsystemRights;
        }


        /**
         * getAllRights
         *
         * This returns all rights to the caller
         * Precondition: loadEntities() must be called before this method or nothing will be returned
         *
         * @return LoadedRights - This returns a map of all the rights keyed on the unique identifier
         *
         */
        virtual LoadedRights RightsDatabaseAccessor::getAllRights();

        
        /**
         * getItem
         *
         * This returns a database object for the item specified. The item is specified by
         * passing in its key.
         * Precondition: loadItems() must be called before this method
         *
         * @param unsigned long - The identifier of the item to retrieve
         *
         * @return IConfigRights* - The database object for this item. This will be NULL if one
         *                            is not found.
         */
        virtual TA_Base_Core::IConfigRights* getItem(unsigned long identifier);

        
        /**
         * newItem
         *
         * This creates a new item. This item will be added to the internal list
         * of current items but will not yet be applied to the database.
         *
         * @param unsigned long - The profilekey this right is for
         * @param unsigned long - The subsystemkey this right is for
         * @param RightsType - This indicates the type of rights the user wants to create
         *
         * @return IConfigRights* - The newly created item ready for its data to be populated
         */
        virtual TA_Base_Core::IConfigRights* newItem(unsigned long profileKey, unsigned long subsystemKey,
			unsigned long subsystemStateKey, RightsType type);


        /**
         * areCurrentChangesPending
         *
         * This is called to determine if there are any current changes that have not been applied.
         * If this returns true then it will populate the vector of strings passed in with all items
         * not applied.
         * 
         * @param vector<string>& - This is an OUT parameter and should be passed in empty. It will
         *                          be populated with names for all data not yet applied
         * 
         * @return bool - This will return true if there are items that have had changes which have  not
         *                been applied.
         */
         bool areCurrentChangesPending(std::vector<std::string>& rightsNotApplied);

        
         /**
          * invalidate
          *
          * This will set m_isLoaded to false indicating that next time load is called all data should
          * be refreshed from the database.
          */
         void invalidateData()
         {
             m_isLoaded = false;
         }


        /**
         * getAllProfiles
         *
         * This returns all profiles and keys to the caller. This goes to the database each time
         * this method is called.
         *
         * @return map<unsigned long, ProfileDetails> - This returns all the profile keys and names.
         */
        std::map<unsigned long, ProfileDetails> getAllProfiles();


        /**
         * getAllSubsystems
         *
         * This returns all locations and keys to the caller. This goes to the database each time
         * this method is called.
         *
         * @param RightsType - This indicates the type of rights the user is trying to configure. This will
         *                     indicate whether we want physical or non-physical subsystems
         *
         * @return map<string, unsigned long> - This returns all the subsystem keys and names.
         */
        std::map<std::string, unsigned long> getAllSubsystems(RightsType type);


        /**
         * getAllDuties
         *
         * This returns all duties and keys to the caller. This goes to the database each time
         * this method is called.
         *
         * @return map<unsigned long, string> - This returns all the duty keys and names.
         */
        std::map<unsigned long, std::string> getAllDuties();

        /**
         * getAllSubsystemStates
         *
         * This returns all subsystem states and keys to the caller. This goes to the database each time
         * this method is called.
         *
         * @return map<unsigned long, string> - This returns all the subsystem state keys and names.
         */
        std::map<unsigned long, std::string> getAllSubsystemStates();

        /**
         * removeItem
         *
         * This deletes an existing item. This item will be deleted from the internal list
         * of current items but will not yet be applied to the database.
         *
         * @param uniqueIdentifer - Identifier of the item
         */
        void removeItem(unsigned long uniqueIdentifer);

    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        RightsDatabaseAccessor( const RightsDatabaseAccessor& theRightsDatabaseAccessor){ };


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        RightsDatabaseAccessor& operator=(const RightsDatabaseAccessor& pm);


        LoadedRights m_subsystemRights;   // This is all the subsystem rights loaded from the database
        LoadedRights m_applicationRights; // This is all the application rights loaded from the database

        bool m_isLoaded;          // This indicates if we have loaded the rights before or not. If we
                                  // have never loaded the rights then when we are told to refresh we
                                  // shouldn't load them.

    };
}

#endif // !defined(RightsDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
