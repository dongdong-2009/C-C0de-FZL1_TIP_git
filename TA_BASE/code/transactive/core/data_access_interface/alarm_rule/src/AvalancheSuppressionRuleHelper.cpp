/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/AvalancheSuppressionRuleHelper.cpp $
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

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/alarm_rule/src/AvalancheSuppressionRuleHelper.h"
#include "core/data_access_interface/alarm_rule/src/AvalancheSuppressionRuleData.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include <algorithm>

using TA_Base_Core::DebugUtil;

typedef TA_Base_Core::AvalancheSuppressionRuleData::EntityKeyAlarmTypePair EntityKeyAlarmTypePair;

namespace TA_Base_Core
{
    const std::string AvalancheSuppressionRuleHelper::TIMEOUT_PARAMETER    = "TIMEOUT";
    const std::string AvalancheSuppressionRuleHelper::TIMEIN_PARAMETER     = "TIMEIN";
    const std::string AvalancheSuppressionRuleHelper::MINALARMS_PARAMETER  = "MINALARMS";
    const std::string AvalancheSuppressionRuleHelper::ENTITY_KEY_PARAMETER = "ENTITY_KEY";
    const std::string AvalancheSuppressionRuleHelper::ALARM_TYPE_PARAMETER = "ALARM_TYPE";



    AvalancheSuppressionRuleHelper::AvalancheSuppressionRuleHelper()
        :
        AlarmRuleHelper(
            AvalancheSuppressionRuleData::RULE_TYPE,
            IAlarmRuleData::RULE_TRIGGER_SUBMIT)
    {
        // Create the timeout parameter
        m_timeoutParameter = addNewParameter(TIMEOUT_PARAMETER, "0");
        m_timeinParameter = addNewParameter(TIMEIN_PARAMETER, "0");
        m_minAlarmsParameter = addNewParameter(MINALARMS_PARAMETER, "0");

        // No children exist yet, so there's no need to add them
    }


    AvalancheSuppressionRuleHelper::AvalancheSuppressionRuleHelper(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleTrigger,
        unsigned long timeoutMillis,
        unsigned long timeinMillis,
        unsigned long minAlarms,
        const std::vector<EntityKeyAlarmTypePair>& entityKeyAlarmTypePairs)
        :
        AlarmRuleHelper(
            entityTypeKey,
            entityKey,
            alarmTypeKey,
            AvalancheSuppressionRuleData::RULE_TYPE,
            ruleTrigger),
        m_timeoutParameter(NULL)
    {
        // Create the timeout parameter
        std::string timeoutString = getUnsignedLongAsString(timeoutMillis);
        m_timeoutParameter = addNewParameter(TIMEOUT_PARAMETER, timeoutString);

        std::string timeinString = getUnsignedLongAsString(timeinMillis);
        m_timeinParameter = addNewParameter(TIMEIN_PARAMETER, timeinString);

        std::string minAlarmsString = getUnsignedLongAsString(minAlarms);
        m_minAlarmsParameter = addNewParameter(MINALARMS_PARAMETER, minAlarmsString);

        // Create parameters for all the child entity key/alarm type pairs
        // of this rule
        addEntityKeyAlarmTypePairParameters(entityKeyAlarmTypePairs);
    }


    AvalancheSuppressionRuleHelper::AvalancheSuppressionRuleHelper(
        unsigned long pkey)
        :
        AlarmRuleHelper(pkey),
        m_timeoutParameter(NULL)
    {
        retrieveParameters();
    }
    

    void AvalancheSuppressionRuleHelper::addEntityKeyAlarmTypePairParameters(
        const std::vector<EntityKeyAlarmTypePair>& entityKeyAlarmTypePairs)
    {
        for(std::vector<EntityKeyAlarmTypePair>::const_iterator it =
                entityKeyAlarmTypePairs.begin();
            it != entityKeyAlarmTypePairs.end();
            it ++)
        {
            addChild(*it);
        }
    }


