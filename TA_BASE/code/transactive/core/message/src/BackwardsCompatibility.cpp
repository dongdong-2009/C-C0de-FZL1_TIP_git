/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/BackwardsCompatibility.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  * This class maintains a map of the channels that are being listened on for
  * events, and allows applications to register based on an event type.
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/message/src/BackwardsCompatibility.h"
#include "core/message/types/MessageTypes.h"
#include "core/message/src/TransactiveMessageSubscription.h"
#include "core/message/src/MessageConstants.h"
#include "core/message/src/MessageFilters.h"
#include "core/message/src/TA_CosUtility.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/TA_String.h"


namespace TA_Base_Core
{

    //xinsong ++
    const unsigned int OCC_LOCATION = 1; //used for subscribe configupdate message, may need to get data from db

    // Creates and populates a SubscriptionFilter object suitable for subscribing to
    // comms messages.
    // If entityKey == ANY_ENTITY, the subscription will be for (contextName, *), and the locationKey
    // and typeName (of the 0th event type) will be added to the server-side filter.
    // If entityKey != ANY_ENTITY, the locationKey and typeName ARE NOT added to the server-side filter,
    // but a CommsMessageFilter is created as a client-side filter.

    // TODO LPP: Should LOCAL_LOCATION and ANY_LOCATION be allowed as the locationKey?
    // TODO LPP: Why is the locationKey not used in the filter if the entityKey != 0?
    // Is it because it is assumed that an entity only exists at a single location?  Why
    // go to the trouble of actually implementing this special case?

    /*gives*/ SubscriptionFilter* createSubscriptionFilterForComms( const std::string& contextName,
                                                                    const std::vector<std::string>& typeKeys,
                                                                    unsigned long entityKey,
                                                                    unsigned long locationKey,
                                                                    const FilterData* filterableData )
    {
        // TODO LPP: We should probably translate LOCAL_LOCATION to gGetLocalLocationKey()
        // and ANY_LOCATION should disable the region filter.
        TA_Assert(locationKey != LOCAL_LOCATION && locationKey != ANY_LOCATION);

        FilterData filterData;

        TA_Assert(typeKeys.size() > 0);
        NameValuePair typeFilter(MESSAGETYPE_FILTER_NAME, typeKeys[0]);
        NameValuePair regionFilter(REGION_FILTER_NAME, gStringFromUnsignedLong( locationKey ));

        if ( ANY_ENTITY == entityKey )
        {
            // If the entitykey is not specified we need to constrain the locationkey
            filterData.push_back(&regionFilter);

            // If there is exactly one type, we are able use a field in the message header to
            // constrain the type (since type_name will be '*').
            if ( 1 == typeKeys.size() )
            {
                filterData.push_back(&typeFilter);
            }
        }

        if (filterableData != NULL)
        {
            filterData.insert(filterData.end(), filterableData->begin(), filterableData->end());
        }

        // The server-side filter uses the so-called 'segmented' entity key for filtering.
        // A client-side filter is used to eliminate all messages except those with the exact entity key.
        return gCreateSubscriptionFilter( contextName,
                                          entityKey == ANY_ENTITY ? WILDCARD_STR : gGetSegmentedEntityKey(entityKey),
                                          typeKeys,
                                          &filterData,
                                          entityKey == ANY_ENTITY ? NULL : new CommsMessageFilter(entityKey) );
    }


    void MessageSubscriptionManager::subscribeToCommsMessage( const MessageContext& context,
                                                              AbstractMessageSubscriber* subscriber,
                                                              unsigned long entityKey,
                                                              unsigned long subsystemKey,
                                                              unsigned long locationKey,
                                                              const FilterData* filterableData /*= NULL*/ )
    {
        TA_Assert(subscriber != NULL);

        SubscriptionFilter* sf = createSubscriptionFilterForComms( context.getContextName(),
                                                                   context.getTypes(),
                                                                   entityKey,
                                                                   locationKey,
                                                                   filterableData );

        subscriber->m_subscriptions.addSubscription( &gGetSubscriptionManager(),
                                                     context.getChannelName(),
                                                     sf,
                                                     locationKey,
                                                     subscriber );
    }


    void MessageSubscriptionManager::subscribeToCommsMessage( const MessageType& messageType,
                                                              AbstractMessageSubscriber* subscriber,
                                                              unsigned long entityKey,
                                                              unsigned long subsystemKey,
                                                              unsigned long locationKey,
                                                              const FilterData* filterableData /*= NULL*/ )
    {
        TA_Assert(subscriber != NULL);

        std::vector<std::string> typeKeys;
        typeKeys.push_back(messageType.getTypeKey());

        SubscriptionFilter* sf = createSubscriptionFilterForComms( messageType.getContextObj().getContextName(),
                                                                   typeKeys,
                                                                   entityKey,
                                                                   locationKey,
                                                                   filterableData );

        subscriber->m_subscriptions.addSubscription( &gGetSubscriptionManager(),
                                                     messageType.getContextObj().getChannelName(),
                                                     sf,
                                                     locationKey,
                                                     subscriber );
    }


    void MessageSubscriptionManager::subscribeToBroadcastCommsMessage( const MessageContext& context,
                                                                       AbstractMessageSubscriber* subscriber,
                                                                       const FilterData* filterableData /*= NULL*/ )
    {
        TA_Assert(subscriber != NULL);

        SubscriptionFilter* sf = gCreateSubscriptionFilter( context.getContextName(), "", context.getTypes(), filterableData);

        subscriber->m_subscriptions.addSubscription( &gGetSubscriptionManager(), context.getChannelName(), sf, ANY_LOCATION, subscriber);
    }


