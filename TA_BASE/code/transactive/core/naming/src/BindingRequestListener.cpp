/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/naming/src/BindingRequestListener.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/09/02 14:28:39 $
  * Last modified by:  $Author: limin.zhu $
  *
  * Listens out for BindingRequestMessages
  * It will subscribe for BindingRequestMessages.
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/naming/IDL/src/BindingRequestOriginatorCorbaDef.h"
#include "core/naming/IDL/src/RebindNameCorbaDef.h"
#include "core/naming/src/BindingRequestListener.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/Hostname.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/BindingRequestMessageSender.h"
#include "core/message/types/NamingService_MessageTypes.h"
#include "core/message/types/NameRebindStateUpdate_MessageTypes.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "core/naming/src/NamingThreadPoolSingleton.h"
#include "core/naming/src/NamingHeartbeatMonitor.h"
#include "core/naming/src/Naming.h" // yanrong++: CL-21230

namespace TA_Base_Core
{

    BindingRequestListener::BindingRequestListener( )
        : m_inControlMode(false),
          m_rebindMessageSender(NULL),
          m_timeToLive( 60 ),
          m_HeartbeatMonitor(NULL),
          m_workItem(),
          m_namingThreadPoolSingleton( NamingThreadPoolSingleton::getInstance() )
    {
        FUNCTION_ENTRY( "BindingRequestListener" );

        // yanrong++: CL-21230
        TA_ASSERT(RunParams::getInstance().isSet(RPARAM_ENTITYNAME), "unset EntityName in run param");
        std::string agentName = RunParams::getInstance().get(RPARAM_ENTITYNAME);
        std::string servantKey = Naming::getInstance().getServantKey(agentName);

        CorbaUtil::getInstance().setServantKey(this, servantKey);
        // ++yanrong
        // Activate the servant so requests can be handled
        activateServant();

        m_myObjRef = _this();

        // Store the agent name
        if ( RunParams::getInstance().isSet( RPARAM_USEAGENTNAME ) )
        {
            // if UseAgentName runparam is set use this value otherwise we use entity name as agent name.
            m_agentName = RunParams::getInstance().get( RPARAM_USEAGENTNAME );
        }
        else
        {
            m_agentName = RunParams::getInstance().get( RPARAM_ENTITYNAME );
        }

        if ( RunParams::getInstance().isSet( RPARAM_NAMINGTTL ) )
        {
            m_timeToLive = strtoul( RunParams::getInstance().get( RPARAM_NAMINGTTL ).c_str(), NULL, 10 );
        }

        TA_ASSERT( !m_agentName.empty(), "The entity name must be specified for TDNS to work correctly" );

        // Check to see if the agent is in control mode
        std::string opMode( TA_Base_Core::RunParams::getInstance().get( RPARAM_OPERATIONMODE ) );

        // Check to see if we are in control mode
        if ( 0 == opMode.compare( RPARAM_CONTROL ) )
        {
            m_inControlMode = true;

        }
        if ( m_inControlMode || 0 == opMode.compare( RPARAM_MONITOR ) || 0 == opMode.compare( RPARAM_STANDBY ) )
        {
            // Create the MessageSubscriber for the given nameContext
            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBindingRequestMessage( TA_Base_Core::NamingService::BindName,
                                                                                                       m_agentName,
                                                                                                       this );

            LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "subscribe to BindingRequest msg name=%s", m_agentName.c_str() );
        }
        if ( m_inControlMode )
        {
            // this will send an empty message with only the context
            // It will force all namedObjRef's to clear their cache's
            broadcastBoundObjects( true );
        }

        // Register as a RunParamUser
        TA_Base_Core::RunParams::getInstance().registerRunParamUser( this, RPARAM_OPERATIONMODE );

        m_workItem.reset();

        if ( TA_Base_Core::RunParams::getInstance().isSet(NamingHeartbeatMonitor::PARAM_NAMING_HEARTBEAT_INTERVAL.c_str()) )
        {
            m_HeartbeatMonitor = new NamingHeartbeatMonitor(m_agentName);
            m_HeartbeatMonitor->doOperationModeChange(m_inControlMode);
            m_HeartbeatMonitor->start();
        }

