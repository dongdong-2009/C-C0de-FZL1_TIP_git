/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_NEW/TA_BASE/transactive/app/notification_service/src/Federator.cpp $
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

#include "app/notification_service/src/Federator.h"

#include "core/corba/src/CorbaUtil.h"
#include "core/message/src/ConnectionChecker.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/TA_String.h"

#include <sstream>


namespace TA_Base_App
{
    Federator::Federator( const std::string& consumerServiceAddr,
                          unsigned long consumerLocationKey,
                          const std::string& supplierServiceAddr,
                          unsigned long supplierLocationKey,
                          const std::string& channelName )
        : IChannelObserver( channelName, supplierLocationKey ),
          m_consumerServiceAddr(consumerServiceAddr),
          m_consumerLocationKey(consumerLocationKey),
          m_supplierServiceAddr(supplierServiceAddr),
          m_channelLocator(TA_Base_Core::IChannelLocator::_nil())
    {
        FUNCTION_ENTRY( "Federator" );

        LOG6( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Federator::Federator(): %s created for federating consumer (%s, %d) to supplier (%s, %d) on channel %s",
            TA_Base_Core::gPtrToStr(this).c_str(),
            consumerServiceAddr.c_str(), consumerLocationKey,
            supplierServiceAddr.c_str(), supplierLocationKey,
            channelName.c_str() );

        // TODO LPP: We add the connection here, but we never remove it.  Is this a problem?
        TA_Base_Core::ChannelLocatorConnectionMgr::getInstance().addConnection( m_supplierServiceAddr, supplierLocationKey );

        // We need to monitor the availability of m_supplierServiceAddr, but to do this
        // we must subscribe to (channelName, supplierLocationKey).
        TA_Base_Core::ChannelLocatorConnectionMgr::getInstance().attach( this );

        FUNCTION_EXIT;
    }


    Federator::~Federator()
    {
        FUNCTION_ENTRY( "~Federator" );

        TA_Base_Core::ChannelLocatorConnectionMgr::getInstance().detach( this );

        if ( !CORBA::is_nil( m_channelLocator ) )
        {
            LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Federator::~Federator(): %s calling unfederateWithRemoteIChannelLocator()", TA_Base_Core::gPtrToStr(this).c_str() );

            try
            {
                m_channelLocator->unfederateWithRemoteIChannelLocator( m_consumerLocationKey, getChannelName().c_str() );
            }
            catch ( const TA_Base_Core::FederationFailed& fex )
            {
                LOG_EXCEPTION_CATCH( SourceInfo,
                    "Federator::~Federator(): FederationFailed - calling unfederateWithRemoteIChannelLocator()",
                    TA_Base_Core::CorbaUtil::getInstance().exceptionToString( fex ).c_str() );
            }
            catch ( const CORBA::Exception& cex )
            {
                LOG_EXCEPTION_CATCH( SourceInfo,
                    "Federator::~Federator(): CORBA::Exception - calling unfederateWithRemoteIChannelLocator()",
                    TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ).c_str() );
            }
        }

        FUNCTION_EXIT;
    }


    void Federator::onChannelUnavailable( const std::string& serviceAddr )
    {
        FUNCTION_ENTRY( "onChannelUnavailable" );

        if ( serviceAddr == m_supplierServiceAddr )
        {
            // Note that we may never have validated successfully in onChannelAvailable().

            LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Federator::onChannelUnavailable(): %s setting IChannelLocator reference to nil",
                TA_Base_Core::gPtrToStr(this).c_str() );

            m_channelLocator = TA_Base_Core::IChannelLocator::_nil();
        }

        FUNCTION_EXIT;
    }


    bool Federator::onChannelAvailable( const std::string& serviceAddr,
                                        const CosNotifyChannelAdmin::EventChannel_ptr channel,
                                        const TA_Base_Core::IChannelLocator_ptr channelLocator )
    {
        FUNCTION_ENTRY( "onChannelAvailable" );

        bool result = true;     // by default we don't want onChannelAvailable() to be retried
        if ( serviceAddr == m_supplierServiceAddr )
        {
            TA_Assert(CORBA::is_nil(m_channelLocator));
            LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Federator::onChannelAvailable(): %s calling federateWithRemoteIChannelLocator()",
                TA_Base_Core::gPtrToStr(this).c_str() );

            try
            {
                channelLocator->federateWithRemoteIChannelLocator( m_consumerServiceAddr.c_str(),
                                                                   m_consumerLocationKey,
                                                                   getChannelName().c_str() );

                m_channelLocator = TA_Base_Core::IChannelLocator::_duplicate( channelLocator );
            }
            catch ( const TA_Base_Core::FederationFailed& fex )
            {
                LOG_EXCEPTION_CATCH( SourceInfo,
                    "Federator::onChannelAvailable(): FederationFailed exception while calling federateWithRemoteIChannelLocator()",
                    TA_Base_Core::CorbaUtil::getInstance().exceptionToString( fex ).c_str() );

                // TODO LPP: Why don't we retry if this happens?
            }
            catch ( const CORBA::Exception& cex )
            {
                LOG_EXCEPTION_CATCH( SourceInfo,
                    "Federator::onChannelAvailable(): CORBA::Exception while calling federateWithRemoteIChannelLocator()",
                    TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ).c_str() );

                result = false;
            }
        }

        FUNCTION_EXIT;
        return result;
    }


};


