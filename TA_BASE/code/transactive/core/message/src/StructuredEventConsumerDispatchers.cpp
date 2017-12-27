/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/StructuredEventConsumerDispatchers.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/message/src/StructuredEventConsumerDispatchers.h"
#include "core/message/src/TA_CosUtility.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TA_String.h"

// TODO: Move SubscriptionFilter into a new header/cpp, then remove these #includes
#include "core/message/src/SubscriptionManager.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    TypeNameDispatcher::~TypeNameDispatcher()
    {
        // Subscribers must have detached by now.
        // Dump the most derived type of the handlers for subscribers which are still attached
        {
            for ( StarList::iterator iter = m_starSubscribers.begin(); iter != m_starSubscribers.end(); ++iter )
            {
                LOG3( SourceInfo, DebugUtil::DebugMsg,
                    "TypeNameDispatcher::~TypeNameDispatcher(): subscriber %s with handler %s "
                    "for event type * still attached to dispatcher %s",
                    gPtrToStr((*iter).m_subFilter).c_str(), gPtrToStr((*iter).m_handler).c_str(), gPtrToStr(this).c_str() );
            }
        }
        {
            for ( TypeNameToSubscriptionFilterMap::iterator iter = m_typeNameSubscribers.begin(); iter != m_typeNameSubscribers.end(); ++iter )
            {
                LOG4( SourceInfo, DebugUtil::DebugMsg,
                    "TypeNameDispatcher::~TypeNameDispatcher(): subscriber %s with handler %s "
                    "for event type %s still attached to dispatcher %s",
                    gPtrToStr((*iter).second.m_subFilter).c_str(), gPtrToStr((*iter).second.m_handler).c_str(),
                    (*iter).first.c_str(), gPtrToStr(this).c_str() );
            }
        }
        // TODO LPP: These assertions should be re-enabled.  They have been temporarily disabled
        // because some applications, perhaps including those derived from GenericGUI, are unable
        // to shut down correctly (GenericGUI might be destroying objects in the wrong order?).
        // Refer to TD11793.
        //TA_Assert(m_starSubscribers.size() == 0);
        //TA_Assert(m_typeNameSubscribers.size() == 0);
    }


    void TypeNameDispatcher::addSubscriber( IEventHandler* handler, SubscriptionFilter* filter )
    {
        TA_Assert(handler != NULL);
        TA_Assert(filter != NULL);

        CosNotification::EventTypeSeq seq = filter->getFilterCriteria().event_types;
        for ( unsigned int i = 0; i < seq.length() ; i++ )
        {
            // Each event type in filter must have the SAME domain_name (since we don't check
            // the domain_name when we append to our data structures below, or when we dispatch
            // events).

            TA_Assert( 0 == strcmp( seq[i].domain_name.in(), seq[0].domain_name.in() ) );
            LOG1( SourceInfo, DebugUtil::DebugMsg,
                "TypeNameDispatcher::addSubscriber(): adding %s",
                gStringFromPair(seq[i].domain_name.in(), seq[i].type_name.in()).c_str() );

            if ( 0 == strcmp( seq[i].type_name.in(), WILDCARD_STR ) )
            {
                // Star list
                m_starSubscribers.push_back( SubscriptionDetail(handler, filter) );
            }
            else
            {
                // TODO LPP: The same filter can be added multiple times for the same type_name
                // since each SubscriptionDetail will be a different instance.  We really need
                // to require that the list of event types in 'filter' contains no duplicates.
                m_typeNameSubscribers.insert( TypeNameToSubscriptionFilterMap::value_type(
                    seq[i].type_name.in(), SubscriptionDetail(handler, filter) ) );
            }
        }
    }


    void TypeNameDispatcher::removeSubscriber( SubscriptionFilter* filter )
    {
        // Find and erase all occurrences of 'filter' from the * list (it seems that
        // a given [domain_name, *] subscription might be added multiple times).
        // TODO LPP: Would std::remove be a more elegant way of writing this?
        StarList::iterator starIt = m_starSubscribers.begin();
        while ( starIt != m_starSubscribers.end() )
        {
            if ( (*starIt).m_subFilter == filter )
            {
                starIt = m_starSubscribers.erase( starIt );
            }
            else
            {
                starIt++;
            }
        }

        // Go through all the rest of the types, there could be multiple occurances
        TypeNameToSubscriptionFilterMap::iterator typeIt = m_typeNameSubscribers.begin();
        while ( typeIt != m_typeNameSubscribers.end() )
        {
            if ( typeIt->second.m_subFilter == filter )
            {
                m_typeNameSubscribers.erase( typeIt++ );
            }
            else
            {
                typeIt++;
            }
        }
    }


    void TypeNameDispatcher::dispatchMessage(const CosNotification::StructuredEvent& event) const
    {
        {
            // dispatch to all subscribers in "*", providing the message is passed by the
            // particular subscriber's filter.

            StarList::const_iterator it = m_starSubscribers.begin();
            for ( ; it != m_starSubscribers.end(); it++ )
            {
                TA_Assert((*it).m_handler != NULL);
                TA_Assert((*it).m_subFilter != NULL);

                if ( (*it).m_subFilter->messageMatchesFilter( event ) )
                {
                    LOG1( SourceInfo, DebugUtil::DebugMsgPub,
                        "TypeNameDispatcher::dispatchMessage(): event matched client side filter for * SubscriptionFilter %s",
                        gPtrToStr((*it).m_subFilter).c_str() );

                    std::vector<const CosNotification::StructuredEvent*> vec;
                    vec.push_back(&event);
                    gLogSpecificMessage(event,"TypeNameDispatcher::dispatchMessage() for star");
                    (*it).m_handler->onEventsReceived(vec);
                }
                else
                {
                    LOG1( SourceInfo, DebugUtil::DebugMsgPub,
                        "TypeNameDispatcher::dispatchMessage(): event didn't match client side filter for * SubscriptionFilter %s",
                        gPtrToStr((*it).m_subFilter).c_str() );
                }
            }
        }

        {
            // dispatch to all subscribers interested in the given type_name.
            // TODO LPP: Should a subscriber receive multiple notifications if it has
            // somehow been added multiple times (eg. the sequence of event types includes
            // the same type multiple times).  Should the sequence be allowed to contain
            // duplicates?
            typedef TypeNameToSubscriptionFilterMap::const_iterator IterType;
            std::pair<IterType, IterType> bounds = m_typeNameSubscribers.equal_range(event.header.fixed_header.event_type.type_name.in());
            for ( IterType iter = bounds.first; iter != bounds.second; ++iter )
            {
                TA_Assert(iter->second.m_subFilter != NULL);
                TA_Assert(iter->second.m_handler != NULL);

                if ( iter->second.m_subFilter->messageMatchesFilter( event ) )
                {
                    LOG1( SourceInfo, DebugUtil::DebugMsgPub,
                        "TypeNameDispatcher::dispatchMessage(): event matched client side filter for SubscriptionFilter %s",
                        gPtrToStr(iter->second.m_subFilter).c_str() );

                    std::vector<const CosNotification::StructuredEvent*> vec;
                    vec.push_back(&event);
                    gLogSpecificMessage(event,"TypeNameDispatcher::dispatchMessage()");
                    iter->second.m_handler->onEventsReceived(vec);
                }
                else
                {
                    LOG1( SourceInfo, DebugUtil::DebugMsgPub,
                        "TypeNameDispatcher::dispatchMessage(): event didn't match client side filter for SubscriptionFilter %s",
                        gPtrToStr(iter->second.m_subFilter).c_str() );
                }
            }
        }
    }


    // Returns true if two EventTypeSeq are the 'same', false otherwise.  The sequences
    // must be the same size and every element in seq1 must occur somewhere in seq2.
    // It doesn't require elements to be in the same order, that every element in seq2
    // also occurs in seq1, or that repeated elements in either sequence are also repeated
    // in the other sequence.
    // TODO LPP: This is a rather unusual relation.  For example, contained_in({A, A, B, B}, {A, B, C, D})
    // would return true.  The intention is probably to test for equality.
    // Can a more generic version be written for arbitrary containers?
    // Perhaps we can provide iterator_traits then use an STL algorithm?
    bool contained_in( const CosNotification::EventTypeSeq& seq1, const CosNotification::EventTypeSeq seq2 )
    {
        // Sequences must be the same length if they are equal.
        // TODO: This implementation doesn't actually test for equality.
        if ( seq1.length() != seq2.length() )
        {
            // They can't be the same!
            return false;
        }

        // Check that each element in seq1 also occurs somewhere (at least once) in seq2.
        for (unsigned int i = 0; i < seq1.length(); i++)
        {
            bool match = false;

            for (unsigned int j = 0; j < seq2.length(); j++)
            {
                // Check if the domain_name & type_name are the same
                if ( ( 0 == strcmp( seq1[i].domain_name.in(), seq2[j].domain_name.in() ) ) &&
                     ( 0 == strcmp( seq1[i].type_name.in(), seq2[j].type_name.in() ) ) )
                {
                    match = true;
                    break;
                }
            }

            if( !match )
            {
                return false;
            }
        }

        // If we get here, we must have a match
        return true;
    }

    SubscriptionFilter* TypeNameDispatcher::findMatchingFilter( SubscriptionFilter* filter ) const
    {
        CosNotification::EventTypeSeq seq = filter->getFilterCriteria().event_types;
        for ( unsigned int i = 0; i < seq.length() ; i++ )
        {
            if ( 0 == strcmp( seq[i].type_name.in(), WILDCARD_STR ) )
            {
                // Check subscribers in the star list
                for ( StarList::const_iterator it = m_starSubscribers.begin(); it != m_starSubscribers.end(); ++it )
                {
                    if ( contained_in((*it).m_subFilter->getFilterCriteria().event_types, seq) )
                    {
                        return ( *it ).m_subFilter;
                    }
                }

                // if we get here we have gone through all the stars, no exact match
                return NULL;
            }
            else
            {
                typedef TypeNameToSubscriptionFilterMap::const_iterator IterType;
                std::pair<IterType, IterType> bounds = m_typeNameSubscribers.equal_range(seq[i].type_name.in());
                for ( IterType iter = bounds.first; iter != bounds.second; ++iter )
                {
                    if ( contained_in(iter->second.m_subFilter->getFilterCriteria().event_types, seq) )
                    {
                        return iter->second.m_subFilter;
                    }
                }
            }
        }

        // If we get here we do not have a full match
        return NULL;
    }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string getDomainNameHACK(const CosNotifyFilter::ConstraintExp& filter)
    {
        // TODO LPP: This is a suspect way of working out the domain name - it only works
        // if all of the event types have the same domain_name.  It is probably done this
        // way because the TypeNameDispatcher also assumes that all event types for a
        // particular constraint have the same domain_name.

        #ifdef _DEBUG
            const CosNotification::EventTypeSeq& seq = filter.event_types;
            for ( unsigned int i = 0; i < seq.length() ; i++ )
            {
                if (strcmp( seq[i].domain_name.in(), seq[0].domain_name.in() ) != 0)
                {
                    LOG3( SourceInfo, DebugUtil::DebugInfo,
                        "getDomainNameHACK domain_name mismatch: seq[%d]=%s, seq[0]=%s", i, seq[i].domain_name.in(), seq[0].domain_name.in() );
                    TA_Assert(0);
                }
            }
        #endif

        return filter.event_types[0].domain_name.in();
    }

    void DomainNameDispatcher::addSubscriber( IEventHandler* handler, SubscriptionFilter* filter )
    {
        // Check to see if the context is a star.
        if ( 0 == getDomainNameHACK(filter->getFilterCriteria()).compare( WILDCARD_STR ) )
        {
            // Add it to the star container
            m_starDispatcher.addSubscriber( handler, filter );
        }
        else
        {
            // add it to the domain list
            m_domainDispatchers.insert(
                DomainNameToDispatcherMap::value_type( getDomainNameHACK(filter->getFilterCriteria()),
                    TypeNameDispatcher() ) ).first->second.addSubscriber( handler, filter );
        }
    }


    void DomainNameDispatcher::removeSubscriber( SubscriptionFilter* filter )
    {
        // TODO LPP: What if 'filter' wasn't found in any of the data structures?
        // Should we assert?

        // Check to see if the context is a star.
        if ( 0 == getDomainNameHACK(filter->getFilterCriteria()).compare( WILDCARD_STR ) )
        {
            // Remove it from the star container
            m_starDispatcher.removeSubscriber( filter );
        }
        else
        {
            // Remove all instances in the type map
            DomainNameToDispatcherMap::iterator domIt = m_domainDispatchers.find( getDomainNameHACK(filter->getFilterCriteria()) );
            if ( domIt != m_domainDispatchers.end() )
            {
                domIt->second.removeSubscriber( filter );
            }
        }
    }


    void DomainNameDispatcher::dispatchMessage( const CosNotification::StructuredEvent& event ) const
    {
        // Dispatch to the * domain as well, they want to receive all messages in the context
        m_starDispatcher.dispatchMessage(event);

        // Get the actual domain
        DomainNameToDispatcherMap::const_iterator domIt = m_domainDispatchers.find( event.header.fixed_header.event_type.domain_name.in() );
        if ( domIt != m_domainDispatchers.end() )
        {
            domIt->second.dispatchMessage(event);
        }
    }


    SubscriptionFilter* DomainNameDispatcher::findMatchingFilter( SubscriptionFilter* filter ) const
    {
        // See if we have a perfect match in the star container

        SubscriptionFilter* matchingFilter = m_starDispatcher.findMatchingFilter(filter);
        if ( NULL == matchingFilter )
        {
            // See if we have a match in the correct domain map
            // if it does not exist in the domain map it is not an exact match
            DomainNameToDispatcherMap::const_iterator it = m_domainDispatchers.find( getDomainNameHACK(filter->getFilterCriteria()) );
            if ( it != m_domainDispatchers.end() )
            {
                matchingFilter = it->second.findMatchingFilter(filter);
            }
        }

        return matchingFilter;
    }


}; // TA_Base_Core


