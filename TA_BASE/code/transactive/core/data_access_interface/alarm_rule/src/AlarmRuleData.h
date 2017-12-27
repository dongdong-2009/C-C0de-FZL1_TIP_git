/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/AlarmRuleData.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/20 22:28:15 $
  * Last modified by:  $Author: huirong.luo $
  *
  * ConfigAlarmRuleData is an implementation of IAlarmRuleData.
  */

#ifndef ALARM_RULE_DATA_H
#define ALARM_RULE_DATA_H

#include "core/data_access_interface/alarm_rule/src/IAlarmRuleData.h"

namespace TA_Base_Core
{
    // Forward declaration
    class AlarmRuleHelper;


    class AlarmRuleData : public IAlarmRuleData
    {

    public:

        /**
         * Destructor
         */
        virtual ~AlarmRuleData();


        /**
         * Constructor
         *
         * Provides this object with an Alarm Rule Helper
         * object to which to delegate its method calls.
         * This object takes responsibility for cleaning up the pointer.
         *
         * @param helper  The IAlarmRuleData object to delegate calls to.
         *                Must not be NULL.
         */
        AlarmRuleData(AlarmRuleHelper* helper);
        

        /** 
          * AlarmRuleData
          *
          * Creates a copy of the given Alarm Rule
          *
          * @param theRule  the rule to copy
          */
        AlarmRuleData(const AlarmRuleData& theRule);

        

        /** 
          * getKey
          *
          * Gets the key associated with this Alarm Rule
          *
          * @return the key
          *
          */
        virtual unsigned long getKey();



        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the item.
         * If the map returned is empty then no changes have been made.
         * This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all item changes
         */
        virtual ItemChanges getAllItemChanges();

        
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the item has
         * been changed by the user.
         *
         * @return bool - This will return true if the item does not match
         *                the one in the database.
         *                It will return false if the data has not changed
         *                from that in the database.
         */
        virtual bool hasChanged();


        /**
         * isNew
         *
         * This should be called to determine if this item is new.
         *  A new item is one that has not yet been applied to the
         *  database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not
         *                been applied to the database.
         */
        virtual bool isNew();

        
        /**
         * getUniqueIdentifier
         *
         * This retrieves the unqiue identifier for this item.
         * We cannot use keys for identification in the configuration editor
         * because new items do not have keys. Therefore we use this method
         * for identification.
         *
         * @return unsigned long - The unique identifier for this item
         */
        virtual unsigned long getUniqueIdentifier();


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
         * deleteThisObject
         *
         * Removes this alarm rule from the database. 
         * The calling application MUST then delete this object,
         * as it makes no sense to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this alarm rule was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This alarm rule has not been deleted
         */
        virtual void deleteThisObject();


        /**
         * getName
         *
         * Returns entity name, entity type name and alarm type name concatenated.
         *
         */
        virtual std::string getName();
		

        /**
         * setName
         *
         * This method is not implemented, as there is no good way of assigning
         * a name to an alarm rule. As such, it will assert if called.
         */
        virtual void setName(const std::string& name);


        /**
         * invalidate
         *
         * Mark the data contained by this Alarm Rule as invalid.
         * The next call to get...() following a call to invalidate() will
         * cause all the data to be reloaded from the database.
         */
        virtual void invalidate();


        
        /** 
          * getEntityTypeKey
          *
          * Returns the key of the entity's type.
          *
          * @return The key as an unsigned long.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
        virtual unsigned long getEntityTypeKey();


        /** 
          * setEntityTypeKey
          *
          * Sets the type of the entity.
          *
          * @param The key for the entity type as an unsigned long.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
        virtual void setEntityTypeKey(unsigned long entityTypeKey);


        /** 
          * setEntityType
          *
          * Sets the entity type by name.
          *
          * @param entityName  the new entity type name of the alarm rule.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
        virtual void setEntityType(const std::string& entityTypeName);
    

        /** 
          * getEntityTypeName
          *
          * Gets the name of the entity type.
          *
          * @return  the name of the entity type, or blank if the
          *          entityTypeKey is not specified (ie 0).
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved, or if the entity type name does not exist.
          *
          */
        virtual std::string getEntityTypeName();

