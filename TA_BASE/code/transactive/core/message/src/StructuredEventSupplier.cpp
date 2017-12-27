/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/StructuredEventSupplier.cpp $
  * @author:  Jade Welton
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <ctime>

#include "core/message/src/StructuredEventSupplier.h"
#include "core/message/src/ConnectionChecker.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TA_String.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/message/src/TA_CosUtility.h"


using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;

namespace TA_Base_Core
{
    const int SEND_MSG_MAX_FAILED_TIMES = 5;
	
	//++RenQiang
	RedundantEventSupplier::RedundantEventSupplier()
		: m_countNoConn(0)
	{
	}
	//RenQiang--

    RedundantEventSupplier::~RedundantEventSupplier()
    {
        TA_THREADGUARD( m_connectionsLock );
        for ( ChannelConnections::iterator iter = m_connections.begin(); iter != m_connections.end(); ++iter )
        {
            TA_Assert(iter->second != NULL);
            TA_Assert(iter->second->m_channelSupplier.hasEventChannel());

            iter->second->m_channelSupplier.setSubChangeHandler( NULL ); // limin++
            iter->second->m_channelSupplier.setEventChannel(CosNotifyChannelAdmin::SupplierAdmin::_nil(), true);

            // TODO LPP: Should we call setSubChangeHandler(NULL) on the CosNotifyChannelSupplier?

            delete iter->second;
            iter->second = NULL;
        }
    }


    bool RedundantEventSupplier::publishEvents(const std::vector<TA_Base_Core::TimetagedStructuredEvent*>& batch)
    {
        TA_Assert(batch.size() > 0);

        // TODO LPP: It is not very efficient to lock the connections map while we are
        // trying to dispatch the events.  Perhaps need to make a temporary copy of
        // the CosNotifyChannelSupplier*s
        TA_THREADGUARD( m_connectionsLock );

        CosNotification::StructuredEvent * firstEvent = batch[0]->first;

        std::string firstEventDesc = gStringFromEventHeader(*firstEvent, true);

		//if no action establish, just log warning, but return true value
		//allow failed 5 times before establish event channel connection

		//xinsong TODO: what happened if failed message accumulate more and more, hwo to garbage these message?? 
		if ( m_connections.size() == 0 )
		{
			/*
			if ( m_msgSendErrorCount < SEND_MSG_MAX_FAILED_TIMES )
			{
				m_msgSendErrorCount++;
				return true;
			}
			else
			*/
			{
                LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, 
					"RedundantEventSupplier::publishEvents(): %s could not dispatching message because of no active connections, firstEventDesc is <%s>", 
					gPtrToStr(this).c_str(), firstEventDesc.c_str() );

				return false;
			}
		}


