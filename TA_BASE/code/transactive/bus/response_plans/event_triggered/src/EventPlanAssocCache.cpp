/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/event_triggered/src/EventPlanAssocCache.cpp $
 * @author:  Brad Cleaver
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This class implements the cache that determines what plans should be executed for a given event.
 */

#include "bus/response_plans/event_triggered/src/EventPlanAssocCache.h"

#include "core/configuration_updates/src/ConfigUpdateDetails.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/data_access_interface/plans/src/EventPlanAssocAccessFactory.h"
#include "core/data_access_interface/plans/src/IEventPlanAssocData.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace
{
    // A 0 from the DAI means that the key is not specified. This means that it should match
    // all keys.
    static const unsigned long ALL_KEYS(0);
};

namespace TA_Base_Bus
{
    EventPlanAssocCache::EventPlanAssocCache() : m_cachePopulated(false)
    {
        // Register for updates to the event plan associations.
        TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests( TA_Base_Core::EVENT_PLAN_ASSOCIATION, *this );
        populateCache();
    }


    EventPlanAssocCache::~EventPlanAssocCache()
    {
        // Unregister for updates.
        TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests( TA_Base_Core::EVENT_PLAN_ASSOCIATION, *this );

        // Now flush the cache.
        flushCache();
    }


    void EventPlanAssocCache::processUpdate( const TA_Base_Core::ConfigUpdateDetails& updateEvent )
    {
        // We've only registered for the one update so don't bother checking, just flush the cache. We
        // don't want to repopulate the cache here since all executables that use the event triggered plan
        // processor would then hammer the database all at once! Instead just clear the list and it will
        // be populated the next time an event is raised.
        flushCache();
    }


    EventPlanAssocCache::PlanNames EventPlanAssocCache::findPlansForEvent( const TA_Base_Core::Event& event )
    {
        TA_Base_Core::ThreadGuard guard( m_cacheLock );

        // First make sure the cache is populated.
        if ( !m_cachePopulated )
        {
            populateCache();
        }

        PlanNames planNames;

        // Now search all associations for matches.
        AssocVector::iterator it;
        for ( it=m_assocCache.begin(); it!=m_assocCache.end(); it++ )
        {
            // If the event matches the association add its name to the list.
            if ( eventMatchesAssoc( event, *it ) )
            {
                planNames.push_back( (*it)->getPlanPath() );
            }
        }

        return planNames;
    }


    bool EventPlanAssocCache::eventMatchesAssoc( const TA_Base_Core::Event& event, TA_Base_Core::IEventPlanAssocData* const assoc )
    {
        TA_ASSERT( NULL!=assoc, "Can't match against a NULL association." );

        // TD14811 for event plan association, no need for entity, just need to check event type.
        /*// Check if the entity key matches.
        unsigned long entityKey = assoc->getEntityKey();
        if ( ( ALL_KEYS != entityKey ) && ( event->getEntityKey() != entityKey ) )
        {
            return false;
        }

        // Check if the entity type key matches.
        unsigned long entityTypeKey = assoc->getEntityTypeKey();
        if ( ( ALL_KEYS != entityTypeKey ) && ( event->getEntityTypeKey() != entityKey ) )
        {
            return false;
        }*/

        // Check if the event type key matches.
        unsigned long eventTypeKey = assoc->getEventTypeKey();
        if ( event->getEventTypeKey() != eventTypeKey )
        {
            return false;
        }

        // If we get to here the entity key, entity type key, and event type key all match.
        return true;
    }


    void EventPlanAssocCache::populateCache()
    {
        TA_Base_Core::ThreadGuard guard( m_cacheLock );

        // If the cache already contains data then flush it.
        if ( !m_assocCache.empty() )
        {
            flushCache();
        }

        try
        {
            // Retrieve the enabled associations from the database.
            m_assocCache = TA_Base_Core::EventPlanAssocAccessFactory::getInstance().getEnabledEventPlanAssociations( false );
            m_cachePopulated = true;
        }
        catch ( ... )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Encountered an error while loading associations." );
        }
    }


    void EventPlanAssocCache::flushCache()
    {
        TA_Base_Core::ThreadGuard guard( m_cacheLock );

        // Iterate through all associations and delete them.
        std::vector<TA_Base_Core::IEventPlanAssocData*>::iterator it;
        for ( it=m_assocCache.begin(); it!=m_assocCache.end(); it++ )
        {
            delete *it;
        }

        // Clear the vector.
        m_assocCache.clear();

        // Cache is no longer populated.
        m_cachePopulated = false;
    }
};
