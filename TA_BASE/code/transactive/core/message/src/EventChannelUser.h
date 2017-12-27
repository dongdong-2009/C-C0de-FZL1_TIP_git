#ifndef EVENT_CHANNEL_USER_H
#define EVENT_CHANNEL_USER_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/EventChannelUser.h $
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

#include <string>
#include "core/corba/src/CorbaUtil.h"
#include "core/corba/IDL/src/IChannelLocatorCorbaDef.h"

// TODO LPP: Rename this file to IChannelObserver

namespace TA_Base_Core
{
    // Interface to be implemented by clients of the ChannelLocatorConnection framework.
    // Observers will be notified when an event channel becomes available or unavailable
    // because the IChannelLocator which hosts it becomes available or unavailable.
    // The event channels themselves are not monititored for availability.

    // Note that the interface relates to a single channelName, but there can be multiple 
    // IChannelLocator instances that host this channelName.  Therefore there can be 
    // multiple EventChannel instances that the IChannelObserver is notified about, and 
    // onChannelAvailable/onChannelUnavailable can be called multiple times with different serviceAddr.

    // TODO LPP: Can location key and channel name be removed from this interface?
	class IChannelObserver
	{
	public:
        // The IChannelObserver will be notified of changes in the availability of IChannelLocator
        // channels with the name 'channelName' at the given virtual location.
		IChannelObserver( const std::string& channelName, unsigned long locationKey );

		virtual ~IChannelObserver();
        
        // Returns the name of the channel that this observer is interested in (ie. channelName) 
		std::string getChannelName() const
		{
			return m_channelName;
		}

        // Returns the locationKey of the channels that this observer is interested in.
		unsigned long getLocationKey() const
		{
			return m_locationKey;
		}

        // INTERFACE TO BE IMPLEMENTED BY CLIENTS

		// onChannelAvailable() is called when an IChannelLocator's availability changes from
        // unavailable (invalid) to available (all EventChannels hosted by that channel
        // locator will become available).
        // It will subsequently be called periodically if the IChannelLocator is available, until
        // true is returned.

        // 'serviceAddr' is the "host:port" identifying the channel locator that has become 
        // available.  'channel' is a Corba object reference to the EventChannel, and 
        // 'channelLocator' is the IChannelLocator that is hosting this channel.
        // Exceptions must NOT be thrown.
		virtual bool onChannelAvailable( const std::string& serviceAddr,
                                         const CosNotifyChannelAdmin::EventChannel_ptr channel,
                                         const TA_Base_Core::IChannelLocator_ptr channelLocator ) = 0;

		// onChannelUnavailable() is called when an IChannelLocator's availability changes from
		// available to unavailable.  Note that this method will be called on clients regardless
		// of the return value of onChannelAvailable().
        // 'serviceAddr' is the "host:port" identifying the channel locator that has become invalid.
        // Exceptions must NOT be thrown.
		virtual void onChannelUnavailable( const std::string& serviceAddr ) = 0;

	private:
		std::string m_channelName;
		unsigned long m_locationKey;
	};

}; // TA_Base_Core
#endif // !EVENT_CHANNEL_USER_H
