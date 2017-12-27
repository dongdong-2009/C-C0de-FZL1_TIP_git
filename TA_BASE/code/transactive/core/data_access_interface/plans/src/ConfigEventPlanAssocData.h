/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/ConfigEventPlanAssocData.h $
  * @author:  Brad Cleaver
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/28 10:18:09 $
  * Last modified by:  $Author: weikun.lin $
  *
  * ConfigEventPlanAssocData is an interface to the EventPlanMap table. It provides read and write
  * access for new and existing EventPlanAssociations
  */

#if !defined(_CONFIG_EVENT_PLAN_ASSOC_DATA_H_)
#define _CONFIG_EVENT_PLAN_ASSOC_DATA_H_

#include <string>

#include "core/data_access_interface/plans/src/EventPlanAssocHelper.h"
#include "core/data_access_interface/plans/src/IConfigEventPlanAssocData.h"

namespace TA_Base_Core
{
    class ConfigEventPlanAssocData : public IConfigEventPlanAssocData
    {
    public:

        /**
         * Constructor
         *
         * Creates a new event plan association.
         */
        ConfigEventPlanAssocData();


        /**
         * Constructor
         *
         * Construct a ConfigEventPlanAssocData object for the entry represented by the specified data.
         * The ConfigEventPlanAssocData will be constructed with valid data.
         *
         * @param key The key of the event plan association.
         */
        ConfigEventPlanAssocData( unsigned long key,
                                  bool enabled,
                                  unsigned long eventTypeKey,
                                  unsigned long entityKey,
                                  unsigned long entityTypeKey,
                                  const std::string& planPath,
                                  time_t dateCreated,
                                  time_t dateModified );


        /**
         * Copy Constructor
         *
         * Creates a new event plan association based of the provided.
         */
        ConfigEventPlanAssocData( const ConfigEventPlanAssocData& rhs );


        /**
         * Destructor
         */
        virtual ~ConfigEventPlanAssocData();


        ///////////////////////////////////
        // Methods inherited from IItem. //
        ///////////////////////////////////

        /**
         * getKey
         *
         * Returns the key for this item.
         *
         * @return The key for this item as an unsigned long.
         */
        virtual unsigned long getKey()
        {
            return m_helper.getKey();
        }


        /**
         * getName
         *
         * Returns the name of this item. If this is the first time data for this item
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName()
        {
            return "";
        }


        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();


        ////////////////////////////////////////
        // Methods inherited from IConfigItem //
        ////////////////////////////////////////

        /**
         * getUniqueIdentifier
         *
         * This retrieves the unqiue identifier for this item. We cannot use keys for identification
         * in the configuration editor because new items do not have keys. Therefore we use this
         * method for identification
         *
         * @return unsigned long - The unique identifier for this item
         */
        virtual unsigned long getUniqueIdentifier()
        {
            return m_uniqueIdentifier;
        }


        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the item. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all item changes
         */
        virtual ItemChanges getAllItemChanges();


        /**
         * hasChanged
         *
         * This should be called to determine if any part of the item has been changed by the user.
         *
         * @return bool - This will return true if the item does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            return !m_eventPlanChanges.empty();
        }


        /**
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        virtual bool isNew()
        {
            return m_helper.isNew();
        }


        /**
         * setName
         *
         * Sets the name of this item locally.
         *
         * @param name The name to give this item.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the item key is invalid (and this is not a new item).
         */
        virtual void setName(const std::string& name)
        {
        }


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the Item object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This item has not been deleted
         */
        virtual void applyChanges();


        ////////////////////////////////////////////////
        // Methods inherited from IEventPlanAssocData //
        ////////////////////////////////////////////////

        /**
         * getEventTypeKey
         *
         * Returns the event type key for this association.
         *
         * @return unsigned long The event type key.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual unsigned long getEventTypeKey();


        /**
         * isEnabled
         *
         * Returns whether the association is enabled.
         *
         * @return bool True if enabled.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual bool isEnabled();


        /** 
         * getEntityKey
         *
         * Returns the entity key for this association.
         *
         * @return unsigned long The entity key.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual unsigned long getEntityKey();


        /** 
         * getEntityTypeKey
         *
         * Returns the entity type key for this association
         *
         * @return unsigned long the entity type key
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual unsigned long getEntityTypeKey();


		/** 
		  * getPlanPath
		  *
		  * Returns the full path of the plan this association maps to.
          * The path is in the format "[/<category1>/<categoryN>]/<planName>"
		  *	
		  * @return std::string fully qualified plan path
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format, if the wrong amount of data is retrieved,
          *            or if the event plan association key is invalid.
		  */
		virtual std::string getPlanPath();


