/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/AlarmRuleActionParameterHelper.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class provides the actual database code for the
  * IAlarmRuleActionParameterData interface.
  * It provides access to the data stored in a single entry
  * in the AlarmRuleActionParameter table.
  *
  */

#ifndef ALARM_RULE_ACTION_PARAMETER_HELPER_H
#define ALARM_RULE_ACTION_PARAMETER_HELPER_H

#include "core/data_access_interface/alarm_rule/src/AlarmRuleHelper.h"
#include "core/data_access_interface/alarm_rule/src/IAlarmRuleActionParameterData.h"

namespace TA_Base_Core
{
    class AlarmRuleActionParameterHelper : public IAlarmRuleActionParameterData
    {

    public:

        /** 
          * AlarmRuleActionParameterHelper
          *
          * Creates a new AlarmRuleActionParameter associated with the given
          * AlarmRule.
          *
          * @param parentAlarmRule  the alarm rule of which this is
          *                         a parameter.
          * @param type             the name of the parameter.
          * @param value            the value of the parameter.
          */
        AlarmRuleActionParameterHelper(
            AlarmRuleHelper& parentAlarmRule,
            const std::string& type,
            const std::string& value = "");
        

        /** 
          * AlarmRuleActionParameterHelper
          *
          * Creates a new AlarmRuleActionParameter that is a copy
          * of the given AlarmRuleActionParameter.
          * It will have the same parent Alarm Rule.
          *
          * @param parameter  the AlarmRuleActionParameter to copy.
          */
        AlarmRuleActionParameterHelper(
            const AlarmRuleActionParameterHelper& parameter);


        /** 
          * AlarmRuleActionParameterHelper
          *
          * Gets an existing AlarmRuleActionParameter from the database.
          *
          * @param pkey  the pkey of the Alarm Rule Action Parameter.
          */
        AlarmRuleActionParameterHelper(unsigned long pkey);


        /** 
          * ~AlarmRuleActionParameterHelper
          *
          * Destructor
          *
          */
        virtual ~AlarmRuleActionParameterHelper();


        /** 
          * getKey
          *
          * Gets the key associated with this parameter.
          *
          * @return the key
          *
          */
        virtual unsigned long getKey();


        /** 
          * getType
          *
          * Returns the type of parameter this is.
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
        virtual std::string getType();

        /** 
          * setType
          *
          * Sets the type of parameter this is.
          *
          * @param type  The type of parameter this is.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
        virtual void setType(const std::string& type);



        /** 
          * getValue
          *
          * Returns the value of parameter this is.
          *
          * @return The value as a string.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
        virtual std::string getValue();

        /** 
          * setValue
          *
          * Sets the value of this parameter.
          *
          * @param value  The value of the parameter.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
        virtual void setValue(const std::string& value);




        /** 
          * getRelatedParameter
          *
          * Gets the parameter related to this one.
          * Note this method will only function correctly if
          * applyChanges is called after setParameterKey.
          *
          * Note: This object is responsible for the memory the pointer
          *       refers to.
          *
          * @return a pointer to an IAlarmRuleActionParameterData that is
          *         the parameter related to this one, or NULL if there is
          *         no related parameter.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if there is a problem with the format of the data
          * retrieved.
          *
          */
        virtual IAlarmRuleActionParameterData* getRelatedParameter();
        

        /** 
          * addRelatedParameter
          *
          * Adds a related parameter to this one, with the given type and value.
          *
          * @param type  the name of the new parameter
          * @param value the value of the new parameter
          *
          */
        virtual void addRelatedParameter(
            const std::string& type, const std::string& value = "");


        /** 
          * deleteRelatedParameter
          *
          * Deletes the related parameter from the database.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          */
        virtual void deleteRelatedParameter();



        /** 
          * getAlarmRuleKey
          *
          * Returns the key of the alarm rule for which this is a parameter.
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
        virtual unsigned long getAlarmRuleKey();


        
        /** 
          * applyChanges
          *
          * Writes the changes made to the AlarmRuleActionParameterHelper object.
          * If a new parameter is being created, creates it in the database.
          * Otherwise, the existing parameter is updated.
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
          * Deletes the parameter from the database.
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



    protected:
        
        
        /** 
          * reload
          *
          * Reloads the data for this parameter from the database,
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
          * Reloads the data for this parameter from the database,
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
          * createNewAlarmRuleActionParameter
          *
          * Creates a new alarm rule action parameter in the database.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if the pkey cannot be retrieved after creating the
          * parameter.
          *
          */
        virtual void createNewAlarmRuleActionParameter();


        /** 
          * updateAlarmRuleActionParameter
          *
          * Updates a parameter in the database.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          * @exception DataException
          * Thrown if the pkey cannot be retrieved after creating the
          * parameter.
          *
          */
        virtual void updateAlarmRuleActionParameter();


        /** 
          * isNew
          *
          * Indicates if this parameter has been written to the database yet.
          *
          * @return false if the parameter has been written to the database,
          *         true otherwise.
          *
          */
        virtual bool isNew();


        /** 
          * AlarmRuleActionParameterHelper
          *
          * Creates a new Alarm Rule Action Parameter with no parent alarm
          * rule.
          *
          * @param type  the name of the parameter
          * @param value the value of the parameter
          *
          */
        AlarmRuleActionParameterHelper(
            const std::string& type,
            const std::string& value = "");


    private:

        AlarmRuleActionParameterHelper& operator=(
            const AlarmRuleActionParameterHelper &);

        AlarmRuleActionParameterHelper();



        unsigned long m_pkey;
        unsigned long m_alarmRuleKey;
        std::string m_type;
        std::string m_value;

        IAlarmRuleActionParameterData* m_relatedParameter;

        // The parent alarm rule.
        // This is NULL unless the parent alarm rule has not yet
        // been written to the database.
        AlarmRuleHelper* m_parentAlarmRule;


        // Indicates whether this Alarm Rule is new
        // (ie has not been written to the database)
        bool m_isNew;

        // Indicates if the data in this object is up to date with
        // the database
        bool m_isValidData;
    };

} // Closes TA_Base_Core

#endif // ALARM_RULE_ACTION_PARAMETER_HELPER_H

