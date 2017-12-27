#ifndef FEDERATOR_H
#define FEDERATOR_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_NEW/TA_BASE/transactive/app/notification_service/src/Federator.h $
  * @author:  Allen Irwin
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/12/17 15:24:44 $
  * Last modified by:  $Author: shurui.liu $
  * 
  * 
  * Federator is responsible for establishing federation from the consumer-side to 
  * a supplier-side IChannelLocator.
  */


#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/message/src/EventChannelUser.h"

#include <string>


namespace TA_Base_App
{
    // The Federator is used by the consumer-side NS.  It manages a federated connection
    // to a specific supplier NS (supplierServiceAddr) by calling federateWithRemoteIChannelLocator()
    // on the supplier-side IChannelLocator when it becomes available.  We subscribe to the
    // ChannelLocatorConnectionManager so we receive availability notifications.
    class Federator : public TA_Base_Core::IChannelObserver
    {
    public:
        // (supplierServiceAddr, supplierLocationKey) is added to the set of connections
        // that the ChannelLocatorConnectionMgr monitors.
        // We subscribe to (channelName, supplierLocationKey), and filter these notifications
        // for the specific supplierServiceAddr we are interested in.
        // (consumerServiceAddr, consumerLocationKey) are passed to the supplier-side IChannelLocator.
        Federator( const std::string& consumerServiceAddr,
                   unsigned long consumerLocationKey,
                   const std::string& supplierServiceAddr,
                   unsigned long supplierLocationKey,
                   const std::string& channelName );

        // Unfederates from the supplier-side IChannelLocator if federation is established.
        virtual ~Federator();

        // Implementation of IChannelObserver
        // (we will receive notifications for (channelName, supplierLocationKey) )
        virtual void onChannelUnavailable( const std::string& serviceAddr );
        virtual bool onChannelAvailable( const std::string& serviceAddr,
                                         const CosNotifyChannelAdmin::EventChannel_ptr channel,
                                         const TA_Base_Core::IChannelLocator_ptr channelLocator );

    private:
        // The consumer's serviceAddr and locationKey are passed to the supplier-side
        // IChannelLocator when federateWithRemoteIChannelLocator is called.
        std::string m_consumerServiceAddr;
        unsigned long m_consumerLocationKey;

        // The serviceAddr of the supplier that we are interested in federating with
        // (we need to filter IChannelObserver notifications because the subscription
        // is for a (channelName, locationKey) rather than a specific serviceAddr).
        std::string m_supplierServiceAddr;

        // Keeps track of the supplier-side IChannelLocator that we are currently
        // federating with so that we can unfederate in our destructor.
        TA_Base_Core::IChannelLocator_var m_channelLocator;
    };

};

#endif
