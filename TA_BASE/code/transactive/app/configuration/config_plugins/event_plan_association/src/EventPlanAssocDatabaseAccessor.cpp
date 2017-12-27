/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/event_plan_association/src/EventPlanAssocDatabaseAccessor.cpp $
  * @author:  Brad Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve event plan association information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/event_plan_association/src/StdAfx.h"

#include "app/configuration/config_plugins/event_plan_association/src/EventPlanAssocDatabaseAccessor.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/plans/src/IConfigEventPlanAssocData.h"
#include "core/data_access_interface/plans/src/IEventPlanAssocData.h"
#include "core/data_access_interface/plans/src/EventPlanAssocAccessFactory.h"
#include "core/data_access_interface/src/IMessageTypeData.h"
#include "core/data_access_interface/src/MessageTypeAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DebugUtil;

namespace
{
    const std::string AUDIT_CHANNEL_NAME( "Audit" );
    const std::string DP_UPDATE_EVENT_NAME( "DataPointValueStatusUpdate" );
}

namespace TA_Base_App
{
    EventPlanAssocDatabaseAccessor::EventPlanAssocDatabaseAccessor()
        : m_isLoaded(false)
    {
    }


    EventPlanAssocDatabaseAccessor::~EventPlanAssocDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        // We must clean up all the database objects we loaded
        LoadedAssociations::iterator assocIt;
        for ( assocIt=m_eventPlanAssocs.begin(); assocIt!=m_eventPlanAssocs.end(); assocIt++ )
        {
            if( NULL != assocIt->second )
            {
                delete assocIt->second;
                assocIt->second = NULL;
            }
        }
        m_eventPlanAssocs.clear();

