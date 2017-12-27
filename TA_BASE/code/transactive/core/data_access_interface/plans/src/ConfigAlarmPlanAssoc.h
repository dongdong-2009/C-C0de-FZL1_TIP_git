/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/ConfigAlarmPlanAssoc.h $
 * @author:  Karen Graham
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/28 10:18:09 $
 * Last modified by:  $Author: weikun.lin $
 * 
 * ConfigAlarmPlanAssoc is an implementation of IConfigAlarmPlanAssoc. It holds the data specific to 
 * alarm plan associations and allows reads and writes.
 */

#if !defined(CONFIG_ALARM_PLAN_ASSOC_H)
#define CONFIG_ALARM_PLAN_ASSOC_H

#include <string>

#include "core/data_access_interface/plans/src/IConfigAlarmPlanAssoc.h"

namespace TA_Base_Core
{
	class AlarmPlanAssocHelper;
	
	class ConfigAlarmPlanAssoc : public IConfigAlarmPlanAssoc
	{

	public:

		/**
		 * ConfigAlarmPlanAssoc (constructor)
		 *
		 * Construct a ConfigAlarmPlanAssoc class for the entry represented by the specified data.
		 * The ConfigAlarmPlanAssoc will be constructed with valid data.
		 *
		 * @param key The key of this Alarm Plan Association in the database
		 * @param unsigned long entityKey
		 * @param unsigned long entityTypeKey
		 * @param const std::string& alarmType
		 * @param const std::string& planPath
         * @param time_t createTime 
         * @param time_t modifyTime
		 */
		ConfigAlarmPlanAssoc( unsigned long key, 
			                  unsigned long entityKey,
			                  unsigned long entityTypeKey,
			                  unsigned long alarmType,
			                  const std::string& planPath,
                              time_t createTime,
                              time_t modifyTime);

        /**
         * Constructor
         *
         * This creates a new alarm plan association
         */
        ConfigAlarmPlanAssoc( );

        /**
         * Copy Constructor
         *
         * This creates a new alarm plan association based on an existing one
         */
        ConfigAlarmPlanAssoc( const ConfigAlarmPlanAssoc& rhs);


		virtual ~ConfigAlarmPlanAssoc();
	
		/**
         * getKey
         *
         * Returns the key for this ConfigAlarmPlanAssoc
         *
         * @return The key for this ConfigAlarmPlanAssoc as an unsigned long.
         */
		virtual unsigned long getKey();


        /**
         * getName
         *
         * Returns the name of this item. For alarm plan associations this is a name built up from the
         * entity or entity type and the alarm type.
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
        virtual std::string getName();
		
        
        /** 
		  * getAlarmType
		  *
		  * Returns the alarm type for this association
		  *
		  * @return std::string The alarm type
		  */
		virtual unsigned long getAlarmType();


		/** 
		  * getEntityKey
		  *
		  * Returns the entity key for this association
		  *
		  * @return unsigned long the entitykey
		  */
		virtual unsigned long getEntityKey();


		/** 
		  * getEntityTypeKey
		  *
		  * Returns the entity type key for this association
		  *
		  * @return unsigned long the entity type key
		  */
		virtual unsigned long getEntityTypeKey();


		/** 
		  * getPlanPath
		  *
		  * Returns the full path of the plan this association maps to.
          * The path is in the format "[/<category1>/<categoryN>]/<planName>"
		  *	
		  * @return std::string fully qualified plan path
		  */
		virtual std::string getPlanPath();


		/** 
		  * match
		  *
		  * This will just return false in this particular instance
		  */
		virtual bool match( const unsigned long& entityKey, const unsigned long& entityTypeKey, const unsigned long& alarmType)
        {
            return false;
        }

        
        /**
         * getAlarmTypeName
         *
         * @return This gets the display name for the current alarm type and returns it
         *
         * @exception DataException
         * @exception DatabaseException
         */
        virtual std::string getAlarmTypeName();

		/**
         * getAlarmTypeName
         *
         * @return This gets the display name for the current alarm type and returns it
         *
         * @exception DataException
         * @exception DatabaseException
         */
        virtual std::string getAlarmTypeDisplayName();

        
        /**
         * getEntityName
         *
         * @return This gets the display name for the current entity and returns it. If the entity is not set
         *         this returns ""
         *
         * @exception DataException
         * @exception DatabaseException
         */
        virtual std::string getEntityName();

        
        /**
         * getEntityTypeName
         *
         * @return This gets the display name for the current entity type and returns it. If the entity type
         *         is not set this returns ""
         *
         * @exception DataException
         * @exception DatabaseException
         */
        virtual std::string getEntityTypeName();


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
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the alarm plan association key is invalid (and this is not a new alarm plan association).
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
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the alarm plan association key is invalid (and this is not a new alarm plan association).
         */
        virtual time_t getDateModified();

        
        /**
         * isUsingEntity
         *
         * This returns whether the entity or entity type is being used
         *
         * @return bool - True if entity is being used, false if entity type is being used
         */
        virtual bool isUsingEntity();

        
        /**
         * setName
         *
         * This method cannot be used and will therefore just assert in this particular situation.
         */
        virtual void setName(const std::string& name);
		
        
        /** 
		  * setAlarmTypeKey
		  *
		  * Sets the alarm type for this association
		  *
		  * @param std::string The alarm type
		  */
        virtual void setAlarmTypeKey(unsigned long alarmType);


