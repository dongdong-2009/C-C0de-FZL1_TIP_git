/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/AvalancheSuppressionRuleHelper.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Provides an interface to Alarm Rules of type avalanche suppression
  * in the database.
  *
  */

#ifndef AVALANCHE_SUPPRESSION_RULE_HELPER_H
#define AVALANCHE_SUPPRESSION_RULE_HELPER_H


#include <utility>
#include <string>
#include <vector>
#include <list>

#include "core/data_access_interface/alarm_rule/src/AlarmRuleHelper.h"
#include "core/data_access_interface/alarm_rule/src/AvalancheSuppressionRuleData.h"
#include "core/data_access_interface/alarm_rule/src/IAlarmRuleActionParameterData.h"

namespace TA_Base_Core
{

    class AvalancheSuppressionRuleHelper : public AlarmRuleHelper
    {

    public:

        // The strings representing the types of the timeout, entity type
        // and alarm type parameters.
        static const std::string TIMEOUT_PARAMETER;
        static const std::string TIMEIN_PARAMETER;
        static const std::string MINALARMS_PARAMETER;
        static const std::string ENTITY_KEY_PARAMETER;
        static const std::string ALARM_TYPE_PARAMETER;



        /** 
          * AvalancheSuppressionRuleHelper
          *
          * @param entityTypeKey            The key of the entity's type.
          * @param entityKey                The entity's key.
          * @param ruleType                 The rule type associated with the alarm rule.
          * @param ruleTrigger              The trigger for this alarm rule.
          * @param timeout                  The timeout in milli-seconds.
          * @param timein                   The timein in milli-seconds.
          * @param minAlarms                The minimun required alarms.
          * @param entityKeyAlarmTypePairs  The entity key/alarm type pairs
          *                                 that define the suppression rules.
          */
        AvalancheSuppressionRuleHelper(
            unsigned long entityTypeKey,
            unsigned long entityKey,
            unsigned long alarmTypeKey,
            const std::string& ruleTrigger,
            unsigned long timeoutMillis,
            unsigned long timeinMillis,
            unsigned long minAlarms,
            const std::vector<AvalancheSuppressionRuleData::EntityKeyAlarmTypePair>&
                entityKeyAlarmTypePairs);



        /** 
          * AvalancheSuppressionRuleHelper
          *
          * Constructs a new avalanche suppression rule helper with blank
          * fields.
          *
          */
        AvalancheSuppressionRuleHelper();


        /** 
          * AvalancheSuppressionRuleHelper
          *
          * Creates an Avalanche Suppression Alarm Rule from the database.
          *
          * @param pkey  the primary key of the avalanche suppression
          *              alarm rule.
          */
        AvalancheSuppressionRuleHelper(unsigned long pkey);

        
        /** 
          * AvalancheSuppressionRuleHelper
          *
          * Creates a copy of the given object.
          *
          * @param original  the object that will be copied.
          */
        AvalancheSuppressionRuleHelper(
            AvalancheSuppressionRuleHelper& original);

            
        /** 
          * ~AvalancheSuppressionRuleHelper
          *
          * Destructor
          *
          */
        virtual ~AvalancheSuppressionRuleHelper();





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
          * Gets the minimum number of alarms required during timein.
          *
          * @return the minimun number of alarms.
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
          * Sets the minimun required alarms.
          *
          * @param minAlarms  the minimum number.
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
        virtual std::vector<AvalancheSuppressionRuleData::EntityKeyAlarmTypePair>
            getChildren();


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
        virtual void addChild(
            const AvalancheSuppressionRuleData::EntityKeyAlarmTypePair& child);


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
        virtual void deleteChild(
            const AvalancheSuppressionRuleData::EntityKeyAlarmTypePair& child);



        /** 
          * setRuleTrigger
          *
          * This is overridden to assert if anything other than
          * IAlarmRuleData::RULE_TRIGGER_SUBMIT is set.
          *
          * @param trigger  the trigger
          */
        virtual void setRuleTrigger(const std::string& trigger);


        /** 
          * applyChanges
          *
          * Overrides the parent class method so that a check can be made
          * to ensure all mandatory parameters are present.
          *
          */
        void applyChanges();

    
    protected:


        /** 
          * forceReload
          *
          * Overrides the parent class method so that caching works.
          *
          * @exception DataException 
          * thrown if there is a problem with the format of the data.
          * @exception DatabaseException
          * thrown if there is a problem communicating with the database.
          *
          */
        virtual void forceReload();



        /** 
          * retrieveParameters
          *
          * Gets all the parameters from the parent class again.
          *
          * @exception DataException 
          * thrown if there is a problem with the format of the data.
          * @exception DatabaseException
          * thrown if there is a problem communicating with the database.
          *
          */
        virtual void retrieveParameters();


        /** 
          * addEntityKeyAlarmTypePairParameters
          *
          * Adds the entity keys passed as a parameter to the database and
          * the list maintained by this class.
          *
          * @param std::vector<EntityKeyAlarmTypePair>& entityKeyAlarmTypePairs
          *
          */
        virtual void addEntityKeyAlarmTypePairParameters(
            const std::vector<AvalancheSuppressionRuleData::EntityKeyAlarmTypePair>&
                entityKeyAlarmTypePairs);


        /** 
          * checkChildIsNotInList
          *
          * Throws an exception if the given EntityKeyAlarmTypePair is already
          * in the list of children maintained by this class.
          */
        void checkChildIsNotInList(
            const AvalancheSuppressionRuleData::EntityKeyAlarmTypePair& child);



    private:

        const AvalancheSuppressionRuleHelper& operator=(const AvalancheSuppressionRuleHelper&);

        
        // Note that this class originally used a vector internally,
        // however what seems to be a bug in the MS vector implementation
        // prevented it from being cleaned up properly.
        // Hence a list is now used.
        std::list<AvalancheSuppressionRuleData::EntityKeyAlarmTypePair>
            m_entityKeyAlarmTypePairs;
        IAlarmRuleActionParameterData* m_timeoutParameter;
        IAlarmRuleActionParameterData* m_timeinParameter;
        IAlarmRuleActionParameterData* m_minAlarmsParameter;


    };


}// TA_Base_Core

#endif // AVALANCHE_SUPPRESSION_RULE_HELPER_H
