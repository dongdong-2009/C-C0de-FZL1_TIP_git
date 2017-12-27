/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/src/AlarmSubscriptionManager.cpp $
* @author:  Cameron Rochester
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2015/01/19 17:43:23 $
* Last modified by:  $Author: CM $
* 
* description
*/
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/alarm/src/AlarmSubscriptionManager.h"
#include "core/message/types/Alarm_MessageTypes.h"
#include "core/message/src/MessageConfig.h"
#include "core/message/src/BackwardsCompatibility.h"
#include "core/message/src/TransactiveMessageSubscription.h"
#include "core/message/src/MessageConstants.h"
#include "core/message/src/NameValuePair.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/TA_String.h"
#include "core/alarm/src/AlarmReceiver.h"

namespace TA_Base_Core
{
	namespace AlarmSubscriptionManager
	{
		void subscribeToEntityAlarms(
			const TA_Base_Core::MessageContext& context,
			AbstractMessageSubscriber* subscriber,
			unsigned long entityKey, 
			unsigned long sourceLocationKey)
		{
			TA_ASSERT( 0 != entityKey, "EntityKey must be specified to subscribe" );
			
			// TODO LPP: We should probably translate LOCAL_LOCATION to gGetLocalLocationKey().
			TA_Assert(sourceLocationKey != LOCAL_LOCATION && sourceLocationKey != ANY_LOCATION);
			
			SubscriptionFilter* si = gCreateSubscriptionFilter(
				context.getContextName(),
				gGetSegmentedEntityKey(entityKey),
				context.getTypes(),
				NULL,
				new AlarmsMessageFilter(entityKey) );
			
			subscriber->m_subscriptions.addSubscription(&gGetSubscriptionManager(), context.getChannelName(), si, sourceLocationKey, subscriber);
		}
		
		
		/*gives*/ SubscriptionFilter* createSubscriptionFilterForAlarmsFromLocation(
        const std::string& contextName,
			const std::vector<std::string>& typeKeys,
			unsigned long sourceLocationKey )       
		{
			// TODO LPP: We should probably translate LOCAL_LOCATION to gGetLocalLocationKey()
			// and ANY_LOCATION should disable region filtering.
			TA_Assert(sourceLocationKey != LOCAL_LOCATION && sourceLocationKey != ANY_LOCATION);
			
			FilterData filterData;
			
			TA_Assert(typeKeys.size() > 0);
			NameValuePair typeFilter(MESSAGETYPE_FILTER_NAME, typeKeys[0]);
			NameValuePair regionFilter(REGION_FILTER_NAME, gStringFromUnsignedLong( sourceLocationKey ));
			filterData.push_back(&regionFilter);
			
			// If there is exactly one type, we are able use a field in the message header to
			// constrain the type.
			// TODO LPP NOW: It's ugly having to check for the WILDCARD_STR, but its necessary 
			// since the constraint $FilterName = '*' will never match anything.  Is there
			// a cleaner approach?
			if (typeKeys.size() == 1 && typeKeys[0] != WILDCARD_STR)
			{
				filterData.push_back(&typeFilter);
			}
			
			// TODO LPP NOW: Why do we pass WILDCARD_STR?  This forces CosNotification::EventTypeSeq
			// to ALWAYS contain the single event type (contextName, *) regardless of typeKeys
			// (see gCreateSubscriptionFilter).
			return gCreateSubscriptionFilter(contextName, WILDCARD_STR, typeKeys, &filterData );
		}
		
		void subscribeToAlarmsFromLocation(
			const TA_Base_Core::MessageContext& context,
			AbstractMessageSubscriber* subscriber, 
			long sourceLocationKey)
		{
			TA_Assert(subscriber != NULL);
			SubscriptionFilter* si = createSubscriptionFilterForAlarmsFromLocation(
				context.getContextName(), context.getTypes(), sourceLocationKey );
			
			subscriber->m_subscriptions.addSubscription(&gGetSubscriptionManager(), context.getChannelName(), si, sourceLocationKey, subscriber);
		}
		
		void subscribeToAlarmsFromLocation(
			const TA_Base_Core::MessageType& messageType,
			AbstractMessageSubscriber* subscriber, 
			long sourceLocationKey)
		{
			TA_Assert(subscriber != NULL);
			
			std::vector<std::string> typeKeys;
			typeKeys.push_back(messageType.getTypeKey());
			SubscriptionFilter* si = createSubscriptionFilterForAlarmsFromLocation(
				messageType.getContextObj().getContextName(), typeKeys, sourceLocationKey);
			
			subscriber->m_subscriptions.addSubscription(&gGetSubscriptionManager(), messageType.getContextObj().getChannelName(), si, sourceLocationKey, subscriber);
		}
		
		void unsubscribeToMessages( AbstractMessageSubscriber* subscriber )
		{
			TA_Assert(subscriber != NULL);
			subscriber->m_subscriptions.removeAllSubscriptions();
		}
	}
}; // namespace TA_Base_Core


