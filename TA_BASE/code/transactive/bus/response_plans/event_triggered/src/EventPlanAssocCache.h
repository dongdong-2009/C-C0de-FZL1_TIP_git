/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/event_triggered/src/EventPlanAssocCache.h $
 * @author:  Brad Cleaver
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This class implements the cache that determines what plans should be executed for a given event.
 */

#if !defined(_EVENT_PLAN_ASSOC_CACHE_H_)
#define _EVENT_PLAN_ASSOC_CACHE_H_

#include <string>
#include <vector>

#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/event_processing/src/IEvent.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    // Forward declarations.
    class ConfigUpdateDetails;
    class IEventPlanAssocData;
};

namespace TA_Base_Bus
{
    class EventPlanAssocCache : TA_Base_Core::IOnlineUpdatable
    {
    public:
        // New types used in the cache.
        typedef std::vector<std::string> PlanNames;

        /**
         * Constructor
         */
        EventPlanAssocCache();


        /**
         * Destructor
         */
        virtual ~EventPlanAssocCache();


        /**
         * processUpdate
         *
         * When there is a configuration update of the type and key matching
         * one registered by this class, this method will be invoked
         * to process the update accordingly.
         */
        virtual void processUpdate( const TA_Base_Core::ConfigUpdateDetails& updateEvent );


        /**
         * findPlansForEvent
         *
         * Searches through the list of associations and returns a vector of names
         * of plans that match.
         *
         * @param event The event find the plans for.
         *
         * @return PlanNames The names of all plans that should be run for the given event.
         */
        PlanNames findPlansForEvent( const TA_Base_Core::Event& event );

    private:
        // Hidden methods.
        EventPlanAssocCache( const EventPlanAssocCache& );
        const EventPlanAssocCache& operator=( const EventPlanAssocCache& );

        /**
         * eventMatchesAssoc
         *
         * Checks to see if the given event matches the specified association.
         *
         * @param event The event to match with.
         * @param assoc The association to match against.
         *
         * @return bool True if the event matches the association.
         */
        bool eventMatchesAssoc( const TA_Base_Core::Event& event, TA_Base_Core::IEventPlanAssocData* const assoc );

        /**
         * populateCache
         *
         * Populates the association cache with data from the database.
         */
        void populateCache();


        /**
         * flushCache
         *
         * Removes all associations from the cache.
         */
        void flushCache();

        // The boolean that is used to flag the state of the cache.
        bool m_cachePopulated;

        // The vector that stores all of the enabled event plan associations.
        typedef std::vector<TA_Base_Core::IEventPlanAssocData*> AssocVector;
        AssocVector m_assocCache;

        // The lock used to ensure cache operations are not interrupted.
        TA_Base_Core::ReEntrantThreadLockable m_cacheLock;
    };
};

#endif // !defined(_EVENT_PLAN_ASSOC_CACHE_H_)