    AvalancheSuppressionRuleHelper::AvalancheSuppressionRuleHelper(
        AvalancheSuppressionRuleHelper& original)
        :
        AlarmRuleHelper(
            original.getEntityTypeKey(),
            original.getEntityKey(),
            original.getAlarmType(),
            original.getRuleType(),
            original.getRuleTrigger())
    {
        std::string timeoutString = getUnsignedLongAsString(original.getTimeout());
        m_timeoutParameter = addNewParameter(TIMEOUT_PARAMETER, timeoutString);

        std::string timeinString = getUnsignedLongAsString(original.getTimein());
        m_timeinParameter = addNewParameter(TIMEIN_PARAMETER, timeinString);

        std::string minAlarmsString = getUnsignedLongAsString(original.getMinAlarms());
        m_minAlarmsParameter = addNewParameter(MINALARMS_PARAMETER, minAlarmsString);

        std::vector<EntityKeyAlarmTypePair> children = original.getChildren();
        for(std::vector<EntityKeyAlarmTypePair>::iterator it = children.begin();
            it != children.end();
            it ++)
        {
            addChild(*it);
        }
    }


    AvalancheSuppressionRuleHelper::~AvalancheSuppressionRuleHelper()
    {
        // Note:
        // This object is not responsible for deleting
        // m_timeoutParameter.
        // The AlarmRuleHelper class deletes all its pointers.
    }



    unsigned long AvalancheSuppressionRuleHelper::getTimeout()
    {
        TA_ASSERT(m_timeoutParameter != NULL,
                  "The timeout parameter must not be NULL");

        return getStringAsUnsignedLong(m_timeoutParameter->getValue());
    }


    void AvalancheSuppressionRuleHelper::setTimeout(unsigned long timeoutMillis)
    {
        TA_ASSERT(m_timeoutParameter != NULL,
                  "The timeout parameter must not be NULL");

        if(timeoutMillis == 0)
        {
            const char* msg = "The timeout must be greater than 0";
            TA_THROW(DataException(msg, DataException::WRONG_TYPE, "timeout"));
        }

        m_timeoutParameter->setValue(getUnsignedLongAsString(timeoutMillis));
    }


    unsigned long AvalancheSuppressionRuleHelper::getTimein()
    {
        TA_ASSERT(m_timeinParameter != NULL,
                  "The timein parameter must not be NULL");

        return getStringAsUnsignedLong(m_timeinParameter->getValue());
    }


    void AvalancheSuppressionRuleHelper::setTimein(unsigned long timeinMillis)
    {
        TA_ASSERT(m_timeinParameter != NULL,
                  "The timein parameter must not be NULL");

        m_timeinParameter->setValue(getUnsignedLongAsString(timeinMillis));
    }


    unsigned long AvalancheSuppressionRuleHelper::getMinAlarms()
    {
        TA_ASSERT(m_minAlarmsParameter != NULL,
                  "The minAlarms parameter must not be NULL");

        return getStringAsUnsignedLong(m_minAlarmsParameter->getValue());
    }


    void AvalancheSuppressionRuleHelper::setMinAlarms(unsigned long minAlarms)
    {
        TA_ASSERT(m_minAlarmsParameter != NULL,
                  "The minAlarms parameter must not be NULL");

        m_minAlarmsParameter->setValue(getUnsignedLongAsString( minAlarms ));
    }


    std::vector<AvalancheSuppressionRuleData::EntityKeyAlarmTypePair>
        AvalancheSuppressionRuleHelper::getChildren()
    {
        reload();
        
        // Note that this class originally used a vector internally,
        // however what seems to be a bug in the vector implementation
        // prevented it from being cleaned up properly.
        // Hence a list is now used.
        std::vector<EntityKeyAlarmTypePair> children;
        children.reserve(m_entityKeyAlarmTypePairs.size());

        for(std::list<EntityKeyAlarmTypePair>::iterator it =
                m_entityKeyAlarmTypePairs.begin();
            it != m_entityKeyAlarmTypePairs.end();
            it ++)
        {
            children.push_back(*it);
        }

        return children;
    }