        FUNCTION_EXIT;
    }


    void EventPlanAssocDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if ( m_isLoaded )
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "We have already loaded the event plan associations, ignoring request..." );
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        LoadedAssociations::iterator toDeleteIt;
        for ( toDeleteIt=m_eventPlanAssocs.begin(); toDeleteIt!=m_eventPlanAssocs.end(); toDeleteIt++ )
        {
            if( NULL != toDeleteIt->second )
            {
                delete toDeleteIt->second;
                toDeleteIt->second = NULL;
            }
        }
        m_eventPlanAssocs.clear();


        // Now reload all the event plan associations
        std::vector<TA_Base_Core::IEventPlanAssocData*> associations = TA_Base_Core::EventPlanAssocAccessFactory::getInstance().getAllEventPlanAssociations( true );

        // And insert the event plan associations into our map. At the same time cast them to IConfigEventPlanAssocData*
        // instead of IEventPlanAssocData*.
        std::vector<TA_Base_Core::IEventPlanAssocData*>::iterator toAddIt;
        for ( toAddIt=associations.begin(); toAddIt!=associations.end(); toAddIt++ )
        {
             TA_Base_Core::IConfigEventPlanAssocData* castItem = dynamic_cast<TA_Base_Core::IConfigEventPlanAssocData*>(*toAddIt);
             if( NULL != castItem )
             {
                m_eventPlanAssocs.insert( LoadedAssociations::value_type(castItem->getUniqueIdentifier(), castItem) );
             }
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> EventPlanAssocDatabaseAccessor::getItemNames( CProgressCtrl& progress )
    {
        TA_ASSERT( false, "This method cannot be called for event plan associations" );

        // Make the compiler happy.
        std::multimap<CString, unsigned long> temp;
        return temp;
    }


    std::map<unsigned long,EventPlanAssocDatabaseAccessor::EventPlanAssociation> EventPlanAssocDatabaseAccessor::getAllItemDetails( CProgressCtrl& progress )
    {
        FUNCTION_ENTRY("getAllItemDetails");

        std::map<unsigned long,EventPlanAssociation> details; 

        // The maximum range is the number of event plan associations multiplied by 2. This is because we want to step for each
        // event plan association while it is loaded and then again while it is added to the list.
        progress.SetRange( 0, m_eventPlanAssocs.size()*2 );

        // Step through all the objects and retrieve the details for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        LoadedAssociations::iterator assocIt;
        for ( assocIt=m_eventPlanAssocs.begin(); assocIt!=m_eventPlanAssocs.end(); assocIt++ )
        {
            if ( NULL != assocIt->second )
            {
                EventPlanAssociation item;
                item.enabled = assocIt->second->isEnabled();
                //item.eventType = assocIt->second->getEventTypeName().c_str();
				item.eventType = assocIt->second->getEventTypeDisplayName().c_str();
                item.entity = assocIt->second->getEntityName().c_str();
                item.entityType = assocIt->second->getEntityTypeName().c_str();
                item.plan = assocIt->second->getPlanPath().c_str();

                details.insert( std::map<unsigned long,EventPlanAssociation>::value_type( assocIt->second->getUniqueIdentifier(), item ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return details;
    }


    EventPlanAssocDatabaseAccessor::EventPlanAssociation EventPlanAssocDatabaseAccessor::getItemDetails( unsigned long identifier )
    {
        FUNCTION_ENTRY("getItemDetails");

        // Attempt to find the event plan association matching the key passed in.
        LoadedAssociations::iterator matchingAssoc = m_eventPlanAssocs.find( identifier );
        if ( matchingAssoc == m_eventPlanAssocs.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the event plan association with identifier %lu", identifier );
            FUNCTION_EXIT;
            EventPlanAssociation item;
            return item;
        }

        EventPlanAssociation item;
        item.enabled = matchingAssoc->second->isEnabled();
        item.entity = matchingAssoc->second->getEntityName().c_str();
        item.entityType = matchingAssoc->second->getEntityTypeName().c_str();
        item.eventType = matchingAssoc->second->getEventTypeName().c_str();
        item.plan = matchingAssoc->second->getPlanPath().c_str();

        FUNCTION_EXIT;
        return item;
    }


    TA_Base_Core::IConfigItem* EventPlanAssocDatabaseAccessor::getItem( unsigned long key )
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the event plan association matching the key passed in.
        LoadedAssociations::iterator matchingAssoc = m_eventPlanAssocs.find( key );
        if ( matchingAssoc == m_eventPlanAssocs.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the event plan association with key %lu", key );
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matchingAssoc->second;
    }


    void EventPlanAssocDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the event plan association matching the key passed in.
        LoadedAssociations::iterator matchingAssoc = m_eventPlanAssocs.find(key);
        if ( matchingAssoc == m_eventPlanAssocs.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the event plan association with key %lu", key );
            FUNCTION_EXIT;
            return;
        }

        TA_ASSERT( matchingAssoc->second != NULL, "Stored event plan association was equal to NULL" );

        // If the item isn't new it exists in the database and must be removed.
        if ( !matchingAssoc->second->isNew() )
        {
            matchingAssoc->second->deleteThisAssociation();
        }

        // Now remove it from the map.
        delete matchingAssoc->second;
        matchingAssoc->second = NULL;
        m_eventPlanAssocs.erase( matchingAssoc );

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* EventPlanAssocDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        // Create the new event plan association and add it to the map.
        TA_Base_Core::IConfigEventPlanAssocData* newAssoc = TA_Base_Core::EventPlanAssocAccessFactory::getInstance().createEventPlanAssociation().release();
        m_eventPlanAssocs.insert( LoadedAssociations::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );

        FUNCTION_EXIT;
        return newAssoc;
    }


    TA_Base_Core::IConfigItem* EventPlanAssocDatabaseAccessor::copyItem( const unsigned long idOfItemToCopy )
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IConfigEventPlanAssocData* newAssoc = NULL;

        // Attempt to find the event plan association matching the key passed in.
        LoadedAssociations::iterator matchingAssoc = m_eventPlanAssocs.find( idOfItemToCopy );

        TA_ASSERT( matchingAssoc != m_eventPlanAssocs.end(), "The key passed could not be found in the map" );
        TA_ASSERT( matchingAssoc->second != NULL, "There was a NULL ConfigEventPlanAssoc stored in the map" );

        // Create the copy and add it to the map.
        newAssoc = TA_Base_Core::EventPlanAssocAccessFactory::getInstance().copyEventPlanAssociation( matchingAssoc->second ).release();
        if ( NULL != newAssoc )
        {
            m_eventPlanAssocs.insert( LoadedAssociations::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );
        }
        
        FUNCTION_EXIT;
        return newAssoc;
    }


    bool EventPlanAssocDatabaseAccessor::areCurrentChangesPending( std::vector<std::string>& itemsNotApplied )
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT( itemsNotApplied.empty(), "The vector of strings passed was not empty" );

        // If the data has not been loaded yet then there can't be any changes pending
        if ( !m_isLoaded )
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        LoadedAssociations::iterator assocIt;
        for ( assocIt=m_eventPlanAssocs.begin(); assocIt!=m_eventPlanAssocs.end(); assocIt++ )
        {
            if ( ( NULL != assocIt->second ) && ( assocIt->second->hasChanged() || assocIt->second->isNew() ) )
            {
                // We just add a single empty item if we know a change has been made. We don't list each
                // individual change for event plan associations
                itemsNotApplied.push_back("");
                break;
            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !itemsNotApplied.empty();
    }


    std::map<unsigned long, std::string> EventPlanAssocDatabaseAccessor::getAllEventTypes()
    {
        FUNCTION_ENTRY("getAllEventTypes");

        std::map<unsigned long, std::string> namesAndKeys;

        // First retrieve all the subsystems from the database
        std::vector<TA_Base_Core::IMessageTypeData*> eventTypes;
        try
        {
            // Get all the audit event types.
            std::vector<std::string> auditMessageFilter;
            auditMessageFilter.push_back( AUDIT_CHANNEL_NAME );
            eventTypes = TA_Base_Core::MessageTypeAccessFactory::getInstance().getAllMessageTypesAndAttributes( auditMessageFilter );
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No event types will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No event types will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return namesAndKeys;
        }

        // Now step through each one and:
        // 1) Add it's name and key to the map we want ot return
        // 2) Delete the IMessageTypeData object when we're done
        std::vector<TA_Base_Core::IMessageTypeData*>::iterator eventTypeIt;
        for ( eventTypeIt=eventTypes.begin(); eventTypeIt!=eventTypes.end(); eventTypeIt++ )
        {
            if ( NULL != *eventTypeIt )
            {
                try
                {
                    //namesAndKeys.insert( std::map<unsigned long, std::string>::value_type( (*eventTypeIt)->getKey(), (*eventTypeIt)->getName() ) );
					namesAndKeys.insert( std::map<unsigned long, std::string>::value_type( (*eventTypeIt)->getKey(), (*eventTypeIt)->getDisplayName() ) );
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "This event type will not be added to the map");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "This event type will not be added to the map");
                }

                // Now we're finished with this iterator so we might as well delete what it contains so we
                // don't have to do it later.
                delete *eventTypeIt;
                *eventTypeIt = NULL;
            }
        }

        FUNCTION_EXIT;
        return namesAndKeys;
    }


    std::map<unsigned long, std::string> EventPlanAssocDatabaseAccessor::getAllEntityTypes()
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No event types will be added to the map. An empty map will be returned" );
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No event types will be added to the map. An empty map will be returned" );
        }

        FUNCTION_EXIT;
        return entityTypes;
    }

}
