/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/AvalancheSuppressionRuleData.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * AvalancheSuppressionRuleData is an implementation of
  * IAvalancheSuppressionRuleData.
  */

#ifndef AVALANCHE_SUPPRESSION_RULE_DATA_H
#define AVALANCHE_SUPPRESSION_RULE_DATA_H

#include "core/data_access_interface/alarm_rule/src/AlarmRuleData.h"

#include <vector>
#include <utility>

namespace TA_Base_Core
{
    class AvalancheSuppressionRuleData : public AlarmRuleData
    {

    public:

        // The string representing this rule type
        static const std::string RULE_TYPE;


        // An entity key/alarm type pair
        typedef std::pair<unsigned long, unsigned long> EntityKeyAlarmTypePair;




        /**
         * Destructor
         */
        virtual ~AvalancheSuppressionRuleData();


        /** 
          * AvalancheSuppressionRuleData
          *
          * Constructs a new avalanche suppression rule with blank fields.
          *
          */
        AvalancheSuppressionRuleData();


        /**
         * Constructor
         *
         * Construct an Alarm Rule class based around a key, this will read
		 * the data from the database and throw a DatabaseException
		 * if not successful
         *
         * @param key The key of this Alarm Rule in the database
         */
        AvalancheSuppressionRuleData( const unsigned long key );

        
        /**
         * Constructor
         *
         * Construct a new alarm rule.
         *
         * @param entityTypeKey            The key of the entity's type.
         * @param entityKey                The entity's key.
         * @param alarmType                The key of the alarm type.
         * @param ruleTrigger              The trigger for this alarm rule.
         * @param timeout                  The timeout in milli-seconds.
         * @param entityKeyAlarmTypePairs  The entity key/alarm type pairs
         *                                 that define the suppression rules.
         */
        AvalancheSuppressionRuleData(
            unsigned long entityTypeKey,
            unsigned long entityKey,
            unsigned long alarmTypeKey,
            const std::string& ruleTrigger,
            unsigned long timeoutMillis,
            unsigned long timeinMillis,
            unsigned long minAlarms,
            const std::vector<EntityKeyAlarmTypePair>& entityKeyAlarmTypePairs);


        /** 
          * AvalancheSuppressionRuleData
          *
          * Creates a copy of the given Alarm Rule
          *
          * @param theRule  the rule to copy
          */
        AvalancheSuppressionRuleData(
            const AvalancheSuppressionRuleData& theRule);


        /**
         *  Prepends the RULE_TYPE to the getName method of the parent class.
         */
        virtual std::string getName();


        /** 
          * getTimeout
          *
          * Gets the timeout in milli-seconds.
          *
          * @return the timeout in milli-seconds.
          *
          * @exception DataException 
          * thrown if there is a problem with the format of the data.
          * @exception DatabaseException
          * thrown if there is a problem communicating with the database.
          *
          */
        virtual unsigned long getTimeout();


        /** 
          * setTimeout
          *
          * Sets the timeout.
          *
          * @param timeoutMillis  sets the timeout in milli-seconds.
          *
          * @exception DataException 
          * thrown if there is a problem with the format of the data.
          * @exception DatabaseException
          * thrown if there is a problem communicating with the database.
          */
        virtual void setTimeout(unsigned long timeoutMillis);



        /** 
          * getTimein
          *
          * Gets the timein in milli-seconds.
          *
          * @return the timein in milli-seconds.
          *
          * @exception DataException 
          * thrown if there is a problem with the format of the data.
          * @exception DatabaseException
          * thrown if there is a problem communicating with the database.
          *
          */
        virtual unsigned long getTimein();


        /** 
          * setTimein
          *
          * Sets the timein.
          *
          * @param timeinMillis  sets the timein in milli-seconds.
          *
          * @exception DataException 
          * thrown if there is a problem with the format of the data.
          * @exception DatabaseException
          * thrown if there is a problem communicating with the database.
          */
        virtual void setTimein(unsigned long timeinMillis);



