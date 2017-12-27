#if !defined(StructuredEventConsumerDispatchers_D7D3B769_3E7E_4621_9507_B6CF420E7E69__INCLUDED_)
#define StructuredEventConsumerDispatchers_D7D3B769_3E7E_4621_9507_B6CF420E7E69__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/StructuredEventConsumerDispatchers.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * The StructuredEventConsumer is used to receive events from an event channel.
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <string>
#include <map>
#include <list>

namespace CosNotification
{
    struct StructuredEvent;
}

// PRIVATE_TO_PACKAGE

namespace TA_Base_Core
{
    class SubscriptionFilter;
    struct IEventHandler;

	/**
    * TypeNameDispatcher keeps track of the type_names of events that subscribers are 
    * interested in receiving, and allows dispatching of events to the appropriate handlers.
    * Note that the domain_name is not considered by TypeNameDispatcher.  It is REQUIRED 
    * that the event types for all subscriptions added to a particular instance are 
    * for the SAME domain_name.
    */

    // TODO LPP: Requiring the subscriptions to be for the same domain name seems to be
    // a major limitation.  It would be reasonable to allow a given subscription to
    // be interested in a set of event types such as (dom1, type1), (dom1, type2), 
    // (dom2, type3).  However, the TypeNameDispatcher would currently dispatch all of 
    // these event types regardless of the message's domain.

	class TypeNameDispatcher
	{
	public:
		TypeNameDispatcher( ){}
		~TypeNameDispatcher();		

		/** 
			* findMatchingFilter
			*
			* See if a subscriber can share a subscription with
			* other subscribers in this container
			*
			* @param SubscriptionFilter*    filter to match
			* @return SubscriptionFilter* 	the first matching filter, or NULL if none matches.
			*/

        // TODO LPP: What does this method actually do?  Is it possible for more than one
        // filter to match (in fact, would we expect all of the filters to match if one 
        // does?).  Is findMatchingFilter an appropriate name (previously exactMatchConstraint)?
		SubscriptionFilter* findMatchingFilter( SubscriptionFilter* filter ) const;

		/** 
			* addSubscriber
			*
			* Add a subscriber who is interested in particular event types, of the form
			* [domain_name,*] or [domain_name,type_name].  Entries will be added to
            * m_starSubscribers or m_types for every event type the subscription is
            * interested in.  
            * NOTE that the domain_name is not considered by addSubscriber().  It is required
            * that ALL event types for ALL filters added to a TypeNameDispatcher have the 
            * same domain_name.
			*
			* @param SubscriptionFilter* filter
			*/
		void addSubscriber( IEventHandler* handler, SubscriptionFilter* filter );

		/** 
			* removeSubscriber
			*
			* Remove a subscriber that was previously added using addSubscriber().
			*
			* @param SubscriptionFilter* filter
			*/
		void removeSubscriber( SubscriptionFilter* filter );

		/** 
			* dispatchMessage
			*
			* Dispatch an event to one or more subscribers based on the event type name.  
            * For example, if type_name is 'abc', IEventHandler::onEventsReceived()
            * will be called on all subscribers with the type_name 'abc' as one of their
            * event types, as well as all * subscribers.
			*
			* @param  CosNotification::StructuredEvent& event (including header fields)
			*/
		void dispatchMessage( const CosNotification::StructuredEvent& event ) const;

	private:
        struct SubscriptionDetail
        {
            SubscriptionDetail(IEventHandler* handler, SubscriptionFilter* subFilter)
                : m_handler(handler), m_subFilter(subFilter)
            {
            }

            IEventHandler* m_handler;
            SubscriptionFilter* m_subFilter;
        };

        // A given subscriber can be added to one or both of these data structures.  

		// Subscribers interested in the [domain_name,*] case.  These subscribers will 
        // receive ALL events.
		typedef std::list< SubscriptionDetail > StarList;
		StarList m_starSubscribers;

		// Subscribers interested in the [domain_name,type_name] case.  These subscribers
        // will only receive events that have the particular type_name.
        // type_name -> SubscriptionDetail
		typedef std::multimap< std::string, SubscriptionDetail > TypeNameToSubscriptionFilterMap;
		TypeNameToSubscriptionFilterMap m_typeNameSubscribers;
	};


	/**
	* DomainNameDispatcher dispatches events to subscribers based on the [domain_name, type_name]
    * of the subscribers and the event.  It delegates dispatching based on type_name to instances
    * of the TypeNameDispatcher class.  It will correctly match "*" domains and "*" type 
    * subscribers with incoming events.
	*/
	class DomainNameDispatcher
	{
	public:
		DomainNameDispatcher(){}
		virtual ~DomainNameDispatcher(){}

		/** 
			* findMatchingFilter
			*
			* See if a subscriber can share a subscription with
			* other subscribers in this container
			*
			* @param SubscriptionFilter*    filter to match
			* @return SubscriptionFilter* 	the first matching filter, or NULL if none matches.
			*/
		SubscriptionFilter* findMatchingFilter( SubscriptionFilter* filter ) const;

		/** 
			* addSubscriber
			*
			* Add a subscriber who is interested in particular event types of the form
            * [ d, t ], [ d, * ], or [ *, * ].
            * NOTE that all of the event types for a given filter that is added to a 
            * DomainNameDispatcher must have the same domain_name d (or *).
			*
			* @param SubscriptionFilter* filter
			*/
		void addSubscriber( IEventHandler* handler, SubscriptionFilter* filter );

		/** 
			* removeSubscriber
			*
			* Remove a subscriber that was previously added using addSubscriber().
			*
			* @param SubscriptionFilter* filter
			*/
		void removeSubscriber( SubscriptionFilter* filter );

		/** 
			* dispatchMessage
			*
			* Dispatch a message to one or more subscribers based on
			* the event type/domain
			*
			* @param  CosNotification::StructuredEvent& event (including header fields)
			*/
		void dispatchMessage( const CosNotification::StructuredEvent& event ) const;

	private:
 		// Subscribers interested in the [*,?] case.  These subscribers will 
        // receive ALL events.
		TypeNameDispatcher m_starDispatcher;

		// Subscribers interested in the [domain_name,?] case.  These subscribers
        // will only receive events that have the particular domain_name.
		// domain_name -> TypeNameDispatcher
		typedef std::map<std::string, TypeNameDispatcher > DomainNameToDispatcherMap;
		DomainNameToDispatcherMap m_domainDispatchers;

	};

}; // TA_Base_Core

#endif // !defined(StructuredEventConsumerDispatchers_D7D3B769_3E7E_4621_9507_B6CF420E7E69__INCLUDED_)
