/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_NEW/TA_BASE/transactive/app/notification_service/src/Gateway.cpp $
  * @author:  Allen Irwin
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/12/17 15:24:44 $
  * Last modified by:  $Author: shurui.liu $
  * 
  * 
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "app/notification_service/src/Gateway.h"
#include "app/notification_service/src/GatewayConsumer.h"

#include "core/corba/src/CorbaUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include <sstream>


namespace TA_Base_App
{
    FederationForwarderMgr::~FederationForwarderMgr()
    {
        FUNCTION_ENTRY( "~FederationForwarderMgr" );

        LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "FederationForwarderMgr::~FederationForwarderMgr(): deleting %d FederationForwarders",
            m_forwarders.size() );

        for ( FederationForwarders::iterator iter = m_forwarders.begin(); iter != m_forwarders.end(); ++iter )
        {
            // The FederationForwarder dtor will disconnect federation if necessary.
            delete iter->second;
            iter->second = NULL;
        }

        FUNCTION_EXIT;
    }


    void FederationForwarderMgr::federateWithConsumers( const std::string& channelName,
                                                        unsigned long consumerLocationKey,
                                                        TA_Base_Core::IFederatableEventChannel_ptr supplierChannel )
    {
        FUNCTION_ENTRY( "federateWithConsumers" );

        std::stringstream ss;
        ss << "FederationForwarderMgr::federateWithConsumers(): federating with consumerLocationKey " << consumerLocationKey
           << ", channelName " << channelName;
        LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, ss.str().c_str());

        TA_THREADGUARD( m_forwardersLock );

        FederationForwarder* forwarder = NULL;

        // Check if we already have a FederationForwarder created for this (channelName, consumerLocationKey).
        FederationConsumerKey key( channelName, consumerLocationKey );
        FederationForwarders::iterator iter = m_forwarders.find(key);
        if ( iter == m_forwarders.end() )
        {
            forwarder = new FederationForwarder( channelName, consumerLocationKey, supplierChannel );
            m_forwarders[key] = forwarder;
        }
        else
        {
            forwarder = iter->second;
        }

        TA_Assert(forwarder != NULL);
        forwarder->ref();

        FUNCTION_EXIT;
    }


    void FederationForwarderMgr::unfederateWithConsumers( const std::string& channelName,
                                                          unsigned long consumerLocationKey )
    {
        FUNCTION_ENTRY( "unfederateWithConsumers" );

        std::stringstream ss;
        ss << "FederationForwarderMgr::unfederateWithConsumers(): unfederating with consumerLocationKey " << consumerLocationKey
           << ", channelName " << channelName;
        LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, ss.str().c_str());

        TA_THREADGUARD( m_forwardersLock );

        FederationConsumerKey key( channelName, consumerLocationKey );
        FederationForwarders::iterator iter = m_forwarders.find(key);
        if ( iter != m_forwarders.end() )
        {
            iter->second->unref();
        }
        else
        {
            LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "FederationForwarderMgr::unfederateWithConsumers(): unable to unfederate as federation was not established" );
        }

        FUNCTION_EXIT;
    }


};


