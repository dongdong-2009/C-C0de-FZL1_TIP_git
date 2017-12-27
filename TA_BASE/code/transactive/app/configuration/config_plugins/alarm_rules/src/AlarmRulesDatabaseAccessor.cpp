/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_rules/src/AlarmRulesDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve alarm plan association information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/alarm_rules/src/StdAfx.h"
#include "app/configuration/config_plugins/alarm_rules/src/AlarmRulesDatabaseAccessor.h"


#include "core/data_access_interface/alarm_rule/src/IAlarmRuleData.h"
#include "core/data_access_interface/alarm_rule/src/AlarmRuleAccessFactory.h"
#include "core/data_access_interface/alarm_rule/src/SuppressionRuleData.h"
#include "core/data_access_interface/alarm_rule/src/SuppressionByDescriptionRuleData.h"
#include "core/data_access_interface/alarm_rule/src/PlanExecutionRuleData.h"
#include "core/data_access_interface/alarm_rule/src/AutomaticAcknowledgementRuleData.h"
#include "core/data_access_interface/alarm_rule/src/AvalancheSuppressionRuleData.h"
#include "core/data_access_interface/alarm_rule/src/MMSRuleData.h"
#include "core/data_access_interface/alarm_rule/src/CCTVRuleData.h"

#include "core/data_access_interface/src/IAlarmTypeData.h"
#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::AlarmRuleAccessFactory;
using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::IEntityData;
using TA_Base_Core::IAlarmRuleData;
using TA_Base_Core::SuppressionRuleData;
using TA_Base_Core::SuppressionByDescriptionRuleData;
using TA_Base_Core::PlanExecutionRuleData;
using TA_Base_Core::AutomaticAcknowledgementRuleData;
using TA_Base_Core::AvalancheSuppressionRuleData;
using TA_Base_Core::MMSRuleData;
using TA_Base_Core::CCTVRuleData;

namespace TA_Base_App
{
    AlarmRulesDatabaseAccessor::AlarmRulesDatabaseAccessor()
        : m_isLoaded(false)
    {
    }


