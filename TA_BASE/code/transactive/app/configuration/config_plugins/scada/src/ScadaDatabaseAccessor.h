/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/ScadaDatabaseAccessor.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve Scada information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */


#if !defined(ScadaDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
#define ScadaDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_

#include <map>
#include <vector>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/IDatabaseAccessor.h"
#include "app/configuration/config_plugins/scada/src/DataPointStateDataHelper.h"

#include "bus/scada/scada_tree/src/IEntityFactory.h"

// Forward declarations
namespace TA_Base_Core
{
    class IConfigEntity;
    class IConfigItem;
    class IConfigDerivedDataPoint;
}


namespace TA_Base_App
{
    class ScadaDatabaseAccessor : virtual public IDatabaseAccessor, virtual public TA_Base_Bus::IEntityFactory
    {

    public:

        /**
         * Constructor
         */
        ScadaDatabaseAccessor();
        ScadaDatabaseAccessor(const std::vector<std::string>& entityTypes);


        /**
         * Destructor
         */
        virtual ~ScadaDatabaseAccessor();


        /**
         * loadItems
         *
         * This does nothing as our tree holds all the database items.
         */
        void loadItems();

        
        /**
         * getItemNames
         *
         * This does nothing as our tree holds all the database items.
         */
        std::multimap<CString,unsigned long> getItemNames(CProgressCtrl& progress);
        

        /**
         * getItem
         *
         * This does nothing as our tree holds all the database items.
         */
        virtual TA_Base_Core::IConfigItem* getItem(unsigned long identifier);


        /**
         * getDerivedDataPointItem
         *
         * This will either retrieve an existing derived datapoint object or retrieve a new one
         * from the database. If necessary it may also create a new object. It will then return
         * it for this identifier
         *
         * @param unsigned long - the unique identifier for the datapoint we want to find the derived data for
         *
         * @return IConfigDerivedDataPoint& - The derived data point object
         *
         * @exception DataException
         * @exception DatabaseException
         */
        virtual TA_Base_Core::IConfigDerivedDataPoint& getDerivedDataPointItem(unsigned long identifier);


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
         * deleteDerivedDataPointItem
         *
         * This will delete the derived datapoint item (if one exists) from the map and the database.
         *
         * @param unsigned long - This is the unique identifier for the item to delete
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is an error
         *                              deleting the item
         */
        void deleteDerivedDataPointItem( unsigned long id);


        /**
         * newItem
         *
         * This method is not used for entities. It will contain an assert. All methods that call this
         * must be overridden for entities.
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
         bool areCurrentChangesPending(std::vector<std::string>& itemsNotApplied);

        
         /**
          * invalidate
          *
          * This will set m_isLoaded to false indicating that next time load is called all data should
          * be refreshed from the database.
          */
         void invalidateData();


         virtual std::vector<TA_Base_Core::IEntityData*> getChildEntitiesOf(unsigned long entityKey);

         virtual std::vector<TA_Base_Core::IEntityData*> getEntitiesOfType(const std::string& entityTypeName);

         virtual TA_Base_Core::IEntityData* getEntity(unsigned long entityKey);

         virtual TA_Base_Core::IEntityData* getEntity(const std::string& entityName);


         /**
          * hasChildren
          *
          * This determines whether the specified entity has children or not.
          * The caller passed in what it has determined so that this method can be lazy if needed
          *
          * @param unsigned long - the key of the entity to check
          * @param bool - This indicates what the entity item object calculated on construction
          */
         virtual bool hasChildren(unsigned long entityKey, bool hasChildren);


         /**
          *  getAllSeverities
          *
          *  Gets all alarm severities from the database and puts them into a
          *  map of key/name pairs.
          *
          *  @exception DatabaseException
          *  This is thrown if the severities could not be retrieved from the DB
          *
          *  @exception DatabaseException
          *  This is thrown if there is a problem with the format of the data
          *  in the DB.
          */
         std::map<unsigned long, std::string> getAllSeverities();
        

         /**
          * getAlarmSeverityName
          *
          * When given a key of an alarm severity this returns the alarm severity name
          *
          * @param unsigned long- An alarm severity pkey
          *
          * @return string - The name pf the alarm severity or an empty string if the severity could not
          *                  be found.
          */
         std::string getAlarmSeverityName(unsigned long key);


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
         * This returns a all physical subsystem and keys to the caller. This goes to the database each time
         * this method is called.
         *
         * @return map<unsigned long, string> - This returns all the subsystem keys and names. This is all
         *                                      information needed by entities to update their data and
         *                                      to display to the user
         */
        std::map<unsigned long, std::string> getAllSubsystems();


        /**
         * getAllAgents
         *
         * This returns all agent names and keys to the caller. This goes to the database each time
         * this method is called. It finds all the agents that can be the agent for a DataPoint
         *
         * @param string - This is the entity type of the entity we must retrieve agents for.
         *
         * @return map<unsigned long, string> - This returns all the agent keys and names. This is all
         *                                      information needed by entities to update their data and
         *                                      to display to the user
         */
        std::map<unsigned long, std::string> getAllAgents(const std::string& type);


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
          * getDpStateHelper
          *
          * Used to set/get datapoint state data
          *
          * @return a reference to the helper
          */
        DataPointStateDataHelper& getDpStateHelper()
        {
            return m_stateHelper;
        }

        
        /**
         * @return true if this datapoint is of the basic derived type - not reserved
         *      for those datapoints of type DERIVED_TYPE (for example, SAFETY_OUTPUT_TYPE
         *      is a derived type data point)
         */
        static bool isDerivedDataPointType(const std::string& dataPointType);

        /**
         * @return map of <string type identifiers : string descriptions> - for use in defining
         *  the MMS states for alarms
         */
        static std::map<std::string, std::string> getMmsAlarmTypes();

    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        ScadaDatabaseAccessor( const ScadaDatabaseAccessor& theScadaDatabaseAccessor){ };


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        ScadaDatabaseAccessor& operator=(const ScadaDatabaseAccessor& pm);


        // This is a map of IConfigEntity* objects stored by their unique identifier
        typedef std::map<unsigned long, TA_Base_Core::IConfigEntity*> LoadedScadaItems;

        // This is a map of IConfigDerivedDataPoint's stored by the unique identifier of the matching
        // SCADA item.
        typedef std::map<unsigned long, TA_Base_Core::IConfigDerivedDataPoint*> LoadedDerivedDataPoints;

        LoadedScadaItems m_items;   // This is all the items loaded from the database
        LoadedDerivedDataPoints m_derivedDataPoints; // This is all the currently loaded derived datapoints

        bool m_isLoaded;   // This indicates if all the root items are loaded

        std::vector<std::string> m_entityTypes;   // This is the types of all entities this object represents

        DataPointStateDataHelper m_stateHelper;
    };
}

#endif // !defined(ScadaDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