        // Connections may only be interested in a subset of the events (due to filtering).
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
            "RedundantEventSupplier::publishEvents(): %s dispatching up to %d events to %d connections, firstEventDesc is <%s>",
            gPtrToStr(this).c_str(), batch.size(), m_connections.size(), firstEventDesc.c_str() );

        // We will dispatch to all connections.  false will be returned if the
        // batch was not successfully dispatched to at least one connection.
        bool result = false;
        for ( ChannelConnections::iterator iter = m_connections.begin(); iter != m_connections.end(); ++iter )
        {
            TA_Assert(iter->second != NULL);
            TA_Assert(iter->second->m_channelSupplier.hasEventChannel());

            std::vector<const CosNotification::StructuredEvent*> filteredBatch;
            filterSubscribedEvents(batch, filteredBatch, iter->second->m_filter);

            if ( filteredBatch.size() > 0 )
            {
                // CosNotifyChannelSupplier::publishEvents() should not fail more than a few times for
                // a given connection, as we would expect to eventually receive an onChannelUnavailable() notification.

                // 'result' will be set to true if the event is dispatched to AT LEAST ONE of the channels.
                result = iter->second->m_channelSupplier.publishEvents(filteredBatch) || result;
            }
            else
            {
                // We consider the dispatch a success if the connection was not actually interested in
                // any of the events.
                // TODO LPP: Its not clear whether this is the right thing to do.
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "RedundantEventSupplier::publishEvents(): %s didn't need to dispatch any events to %s(%s) as no subscribers are interested, firstEvent is <%s>",
                    gPtrToStr(this).c_str(), gPtrToStr(&iter->second->m_channelSupplier).c_str(), iter->first.c_str(), firstEventDesc.c_str() );

                result = true;
            }
        }

        return result;
    }


    bool RedundantEventSupplier::addChannel( const std::string& serviceAddr,
                                             const CosNotifyChannelAdmin::SupplierAdmin_ptr supplierAdmin,
                                             /*takes*/ ISubChangeHandler* subChangeHandler )
    {
        TA_Assert(!CORBA::is_nil(supplierAdmin));

        TA_THREADGUARD( m_connectionsLock );

        LOG3(SourceInfo, DebugUtil::DebugMsg,
            "RedundantEventSupplier::addChannel(): %s connecting to %s.  Connecting handler %s",
            gPtrToStr(this).c_str(), serviceAddr.c_str(), gPtrToStr(subChangeHandler).c_str() );

        SupplierChannelConnection* connection = new SupplierChannelConnection();
        connection->m_fan.addHandler(&connection->m_filter);
        connection->m_subChangeHandler = std::auto_ptr<ISubChangeHandler>(subChangeHandler);
        connection->m_channelSupplier.setServiceAddr( serviceAddr.c_str() );

        if ( subChangeHandler != NULL )
        {
            connection->m_fan.addHandler(subChangeHandler);
        }

        connection->m_channelSupplier.setSubChangeHandler(&connection->m_fan);

        // Only insert the connection into m_connections if the channel is able
        // to be set successfully.
        bool result = connection->m_channelSupplier.setEventChannel(supplierAdmin, true);
        if ( result )
        {
            std::pair<ChannelConnections::iterator, bool> result =
                m_connections.insert(ChannelConnections::value_type(serviceAddr, connection) );
            TA_Assert( true == result.second );
        }
        else
        {
            delete connection;
            connection = NULL;
        }

        return result;
    }


    void RedundantEventSupplier::removeChannel( const std::string& serviceAddr )
    {
        LOG2(SourceInfo, DebugUtil::DebugMsg,
            "RedundantEventSupplier::removeChannel(): %s disconnecting from %s", gPtrToStr(this).c_str(), serviceAddr.c_str() );

        TA_THREADGUARD( m_connectionsLock );

        ChannelConnections::iterator it = m_connections.find( serviceAddr );
        if ( it != m_connections.end() )
        {
            TA_Assert(it->second != NULL);

            // TODO: should we do clean disconnect here to notify event channel?
            it->second->m_channelSupplier.setSubChangeHandler( NULL ); // limin++
            it->second->m_channelSupplier.setEventChannel(CosNotifyChannelAdmin::SupplierAdmin::_nil(), false);

            delete it->second;
            it->second = NULL;

            m_connections.erase(it);
        }

        LOG2(SourceInfo, DebugUtil::DebugMsg,
            "RedundantEventSupplier::removeChannel(): %s disconnected from %s", gPtrToStr(this).c_str(), serviceAddr.c_str() );
    }


    void RedundantEventSupplier::filterSubscribedEvents( const std::vector<TA_Base_Core::TimetagedStructuredEvent*>& batch,
                                                         std::vector<const CosNotification::StructuredEvent*>& filteredBatch,
                                                         const SubscribedEventFilter& filter )
    {
        TA_Assert(filteredBatch.empty());

        LOG3( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
            "RedundantEventSupplier::filterSubscribedEvents(): %s applying filter %s to %d events:",
            gPtrToStr(this).c_str(), gPtrToStr(&filter).c_str(), batch.size() );

        filteredBatch.reserve(batch.size());

        for ( unsigned int i = 0; i < batch.size(); ++i )
        {
            if ( batch[i]->second != 0 && batch[i]->second < static_cast<unsigned long>( time( NULL ) ) )
            {
                continue;
            }

            CosNotification::StructuredEvent& event = *(batch[i]->first);

            if ( filter.isSubscribedEvent(event) )
            {
                LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
                    "SKEEP    EVT %d: %s", i, gStringFromEventHeader(event).c_str() );

                filteredBatch.push_back(&event);
            }
            else
            {
                // Discard the event as there are no interested subscribers
                LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
                    "SDISCARD EVT %d: %s", i, gStringFromEventHeader(event).c_str() );
            }
        }
    }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SESQueueHandler::SESQueueHandler( const std::string& channelName,
                                      unsigned long locationKey,
                                      /*takes*/ IAdminProvider* adminProvider,
                                      /*takes*/ ISubChangeHandlerFactory* subChangeHandlerFactory )
        : IChannelObserver(channelName, locationKey),
          m_adminProvider(adminProvider),
          m_subChangeHandlerFactory( subChangeHandlerFactory )
    {
        LOG4( SourceInfo, DebugUtil::DebugMsg,
            "SESQueueHandler::SESQueueHandler(): Creating %s for channel %s, locationKey %d, RedundantEventSupplier %s",
            gPtrToStr(this).c_str(), channelName.c_str(), locationKey, gPtrToStr(&m_redundantSupplier).c_str() );

        TA_Assert(m_adminProvider.get() != NULL)
    }


    bool SESQueueHandler::onDispatchObjects(const std::vector<TA_Base_Core::TimetagedStructuredEvent*>& batch)
    {
        return m_redundantSupplier.publishEvents(batch);
    }


    void SESQueueHandler::onChannelUnavailable( const std::string& serviceAddr )
    {
        // Note that the channel wasn't necessarily added by onChannelAvailable().
        m_redundantSupplier.removeChannel(serviceAddr);
    }


    bool SESQueueHandler::onChannelAvailable( const std::string& serviceAddr,
                                              const CosNotifyChannelAdmin::EventChannel_ptr channel,
                                              const TA_Base_Core::IChannelLocator_ptr channelLocator )
    {
        TA_Assert(m_adminProvider.get() != NULL)

        CosNotifyChannelAdmin::SupplierAdmin_var supplierAdmin = m_adminProvider->getSupplierAdmin(channel);
        if ( !CORBA::is_nil(supplierAdmin) )
        {
            ISubChangeHandler* handler = NULL;
            if ( m_subChangeHandlerFactory.get() != NULL )
            {
                handler = m_subChangeHandlerFactory->createInstance();
            }

            return m_redundantSupplier.addChannel(serviceAddr, supplierAdmin, handler);
        }
        else
        {
            // TODO LPP: Should we retry, or abort?
            return false;
        }
    }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    StructuredEventSupplier::StructuredEventSupplier(/*takes*/ IQueueHandler<TA_Base_Core::TimetagedStructuredEvent>* handler)
        : m_handler( handler )
    {
        TA_ASSERT( RunParams::getInstance().isSet( RPARAM_NOTIFYHOSTS ), "No Notification Service hosts specified" );

        LOG2( SourceInfo, DebugUtil::DebugDebug,
            "StructuredEventSupplier::StructuredEventSupplier(): Creating %s with handler %s",
            gPtrToStr(this).c_str(), gPtrToStr(handler).c_str() );

        // TODO LPP: If more than one event is dispatched to a proxy consumer at a time,
        // proxy suppliers will dispatch events in the wrong order (on Windows at least).
        // So we are currently restricted to using a batch size of 1.  This seems to be
        // a bug in OmniNotify 2.1.
        m_queue.setBatchingProperties(
            //1,  // WORKAROUND for bug in OmniNotify 2.1
            getRunParamValue(RPARAM_BATCHSIZE, DEFAULT_BATCH_SIZE), 
            getRunParamValue(RPARAM_SUCCESSBATCHINGINTERVAL, DEFAULT_SUCCESS_BATCH_INTERVAL), 
            getRunParamValue(RPARAM_FAILEDBATCHINGINTERVAL, DEFAULT_FAILED_BATCH_INTERVAL) );

        m_queue.setQueueHandler( m_handler.get() );
        m_queue.start();
    }


    StructuredEventSupplier::~StructuredEventSupplier()
    {
        LOG0( SourceInfo, DebugUtil::DebugDebug,
            "StructuredEventSupplier::~StructuredEventSupplier(): Shutting down BatchingQueue" );

        m_queue.terminateAndWait();
    }


    void StructuredEventSupplier::publishEvents( /*takes*/ std::vector<TA_Base_Core::TimetagedStructuredEvent*>& events )
    {
        LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
            "StructuredEventSupplier::publishEvent(): %s pushing %d events onto send queue",
            gPtrToStr(this).c_str(), events.size() );

        for ( unsigned int i = 0; i < events.size(); ++i )
        {
            TA_Assert(events[i]->first != NULL);
            m_seqNumAppender.appendToHeader(*(events[i]->first));

            LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
                "SQUEUE  EVT %d: %s", i, gStringFromEventHeader(*(events[i]->first)).c_str() );
            gLogSpecificMessage(*(events[i]->first), "StructuredEventSupplier::publishEvents");
        }

        m_queue.pushObjects(events);
    }


    void StructuredEventSupplier::publishEvent( /*takes*/ CosNotification::StructuredEvent& event, unsigned long expiredTime /* =0 */ )
    {
        std::vector<TA_Base_Core::TimetagedStructuredEvent*> vec;
        TA_Base_Core::TimetagedStructuredEvent* newpair;
        newpair = new TA_Base_Core::TimetagedStructuredEvent(&event, expiredTime);
        vec.push_back(newpair);
        publishEvents(vec);
    }


}; // TA_Base_Core

