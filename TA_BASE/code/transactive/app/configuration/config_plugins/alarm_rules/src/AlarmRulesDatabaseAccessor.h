/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_rules/src/AlarmRulesDatabaseAccessor.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class uses the Data Access Interface to retrieve alarm plan association information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */


#if !defined(AlarmRulesDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
#define AlarmRulesDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_

#include <map>
#include <vector>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/IDatabaseAccessor.h"
#include "core/data_access_interface/alarm_rule/src/AvalancheSuppressionRuleData.h"

// Forward declarations
namespace TA_Base_Core
{
    class IAlarmRuleData;
    class IConfigItem;
}


namespace TA_Base_App
{
    class AlarmRulesDatabaseAccessor : public IDatabaseAccessor
    {

    public:

        struct AlarmRule
        {
            CString ruleType;    // The Rule Type. Must not be blank
            CString status;      // This will be the current applied status of the item (ie new, modified etc)
                                 // It will be set elsewhere so we don't worry about it
            CString alarmType;   // The alarm type name

			//add for chinese display:
			CString alarmTypeDisplay; //The alarm type display name  

            CString entity;      // The entity name or blank
            CString entityType;  // The entity type name or blank
            CString ruleTrigger; // The rule trigger or blank
			CString alarmDescription; // The alarm description or blank
        };


        /**
         * Constructor
         */
        AlarmRulesDatabaseAccessor();


        /**
         * Destructor
         */
        virtual ~AlarmRulesDatabaseAccessor();


        /**
         * loadItems
         *
         * This will retrieve all the alarm plan associations from the database and store them in m_alarmRules.
         *
         * @exception DataException - This will be thrown if there is an error with the alarm plan association
         *                            data and it cannot be retrieved for some reason.
         * @exception DatabaseException - This will be thrown if there is an error communicating with the
         *                                database.
         */
        void loadItems();

        
        /**
         * getItemNames
         *
         * This method cannot be used by alarm plan associations and will just assert
         */
        std::multimap<CString,unsigned long> getItemNames(CProgressCtrl& progress);
        

        /**
         * getAllItemDetails
         *
         * This retrieves the items' details in a display format.
         * Precondition: loadItems() must be called before this method
         * 
         * @param CProgressCtrl - This is used to show how long the loading is taking
         *
         * @return map<unsigned long, AlarmRule> - This is all the alarm plan assocations keyed
         *                                                    on their unique identifier
         */
        std::map<unsigned long,AlarmRule> getAllItemDetails(CProgressCtrl& progress);
        
        
        /**
         * getItemDetails
         *
         * This retrieves the item's details in a display format. The items is specified by passing
         * in its unique identifier.
         * Precondition: loadItems() must be called before this method
         * 
         * @param unsigned long - The identifier of the item to retrieve details for
         *
         * @return AlarmRule - This is all the alarm plan assocation details
         */
        AlarmRule getItemDetails(unsigned long identifier);
 
        
        /**
         * getItem
         *
         * This returns a database object for the item specified. The item is specified by
         * passing in its unique identifier..
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
         * @param the type of the alarm rule. This will indicate what subclass
         *        of IAlarmRuleData will be returned.
         *
         * @return IConfigItem* - The newly created item ready for its data to be populated
         */
        virtual TA_Base_Core::IConfigItem* newItem(const std::string& alarmRuleType);


        /** 
          * newItem
          *
          * Do not use this method. Use newItem(const std::string&) instead.
          *
          */
        TA_Base_Core::IConfigItem* newItem();


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
         void invalidateData()
         {
             m_isLoaded = false;
         }


        /**
         * getAllAlarmTypes
         *
         * This returns all alarm types and keys to the caller. This goes to the database each time
         * this method is called.
         *
         * @return map<unsigned long, string> - This returns all the alarm type keys and names. This is all
         *                                      information needed to update data and to display to the user
         */
        std::map<unsigned long, std::string> getAllAlarmTypes();

		/**
         * getAllAlarmTypeDisplays
         *
         * This returns all alarmType display names and keys to the caller. This goes to the database each time
         * this method is called.
         *
         * @return map<unsigned long, string> - This returns all the alarm type keys and displaynames. This is all
         *                                      information needed to update data and to display to the user
         */
        std::map<unsigned long, std::string> getAllAlarmTypeDisplays();


        /**
         * getAllEntityTypes
         *
         * This returns all entity types and keys to the caller. This goes to the database each time
         * this method is called.
         *
         * @return map<unsigned long, string> - This returns all the entity type keys and names. This is all
         *                                      information needed to update data and to display to the user
         */
        std::map<unsigned long, std::string> getAllEntityTypes();


        /** 
          * getEntityDescendants
          *
          * Gets all descendants of the entity with the given key.
          *
          * @param pkey  the key of the entity whose descendants are returned.
          *
          * @return  a vector of entity keys
          */
        std::vector<unsigned long> getEntityDescendants(unsigned long pkey);
		
		bool isChildAlsoAHead(TA_Base_Core::AvalancheSuppressionRuleData::EntityKeyAlarmTypePair& child,TA_Base_Core::IAlarmRuleData**head);

		bool isExistChild(TA_Base_Core::IAlarmRuleData* head, TA_Base_Core::IAlarmRuleData* child);

    protected:

        /** 
          * getEntityDescendantsRecursive
          *
          * Gets the children of the given entity recursively.
          * Called by getEntityDescendants.
          *
          * @param pkey  the key of the entity whose descendants are retrieved.
          *
          * @return a vector of pkeys of the children
          */
        std::vector<unsigned long> getEntityDescendantsRecursive(
                unsigned long pkey);


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        AlarmRulesDatabaseAccessor( const AlarmRulesDatabaseAccessor& theAlarmRulesDatabaseAccessor){ };


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        AlarmRulesDatabaseAccessor& operator=(const AlarmRulesDatabaseAccessor& pm);


        // This is a map of IConfigAlarmRules* objects stored by their key (unsigned long)
        typedef std::map<unsigned long, TA_Base_Core::IAlarmRuleData*> LoadedAlarmRules;

        LoadedAlarmRules m_alarmRules;   // This is all the alarm plan associations loaded from the database

        bool m_isLoaded;          // This indicates if we have loaded the alarm plan associations before or not.
                                  // If we have never loaded the alarm plan associations then when we are told
                                  // to refresh we shouldn't load them.

    };
}

#endif // !defined(AlarmRulesDatabaseAccessor_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
