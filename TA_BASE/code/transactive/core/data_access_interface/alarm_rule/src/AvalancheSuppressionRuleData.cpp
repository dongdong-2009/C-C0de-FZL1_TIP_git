/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/alarm_rule/src/AvalancheSuppressionRuleData.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * AvalancheSuppressionRuleData is an implementation of
  * IAvalancheSuppressionRuleData.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif


#include "core/data_access_interface/alarm_rule/src/AvalancheSuppressionRuleHelper.h"
#include "core/data_access_interface/alarm_rule/src/AvalancheSuppressionRuleData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"
#include "core/data_access_interface/src/IAlarmTypeData.h"
#include "core/utilities/src/TAAssert.h"

#include <sstream>
#include <algorithm>

namespace TA_Base_Core
{
    // The string representing this rule type
    const std::string AvalancheSuppressionRuleData::RULE_TYPE = "AvalancheSuppression";

    const std::string AvalancheSuppressionRuleData::TIMEOUT         = "Timeout";
    const std::string AvalancheSuppressionRuleData::TIMEIN          = "Timein";
    const std::string AvalancheSuppressionRuleData::MINALARMS       = "MinAlarms";
    const std::string AvalancheSuppressionRuleData::CHILD_PARAMETER = "Child Parameter";
    
    

    AvalancheSuppressionRuleData::~AvalancheSuppressionRuleData()
    {
    }


    AvalancheSuppressionRuleData::AvalancheSuppressionRuleData(
        const unsigned long key)
        :
        AlarmRuleData( new AvalancheSuppressionRuleHelper(key) ),
        m_oldChildParams(getChildren())
    {
    }


    AvalancheSuppressionRuleData::AvalancheSuppressionRuleData()
        :
        AlarmRuleData(new AvalancheSuppressionRuleHelper())
    {
    }


    AvalancheSuppressionRuleData::AvalancheSuppressionRuleData(
        unsigned long entityTypeKey,
        unsigned long entityKey,
        unsigned long alarmTypeKey,
        const std::string& ruleTrigger,
        unsigned long timeoutMillis,
        unsigned long timeinMillis,
        unsigned long minAlarms,
        const std::vector<AvalancheSuppressionRuleData::EntityKeyAlarmTypePair>&
            entityKeyAlarmTypePairs)
        :
        AlarmRuleData(
            new AvalancheSuppressionRuleHelper(
                entityTypeKey,
                entityKey,
                alarmTypeKey,
                ruleTrigger,
                timeoutMillis,
                timeinMillis,
                minAlarms,
                entityKeyAlarmTypePairs) )
    {
    }


    AvalancheSuppressionRuleData::AvalancheSuppressionRuleData(
        const AvalancheSuppressionRuleData& theRule)
        :
        AlarmRuleData(
            new AvalancheSuppressionRuleHelper(
                dynamic_cast<AvalancheSuppressionRuleHelper&>( theRule.getHelper() )))
    {
    }


    std::string AvalancheSuppressionRuleData::getName()
    {
        return RULE_TYPE + " " + AlarmRuleData::getName();
    }


    unsigned long AvalancheSuppressionRuleData::getTimeout()
    {
        AvalancheSuppressionRuleHelper& helper =
            dynamic_cast<AvalancheSuppressionRuleHelper&>( getHelper() );

        return helper.getTimeout();
    }


    void AvalancheSuppressionRuleData::setTimeout(unsigned long timeoutMillis)
    {
        AvalancheSuppressionRuleHelper& helper =
            dynamic_cast<AvalancheSuppressionRuleHelper&>( getHelper() );

        unsigned long oldTimeout = helper.getTimeout();
        helper.setTimeout(timeoutMillis);

        updateAlarmRuleChanges(TIMEOUT, oldTimeout, timeoutMillis);
    }



    unsigned long AvalancheSuppressionRuleData::getTimein()
    {
        AvalancheSuppressionRuleHelper& helper =
            dynamic_cast<AvalancheSuppressionRuleHelper&>( getHelper() );

        return helper.getTimein();
    }


    void AvalancheSuppressionRuleData::setTimein(unsigned long timeinMillis)
    {
        AvalancheSuppressionRuleHelper& helper =
            dynamic_cast<AvalancheSuppressionRuleHelper&>( getHelper() );

        unsigned long oldTimein = helper.getTimein();
        helper.setTimein(timeinMillis);

        updateAlarmRuleChanges(TIMEIN, oldTimein, timeinMillis);
    }



    unsigned long AvalancheSuppressionRuleData::getMinAlarms()
    {
        AvalancheSuppressionRuleHelper& helper =
            dynamic_cast<AvalancheSuppressionRuleHelper&>( getHelper() );

        return helper.getMinAlarms();
    }


    void AvalancheSuppressionRuleData::setMinAlarms(unsigned long minAlarms)
    {
        AvalancheSuppressionRuleHelper& helper =
            dynamic_cast<AvalancheSuppressionRuleHelper&>( getHelper() );

        unsigned long oldMinAlarms = helper.getMinAlarms();
        helper.setMinAlarms(minAlarms);

        updateAlarmRuleChanges(MINALARMS, oldMinAlarms, minAlarms);
    }



