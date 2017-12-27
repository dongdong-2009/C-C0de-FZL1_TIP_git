
#if !defined(TransactiveMessageSubscription_F0FBFCD4_1728_49e8_8FF2_9647110C3E62__INCLUDED_)
#define TransactiveMessageSubscription_F0FBFCD4_1728_49e8_8FF2_9647110C3E62__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/TransactiveMessageSubscription.h $
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

#include "core/message/src/SubscriptionManager.h"
#include "core/message/src/NameValuePair.h"

// TODO LPP: Rename this file.  MessageSubscriptionManager doesn't exist anymore.

namespace TA_Base_Core
{
    // The EventType's type_name is sometimes used to convey more than one field. For
    // example, we might use <agentName>_<messageType> or <segmentedEntityKey>_<messageType>.
    // Unfortunately this complicates wildcard matching, where one of the fields is '*'.
    // The use of '*' as the wildcard is necessary because omniNotify apparently doesn't
    // pattern match strings containing other characters - eg. a_* will not match the 
    // string 'a_b', only 'a_*'.  Pattern matching will only be done when the pattern
    // string is '*'.  
    // Therefore <prefix>* and *<typekey> are not valid wildcards, and it is
    // necessary to use '*' in both cases.

    // Returns a vector of string elements formed from 'prefix' and 'eventTypes' as follows:
    // - if 'prefix' contains '*', OR at least one element of eventTypes contains '*', 
    //   the single element '*' will be returned ('*' will match ALL event types, so 
    //   we only need to return the single * element).
    // - otherwise, the elements are formed by <prefix><eventType>.  'prefix' can be an 
    //   empty string, but if not, by convention it should end with '_'.
    std::vector<std::string> gPrefixEventTypeElementsExceptStar( const std::string& prefix,
                                                                 const std::vector<std::string>& eventTypes );


    // Returns the global subscription manager (a singleton instance)
    SubscriptionManager& gGetSubscriptionManager();

    // Destroys the global subscription manager, which calls SubscriptionManager::cleanUpConsumers().
    // This should only be called if gGetSubscriptionManager() was called.
    // NOTE: this function must be called sometime before attempting to shutdown CorbaUtil, 
    // otherwise the application may not be able to terminate (CorbaUtil::shutdown() seems 
    // to block waiting for ChannelLocatorConnection threads to terminate - these threads
    // only terminate when all ChannelObservers have detached).
    void gDestroySubscriptionManager();


    // NOTE: In the following methods, ownership of the SubscriptionFilter object is passed to the 
    // caller.  Of course, the filter object must not be deleted until after it has been unsubscribed.

    // Creates and populates a SubscriptionFilter object suitable for subscribing to all
    // message types except for comms messages. 
    // contextName      - the domain_name of an event has to match the contextName.
    // typeKeyPrefix    - string that will be prepended to each type_name in typeKeys.  If the
    //                    prefix isn't an empty string, '_' will be appended.
    // typeKeys         - the type_name of an event has to match one of those in this vector.
    // filterableData   - std::vector<NameValuePair>, where the 'name' identifies a field in
    //                    the variable header or filterable data header.
    // clientSideFilter - optional client-side filter
    /*gives*/ SubscriptionFilter* gCreateSubscriptionFilter( const std::string& contextName,
                                                             const std::string& typeKeyPrefix,
                                                             const std::vector<std::string>& typeKeys,
                                                             const FilterData* filterableData = NULL,
                                                             IMessageFilter* clientSideFilter = NULL );

}; // TA_Base_Core

#endif // !defined(TransactiveMessageSubscription_F0FBFCD4_1728_49e8_8FF2_9647110C3E62__INCLUDED_)
