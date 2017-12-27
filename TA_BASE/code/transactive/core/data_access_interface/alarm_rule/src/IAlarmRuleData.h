/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/IAlarmRuleData.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/20 22:28:15 $
  * Last modified by:  $Author: huirong.luo $
  *
  * IAlarmRuleData is an interface to the AlarmRule table. It provides
  * access for existing alarm rules, and the ability to create new alarm rules.
  *
  */

#if !defined(IALARMRULEDATA_H)
#define IALARMRULEDATA_H

#include <string>

#include "core/data_access_interface/src/IItem.h"
#include "core/data_access_interface/src/IConfigItem.h"

namespace TA_Base_Core
{

    class IAlarmRuleData : public IItem, public IConfigItem
    {

    public:
 
        // The allowable values for the rule trigger
        static const std::string RULE_TRIGGER_ACKNOWLEDGE;
        static const std::string RULE_TRIGGER_SUBMIT;
        static const std::string RULE_TRIGGER_CLOSE;



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
        virtual unsigned long getEntityTypeKey() = 0;


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
        virtual void setEntityTypeKey(unsigned long entityTypeKey) = 0;


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
        virtual void setEntityType(const std::string& entityTypeName) = 0;

        
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
          * retrieved, or if the entity type does not exist.
          *
          */
        virtual std::string getEntityTypeName() = 0;


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
        virtual unsigned long getEntityKey() = 0;


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
        virtual void setEntityKey(unsigned long entityKey) = 0;


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
        virtual void setEntity(const std::string& entityName) = 0;


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
        virtual std::string getEntityName() = 0;


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
        virtual void setAlarmDescription(const std::string& description) = 0;


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
        virtual std::string getAlarmDescription() = 0;

        /** 
          * getAlarmType
          *
          * Returns the key of the type of alarm.
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
        virtual unsigned long getAlarmType() = 0;


        /** 
          * setAlarmType
          *
          * Sets the key of the type of alarm associated with the rule.
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
        virtual void setAlarmType(const unsigned long alarmType) = 0;
        

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
        virtual std::string getAlarmTypeName() = 0;

		virtual std::string getAlarmTypeDisplayName() = 0;


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
        virtual void setAlarmTypeByName(const std::string& alarmTypeName) = 0;

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
        virtual void setAlarmTypeByDisplayName(const std::string& alarmTypeDisplayName) = 0;

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
        virtual std::string getRuleTrigger() = 0;


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
        virtual void setRuleTrigger(const std::string& alarmRuleTrigger) = 0;


        /** 
          * getRuleType
          *
          * Returns the type of this alarm rule.
          *
          * @return The type as a string.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
        virtual std::string getRuleType() = 0;



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
        virtual time_t getDateCreated() = 0;


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
        virtual time_t getDateModified() = 0;

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
        virtual void deleteThisObject() = 0;



        virtual ~IAlarmRuleData() {}

    };


} // Closes TA_Base_Core

#endif // IALARMRULEDATA_H