    void MessageSubscriptionManager::subscribeToBroadcastCommsMessage( const MessageType& messageType,
                                                                       AbstractMessageSubscriber* subscriber,
                                                                       const FilterData* filterableData /*= NULL*/ )
    {
        TA_Assert(subscriber != NULL);

        std::vector<std::string> typeKeys;
        typeKeys.push_back(messageType.getTypeKey());

        SubscriptionFilter* sf = gCreateSubscriptionFilter( messageType.getContextObj().getContextName(), "", typeKeys, filterableData );

        subscriber->m_subscriptions.addSubscription( &gGetSubscriptionManager(),
                                                     messageType.getContextObj().getChannelName(),
                                                     sf,
                                                     ANY_LOCATION,
                                                     subscriber);
    }


    void MessageSubscriptionManager::subscribeToAuditMessage( const MessageContext& context,
                                                              AbstractMessageSubscriber* subscriber )
    {
        TA_Assert(subscriber != NULL);
        SubscriptionFilter* sf = gCreateSubscriptionFilter( context.getContextName(), "", context.getTypes() );

        subscriber->m_subscriptions.addSubscription( &gGetSubscriptionManager(), context.getChannelName(), sf, LOCAL_LOCATION, subscriber );
    }


    void MessageSubscriptionManager::subscribeToAuditMessage( const MessageType& messageType,
                                                              AbstractMessageSubscriber* subscriber )
    {
        TA_Assert(subscriber != NULL);

        std::vector<std::string> typeKeys;
        typeKeys.push_back(messageType.getTypeKey());

        SubscriptionFilter* sf = gCreateSubscriptionFilter( messageType.getContextObj().getContextName(), "", typeKeys );

        subscriber->m_subscriptions.addSubscription( &gGetSubscriptionManager(),
                                                     messageType.getContextObj().getChannelName(),
                                                     sf,
                                                     LOCAL_LOCATION,
                                                     subscriber );
    }


    void MessageSubscriptionManager::subscribeToConfigUpdateMessage( const MessageContext& context,
                                                                     AbstractMessageSubscriber* subscriber )
    {
        TA_Assert(subscriber != NULL);

        SubscriptionFilter* sf = gCreateSubscriptionFilter( context.getContextName(), "", context.getTypes() );

        subscriber->m_subscriptions.addSubscription( &gGetSubscriptionManager(), context.getChannelName(), sf, OCC_LOCATION, subscriber );
    }


    void MessageSubscriptionManager::subscribeToConfigUpdateMessage( const MessageType& messageType,
                                                                     AbstractMessageSubscriber* subscriber)
    {
        TA_Assert(subscriber != NULL);

        std::vector<std::string> typeKeys;
        typeKeys.push_back(messageType.getTypeKey());

        SubscriptionFilter* sf = gCreateSubscriptionFilter( messageType.getContextObj().getContextName(), "", typeKeys );

        subscriber->m_subscriptions.addSubscription( &gGetSubscriptionManager(),
                                                     messageType.getContextObj().getChannelName(),
                                                     sf,
                                                     OCC_LOCATION,
                                                     subscriber);
    }


    void MessageSubscriptionManager::subscribeToStateUpdateMessage( const MessageContext& context,
                                                                    AbstractMessageSubscriber* subscriber,
                                                                    const std::string& agentName,
                                                                    const FilterData* filterableData /*= NULL*/ )
    {
        TA_Assert(subscriber != NULL);

        SubscriptionFilter* sf = gCreateSubscriptionFilter( context.getContextName(), agentName, context.getTypes(), filterableData );

        subscriber->m_subscriptions.addSubscription( &gGetSubscriptionManager(), context.getChannelName(), sf, LOCAL_LOCATION, subscriber);
    }


    void MessageSubscriptionManager::subscribeToStateUpdateMessage( const MessageType& messageType,
                                                                    AbstractMessageSubscriber* subscriber,
                                                                    const std::string& agentName,
                                                                    const FilterData* filterableData /*= NULL*/ )
    {
        TA_Assert(subscriber != NULL);

        std::vector<std::string> typeKeys;
        typeKeys.push_back(messageType.getTypeKey());

        SubscriptionFilter* sf = gCreateSubscriptionFilter( messageType.getContextObj().getContextName(), agentName, typeKeys, filterableData );

        subscriber->m_subscriptions.addSubscription( &gGetSubscriptionManager(),
                                                     messageType.getContextObj().getChannelName(),
                                                     sf,
                                                     LOCAL_LOCATION,
                                                     subscriber);
    }


    void MessageSubscriptionManager::subscribeToBindingRequestMessage( const MessageType& messageType,
                                                                       const std::string& agentName,
                                                                       AbstractMessageSubscriber* subscriber )
    {
        TA_Assert(subscriber != NULL);

        std::vector<std::string> typeKeys;
        typeKeys.push_back(messageType.getTypeKey());

        SubscriptionFilter* sf = gCreateSubscriptionFilter( messageType.getContextObj().getContextName(), agentName, typeKeys );

        subscriber->m_subscriptions.addSubscription( &gGetSubscriptionManager(),
                                                     messageType.getContextObj().getChannelName(),
                                                     sf,
                                                     ANY_LOCATION,
                                                     subscriber );
    }


    void MessageSubscriptionManager::unsubscribeToMessages( AbstractMessageSubscriber* subscriber )
    {
        TA_Assert(subscriber != NULL);
        subscriber->m_subscriptions.removeAllSubscriptions();
    }

}; // TA_Base_Core
