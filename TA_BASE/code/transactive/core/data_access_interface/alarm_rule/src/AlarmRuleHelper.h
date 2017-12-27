/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/AlarmRuleHelper.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/20 22:28:15 $
  * Last modified by:  $Author: huirong.luo $
  *
  * This class provides the actual database code for the IAlarmRuleData
  * interface. It provides access to the data stored in a single entry
  * in the AlarmRule table.
  *
  */

#ifndef ALARM_RULE_HELPER_H
#define ALARM_RULE_HELPER_H

#include "core/data_access_interface/alarm_rule/src/IAlarmRuleData.h"
#include "core/data_access_interface/alarm_rule/src/IAlarmRuleActionParameterData.h"

#include <vector>
#include <ctime>

namespace TA_Base_Core
{
    // Forward declaration
    class AlarmRuleActionParameterHelper;
    
    
    class AlarmRuleHelper
    {

    public:
        
        typedef IAlarmRuleActionParameterData::ActionParameterVector
            ActionParameterVector;


        /** 
          * AlarmRuleHelper
          *
          * Creates a new AlarmRuleHelper object for a particular entity.
          *
          * @param entityTypeKey  The key of the entity's type.
          * @param entityKey      The entity's key.
          * @param alarmType      The key of the alarm type.
          * @param ruleType       The rule type associated with the alarm rule.
          * @param ruleTrigger    The trigger for this alarm rule.
          *
          */
        AlarmRuleHelper(
            unsigned long entityTypeKey,
            unsigned long entityKey,
            unsigned long alarmType,
            const std::string& ruleType,
            const std::string& ruleTrigger);


        /** 
          * AlarmRuleHelper
          *
          * Creates a new AlarmRuleHelper object for a particular entity.
          *
          * @param entityTypeKey        The key of the entity's type.
          * @param entityKey            The entity's key.
          * @param alarmType            The key of the alarm type.
          * @param ruleType             The rule type associated with the alarm rule.
          * @param ruleTrigger          The trigger for this alarm rule.
          * @param createdBySessionKey  The key of session under which the rule was created by
          *
          */
        AlarmRuleHelper(
            unsigned long entityTypeKey,
            unsigned long entityKey,
            unsigned long alarmType,
            const std::string& ruleType,
            const std::string& ruleTrigger,
			const std::string& alarmDescription,
			const std::string& createdBySessionKey = "");


        /** 
          * AlarmRuleHelper
          *
          * Gets an existing Alarm Rule from the database.
          *
          * @param pkey  the pkey of the Alarm Rule.
          */
        AlarmRuleHelper(unsigned long pkey);


        /** 
          * AlarmRuleHelper
          *
          * Creates a copy of the given object.
          *
          * @param original  the object that will be copied.
          */
        AlarmRuleHelper(
            AlarmRuleHelper& original);


        /** 
          * ~AlarmRuleHelper
          *
          * Desctructor
          *
          */
        virtual ~AlarmRuleHelper();


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
         * getName
         *
         * This method is not implemented, as there is no good way of assigning
         * a name to an alarm rule. As such, it will assert if called.
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
          * Gets the name of the entity type with the given pkey.
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
        virtual void setAlarmType(unsigned long alarmType);

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
          * Gets the display name of the alarm type.
          *
          * @return  the display name of the alarm type.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved, or if the alarm type does not exist.
          *
          */
		std::string getAlarmTypeDisplayName();

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
          * setRuleType
          *
          * Sets the rule type to be associated with this alarm rule.
          *
          * @param The rule type as a string.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
        virtual void setRuleType(const std::string& ruleType);

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
          * getCreatedBySessionKey
          *
          * Returns the key of the session under which the alarm rule was created.
          *
          * @return The key of the session under which the alarm rule was created.
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
	    void setCreatedByProfileKey(unsigned long profileKey);
		std::string getCreatedByOperatorName();
		void setCreatedByOperatorName(std::string operatorName);

        /** 
          * applyChanges
          *
          * Writes the changes made to the AlarmRuleHelper object.
          * If a new Alarm Rule is being created, creates it in the database.
          * Otherwise, the existing Alarm Rule is updated.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataConfigurationException Thrown if the alarm
          *            rule data is not fully specified.
          */
        virtual void applyChanges();

        /**
          * deleteThisObject
          *
          * Deletes the alarm rule associated with this AlarmRuleHelper
          * object from the database, and any parameters it has from the
          * AlarmRuleActionParameter table.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          */
        virtual void deleteThisObject();

        
        /** 
          * invalidate
          *
          * Used to flag this object as being out of date so that it is
          * reloaded from the database the next time a get method is called.
          *
          */
        virtual void invalidate();




        /** 
          * isNew
          *
          * Indicates if this rule has been written to the database yet.
          *
          * @return false if the rule has been written to the database,
          *         true otherwise.
          *
          */
        virtual bool isNew();


