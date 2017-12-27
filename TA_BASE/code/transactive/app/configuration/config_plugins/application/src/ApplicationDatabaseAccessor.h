/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/application/src/ApplicationDatabaseAccessor.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve application information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  */


#if !defined(ApplicationDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
#define ApplicationDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_

#include <map>
#include <vector>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/IDatabaseAccessor.h"


// Forward declarations
namespace TA_Base_Core
{
    class IConfigGui;
    class IConfigItem;
}


namespace TA_Base_App
{
    class ApplicationDatabaseAccessor : public IDatabaseAccessor
    {

    public:

        /**
         * Constructor
         *
         * @param vector<string>& - These are all entity types that this component should
         *                          represent
         */
        ApplicationDatabaseAccessor(const std::vector<std::string>& entityTypes);


        /**
         * Destructor
         */
        virtual ~ApplicationDatabaseAccessor();


        /**
         * loadItems
         *
         * This will retrieve all the applications from the database and store them in m_applications.
         *
         * @exception DataException - This will be thrown if there is an error with the Application data and it
         *                             cannot be retrieved for some reason.
         * @exception DatabaseException - This will be thrown if there is an error communicating with the
         *                                database.
         */
        void loadItems();

        
        /**
         * getItemNames
         *
         * This returns all Application names and keys to the caller. 
         * Precondition: loadItems() must be called before this method
         *
         * @param CProgressCtrl& - The progress control to advance as the names are retrieved.
         *
         * @return map<CString, unsigned long> - This returns a map of all the Application names. The map also
         *                                       contains the identifier of the Application as the caller will probably
         *                                       need this later to specify a specific Application.
         *
         * @exception DataException - This will be thrown if there is an error with the Application's data and it
         *                            cannot be retrieved for some reason.
         * @exception DatabaseException - This will be thrown if there is an error communicating with the
         *                                database.
         */
        std::multimap<CString,unsigned long> getItemNames(CProgressCtrl& progress);
        

        /**
         * getItem
         *
         * This returns a database object for the item specified. The item is specified by
         * passing in its key.
         * Precondition: loadItems() must be called before this method
         *
         * @param unsigned long - The identifier of the item to retrieve
         *
         * @return IConfigItem* - The database object for this item. This will be NULL if one
         *                            is not found.
         */
        virtual TA_Base_Core::IConfigItem* getItem(unsigned long identifier);


        /**
         * deleteItem
         *
         * This will delete the specified item from the database and it will then delete the
         * IConfigItem object held.
         *
         * @param unsigned long - This is the unique identifier for the item to delete
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is an error
         *                              deleting the item
         */
        virtual void deleteItem(unsigned long id);


        /**
         * newItem
         *
         * This creates a new item. This item will be added to the internal list
         * of current items but will not yet be applied to the database.
         *
         * @return IConfigItem* - The newly created item ready for its data to be populated
         */
        virtual TA_Base_Core::IConfigItem* newItem();


        /**
         * newItem
         *
         * This method will create a new entity of the type specified
         *
         * @param string& - The type of entity to create
         *
         * @return IConfigEntity* - The newly created entity.
         */
        virtual TA_Base_Core::IConfigItem* newItem(const std::string& type);

        
        /**
         * copyItem
         *
         * This creates a new item with the same data in it as the item passed in.
         * This item will be added to the internal list of current items but will not yet be
         * applied to the database.
         *
         * @param unsigned long - The unique identifier of the item to copy
         * 
         * @return IConfigItem* - The newly created item ready for its data to be populated
         *
         * @exception DatabaseException - This is thrown if data could not be retrieved for the item
         *                                that is being copied.
         */
        virtual TA_Base_Core::IConfigItem* copyItem(const unsigned long idOfItemToCopy);

        
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
         bool areCurrentChangesPending(std::vector<std::string>& applicationsNotApplied);

        
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
         * getEntityTypes
         *
         * This returns all entity types that this component is representing
         *
         * @param vector<string>& - A reference to the list of entity type names that this class holds
         */
        std::vector<std::string>& getEntityTypes()
        {
            return m_entityTypes;
        };


        /**
         * getAllLocations
         *
         * This returns a all locations and keys to the caller. This goes to the database each time
         * this method is called.
         *
         * @return map<unsigned long, string> - This returns all the location keys and names. This is all
         *                                      information needed by entities to update their data and
         *                                      to display to the user
         */
        std::map<unsigned long, std::string> getAllLocations();


        /**
         * getAllSubsystems
         *
         * This returns a all locations and keys to the caller. This goes to the database each time
         * this method is called.
         *
         * @return map<unsigned long, string> - This returns all the subsystem keys and names. This is all
         *                                      information needed by entities to update their data and
         *                                      to display to the user
         */
        std::map<unsigned long, std::string> getAllSubsystems();


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        ApplicationDatabaseAccessor( const ApplicationDatabaseAccessor& theApplicationDatabaseAccessor){ };


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        ApplicationDatabaseAccessor& operator=(const ApplicationDatabaseAccessor& pm);


        // This is a map of IConfigApplication* objects stored by their Application key (unsigned long)
        typedef std::map<unsigned long, TA_Base_Core::IConfigGui*> LoadedApplications;

        LoadedApplications m_applications;   // This is all the applications loaded from the database

        std::vector<std::string> m_entityTypes;   // This is the types of all entities this object has loaded

        bool m_isLoaded;          // This indicates if we have loaded the applications before or not. If we
                                  // have never loaded the applications then when we are told to refresh we
                                  // shouldn't load them.

    };
}

#endif // !defined(ApplicationDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