    void AvalancheSuppressionRuleHelper::addChild(
        const EntityKeyAlarmTypePair& child)
    {
        std::string entityKeyStr = getUnsignedLongAsString(child.first);
		std::string alarmTypeKeyStr = getUnsignedLongAsString(child.second);

        // Throw an exception if the new entity key/alarm type pair is
        // already in the list
        checkChildIsNotInList(child);

        // Add the entity key as a child parameter of this alarm rule
        IAlarmRuleActionParameterData* entityKeyParameter =
            addNewParameter(ENTITY_KEY_PARAMETER, entityKeyStr);

        // Add the alarm type as the related parameter to the entity key
        entityKeyParameter->addRelatedParameter(
            ALARM_TYPE_PARAMETER, alarmTypeKeyStr);

        // Save the entity key/alarm type pair
        m_entityKeyAlarmTypePairs.push_back(child);
    }


    void AvalancheSuppressionRuleHelper::checkChildIsNotInList(
        const EntityKeyAlarmTypePair& child)
    {
        std::list<EntityKeyAlarmTypePair>::iterator it =
            std::find(
                m_entityKeyAlarmTypePairs.begin(),
                m_entityKeyAlarmTypePairs.end(),
                child);

        if(it != m_entityKeyAlarmTypePairs.end())
        {
            const char* message =
                "An avalanche suppression rule child with that entity "
                "and alarm type already exists.";

            TA_THROW(
                DataException(
                    message, DataException::NOT_UNIQUE, message));
        }
    }


    void AvalancheSuppressionRuleHelper::replaceChild(
        const EntityKeyAlarmTypePair& existingChild,
        const EntityKeyAlarmTypePair& newChild)
    {
        // Get the entity key and alarm type from the existingChild parameter
        unsigned long entityKey = existingChild.first;
        unsigned long alarmTypeKey  = existingChild.second;

        
        std::list<EntityKeyAlarmTypePair>::iterator it =
            std::find(
                m_entityKeyAlarmTypePairs.begin(),
                m_entityKeyAlarmTypePairs.end(),
                existingChild);

        // The parameter must exist.
        if(it == m_entityKeyAlarmTypePairs.end())
        {
            const char* message =
                "Could not replace parameter because it does not exist.";

            TA_THROW(DataException(message, DataException::MISSING_MANDATORY, ""));
        }


        // Throw an exception if the new entity key/alarm type pair is
        // already in the list
        checkChildIsNotInList(newChild);

        // Replace the child in the list maintained by this object
        *it = newChild;


        // Find the child in the list maintained by the parent class
        ActionParameterVector parameters = getParameters();

        std::string entityKeyString = getUnsignedLongAsString(entityKey);
		std::string alarmTypeKeyString	= getUnsignedLongAsString(alarmTypeKey);

        ActionParameterVector::iterator parentIt = parameters.begin();
        for(;
            parentIt != parameters.end();
            parentIt ++)
        {
            // The immediate child of the alarm rule object should be
            // the entity key
            IAlarmRuleActionParameterData* entityKeyParameter = *parentIt;
            TA_ASSERT(entityKeyParameter != NULL,
                      "The First parameter should never be NULL");

            // If the entity key matches, check the alarm type
            if( entityKeyParameter->getType() == ENTITY_KEY_PARAMETER &&
                entityKeyParameter->getValue() == entityKeyString )
            {
                // The related entity of the entity key should be the
                // alarm type
                IAlarmRuleActionParameterData* alarmTypeParameter =
                    entityKeyParameter->getRelatedParameter();
                
                if(alarmTypeParameter == NULL ||
                   alarmTypeParameter->getType() != ALARM_TYPE_PARAMETER)
                {
                    const char* message = "Alarm Type parameter is missing";
                    TA_THROW(
                        DataException(
                            message, DataException::MISSING_MANDATORY, ""));
                }
                
                // If the alarm type matches, we've found the parameter
                if(alarmTypeParameter->getValue() == alarmTypeKeyString)
                {
                    // Replace the parameters values with the new values
                    std::string newEntityKey =
                        getUnsignedLongAsString(newChild.first);

                    std::string newAlarmType = 
						getUnsignedLongAsString(newChild.second);

                    entityKeyParameter->setValue(newEntityKey);
                    alarmTypeParameter->setValue(newAlarmType);
                    return;
                }
            }
        }
        
        // The parameter must exist.
        const char* message =
            "Could not replace parameter because it does not exist.";

        TA_THROW(DataException(message, DataException::MISSING_MANDATORY, ""));
    }