        /**
         * getDateCreated
         *
         * Returns the date created for this alarm plan association.
         *
         * @return The date created for this alarm plan association as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this alarm plan association.
         *
         * @return The date modified for this alarm plan association as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual time_t getDateModified();


        //////////////////////////////////////////////////////
        // Methods inherited from IConfigEventPlanAssocData //
        //////////////////////////////////////////////////////

        /**
         * deleteThisAssociation
         *
         * Removes this event plan association from the database. 
         * The calling application MUST then delete this event plan association object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this event plan association was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This event plan association has not been deleted
         */
        virtual void deleteThisAssociation();


        /**
         * isUsingEntity
         *
         * This returns whether the entity or entity type is being used
         *
         * @return bool - True if entity is being used, false if entity type is being used
         */
        virtual bool isUsingEntity();


        /** 
         * setEventTypeKey
         *
         * Sets the event type key for this association
         *
         * @param eventTypeKey The event type key.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual void setEventTypeKey( const unsigned long eventTypeKey );


        /**
         * setEnabled
         *
         * Sets whether the association is enabled.
         *
         * @param enabled True if enabled.
         */
        virtual void setEnabled( const bool enabled );


        /** 
          * setEntityKey
          *
          * Sets the entity key for this association. Set to 0 if the entity key is not used.
          *
          * @param unsigned long the entitykey
          */
        virtual void setEntityKey( const unsigned long entityKey );


        /** 
          * setEntityTypeKey
          *
          * Sets the entity type key for this association. Set to 0 if the entity type key is not used.
          *
          * @param entityTypeKey The entity type key.
          */
        virtual void setEntityTypeKey( const unsigned long entityTypeKey );


        /** 
		  * setPlanPath
		  *
		  * Sets the plan path for this association
		  *
		  * @param std::string the plan path
		  */
		virtual void setPlanPath(const std::string& planPath);


        /**
         * getEventTypeName
         *
         * Returns the name of the event type for this association.
         *
         * @return std::string The event type name.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual std::string getEventTypeName();

        /**
         * getEventTypeDisplayName
         *
         * Returns the name of the event type for this association.
         *
         * @return std::string The event type name.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual std::string getEventTypeDisplayName();
		

        /**
         * setEventTypeKeyFromName
         *
         * Sets the event type key to that corresponding with the provided name.
         *
         * @param eventTypeName The name of the event type to associate.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
        virtual void setEventTypeKeyFromName( const std::string& eventTypeName );


        /**
         * getEntityName
         *
         * Returns the name of the entity for this association.
         *
         * @return std::string The entity name.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
         virtual std::string getEntityName();


        /**
         * setEntityKeyFromName
         *
         * Sets the entity key to that corresponding with the provided name.
         *
         * @param entityName The name of the entity to associate.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
         virtual void setEntityKeyFromName( const std::string& entityName );


        /**
         * getEntityTypeName
         *
         * Returns the name of the entity type for this association.
         *
         * @return std::string The entity type name.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
         virtual std::string getEntityTypeName();


        /**
         * setEntityTypeKeyFromName
         *
         * Sets the entity type key to that corresponding with the provided name.
         *
         * @param entityTypeName The name of the entity type to associate.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format, if the wrong amount of data is retrieved,
         *            or if the event plan association key is invalid.
         */
         virtual void setEntityTypeKeyFromName( const std::string& entityTypeName );


    private:
        // Hidden methods.
        const ConfigEventPlanAssocData& operator=( const ConfigEventPlanAssocData& );

        /**
         * updateChanges
         *
         * Tracks any configuration changes.
         *
         * @param name The name of the changed item.
         * @param oldValue The previous value of the item.
         * @param newValue The current value of the time.
         */
        template <typename ItemType>
        void updateChanges(const std::string& name, const ItemType& oldValue, const ItemType& newValue);


        // The helper used for most DB access.
        EventPlanAssocHelper m_helper;

        // This variable stores the unique key for this association. It is NOT equivalent
        // to the pkey of the underlying DAI object.
        unsigned long m_uniqueIdentifier;

        // Use to ensure that m_uniqueIdentifier is indeed unique.
        static unsigned long s_uniqueIdentifier;

        ItemChanges m_eventPlanChanges; // This is a map of all changes made to the event plan association so
                                        // that they are recorded and can be sent in audit or online update messages.
                                        // It is keyed on the attribute name.

        typedef enum
        {
            E_ENTITY,
            E_ENTITYTYPE,
            E_UNDETERMINED
        } EUsing;

        // This variable tracks whether the entity or entity type is being used.
        EUsing m_using;
    };

} // Closes TA_Base_Core

#endif // !defined(_CONFIG_EVENT_PLAN_ASSOC_DATA_H_) 