        FUNCTION_EXIT;
    }


    BindingRequestListener::~BindingRequestListener()
    {
        FUNCTION_ENTRY( "~BindingRequestListener" );

        // Remove the message subscription
        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( this );

        // if we don't copy m_workItemVector, we'll do like this:
        //
        // ~BindingRequestListener()  <-- thread one   ( time stamp )
        //                |            m_workvecLock            (1)
        //                v
        // removeWorkItem() -->        m_workItemListLock        (3) enter this, wait (2) release, mean wait thread two run to buttom.
        //
        // processWorkItem()  <-- thread two
        //            |                m_workItemListLock        (2)
        //            v
        // processMessage() -->        m_workvecLock            (4) this line will wait (1) release, so we occur deadlock.

        // cl-21128 gongzong++
        std::vector<TA_Base_Core::IWorkItemPtr> tempItems;
        {
            TA_Base_Core::ThreadGuard guard( m_workvecLock );
            tempItems.swap( m_workItemVector );
        }
        
        for ( std::vector<TA_Base_Core::IWorkItemPtr>::iterator itr = tempItems.begin(); itr != tempItems.end(); itr ++ )
        {
            m_namingThreadPoolSingleton->removeWorkItem( *itr );
        }

        // std::vector<TA_Base_Core::IWorkItemPtr>::iterator itr = m_workItemVector.begin();
        // for ( ; itr != m_workItemVector.end(); itr ++ )
        // {
        //     m_namingThreadPoolSingleton->removeWorkItem( *itr );
        //     m_workItemVector.erase( itr );
        // }
        // ++gongzong

        m_namingThreadPoolSingleton->removeWorkItem( m_workItem );
        m_workItem.reset();

        // delete the sender
        delete m_rebindMessageSender;

        // Cleanout the map on destruction
        m_bindingsMap.clear();

        // cleanup m_HeartbeatMonitor
        if ( m_HeartbeatMonitor != NULL )
        {
            m_HeartbeatMonitor->terminateAndWait();
            m_HeartbeatMonitor->deactivateAndDeleteServant();
            m_HeartbeatMonitor = NULL;
        }

        TA_Base_Core::NamingThreadPoolSingleton::removeInstance( m_namingThreadPoolSingleton );

        FUNCTION_EXIT;
    }


    void BindingRequestListener::doOperationModeChange( bool toControl )
    {
        FUNCTION_ENTRY( "doOperationModeChange" );

        // if we are changing to control, and we are currently in control, we
        // want to send out a reminder message that we are the control agent!
        if ( m_inControlMode && toControl )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                "BindingRequestListener::doOperationModeChange() - Control -> Control Mode change" );

            // Send out a message
            // gongzong++ CL-21311 for video switch agent.
            //broadcastBoundObjects( );
            broadcastBoundObjects(true);
            // ++gongzong
            FUNCTION_EXIT;
            return;
        }

        if ( m_HeartbeatMonitor != NULL )
        {
            m_HeartbeatMonitor->doOperationModeChange(toControl);
        }

        // If we are in control, and go to monitor remove our message subscription
        if ( m_inControlMode && !toControl )
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                "BindingRequestListener::doOperationModeChange() - Control -> Monitor Mode change");

            // we are no longer in control
            m_inControlMode = toControl;

            // don't unsubscribe when switch to Monitor
            // de-register the message consumer
            // TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( this );
            //
            // de-register the message sender
            // delete m_rebindMessageSender;
            // m_rebindMessageSender = NULL;

            // Return from the method
            FUNCTION_EXIT;
            return;
        }

        // if we are in monitor mode and have changed to control mode we
        // need to send out a message containing all our registered names
        // and their pointers
        if ( !m_inControlMode && toControl )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                "BindingRequestListener::doOperationModeChange() - Monitor -> Control Mode change" );

            // we are in control now
            m_inControlMode = toControl;

            // Register the message consumer
            // TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBindingRequestMessage(
            //     TA_Base_Core::NamingService::BindName,
            //     m_agentName,
            //     this);
            //
            // Send out a message
            // gongzong++ CL-21311 for video switch agent.
            //broadcastBoundObjects( );
            broadcastBoundObjects(true);
            // ++gongzong
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                "BindingRequestListener::doOperationModeChange() - Monitor -> Control Mode change successfully after broadcastBoundObjects" );

            FUNCTION_EXIT;
            return;
        }

        // We are already in monitor mode, nothing left to do

        FUNCTION_EXIT;
    }


    void BindingRequestListener::broadcastBoundObjects( bool contextOnly )
    {
        FUNCTION_ENTRY( "broadcastBoundObjects" );

        //for nsa, no need to broadcast RebindName message
        if (m_agentName.find("NotificationServiceAgent") != std::string::npos)
        {
            return;
        }

        // Check to see if the message sender is valid
        if ( NULL == m_rebindMessageSender )
        {
            // resolve the message sender
            m_rebindMessageSender =
                TA_Base_Core::MessagePublicationManager::getInstance().getBindingRequestMessageSender(
                    TA_Base_Core::NamingService::Context );

            TA_ASSERT( NULL != m_rebindMessageSender, "The RebindName message sender was NULL" );
        }

        // This is where our message will be stored
        CORBA::Any updateInfo;

        // Create a new RebindNameCorbaDef
        RebindNameCorbaDef rebindMessage;
        rebindMessage.agentName = CORBA::string_dup( m_agentName.c_str() );
        rebindMessage.timeToLive = m_timeToLive;
        rebindMessage.objectRepository = this->_this();
        rebindMessage.contextOnly = CORBA::Boolean( contextOnly );

        // If this is context only then send the message straight away
        if ( contextOnly )
        {
            // Inject the message into the Any
            updateInfo <<= rebindMessage;

            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                "BindingRequestListener::broadcastBoundObjects() - Sending context [%s] only RebindName message", m_agentName.c_str() );

            // send the message
            m_rebindMessageSender->sendRebindNameMessage( TA_Base_Core::NamingService::RebindName, m_agentName, updateInfo );

            FUNCTION_EXIT;
            return;
        }

        // Lock while we populate the outgoing message
        TA_Base_Core::ThreadGuard guard(m_lock);

        // set the length of the NameObjectPairSequence
        rebindMessage.boundObjects.length( m_bindingsMap.size() );

        // iterate through the list of objects and send the message!
        int count(0);
        for ( std::map<std::string, CORBA::Object_ptr>::iterator it = m_bindingsMap.begin(); it != m_bindingsMap.end(); ++it )
        {
            // Create a NameObjectPair
            NameObjectPair nop;

            // Insert the name and object into the list
            nop.objectName = it->first.c_str();
            nop.objectReference = CORBA::Object::_duplicate( it->second );

            // Add the NameObjectPair to the seq
            rebindMessage.boundObjects[ count ] = nop;

            // Increase counter
            count++;
        }

        // Inject the message into the Any
        updateInfo <<= rebindMessage;

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            "BindingRequestListener::broadcastBoundObjects() - Sending RebindName Message for all objects in context [%s]", m_agentName.c_str() );

        // Send the message
        m_rebindMessageSender->sendRebindNameMessage( TA_Base_Core::NamingService::RebindName, m_agentName, updateInfo );

        FUNCTION_EXIT;
    }


    void BindingRequestListener::bind(CORBA::Object_ptr objRef, const std::string& name)
    {
        FUNCTION_ENTRY( "bind" );

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
            "BindingRequestListener::bind() called for [ %s ]", name.c_str() );

        // No need to check if the input is valid at the Naming::registerName checks this

        // lock on the binding
        TA_Base_Core::ThreadGuard guard(m_lock);

        // Store the key, if it is already registered then update the object
        std::map<std::string, CORBA::Object_ptr>::iterator it = m_bindingsMap.find(name);

        if ( it == m_bindingsMap.end() )
        {
            // Not found, add a new binding
            m_bindingsMap[name] = CORBA::Object::_duplicate(objRef);
        }
        else
        {
            // Update the binding
            it->second = objRef;
        }

        FUNCTION_EXIT;
    }


    void BindingRequestListener::unbind(const std::string& name)
    {
        FUNCTION_ENTRY( "unbind" );

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
            "BindingRequestListener::unbind() called for [ %s ]", name.c_str() );

        TA_Base_Core::ThreadGuard guard(m_lock);

        // find the name in the map & remove the element
        std::map<std::string, CORBA::Object_ptr>::iterator it = m_bindingsMap.find(name);

        if ( it != m_bindingsMap.end() )
        {
            // erase the binding
            CORBA::release(it->second);
            m_bindingsMap.erase(it);
        }
        //else do nothing, not found!

        FUNCTION_EXIT;
    }


    CORBA::Object_ptr BindingRequestListener::getObject( const char* objectName )
    {
        FUNCTION_ENTRY( "getObject" );

        // Check to make sure we are in control
        checkOperationMode("getObject()");

        static std::string hostname = TA_Base_Core::Hostname::getHostname();

        // Guard the find
        TA_Base_Core::ThreadGuard guard(m_lock);

        std::map<std::string, CORBA::Object_ptr>::iterator it = m_bindingsMap.find( objectName );
        if ( it != m_bindingsMap.end() )
        {
            // callback with the object as the argument
            if ( CORBA::is_nil( it->second) )
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
                    "%s NamedObjectRepository received request for [ %s ]. Object reference invalid", m_agentName.c_str(), objectName );

                // Throw exception
                std::stringstream errMsg;
                errMsg << "Object reference invalid for " << objectName << ", Hostname=" << hostname;
                throw TA_Base_Core::NamedObjectRepositoryCorbaDef::NameNotFound( errMsg.str().c_str() );
            }

            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                "%s NamedObjectRepository received request for [ %s ]. Object found", m_agentName.c_str(), objectName );

            {   //Bohong++ for more infomation when resolve
                CORBA::String_var iorstring = CorbaUtil::getInstance().getOrb().object_to_string(it->second);
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "BindingRequestListener::getObject: %s %s %s",
                    objectName, CorbaUtil::getInstance().getObjectAddress(it->second).c_str(), iorstring.in() );
            }   //++Bohong

            FUNCTION_EXIT;
            return CORBA::Object::_duplicate( it->second );
        }
        else
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
                "%s NamedObjectRepository received request for [ %s ]. Object NOT found", m_agentName.c_str(), objectName );

            // Throw exception
            std::stringstream errMsg;
            errMsg << "Object reference not found for " << objectName << ", Hostname=" << hostname;
            throw TA_Base_Core::NamedObjectRepositoryCorbaDef::NameNotFound( errMsg.str().c_str() );
        }
    }


    void BindingRequestListener::receiveSpecialisedMessage( const TA_Base_Core::BindingRequestMessageCorbaDef& message )
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );

        // This is in the servant side. It handles callback to the client requesting the named object
        // only Control Agent would process BindingRequest msg
        if ( ! m_inControlMode )
        {
            LOG3( SourceInfo, TA_Base_Core::DebugUtil::DebugMsg,
                "Monitor Agent ignore to BindingRequest for NamedObjectRepository: messageType=%s, nameContext=%s, name=%s",
                message.messageTypeKey.in(), message.nameContext.in(), message.name.in() );

            FUNCTION_EXIT;
            return;
        }

        LOG4( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Agent received BindingRequest message for NamedObjectRepository: messageType=%s, nameContext=%s, name=%s, address=%s",
            message.messageTypeKey.in(), message.nameContext.in(), message.name.in(), CorbaUtil::getInstance().getObjectAddress(message.originator).c_str() );

        m_workItem.reset( new NamingWorkItem( this, message) );
        m_namingThreadPoolSingleton->queueWorkItem( m_workItem );

        { // cl-21128 gongzong++
            TA_Base_Core::ThreadGuard guard( m_workvecLock );
            m_workItemVector.push_back(m_workItem);
        } // ++gongzong

        FUNCTION_EXIT;
    }


    void BindingRequestListener::processMessage(TA_Base_Core::IWorkItem* item, const TA_Base_Core::BindingRequestMessageCorbaDef& message )
    {
        FUNCTION_ENTRY( "processMessage" );

        { // cl-21128 gongzong++
            TA_Base_Core::ThreadGuard guard( m_workvecLock );

            for ( std::vector<TA_Base_Core::IWorkItemPtr>::iterator itr = m_workItemVector.begin(); itr != m_workItemVector.end(); ++itr )
            {
                if ( &(**itr) == item )
                {
                    m_workItemVector.erase( itr );
                    break;
                }
            }
        } // ++gongzong

        // This call is in the messaging thread, a long wait here for a failed callback will block all other requests
        // So, there is no point waiting any longer than the client will wait, we will set the timeout to that value
        // for the callback to the client so in the case of failure the thread will not be blocked
        // not we cannot assume the timing is equal and arbitrarily divide the timeout by 2 on this side,
        // we should still use the full timeout
        static const unsigned long DEFAULT_RESOLVE_TIMEOUT_MS = 2000;
        static const unsigned long s_resolveTimeoutMs = getRunParamValue( RPARAM_RESOLVE_TIMEOUT_MS, DEFAULT_RESOLVE_TIMEOUT_MS );

        // This is for resolving the agent
        if ( m_agentName.compare( message.name ) == 0 )
        {
            try
            {
                TA_Base_Core::BindingRequestOriginatorCorbaDef_var originator =
                    TA_Base_Core::BindingRequestOriginatorCorbaDef::_narrow(message.originator);

                LOG3( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "Agent replying to BindingRequest for NamedObjectRepository: messageType=%s, nameContext=%s, name=%s",
                    message.messageTypeKey.in(), message.nameContext.in(), message.name.in() );

                if ( !CORBA::is_nil(originator) )
                {
                    // set the callback timeout to the resolve timeout
                    // see the long winded comment above
                    omniORB::setClientCallTimeout( originator, s_resolveTimeoutMs );
                    
                    {   //Bohong++ for more infomation when resolve
                        CORBA::String_var iorstring = CorbaUtil::getInstance().getOrb().object_to_string(originator);
                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "callback to: %s %s", CorbaUtil::getInstance().getObjectAddress(originator).c_str(), iorstring.in() );
                    }   //++Bohong
                    
                    // now make the callback onto the client
                    originator->callback( m_myObjRef.in(), m_timeToLive );
                }
                else
                {
                    // this is a NIL object reference, so dont bother trying to make the callback
                    // just log a noisy error message
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Given a NIL object reference for Binding Request callback" );
                }
            }
            catch ( const CORBA::Exception& ex )
            {
                std::string errMsg( CorbaUtil::getInstance().exceptionToString( ex ) );
                errMsg += " - Thrown while making callback";

                LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Corba Exception", errMsg.c_str() );
            }
            catch ( ... )
            {
                LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown Exception", "Thrown while making callback");
            }
        }

        FUNCTION_EXIT;
    }


    void BindingRequestListener::onRunParamChange(const std::string& name, const std::string& value)
    {
        FUNCTION_ENTRY( "onRunParamChange" );

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            "BindingRequestListener::onRunParamChange() %s=%s", name.c_str(), value.c_str() );

        if ( name.compare( RPARAM_OPERATIONMODE ) == 0 )
        {
            // We want to find out what the mode has changed to
            if (0 == value.compare (RPARAM_CONTROL))
            {
                // We have changed to contol mode
                doOperationModeChange(true);
            }
            else
            {
                // We have changed to control mode
                doOperationModeChange(false);
            }
        }

        FUNCTION_EXIT;
    }


    void BindingRequestListener::poll()
    {
        FUNCTION_ENTRY( "poll" );

        checkOperationMode("poll()");

        FUNCTION_EXIT;
    }


    void BindingRequestListener::checkOperationMode(const std::string& func)
    {
        // Check to make sure we are in control
        std::string operationMode = RunParams::getInstance().get(RPARAM_OPERATIONMODE);
        std::string processStatus = RunParams::getInstance().get(RPARAM_PROCESSSTATUS);

        if ((RPARAM_CONTROL == operationMode) && (RPARAM_RUNNINGCONTROL == processStatus))
        {
            return;
        }

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
            "%s called while agent is not in Control mode, throwing OperationModeException", func.c_str() );

        // Throw exception
        std::stringstream strm;
        static std::string hostname = TA_Base_Core::Hostname::getHostname();
        strm << func << " called while agent is not in Control mode. "
            << "Hostname=" << hostname << ", OperationMode=" << operationMode << ", ProcessStatus=" << processStatus;
        throw TA_Base_Core::OperationModeException(strm.str().c_str());
    }


    NamingWorkItem::NamingWorkItem(TA_Base_Core::BindingRequestListener* callback, const TA_Base_Core::BindingRequestMessageCorbaDef& message )
        : m_callback( callback ),
          m_message( message )
    {
        FUNCTION_ENTRY( "NamingWorkItem" );
        FUNCTION_EXIT;
    }


    void NamingWorkItem::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        m_callback->processMessage( this, m_message );

        FUNCTION_EXIT;
    }

}; // TA_Base_Core

