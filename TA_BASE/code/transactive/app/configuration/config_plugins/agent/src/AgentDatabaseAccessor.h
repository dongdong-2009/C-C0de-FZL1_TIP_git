/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/agent/src/AgentDatabaseAccessor.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve agent information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  */


#if !defined(AgentDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
#define AgentDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_

#include <map>
#include <vector>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/IDatabaseAccessor.h"


// Forward declarations
namespace TA_Base_Core
{
    class IConfigEntity;
    class IConfigItem;
    class ILocation;
    class IItem;
}


namespace TA_Base_App
{
    // Forward declaration
    class ConfigAgent;


    class AgentDatabaseAccessor : public IDatabaseAccessor
    {

    public:

        /**
         * Constructor
         *
         * @param vector<string>& - These are all entity types that this component should
         *                          represent
         */
        AgentDatabaseAccessor(const std::vector<std::string>& entityTypes);


        /**
         * Constructor
         */
        AgentDatabaseAccessor();

        
        /**
         * Destructor
         */
        virtual ~AgentDatabaseAccessor();


        /**
         * loadItems
         *
         * This will retrieve all the agents from the database and store them in m_agents.
         *
         * @exception DataException - This will be thrown if there is an error with the Agent data and it
         *                             cannot be retrieved for some reason.
         * @exception DatabaseException - This will be thrown if there is an error communicating with the
         *                                database.
         */
        void loadItems();

        
        /**
         * getItemNames
         *
         * This returns all Agent names and keys to the caller. 
         * Precondition: loadItems() must be called before this method
         *
         * @param CProgressCtrl& - The progress control to advance as the names are retrieved.
         *
         * @return map<CString, unsigned long> - This returns a map of all the Agent names. The map also
         *                                       contains the identifier of the Agent as the caller will probably
         *                                       need this later to specify a specific Agent.
         *
         * @exception DataException - This will be thrown if there is an error with the Agent's data and it
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
         * getAgentFromKey
         *
         * This returns an agent matching the passed in pkey
         * 
         * @param unsigned long - The key of the agent entity. This is not the key of the monitored process
         *                        entity but the agent entity under that
         *
         * @return ConfigAgent* - A matching agent or NULL if one was not found
         */
        virtual ConfigAgent* getAgentFromKey(unsigned long key);


        /**
         * getItemsAtLocation
         *
         * This returns all the items at the location specified
         *
         * @param unsigned long - The key of a location
         *
         * @return vector<IConfigItem*> - The agent items that belong to the location
         *
         * @exception DataException - This will be thrown if there is an error with the Agent's data and it
         *                            cannot be retrieved for some reason.
         * @exception DatabaseException - This will be thrown if there is an error communicating with the
         *                                database.
         */
        virtual std::vector<TA_Base_Core::IConfigItem*> getItemsAtLocation(unsigned long locationKey);


        /**
         * locationHasChildren
         *
         * This determines if the location passed in has any child items
         *
         * @return bool
         */
        virtual bool locationHasChildren(unsigned long locationKey);


        /**
         * getChildItemsOfAgent
         *
         * This returns all items that are children of the specified agent.
         * 
         * @param unsigned long - The unique identifier of the agent
         *
         * @return vector<IConfigItem*> - The entity items that are children of the specified agent
         *
         * @exception DataException - This will be thrown if there is an error with the entity's data and it
         *                            cannot be retrieved for some reason.
         * @exception DatabaseException - This will be thrown if there is an error communicating with the
         *                                database.
         */
        virtual std::vector<TA_Base_Core::IConfigItem*> getChildItemsOfAgent(unsigned long agentId);


        /**
         * agentHasChildren
         *
         * This determines if the agent passed in has any child items
         *
         * @return bool
         */
        virtual bool agentHasChildren(unsigned long agentId);


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
         bool areCurrentChangesPending(std::vector<std::string>& agentsNotApplied);

        
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
         * @return vector<string>& - A reference to the list of entity type names that this class holds
         */
        std::vector<std::string>& getEntityTypes()
        {
            return m_entityTypes;
        };


        /**
         * getAllEntityTypes
         *
         * This returns all the entity types configured for this component to represent as well as all
         * agent entity types (retrieved cos they are children of the MonitoredProcess entity type
         *
         * @return vector<string> - A reference to the list of entity type names that this class holds
         */
        std::vector<std::string> getAllEntityTypes();


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
         * getAllLocationObjects
         *
         * This returns a all locations and keys to the caller. This goes to the database each time
         * this method is called.
         *
         * @return vector<IConfigLocation*> - This returns all the locations. The caller must delete these
         */
        std::vector<TA_Base_Core::ILocation*> getAllLocationObjects();

        /**
         * safeToDeleteItem
         *
         * Determines if it is safe to delete the specified entity. If the entity is in use an
         * exception will be thrown so that the plugin will show the 'Cant Delete' message box.
         *
         * @param item The item that needs to be checked.
         *
         * @exception DataException(FailType = CANNOT_DELETE) If its not safe to delete.
         */
        void safeToDeleteItem( TA_Base_Core::IItem* item );
    
    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        AgentDatabaseAccessor( const AgentDatabaseAccessor& theAgentDatabaseAccessor){ };


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        AgentDatabaseAccessor& operator=(const AgentDatabaseAccessor& pm);


        // This is a map of ConfigAgent* objects stored by their
        // entity key (unsigned long)
        typedef std::map<unsigned long, ConfigAgent*> LoadedAgents;
        typedef std::map<unsigned long, TA_Base_Core::IConfigEntity*> LoadedEntities;

        LoadedAgents m_agents;   // This is all the agents loaded from the database
        LoadedEntities m_entities;  // This is all the other child entities of agents that we are managing

        std::vector<std::string> m_entityTypes;   // This is the types of all entities this object has loaded.
                                                  // This does not include the agent entity types.

        bool m_isLoaded;          // This indicates if we have loaded the agents and entities before or not. If
                                  // we have never loaded the agents and entitiesthen when we are told to
                                  // refresh we shouldn't load them.

    };
}

#endif // !defined(AgentDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