    AlarmRulesDatabaseAccessor::~AlarmRulesDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedAlarmRules::iterator iter = m_alarmRules.begin(); iter != m_alarmRules.end(); ++iter)
            {
                if( (iter->second) != NULL )
                {
                    delete iter->second;
                    iter->second = NULL;
                }
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }


    void AlarmRulesDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the alarm rules and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (LoadedAlarmRules::iterator assocToDelete = m_alarmRules.begin(); assocToDelete != m_alarmRules.end(); ++assocToDelete)
        {
            if( (assocToDelete->second) != NULL )
            {
                delete assocToDelete->second;
                assocToDelete->second = NULL;
            }
        }
        m_alarmRules.clear();


        // Now reload all the alarm rules
        std::vector<TA_Base_Core::IAlarmRuleData*> rules
            = TA_Base_Core::AlarmRuleAccessFactory::getInstance().getAllAlarmRules();

        // And insert the alarm rules into our map.
        for (std::vector<TA_Base_Core::IAlarmRuleData*>::iterator iter = rules.begin();
             iter != rules.end();
             ++iter)
        {
             m_alarmRules.insert(
                 LoadedAlarmRules::value_type(
                    (*iter)->getUniqueIdentifier(), *iter ) );
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> AlarmRulesDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        TA_ASSERT(false, "This method cannot be called for alarm rules");

        std::multimap<CString, unsigned long> temp;
        return temp;
    }


    std::map<unsigned long, AlarmRulesDatabaseAccessor::AlarmRule> AlarmRulesDatabaseAccessor::getAllItemDetails(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getAllItemDetails");

        std::map<unsigned long, AlarmRule> details; 

		bool isEntityFoundExc = false;

        // The maximum range is the number of alarm rules times by 2.
        // This is because we want to step for each Alarm Rule while it is
        // loaded and then again while each Alarm Rule is added to the list.
        progress.SetRange( 0, m_alarmRules.size() * 2 );

        // Step through all the objects and retrieve the details for each.
        // The get methods can throw exceptions and they will be thrown straight
        // out of this method.
        for (LoadedAlarmRules::iterator iter = m_alarmRules.begin();
             iter != m_alarmRules.end();
             ++iter)
        {
            if (iter->second != NULL)
            {
                AlarmRule item;
                item.ruleType    = iter->second->getRuleType().c_str();
                //item.alarmType   = iter->second->getAlarmTypeName().c_str();
				item.alarmTypeDisplay   = iter->second->getAlarmTypeDisplayName().c_str();

				//catch the exception if there is, set the value to empty and continue
				try
				{
					item.entity      = iter->second->getEntityName().c_str();

				}
				catch ( const TA_Base_Core::DataException& )
				{

					LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                         "Entity with key %d does not exist. Setting value to empty.", 
						 iter->second->getEntityKey());

					item.entity	= "";
					isEntityFoundExc = true;
					
				}
			
				//catch the exception if there is, set the value to empty and continue
				try
				{
					item.entityType  = iter->second->getEntityTypeName().c_str();
				}
				catch ( const TA_Base_Core::DataException& )
				{

					LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                         "EntityType with key %d does not exist. Setting value to empty.", 
						 iter->second->getEntityTypeKey());

					item.entityType = "";
					isEntityFoundExc = true;
				}
                item.ruleTrigger = iter->second->getRuleTrigger().c_str();
				item.alarmDescription = iter->second->getAlarmDescription().c_str();

                details.insert(
                    std::map<unsigned long, AlarmRule>::value_type(
                        iter->second->getUniqueIdentifier(), item ) );

                progress.StepIt();
            }
        }

			 
		//pop-up once for Entity not found issue
		if(isEntityFoundExc)
		{
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_030001);
		}


        FUNCTION_EXIT;
        return details;
    }


    AlarmRulesDatabaseAccessor::AlarmRule AlarmRulesDatabaseAccessor::getItemDetails(unsigned long identifier)
    {
        FUNCTION_ENTRY("getItemDetails");

        // Attempt to find the alarm plan association matching the key passed in.
        LoadedAlarmRules::iterator matching = m_alarmRules.find(identifier);
        if (matching == m_alarmRules.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the alarm plan association with identifier %lu",identifier);
            FUNCTION_EXIT;
            AlarmRule item;
            return item;
        }

        AlarmRule item;
        item.ruleType    = matching->second->getRuleType().c_str();
        //item.alarmType   = matching->second->getAlarmTypeName().c_str();
		item.alarmTypeDisplay   = matching->second->getAlarmTypeDisplayName().c_str();
		try
		{
			item.entity      = matching->second->getEntityName().c_str();
		}
		catch ( const TA_Base_Core::DataException& )
		{
			
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
				"Entity with key %d does not exist. Setting value to empty.", 
				matching->second->getEntityKey());
			
			item.entity	= "";		
		}
		
		//catch the exception if there is, set the value to empty and continue
		try
		{
			item.entityType  = matching->second->getEntityTypeName().c_str();
		}
		catch ( const TA_Base_Core::DataException& )
		{
			
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
				"EntityType with key %d does not exist. Setting value to empty.", 
				matching->second->getEntityTypeKey());
			
			item.entityType = "";
						
		}

        item.ruleTrigger = matching->second->getRuleTrigger().c_str();
		item.alarmDescription = matching->second->getAlarmDescription().c_str();

        FUNCTION_EXIT;
        return item;
    }


    TA_Base_Core::IConfigItem* AlarmRulesDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the alarm plan association matching the key passed in.
        LoadedAlarmRules::iterator matching = m_alarmRules.find(key);
        if (matching == m_alarmRules.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the alarm plan association with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void AlarmRulesDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the alarm plan association matching the key passed in.
        LoadedAlarmRules::iterator matching = m_alarmRules.find(key);
        if (matching == m_alarmRules.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the alarm plan association with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored alarm plan association was equal to NULL");

        if (!matching->second->isNew())
        {
            matching->second->deleteThisObject();
        }
        delete matching->second;
        matching->second = NULL;
        m_alarmRules.erase(matching);

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* AlarmRulesDatabaseAccessor::newItem(
        const std::string& alarmRuleType)
    {
        FUNCTION_ENTRY("newItem");


        // The type of alarm rule determines what subclass of IAlarmRuleData
        // is returned.

        TA_Base_Core::IAlarmRuleData* newAlarmRule = NULL;

        // Suppression by type
        if(alarmRuleType == SuppressionRuleData::RULE_TYPE)
        {
            newAlarmRule =
                AlarmRuleAccessFactory::getInstance().createSuppressionRule();
        }

		// Suppression by description
        else if(alarmRuleType == SuppressionByDescriptionRuleData::RULE_TYPE)
        {
            newAlarmRule =
                AlarmRuleAccessFactory::getInstance().createSuppressionByDescriptionRule();
        }

        // Plan Execution
        else if(alarmRuleType == PlanExecutionRuleData::RULE_TYPE)
        {
            newAlarmRule =
                AlarmRuleAccessFactory::getInstance().createPlanExecutionRule();
        }
        
        // Automatic Acknowledgement
        else if(alarmRuleType == AutomaticAcknowledgementRuleData::RULE_TYPE)
        {
            newAlarmRule =
                AlarmRuleAccessFactory::getInstance().createAutomaticAcknowledgementRule();
        }
        
        // Avalanche Suppression
        else if(alarmRuleType == AvalancheSuppressionRuleData::RULE_TYPE)
        {
            newAlarmRule =
                AlarmRuleAccessFactory::getInstance().createAvalancheSuppressionRule();
        }

        // MMS
        else if (alarmRuleType == MMSRuleData::RULE_TYPE)
        {
            newAlarmRule =
                AlarmRuleAccessFactory::getInstance().createMMSRule();
        }

        // CCTV
        else if (alarmRuleType == CCTVRuleData::RULE_TYPE)
        {
            newAlarmRule = 
                AlarmRuleAccessFactory::getInstance().createCCTVRule();
        }

        else
        {
            // Assert if there is no matching rule type
            TA_ASSERT(false, "Unrecognised alarm rule type");
        }


        m_alarmRules.insert(
            LoadedAlarmRules::value_type(
                newAlarmRule->getUniqueIdentifier(), newAlarmRule ) );

        return newAlarmRule;

        FUNCTION_EXIT;
    }



    TA_Base_Core::IConfigItem* AlarmRulesDatabaseAccessor::newItem()
    {
        TA_ASSERT(false,
                  "This method should not be used. "
                  "Use newItem(const std::string) instead");
        
        return NULL;
    }


    TA_Base_Core::IConfigItem* AlarmRulesDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IAlarmRuleData* newAlarmRule = NULL;

        // Attempt to find the alarm plan association matching the key passed in.
        LoadedAlarmRules::iterator matching = m_alarmRules.find(idOfItemToCopy);


        IAlarmRuleData* alarmRule = (*matching).second;

        TA_ASSERT(matching != m_alarmRules.end(), "The key passed could not be found in the map");
        TA_ASSERT(alarmRule != NULL, "There was a NULL AlarmRule stored in the map");

        // The type of alarm rule determines what subclass of IAlarmRuleData
        // is returned.

        std::string alarmRuleType = alarmRule->getRuleType();

        // Suppression by Type
        if(alarmRuleType == SuppressionRuleData::RULE_TYPE)
        {
            SuppressionRuleData* suppressionRule =
                dynamic_cast<SuppressionRuleData*>(alarmRule);

            TA_ASSERT(suppressionRule != NULL, "The alarm rule cast has failed");

            newAlarmRule =
                AlarmRuleAccessFactory::getInstance().copySuppressionRule(
                    *suppressionRule);
        }

		// Suppression by Description
        else if(alarmRuleType == SuppressionByDescriptionRuleData::RULE_TYPE)
        {
            SuppressionByDescriptionRuleData* suppressionByDescriptionRule =
                dynamic_cast<SuppressionByDescriptionRuleData*>(alarmRule);

            TA_ASSERT(suppressionByDescriptionRule != NULL, "The alarm rule cast has failed");

            newAlarmRule =
                AlarmRuleAccessFactory::getInstance().copySuppressionByDescriptionRule(
                    *suppressionByDescriptionRule);
        }

        // Plan Execution
        else if(alarmRuleType == PlanExecutionRuleData::RULE_TYPE)
        {
            PlanExecutionRuleData* planExecutionRule =
                dynamic_cast<PlanExecutionRuleData*>(alarmRule);

            TA_ASSERT(planExecutionRule != NULL, "The alarm rule cast has failed");

            newAlarmRule =
                AlarmRuleAccessFactory::getInstance().copyPlanExecutionRule(
                    *planExecutionRule);
        }
        
        // Automatic Acknowledgement
        else if(alarmRuleType == AutomaticAcknowledgementRuleData::RULE_TYPE)
        {
            AutomaticAcknowledgementRuleData* autoAckRuleData =
                dynamic_cast<AutomaticAcknowledgementRuleData*>(alarmRule);

            TA_ASSERT(autoAckRuleData != NULL, "The alarm rule cast has failed");

            newAlarmRule =
                AlarmRuleAccessFactory::getInstance().copyAutomaticAcknowledgementRule(
                    *autoAckRuleData);
        }
        
        // Avalanche Suppression
        else if(alarmRuleType == AvalancheSuppressionRuleData::RULE_TYPE)
        {
            AvalancheSuppressionRuleData* avalancheSuppressionRule =
                dynamic_cast<AvalancheSuppressionRuleData*>(alarmRule);

            TA_ASSERT(avalancheSuppressionRule != NULL,
                      "The alarm rule cast has failed");

            newAlarmRule =
                AlarmRuleAccessFactory::getInstance().copyAvalancheSuppressionRule(
                    *avalancheSuppressionRule);
        }

        // MMS
        else if(alarmRuleType == MMSRuleData::RULE_TYPE)
        {
            MMSRuleData* mmsRule =
                dynamic_cast<MMSRuleData*>(alarmRule);

            TA_ASSERT(mmsRule != NULL, "The alarm rule cast has failed");
                
            newAlarmRule =
                AlarmRuleAccessFactory::getInstance().copyMMSRule(
                    *mmsRule);
        }

        // CCTV
        else if(alarmRuleType == CCTVRuleData::RULE_TYPE)
        {
            CCTVRuleData* cctvRule =
                dynamic_cast<CCTVRuleData*>(alarmRule);

            TA_ASSERT(cctvRule != NULL, "The alarm rule cast has failed");
                
            newAlarmRule =
                AlarmRuleAccessFactory::getInstance().copyCCTVRule(
                    *cctvRule);
        }

        else
        {
            // Assert if there is no matching rule type
            TA_ASSERT(false, "Unrecognised alarm rule type");
        }

        
        if (newAlarmRule != NULL)
        {
            m_alarmRules.insert(
                LoadedAlarmRules::value_type(
                    newAlarmRule->getUniqueIdentifier(), newAlarmRule ) );
        }


        return newAlarmRule;

        FUNCTION_EXIT;
    }


    bool AlarmRulesDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& itemsNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(itemsNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedAlarmRules::iterator iter = m_alarmRules.begin(); iter != m_alarmRules.end();  ++iter)
        {
            if( (iter->second != NULL) && (iter->second->hasChanged() || iter->second->isNew()) )
            {
                // We just add a single empty item if we know a change has been made. We don't list each
                // individual change for alarm rules
                itemsNotApplied.push_back("");
                break;
            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !itemsNotApplied.empty();
    }


    std::map<unsigned long, std::string> AlarmRulesDatabaseAccessor::getAllAlarmTypes()
    {
        std::map<unsigned long, std::string> namesAndKeys;

        using TA_Base_Core::IAlarmTypeData;

        // First retrieve all the subsystems from the database
        std::vector<IAlarmTypeData*> alarmTypes;
        try
        {
            alarmTypes = TA_Base_Core::AlarmTypeAccessFactory::getInstance().getAllAlarmTypes(false);
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No alarm types will be added to the map. An empty map will be returned");
            return namesAndKeys;
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No alarm types will be added to the map. An empty map will be returned");
            return namesAndKeys;
        }


        // Now step through each one and:
        // 1) Add it's name and key to the map we want to return
        // 2) Delete the IAlarmTypeData object when we're done
        for (std::vector<IAlarmTypeData*>::iterator iter = alarmTypes.begin(); iter != alarmTypes.end(); ++iter)
        {
            if (*iter != NULL)
            {
                try
                {
                    namesAndKeys.insert( std::map<unsigned long, std::string>::value_type( (*iter)->getKey(), (*iter)->getName() ) );
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "This alarm type will not be added to the map");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "This alarm type will not be added to the map");
                }

                // Now we're finished with this iterator so we might as well delete what it contains so we
                // don't have to do it later.
                delete *iter;
                *iter = NULL;
            }
        }

        return namesAndKeys;
    }

	std::map<unsigned long, std::string> AlarmRulesDatabaseAccessor::getAllAlarmTypeDisplays()
	{
		std::map<unsigned long, std::string> namesAndKeys;

		using TA_Base_Core::IAlarmTypeData;

		// First retrieve all the subsystems from the database
		std::vector<IAlarmTypeData*> alarmTypes;
		try
		{
			alarmTypes = TA_Base_Core::AlarmTypeAccessFactory::getInstance().getAllAlarmTypes(false);
		}
		catch( const TA_Base_Core::DatabaseException& )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No alarm types will be added to the map. An empty map will be returned");
			return namesAndKeys;
		}
		catch( const TA_Base_Core::DataException& )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No alarm types will be added to the map. An empty map will be returned");
			return namesAndKeys;
		}


		// Now step through each one and:
		// 1) Add it's name and key to the map we want to return
		// 2) Delete the IAlarmTypeData object when we're done
		for (std::vector<IAlarmTypeData*>::iterator iter = alarmTypes.begin(); iter != alarmTypes.end(); ++iter)
		{
			if (*iter != NULL)
			{
				try
				{
					namesAndKeys.insert( std::map<unsigned long, std::string>::value_type( (*iter)->getKey(), (*iter)->getDisplayName() ) );
				}
				catch ( const TA_Base_Core::DataException& )
				{
					LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "This alarm type will not be added to the map");
				}
				catch ( const TA_Base_Core::DatabaseException& )
				{
					LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "This alarm type will not be added to the map");
				}

				// Now we're finished with this iterator so we might as well delete what it contains so we
				// don't have to do it later.
				delete *iter;
				*iter = NULL;
			}
		}

		return namesAndKeys;
	}

    std::vector<unsigned long> AlarmRulesDatabaseAccessor::getEntityDescendants(
        unsigned long pkey)
    {
        // Add the pkey passed as a parameter to the descendants list
        std::vector<unsigned long> descendants;
        descendants.push_back(pkey);


        // Add the rest of the descendants
        std::vector<unsigned long> children = getEntityDescendantsRecursive(pkey);
            
        descendants.reserve(descendants.size() + children.size());


        for(std::vector<unsigned long>::iterator childIt = children.begin();
            childIt != children.end();
            childIt ++)
        {
            descendants.push_back(*childIt);
        }

        return descendants;
    }

    
    std::vector<unsigned long>
        AlarmRulesDatabaseAccessor::getEntityDescendantsRecursive(
            unsigned long pkey)
    {
        std::vector<unsigned long> descendants;

        typedef std::vector< IEntityData* > IEntityDataList;
        IEntityDataList entities =
            EntityAccessFactory::getInstance().getChildEntitiesOf(pkey);

        descendants.reserve(entities.size());

        for (IEntityDataList::iterator it = entities.begin();
             it != entities.end();
             it ++)
        {
            unsigned long key = (*it)->getKey();
            descendants.push_back(key);

            // Get the descendants of each child of this entity,
            // and add them to the descendants vector.
            std::vector<unsigned long> children =
                getEntityDescendantsRecursive(key);
            
            descendants.reserve(descendants.size() + children.size());
            for(std::vector<unsigned long>::iterator childIt = children.begin();
                childIt != children.end();
                childIt ++)
            {
                descendants.push_back( (*childIt) );
            }

            // Now we're finished with this iterator so we might as well
            // delete what it contains so we don't have to do it later.
            delete *it;
            *it = NULL;
        }

        return descendants;
    }

    
    std::map<unsigned long, std::string> AlarmRulesDatabaseAccessor::getAllEntityTypes()
    {
        FUNCTION_ENTRY("getAllEntityTypes");

        // First retrieve all the subsystems from the database
        std::map<unsigned long, std::string> entityTypes;
        try
        {
            entityTypes = TA_Base_Core::EntityAccessFactory::getInstance().getEntityTypes();
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No alarm types will be added to the map. An empty map will be returned");
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No alarm types will be added to the map. An empty map will be returned");
        }

        FUNCTION_EXIT;
        return entityTypes;
    }
	bool AlarmRulesDatabaseAccessor::isChildAlsoAHead(TA_Base_Core::AvalancheSuppressionRuleData::EntityKeyAlarmTypePair& child,TA_Base_Core::IAlarmRuleData**head)
	{
	     // Step through all the objects
        for (LoadedAlarmRules::iterator iter = m_alarmRules.begin(); iter != m_alarmRules.end();  ++iter)
        {
        	if((iter->second != NULL) && iter->second->getRuleType()!= AvalancheSuppressionRuleData::RULE_TYPE)
    		{
	    		continue;
    		}
            if( (iter->second != NULL) && iter->second->getEntityKey()==child.first && iter->second->getAlarmType()==child.second)
            {
                *head = iter->second;
                return true;
            }
        }
		
		return false;
	}
	bool AlarmRulesDatabaseAccessor::isExistChild(TA_Base_Core::IAlarmRuleData* head, TA_Base_Core::IAlarmRuleData* child)
	{
		if(NULL==head) return false;
		std::vector<AvalancheSuppressionRuleData::EntityKeyAlarmTypePair> Children=dynamic_cast<AvalancheSuppressionRuleData*>(head)->getChildren();
		for(std::vector<AvalancheSuppressionRuleData::EntityKeyAlarmTypePair>::iterator itr=Children.begin();itr!=Children.end();itr++)
		{
			TA_Base_Core::IAlarmRuleData* headForGoThrough=NULL;
			if(itr->first == child->getEntityKey() && itr->second == child->getAlarmType())
			{
				return true;
			}
			else if(isChildAlsoAHead(*itr, &headForGoThrough))
			{
				return isExistChild(headForGoThrough, child);
			}
		}
		return false;
	}
}