        /** 
          * getEntityTypeName
          *
          * Gets the name of the entity type.
          *
          * @return  the name of the entity type, or blank if the
          *          entityTypeKey is not specified (ie 0).
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved, or if the entity type name does not exist.
          *
          */
        virtual std::string getEntityTypeName(unsigned long entityTypeKey);

        
        /** 
          * getEntityKey
          *
          * Returns the key of the entity associated with this alarm rule.
          *
          * @return The key as an unsigned long.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
        virtual unsigned long getEntityKey();


        /** 
          * setEntityKey
          *
          * Sets the entity associated with this alarm rule.
          *
          * @param entityKey The key of the entity.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
        virtual void setEntityKey(unsigned long entityKey);


        /** 
          * setEntity
          *
          * Sets the entity by name.
          *
          * @param entityName  the new entity name of the alarm rule.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
        virtual void setEntity(const std::string& entityName);
    

        /** 
          * getEntityName
          *
          * Gets the name of the entity.
          *
          * @return  the name of the entity, or blank if the
          *          entityKey is not specified (ie 0).
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved, or if the entity does not exist.
          *
          */
        virtual std::string getEntityName();


        /** 
          * getAlarmType
          *
          * Returns the key of the type of the alarm.
          *
          * @return A string containing the alarm type key.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
        virtual unsigned long getAlarmType();


        /** 
          * setAlarmType
          *
          * Sets the key of the type of the alarm associated with the rule.
          *
          * @param alarmType A string containing the alarm type key.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
        virtual void setAlarmType(const unsigned long alarmType);


        /** 
          * getAlarmTypeName
          *
          * Gets the name of the alarm type.
          *
          * @return  the name of the alarm type.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved, or if the alarm type does not exist.
          *
          */
        virtual std::string getAlarmTypeName();

		 /** 
          * getAlarmTypeDisplayName
          *
          * Gets the displayName of the alarm type.
          *
          * @return  the displayName of the alarm type.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved, or if the alarm type does not exist.
          *
          */
        virtual std::string getAlarmTypeDisplayName();




        /** 
          * setAlarmTypeByName
          *
          * Sets the alarm type by name.
          *
          * @param alarmTypeName  the new alarm type name of the alarm rule.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
        virtual void setAlarmTypeByName(const std::string& alarmTypeName);

		/** 
          * setAlarmTypeByDisplayName
          *
          * Sets the alarm type by display name.
          *
          * @param alarmTypeDisplayName  the new alarm type display name of the alarm rule.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
        virtual void setAlarmTypeByDisplayName(const std::string& alarmTypeDisplayName);

        
        /** 
          * getRuleType
          *
          * Returns the rule type associated with this alarm rule
          *
          * @return The rule type as a string.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
        virtual std::string getRuleType();


        /** 
          * getRuleTrigger
          *
          * Returns the trigger associated with this alarm rule.
          *
          * @return The rule trigger as a string.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
        virtual std::string getRuleTrigger();


        /** 
          * setRuleTrigger
          *
          * Sets the trigger associated with this alarm rule. Must be
          * either "submit", "acknowledge", or "close".
          *
          * @param ruleTrigger A string containing the trigger.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
        virtual void setRuleTrigger(const std::string& ruleTrigger);


        /** 
          * setAlarmDescription
          *
          * Sets the alarm description.
          *
          * @param description  the new alarm description of the alarm rule.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
        virtual void setAlarmDescription(const std::string& description);


        /** 
          * getAlarmDescription
          *
          * Gets the description of the alarm.
          *
          * @return  the description of the alarm, or blank if
          *          not specified (ie 0).
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved, or if the entity does not exist.
          *
          */
        virtual std::string getAlarmDescription();

        /**
          * getCreatedBySessionKey
          *
          * Returns the key for the session under which the alarm rule was created under.
          *
          * @return The key for the session under which the alarm rule was created under.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
		std::string getCreatedBySessionKey();
		unsigned long getCreatedByProfileKey();


        /**
         * getDateCreated
         *
         * Returns the date created for this alarm rule.
         *
         * @return The date created for this alarm rule as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
        time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this alarm rule.
         *
         * @return The date modified for this alarm rule as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
        time_t getDateModified();



        /** 
          * getUnsignedLongAsString
          *
          * Converts the parameter to a string and returns it.
          *
          * @return the number as a string
          *
          */
        static std::string getUnsignedLongAsString(unsigned long number);


