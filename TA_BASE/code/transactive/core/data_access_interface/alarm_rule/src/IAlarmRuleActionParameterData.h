/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/IAlarmRuleActionParameterData.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class provides an interface to Alarm Rule Action Parameters
  * stored in the database.
  *
  */

#ifndef IALARM_RULE_ACTION_PARAMETER_DATA_H
#define IALARM_RULE_ACTION_PARAMETER_DATA_H


#include <string>
#include <vector>

namespace TA_Base_Core
{

    class IAlarmRuleActionParameterData
    {
    public:


        // A vector of action parameters.
        typedef std::vector<IAlarmRuleActionParameterData*>
            ActionParameterVector;


        virtual ~IAlarmRuleActionParameterData() {}


        
        /** 
          * getKey
          *
          * Gets the key associated with this parameter.
          *
          * @return the key
          *
          */
        virtual unsigned long getKey() = 0;


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
        virtual std::string getType() = 0;

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
        virtual void setType(const std::string& type) = 0;



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
        virtual std::string getValue() = 0;

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
        virtual void setValue(const std::string& value) = 0;


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
        virtual IAlarmRuleActionParameterData* getRelatedParameter() = 0;
        

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
            const std::string& type, const std::string& value = "") = 0;


        /** 
          * deleteRelatedParameter
          *
          * Deletes the related parameter from the database.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          *
          */
        virtual void deleteRelatedParameter() = 0;



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
        virtual unsigned long getAlarmRuleKey() = 0;


        /** 
          * isNew
          *
          * Indicates if this parameter has been written to the database yet.
          *
          * @return false if the parameter has been written to the database,
          *         true otherwise.
          *
          */
        virtual bool isNew() = 0;


        
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
        virtual void applyChanges() = 0;


        /**
          * deleteThisObject
          *
          * Deletes the parameter from the database.
          *
          * @exception DatabaseException
          * Thrown if there is a problem communicating with the database.
          */
        virtual void deleteThisObject() = 0;

        
        /** 
          * invalidate
          *
          * Used to flag this object as being out of date so that it is
          * reloaded from the database the next time a get method is called.
          *
          */
        virtual void invalidate() = 0;
    };

} // Closes TA_Base_Core

#endif // IALARM_RULE_ACTION_PARAMETER_DATA_H