    std::vector<AvalancheSuppressionRuleData::EntityKeyAlarmTypePair>
        AvalancheSuppressionRuleData::getChildren()
    {
        AvalancheSuppressionRuleHelper& helper =
            dynamic_cast<AvalancheSuppressionRuleHelper&>( getHelper() );

        return helper.getChildren();
    }


    void AvalancheSuppressionRuleData::addChild(
        const AvalancheSuppressionRuleData::EntityKeyAlarmTypePair& child)
    {
        AvalancheSuppressionRuleHelper& helper =
            dynamic_cast<AvalancheSuppressionRuleHelper&>( getHelper() );

        helper.addChild(child);
    }


    void AvalancheSuppressionRuleData::replaceChild(
        const AvalancheSuppressionRuleData::EntityKeyAlarmTypePair& existingChild,
        const AvalancheSuppressionRuleData::EntityKeyAlarmTypePair& newChild)
    {
        AvalancheSuppressionRuleHelper& helper =
            dynamic_cast<AvalancheSuppressionRuleHelper&>( getHelper() );

        helper.replaceChild(existingChild, newChild);
    }


    void AvalancheSuppressionRuleData::deleteChild(
        const AvalancheSuppressionRuleData::EntityKeyAlarmTypePair& child)
    {
        AvalancheSuppressionRuleHelper& helper =
            dynamic_cast<AvalancheSuppressionRuleHelper&>( getHelper() );

        helper.deleteChild(child);
    }



    std::string AvalancheSuppressionRuleData::getChildAsString(
        const AvalancheSuppressionRuleData::EntityKeyAlarmTypePair& child)
    {
        std::string entityName    = getEntityName(child.first);
        std::string alarmTypeName = getAlarmTypeName(child.second);

        std::ostringstream stream;
        stream <<
            "Entity: " << entityName << ", Alarm Type: " << alarmTypeName;

        return stream.str();
    }


    ItemChanges AvalancheSuppressionRuleData::getAllItemChanges()
    {
        typedef AvalancheSuppressionRuleData::EntityKeyAlarmTypePair EntityKeyAlarmTypePair;

        ItemChanges changes;

        std::vector<EntityKeyAlarmTypePair> oldPairs(m_oldChildParams);
        std::vector<EntityKeyAlarmTypePair> newPairs(getChildren());

        //
        // Join the old pairs and new pairs into a big vector
        //
        std::vector<EntityKeyAlarmTypePair> allPairs;
        allPairs.reserve( oldPairs.size() + newPairs.size() );
        unsigned int i = 0;
        for(i = 0; i < oldPairs.size(); i ++)
        {
            EntityKeyAlarmTypePair pair = oldPairs[i];
            allPairs.push_back(pair);
        }
        for(i = 0; i < newPairs.size(); i ++)
        {
            EntityKeyAlarmTypePair pair = newPairs[i];
            allPairs.push_back(pair);
        }

        //
        // Sort the vector
        //
        std::sort(allPairs.begin(), allPairs.end());


        //
        // Go through the vector, and where there are two
        // entity key/alarm type pairs in a row, ignore them.
        // For the others, check if they are in the old list or
        // the new list.
        //
        unsigned int childCount = 1;
        for(i = 0; i < allPairs.size();)
        {
            //
            // If this is not the last entry and if it's the same as
            // the next entry, skip them both.
            //
            if( (i != (allPairs.size() - 1)) && allPairs[i] == allPairs[i + 1] )
            {
                i += 2;
            }
            else
            {
                std::vector<EntityKeyAlarmTypePair>::iterator it =
                    std::find(oldPairs.begin(), oldPairs.end(), allPairs[i]);

                Values values;

                //
                // It's an old child parameter that's been deleted
                //
                if(it != oldPairs.end())
                {
                    values.oldValue = getChildAsString(*it);
                    values.newValue = "";
                }
                //
                // It's a new child parameter that's been created
                //
                else
                {
                    it = std::find(newPairs.begin(), newPairs.end(), allPairs[i]);

                    TA_ASSERT(it != newPairs.end(),
                              "Child must be either an old or new child");

                    values.oldValue = "";
                    values.newValue = getChildAsString(*it);
                }

                std::ostringstream parameterName;
                parameterName << CHILD_PARAMETER << " (" << childCount << ")";

                changes.insert(
                    ItemChanges::value_type(parameterName.str(), values));

                childCount ++;
                i ++;
            }

        }// for(unsigned int i = 0; i < allPairs.size();)



        //
        // Copy any other changes that have been made into the ItemChanges
        // map, and return it.
        //
        ItemChanges otherChanges = AlarmRuleData::getAllItemChanges();
        for(ItemChanges::iterator it = otherChanges.begin();
            it != otherChanges.end();
            it ++)
        {
            changes.insert(*it);
        }

        return changes;
    }


    bool AvalancheSuppressionRuleData::hasChanged()
    {
        //
        // There have been changes if this class has changed or the parent
        // class has registered changes.
        //
        bool haveChildrenChanged = (getAllItemChanges().size() > 0);
        bool hasTimeoutChanged   = AlarmRuleData::hasChanged();
        
        return (haveChildrenChanged || hasTimeoutChanged);
    }


    void AvalancheSuppressionRuleData::applyChanges()
    {
        AlarmRuleData::applyChanges();

        m_oldChildParams = getChildren();
    }



} // closes TA_Base_Core