        /** 
          * getStringAsUnsignedLong
          *
          * Converts the parameter to an unsigned long and returns it.
          *
          * @return the string as an unsigned long
          *
          * @exception DataException  if the conversion cannot be made.
          */
        static unsigned long getStringAsUnsignedLong(
            const std::string& theString);


        /** 
          * getEntityName
          *
          * Given an entity key, gets the name of that entity.
          *
          * @return the name of the entity
          *
          * @param entityKey  the primary key of the entity
          */
        static std::string getEntityName(unsigned long entityKey);


        /** 
          * getEntityKeyFromName
          *
          * Given an entity name, gets the key of that entity.
          *
          * @return the key as an unsigned long
          *
          * @param name  the entity name.
          */
        static unsigned long getEntityKeyFromName(const std::string& name);


        /** 
          * getAlarmTypeName
          *
          * Given an alarm type key, gets the name of that alarm type.
          *
          * @return the name of the alarm type.
          *
          * @param alarmTypeKey  the primary key of the alarm type
          */
        static std::string getAlarmTypeName(
            unsigned long alarmTypeKey);

		 /** 
          * getAlarmTypeDisplayName
          *
          * Given an alarm type key, gets the display name of that alarm type.
          *
          * @return the display name of the alarm type.
          *
          * @param alarmTypeKey  the primary key of the alarm type
          */
		static std::string AlarmRuleData::getAlarmTypeDisplayName(
			unsigned long alarmTypeKey);


        /** 
          * getAlarmTypeKeyFromName
          *
          * Given an alarm type name, gets the key of that alarm type.
          *
          * @return the alarm type key
          *
          * @param alarmTypeName  the name of the alarm type.
          */
        static unsigned long getAlarmTypeKeyFromName(

			const std::string& alarmTypeName);

		 /** 
          * getAlarmTypeKeyFromDisplayName
          *
          * Given an alarm type display name, gets the key of that alarm type.
          *
          * @return the alarm type key
          *
          * @param alarmTypeDisplayName  the display name of the alarm type.
          */
		static unsigned long AlarmRuleData::getAlarmTypeKeyFromDisplayName(
			const std::string& alarmTypeDisplayName);




protected:

        /** 
          * getHelper
          *
          * Gets the helper passed to the constructor.
          *
          * @return a reference to the AlarmRuleHelper passed to the
          *         constructor.
          *
          */
        AlarmRuleHelper& getHelper() const;

	    
        /**
         * updateAlarmRuleChanges
         *
         * This updates the map recording the changes to the Alarm Rule.
         * Whenever a call to set is made in this class then this method
         * must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updateAlarmRuleChanges(
            const std::string& name,
            const std::string& oldValue,
            const std::string& newValue);
 

        /**
         * updateAlarmRuleChanges
         *
         * This updates the map recording the changes to the Alarm Rule.
         * Whenever a call to set is made in this class then this method
         * must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param unsigned long - The old value that was stored for this attribute
         * @param unsigned long - The new value to be stored for this attribute
         */
        virtual void updateAlarmRuleChanges(
            const std::string& name,
            unsigned long oldValue,
            unsigned long newValue);
		


private:
        // Unused methods
		AlarmRuleData& operator=(const AlarmRuleData&);
        AlarmRuleData();



        AlarmRuleHelper* m_alarmRuleHelper;

        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of alarm rules in the
                                          // Configuration Editor because new alarm rules do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available

        ItemChanges m_alarmRuleChanges; // This is a map of all changes made to the alarm rule so that they are
                                        // recorded and can be sent in audit or online update messages.


        // The names of the parameters that are changed.
        static const std::string ENTITY_PARAM;
        static const std::string ENTITY_TYPE_PARAM;
        static const std::string RULE_TRIGGER_PARAM;
        static const std::string ALARM_TYPE_PARAM;
		static const std::string ALARM_DESCRIPTION_PARAM;



    };
} // closes TA_Base_Core

#endif // ALARM_RULE_DATA_H