        /** 
          * getParameters
          *
          * Gets the a list of the direct children of this Alarm Rule.
          * Note that these children themselves can have an associated
          * Alarm Rule Action Parameter, and so on in a linked list.
          *
          * eg
          *
          *     -->AP-->AP
          *     |
          *  AR -->AP-->AP-->AP
          *     |
          *     -->AP
          *
          *
          * Only the action parameters in the first column are returned.
          *
          * @return an ActionParameterVector containing the immediate
          *         children of this Alarm Rule.
          *
          */
        virtual ActionParameterVector getParameters();

        
        /** 
          * addNewParameter
          *
          * Creates a new parameter and adds it to the list of parameters
          * associated with this Alarm Rule.
          *
          * @param type   the name of the parameter
          * @param value  the value of the parameter (optional)
          *
          * @return the parameter that was added.
          */
        virtual IAlarmRuleActionParameterData* addNewParameter(
            const std::string& type, const std::string& value = "");


        /** 
          * addCopiedParameter
          *
          * Creates a copy of the given parameter and adds it to the
          * list of parameters associated with this Alarm Rule.
          *
          * @param AlarmRuleActionParameter& parameterToCopy
          *
          * @return the parameter that was added.
          */
        virtual IAlarmRuleActionParameterData* addCopiedParameter(
            const AlarmRuleActionParameterHelper& parameterToCopy);


        /** 
          * deleteParameter
          *
          * Deletes the parameter with the given pkey from the list
          * of parameters associated with this Alarm Rule.
          * Note that this method must not be called on new parameters.
          * Use deleteParameter(IAlarmRuleActionParameterData* parameter)
          * instead.
          *
          * @param pkey  the key associated with the parameter.
          */
        virtual void deleteParameter(unsigned long pkey);

        /** 
          * deleteParameter
          *
          * Deletes the given parameter from this alarm rule.
          *
          * @param parameter  the parameter to delete.
          */
        virtual void deleteParameter(IAlarmRuleActionParameterData* parameter);


    protected:
        
        
        /** 
          * AlarmRuleHelper
          *
          * Constructs a new AlarmRuleHelper object that is uninitialised.
          * The attributes of the object must then be set using the 
          * methods of this class.
          *
          * @param ruleType
          * the type of rule, eg SuppressionRuleData::RULE_TYPE
          *
          * @param ruleTrigger
          * the rule trigger, eg IAlarmRuleData::ACKNOWLEDGE_RULE_TRIGGER
          */
        AlarmRuleHelper(
            const std::string& ruleType, const std::string ruleTrigger);


        /** 
          * createNewAlarmRule
          *
          * Creates a new alarm rule in the database.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if the pkey cannot be retrieved after creating the
          * Alarm Rule.
          *
          */
        virtual void createNewAlarmRule();

        /** 
          * updateAlarmRule
          *
          * Updates an alarm rule in the database.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if the pkey cannot be retrieved after creating the
          * Alarm Rule.
          *
          */
        virtual void updateAlarmRule();



        /** 
          * cleanUpDeletedParameters
          *
          * Deletes the pointers to parameters that are in the
          * deletedParameters vector and clears it.
          * Note that this does not delete the objects out of the database.
          *
          */
        virtual void cleanUpDeletedParameters();

        /** 
          * cleanUpParameters
          *
          * Deletes the pointers to parameters that are in the
          * parameters vector and clears it.
          * Note that this does not delete the objects out of the database.
          *
          */
        virtual void cleanUpParameters();



        /** 
          * reload
          *
          * Reloads the data for this alarm rule from the database,
          * if it is not new or up to date.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
        virtual void reload();

        /** 
          * forceReload
          *
          * Reloads the data for this alarm rule from the database,
          * regardless of whether it is up to date.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
        virtual void forceReload();


        /** 
          * forceReloadAlarmRule
          *
          * Reloads the attributes of this alarm rule from the database.
          *
          */
        virtual void forceReloadAlarmRule();


        /** 
          * forceReloadParameters
          *
          * Reloads the parameters associated with this alarm rule from
          * the database.
          *
          */
        virtual void forceReloadParameters();



        /** 
          * getUnsignedLongAsString
          *
          * Converts the parameter to a string and returns it.
          *
          * @return the number as a string
          *
          */
        virtual std::string getUnsignedLongAsString(unsigned long number);


        /** 
          * getStringAsUnsignedLong
          *
          * Converts the parameter to an unsigned long and returns it.
          *
          * @return the string as an unsigned long
          *
          * @exception DataException  if the conversion cannot be made.
          */
        virtual unsigned long getStringAsUnsignedLong(
            const std::string& theString);





    private:

        AlarmRuleHelper& operator=(const AlarmRuleHelper &);
        AlarmRuleHelper();



        unsigned long m_pkey;
        unsigned long m_entityTypeKey;
        unsigned long m_entityKey;      
        unsigned long m_alarmTypeKey;        
        std::string m_ruleType;
        std::string m_ruleTrigger;
		std::string m_alarmDescription;
        time_t m_dateCreated;
        time_t m_dateModified;
		std::string m_createdBySessionKey;
		unsigned long  m_createdByProfileKey;
		std::string m_createdByOperatorName;


        // The parameters associated with this Alarm Rule
        ActionParameterVector m_parameters;

        // Any parameters that have been deleted. These will be deleted
        // from the database when applyChanges is called.
        ActionParameterVector m_deletedParameters;


        // Indicates whether this Alarm Rule is new or should
        // be read from the database.
        bool m_isNew;

        // Indicates whether this Alarm Rule is new
        // (ie has not been written to the database)
        bool m_isValidData;
    };

} // Closes TA_Base_Core

#endif // ALARM_RULE_HELPER_H

