/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/TransactiveMessageSubscription.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Helper functions for creating subscriptions to TransActive events.
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/message/src/TransactiveMessageSubscription.h"
#include "core/message/src/MessageConfig.h"
#include "core/message/src/TA_CosUtility.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/TA_String.h"


namespace TA_Base_Core
{
    // The members in the CosNotification::_EventType structure are used throughout the messaging
    // layer as follows:
    // domain_name - message context
    // type_name   - message type


    std::vector<std::string> gPrefixEventTypeElementsExceptStar( const std::string& prefix,
                                                                 const std::vector<std::string>& eventTypes )
    {
        std::vector<std::string> vec;

        if (prefix.find(WILDCARD_STR) != std::string::npos)
        {
            vec.push_back(WILDCARD_STR);
        }
        else
        {
            vec.reserve(eventTypes.size());

            for (unsigned int i = 0; i < eventTypes.size(); ++i)
            {
                if (eventTypes[i].find(WILDCARD_STR) != std::string::npos)
                {
                    // If any of the elements contains the wildcard character, we return
                    // a single wildcard character (because the wildcard matches all
                    // event types anyway).
                    vec.clear();
                    vec.push_back(WILDCARD_STR);
                    break;
                }

                vec.push_back(prefix + eventTypes[i]);
            }
        }

        return vec;
    }


    /*gives*/ SubscriptionFilter* gCreateSubscriptionFilter( const std::string& contextName,
                                                             const std::string& typeKeyPrefix,
                                                             const std::vector<std::string>& typeKeys,
                                                             const FilterData* filterableData /*= NULL*/,
                                                             IMessageFilter* clientSideFilter /*= NULL*/ )
    {
        TA_ASSERT( !contextName.empty(), "Supplied contextName was empty" );

        std::string prefix;
        if (typeKeyPrefix.size() > 0)
        {
            prefix = typeKeyPrefix + TYPE_NAME_SEPARATOR;
        }

        std::vector<std::string> prefixedTypeKeys = gPrefixEventTypeElementsExceptStar(prefix, typeKeys);

        CosNotification::EventTypeSeq evtTypeSeq;
        gAppendToEventTypeSeq(evtTypeSeq, contextName, prefixedTypeKeys);

        std::string constraint;
        constraint = gGenerateConstraintExpression(filterableData);

        // Create a new SubscriptionFilter object
        SubscriptionFilter* subFilter = new SubscriptionFilter( evtTypeSeq,
                                                                constraint,
                                                                clientSideFilter );

        return subFilter;
    }


    namespace
    {
        TA_Base_Core::NonReEntrantThreadLockable s_subscriptionMgrLock;
        SubscriptionManager* s_subscriptionMgr = NULL;
    }

    SubscriptionManager& gGetSubscriptionManager()
    {
        TA_THREADGUARD( s_subscriptionMgrLock );

        if ( NULL == s_subscriptionMgr )
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                "gGetSubscriptionManager(): Creating SubscriptionManager" );

            s_subscriptionMgr = new SubscriptionManager();
        }

        return *s_subscriptionMgr;
    }

    void gDestroySubscriptionManager()
    {
        // TODO LPP: Do we need to use s_subscriptionMgrLock here?

        // Clean up the message config libraries
        MessageTypeTableAccessor::destroyInstance();

        // TODO LPP: The contract on gDestroySubscriptionManager() *should* require that
        // gGetSubscriptionManager() has already been called - it makes sense that we
        // shouldn't call a shutdown function if we haven't called the startup function.
        // However, various clients (such as GenericGUI) currently don't work this way.
        // So we won't enforce this requirement.
        // TA_Assert(s_subscriptionMgr != NULL);
        if (s_subscriptionMgr != NULL)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                "gDestroySubscriptionManager(): Deleting SubscriptionManager" );

            s_subscriptionMgr->cleanUpConsumers();
            delete s_subscriptionMgr;
            s_subscriptionMgr = NULL;
        }
    }

}; // TA_Base_Core