        /** 
          * getMinAlarms
          *
          * Gets the minimun required alarms.
          *
          * @return the min alarms.
          *
          * @exception DataException 
          * thrown if there is a problem with the format of the data.
          * @exception DatabaseException
          * thrown if there is a problem communicating with the database.
          *
          */
        virtual unsigned long getMinAlarms();


        /** 
          * setMinAlarms
          *
          * Sets the minimum required alarms.
          *
          * @param minAlarms the minimum required alarms.
          *
          * @exception DataException 
          * thrown if there is a problem with the format of the data.
          * @exception DatabaseException
          * thrown if there is a problem communicating with the database.
          */
        virtual void setMinAlarms(unsigned long minAlarms);



        /** 
          * getChildren
          *
          * Gets the child parameters of this alarm rule that are
          * entity key/alarm type pairs.
          *
          * @return a vector of EntityKeyAlarmTypePairs
          *
          * @exception DataException 
          * thrown if there is a problem with the format of the data.
          * @exception DatabaseException
          * thrown if there is a problem communicating with the database.
          *
          */
        virtual std::vector<EntityKeyAlarmTypePair> getChildren();


        /** 
          * addChild
          *
          * Adds a new entity key/alarm type pair to the list maintained by
          * this object.
          *
          * @param EntityKeyAlarmTypePair& child
          *
          * @exception DataException 
          * thrown if there is a problem with the format of the data.
          * @exception DatabaseException
          * thrown if there is a problem communicating with the database.
          *
          */
        virtual void addChild(const EntityKeyAlarmTypePair& child);


        /** 
          * replaceChild
          *
          * Replaces the child matching the first parameter with a new one
          * matching the second parameter.
          *
          * @param existingChild  the child to be replaced
          * @param newChild       the replacement
          *
          * @exception DataException 
          * thrown if there is a problem with the format of the data,
          * or if the child does not exist.
          * @exception DatabaseException
          * thrown if there is a problem communicating with the database.
          *
          */
        void replaceChild(
            const AvalancheSuppressionRuleData::EntityKeyAlarmTypePair& existingChild,
            const AvalancheSuppressionRuleData::EntityKeyAlarmTypePair& newChild);


        /** 
          * deleteChild
          *
          * Removes the entity key/alarm type pair that matches the parameter
          * from the list maintained by this object.
          *
          * @param EntityKeyAlarmTypePair& child
          *
          * @exception DataException 
          * thrown if there is a problem with the format of the data,
          * or if the child does not exist.
          * @exception DatabaseException
          * thrown if there is a problem communicating with the database.
          *
          */
        virtual void deleteChild(const EntityKeyAlarmTypePair& child);


        /**
         *  Overrides the parent class method so that item changes kept
         *  by this class are cleared.
         */
        void applyChanges();

        /**
         *  Overrides the parent class method so that changes made in this
         *  object are accounted for.
         */
        bool hasChanged();

        /**
         *  Overrides the parent class method so that changes made in this
         *  class are included.
         */
        ItemChanges AvalancheSuppressionRuleData::getAllItemChanges();


    protected:
    

        /** 
          * getChildAsString
          *
          * Gets the given child parameter as a string
          * eg "Entity: System, Alarm Type: HelpMeAlarm"
          *
          * @param child  the child that will be converted to a string
          */
        std::string AvalancheSuppressionRuleData::getChildAsString(
            const AvalancheSuppressionRuleData::EntityKeyAlarmTypePair& child);




private:
        // Unused methods
		AvalancheSuppressionRuleData& operator=(const AvalancheSuppressionRuleData&);


        // The names of the parameters that may change.
        static const std::string TIMEOUT;
        static const std::string TIMEIN;
        static const std::string MINALARMS;
        static const std::string CHILD_PARAMETER;

        // Used when working out what changes have been made to this object
        // since the last time the data was saved to the database.
        std::vector<EntityKeyAlarmTypePair> m_oldChildParams;

    };
} // closes TA_Base_Core

#endif // AVALANCHE_SUPPRESSION_RULE_DATA_H