        /** 
		  * setAlarmType
		  *
		  * Sets the alarm type for this association. This is set using the name and must therefore look up
          * the key.
		  *
		  * @param string - An alarm type name
          * 
          * @exception DatabaseException
          * @exception DataException - This will be thrown if the name is invalid and cannot be found in 
          *                            the database. The type will be set to NO_VALUE.
		  */
        virtual void setAlarmType(const std::string& alarmTypeName);

		  /** 
		  * setAlarmTypeDisplay
		  *
		  * Sets the alarm type for this association. This is set using the display name and must therefore look up
          * the key.
		  *
		  * @param string - An alarm type display name
          * 
          * @exception DatabaseException
          * @exception DataException - This will be thrown if the name is invalid and cannot be found in 
          *                            the database. The type will be set to NO_VALUE.
		  */
        virtual void setAlarmTypeByDisplayName(const std::string& alarmTypeDisplayName);


        /** 
		  * setEntityKey
		  *
		  * Sets the entity key for this association. Set to 0 if the entity key is not used.
		  *
		  * @param unsigned long the entitykey
		  */
		virtual void setEntityKey(unsigned long entityKey);


		/** 
		  * setEntity
		  *
		  * Sets the entity for this association. This is set using the name and must therefore look up
          * the key. If the name is empty then this implies the entity is not set.
		  *
		  * @param string - An entity name
          * 
          * @exception DatabaseException
          * @exception DataException - This will be thrown if the name is invalid and cannot be found in 
          *                            the database. The type will be set to NO_VALUE.
		  */
        virtual void setEntity(const std::string& entityName);

        
        /** 
		  * setEntityTypeKey
		  *
		  * Sets the entity type key for this association. Set to 0 if the entity type key is not used.
		  *
		  * @param unsigned long the entity type key
		  */
		virtual void setEntityTypeKey(unsigned long entityTypeKey);


		/** 
		  * setEntityType
		  *
		  * Sets the entity type for this association. This is set using the name and must therefore look up
          * the key. If the name is empty then this implies the entity type is not set.
		  *
		  * @param string - An entity type name
          * 
          * @exception DatabaseException
          * @exception DataException - This will be thrown if the name is invalid and cannot be found in 
          *                            the database. The type will be set to NO_VALUE.
		  */
        virtual void setEntityType(const std::string& entityTypeName);

        
        /** 
		  * setPlanPath
		  *
		  * Sets the plan path for this association
		  *
		  * @param std::string the plan path
		  */
		virtual void setPlanPath(const std::string& planPath);

        
        /**
         * invalidate
         *
         * Mark the data contained by this Alarm type as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();


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
            // If the alarm plan association changes is empty then nothing has changed
            return !m_alarmPlanChanges.empty();
        }


        /**
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        virtual bool isNew();

        
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
        };


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


        /**
         * deleteThisAssociation
         *
         * Removes this alarm plan association from the database. 
         * The calling application MUST then delete this alarm plan association object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this alarm plan association was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This alarm plan association has not been deleted
         */
        virtual void deleteThisAssociation();


	private:
		ConfigAlarmPlanAssoc& operator=(const ConfigAlarmPlanAssoc &);

        /**
         * updateChanges
         *
         * This updates the map recording the changes to the alarm plan association. Whenever a call to set
         * is made in this class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const ItemType& - The old value that was stored for this attribute
         * @param const ItemType& - The new value to be stored for this attribute
         */
        template <typename ItemType>
        void updateChanges(const std::string& name, const ItemType& oldValue, const ItemType& newValue);


        // These are the names of the fixed fields that will be used in places where we are creating
        // strings that will be shown to the user.
        static const std::string ENTITY;
        static const std::string ENTITYTYPE;
        static const std::string ALARMTYPE;
        static const std::string PLAN;

		AlarmPlanAssocHelper* m_helper;


        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of alarm plan associations
                                          // in the Configuration Editor because new alarm plan associations
                                          // do not have keys. Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available

        bool m_isUsingEntity; // Indicates whether entity or entity type is being used.

        bool m_isValidData;

        ItemChanges m_alarmPlanChanges; // This is a map of all changes made to the alarm plan association so
                                        // that they are recorded and can be sent in audit or online update messages.
                                        // It is keyed on the attribute name.
	};
} // closes TA_Base_Core

#endif // !defined(CONFIG_ALARM_PLAN_ASSOC_H)