    void AvalancheSuppressionRuleHelper::deleteChild(
        const EntityKeyAlarmTypePair& child)
    {
        // Get the entity key and alarm type from the function parameter
        unsigned long entityKey = child.first;
        unsigned long alarmTypeKey  = child.second;


        std::list<EntityKeyAlarmTypePair>::iterator it =
            std::find(
                m_entityKeyAlarmTypePairs.begin(),
                m_entityKeyAlarmTypePairs.end(),
                child);

        // The parameter must exist.
        if(it == m_entityKeyAlarmTypePairs.end())
        {
            const char* message =
                "Could not delete parameter because it does not exist.";

            TA_THROW(DataException(message, DataException::MISSING_MANDATORY, ""));
        }

        // Delete the child from the list maintained by this object
        m_entityKeyAlarmTypePairs.remove(*it);



        // Find the child in the list maintained by the parent class
        ActionParameterVector parameters = getParameters();

        std::string entityKeyString = getUnsignedLongAsString(entityKey);
		std::string alarmTypeKeyString = getUnsignedLongAsString(alarmTypeKey);

        ActionParameterVector::iterator parentIt = parameters.begin();
        for(;
            parentIt != parameters.end();
            parentIt ++)
        {
            // The immediate child of the alarm rule object should be
            // the entity key
            IAlarmRuleActionParameterData* entityKeyParameter = *parentIt;
            TA_ASSERT(entityKeyParameter != NULL,
                      "The First parameter should never be NULL");

            // If the entity key matches, check the alarm type
            if( entityKeyParameter->getType() == ENTITY_KEY_PARAMETER &&
                entityKeyParameter->getValue() == entityKeyString )
            {
                // The related entity of the entity key should be the
                // alarm type
                IAlarmRuleActionParameterData* alarmTypeParameter =
                    entityKeyParameter->getRelatedParameter();
                
                if(alarmTypeParameter == NULL ||
                   alarmTypeParameter->getType() != ALARM_TYPE_PARAMETER)
                {
                    const char* message = "Alarm Type parameter is missing";
                    TA_THROW(
                        DataException(
                            message, DataException::MISSING_MANDATORY, ""));
                }
                
                // If the alarm type matches, we've found the parameter
                if(alarmTypeParameter->getValue() == alarmTypeKeyString)
                {
                    // Delete the child from the list maintained by the
                    // parent class
                    deleteParameter(*parentIt);
                    return;
                }
            }
        }
        
        // The parameter must exist.
        const char* message =
            "Could not delete parameter because it does not exist.";

        TA_THROW(DataException(message, DataException::MISSING_MANDATORY, ""));

    }




    void AvalancheSuppressionRuleHelper::forceReload()
    {
        AlarmRuleHelper::forceReload();

        retrieveParameters();
    }


