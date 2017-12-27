#ifndef GATEWAY_H
#define GATEWAY_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_NEW/TA_BASE/transactive/app/notification_service/src/Gateway.h $
  * @author:  Allen Irwin
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/12/17 15:24:44 $
  * Last modified by:  $Author: shurui.liu $
  * 
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/corba/IDL/src/IFederatableEventChannel.hh"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include <map>
#include <string>
#include <utility>


namespace TA_Base_App
{
    class FederationForwarder;

    // The FederationForwarderMgr is used by supplier-side IChannelLocators to connect
    // the supplier to consumer-side IChannelLocators.  Each (channelName, consumerLocationKey)
    // pair will have it's own FederationForwarder - note that this may correspond to more
    // than one consumer-side IChannelLocator.
    class FederationForwarderMgr
    {
    public:
        // Destroys all of the FederationForwarders - these might unfederate in their destructors.
        ~FederationForwarderMgr();

        // federateWithConsumers() establishes federation with consumer IChannelLocators.
        // It will create a FederationForwarder for connecting 'supplierChannel' to consumers
        // that have the given (channelName, consumerLocationKey) if none already exists,
        // and increment it's ref count.
        // Note that the caller must already have added the consumer-side IChannelLocator
        // instances to the ChannelLocatorConnectionMgr using addConnection().
        void federateWithConsumers( const std::string& channelName,
                                    unsigned long consumerLocationKey,
                                    TA_Base_Core::IFederatableEventChannel_ptr supplierChannel );

        // unfederateWithConsumers() decrements the ref count for the FederationForwarder
        // with the given (channelName, consumerLocationKey).
        void unfederateWithConsumers( const std::string& channelName,
                                      unsigned long consumerLocationKey );

    private:
        // channelName, consumerLocationKey
        typedef std::pair<std::string, unsigned long> FederationConsumerKey;
        typedef std::map<FederationConsumerKey, FederationForwarder*> FederationForwarders;

        FederationForwarders m_forwarders;
        TA_Base_Core::NonReEntrantThreadLockable m_forwardersLock;    // lock for m_forwarders map.
    };

};

#endif