    void AvalancheSuppressionRuleHelper::retrieveParameters()
    {
        ActionParameterVector parameters = getParameters();

        // Clear the pairs from this object's list
        m_entityKeyAlarmTypePairs.clear();
        

        for(ActionParameterVector::iterator parentIt = parameters.begin();
            parentIt != parameters.end();
            parentIt ++)
        {
            // The immediate child of the alarm rule object may be
            // the entity key or the timeout parameter.
            IAlarmRuleActionParameterData* immediateChild = *parentIt;
            TA_ASSERT(immediateChild != NULL,
                      "The First parameter should never be NULL");

            // If it's the timeout parameter, set the m_timeoutParameter
            if(immediateChild->getType() == TIMEOUT_PARAMETER)
            {
                m_timeoutParameter = immediateChild;
            }

            // If it's the timein parameter, set the m_timeinParameter
            if(immediateChild->getType() == TIMEIN_PARAMETER)
            {
                m_timeinParameter = immediateChild;
            }

            // If it's the minAlarms parameter, set the m_minAlarmsParameter
            if(immediateChild->getType() == MINALARMS_PARAMETER)
            {
                m_minAlarmsParameter = immediateChild;
            }

            // If it's the entity key parameter, get the alarm type
            else if( immediateChild->getType() == ENTITY_KEY_PARAMETER )
            {
                // The related entity of the entity key should be the
                // alarm type
                IAlarmRuleActionParameterData* alarmTypeParameter =
                    immediateChild->getRelatedParameter();
                
                if(alarmTypeParameter == NULL ||
                   alarmTypeParameter->getType() != ALARM_TYPE_PARAMETER)
                {
                    const char* message = "Alarm Type parameter is missing";
                    TA_THROW(
                        DataException(
                            message, DataException::MISSING_MANDATORY, ""));
                }
                
                
                // Add the pair to the list maintained by this object
                std::string entityKeyString = immediateChild->getValue();
                unsigned long entityKey =
                    getStringAsUnsignedLong(entityKeyString);

                std::string alarmTypeKeyString = alarmTypeParameter->getValue();
				 unsigned long alarmTypeKey =
                    getStringAsUnsignedLong(alarmTypeKeyString);

                EntityKeyAlarmTypePair pair(entityKey, alarmTypeKey);

                m_entityKeyAlarmTypePairs.push_back(pair);
            }
        }

    }


    void AvalancheSuppressionRuleHelper::applyChanges()
    {
        // If the timeout value has not been set
        if(m_timeoutParameter->getValue() == "")
        {
            // Throw an exception
            const char* message = "The timeout parameter has not been set.";
            TA_THROW(
                DataException(
                    message, DataException::MISSING_MANDATORY, "timeout"));
        }

        // If the timein value has not been set
        if(m_timeinParameter->getValue() == "")
        {
            // Throw an exception
            const char* message = "The timein parameter has not been set.";
            TA_THROW(
                DataException(
                    message, DataException::MISSING_MANDATORY, "timein"));
        }

        // If the minAlarms value has not been set
        if(m_minAlarmsParameter->getValue() == "")
        {
            // Throw an exception
            const char* message = "The minimum alarms parameter has not been set.";
            TA_THROW(
                DataException(
                    message, DataException::MISSING_MANDATORY, "minAlarms"));
        }

        // If any of the parameters have not been fully specified,
        // throw an exception
        for(std::list<EntityKeyAlarmTypePair>::iterator it =
                m_entityKeyAlarmTypePairs.begin();
            it != m_entityKeyAlarmTypePairs.end();
            it ++)
        {
            if((*it).first == 0 || (*it).second == 0)
            {
                // Throw an exception
                const char* message =
                    "Not all avalanche suppression child parameters have "
                    "been fully specified.";
                TA_THROW(
                    DataException(
                        message,
                        DataException::MISSING_MANDATORY,
                        message));
            }
        }


        AlarmRuleHelper::applyChanges();
    }


    void AvalancheSuppressionRuleHelper::setRuleTrigger(
        const std::string& trigger)
    {
        TA_ASSERT(trigger == IAlarmRuleData::RULE_TRIGGER_SUBMIT,
                  "Avalanche Suppression can only occur when an "
                  "alarm is submitted");
    }


}// TA_Base_Core
