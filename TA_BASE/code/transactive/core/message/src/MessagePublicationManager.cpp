/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/MessagePublicationManager.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * <description>
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/ConfigUpdateMessageSender.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/src/BindingRequestMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/message/src/StructuredEventSupplier.h"
#include "core/message/src/PublicationManager.h"

namespace TA_Base_Core
{
    // initialise statics
    TA_Base_Core::NonReEntrantThreadLockable MessagePublicationManager::m_singletonLock;
    MessagePublicationManager* MessagePublicationManager::m_theClassInstance = NULL;
    bool MessagePublicationManager::m_enableSubChangeUpdates = false; // limin++ CL-21308

    MessagePublicationManager::~MessagePublicationManager()
    {
		m_theClassInstance = NULL;
    }


    MessagePublicationManager& MessagePublicationManager::getInstance()
    {
        TA_THREADGUARD( m_singletonLock );

        // If guard acquired then make sure the singleton is still NULL
        if ( NULL == m_theClassInstance )
        {
            // Create the one & only object
            m_theClassInstance = new MessagePublicationManager();
        }

        return *m_theClassInstance;
	}


     // limin++ CL-21308
    bool MessagePublicationManager::getEnableSubChangeUpdates()
    {
        return m_enableSubChangeUpdates;
    }


    void MessagePublicationManager::setEnableSubChangeUpdates( bool enable /* = true */ )
    {
        m_enableSubChangeUpdates = enable;
    }
    // ++limin CL-21308


    CommsMessageSender* MessagePublicationManager::getCommsMessageSender( const MessageContext& messageContext ) const
    {
        return gCreateMessageSender<CommsMessageSender>(messageContext.getChannelName());
    }


    CommsMessageSender* MessagePublicationManager::getCommsMessageSender( const MessageType& messageType ) const
    {
        return getCommsMessageSender(messageType.getContextObj());
    }


    AuditMessageSender* MessagePublicationManager::getAuditMessageSender( const MessageContext& messageContext ) const
    {
        return gCreateMessageSender<AuditMessageSender>(messageContext.getChannelName());
    }


    AuditMessageSender* MessagePublicationManager::getAuditMessageSender( const MessageType& messageType ) const
    {
        return getAuditMessageSender(messageType.getContextObj());
    }


    ConfigUpdateMessageSender* MessagePublicationManager::getConfigUpdateMessageSender( const MessageContext& messageContext ) const
    {
        return gCreateMessageSender<ConfigUpdateMessageSender>(messageContext.getChannelName());
    }


    ConfigUpdateMessageSender* MessagePublicationManager::getConfigUpdateMessageSender( const MessageType& messageType ) const
    {
        return getConfigUpdateMessageSender(messageType.getContextObj());
    }


    StateUpdateMessageSender* MessagePublicationManager::getStateUpdateMessageSender( const MessageContext& messageContext ) const
    {
        return gCreateMessageSender<StateUpdateMessageSender>(messageContext.getChannelName());
    }


    StateUpdateMessageSender* MessagePublicationManager::getStateUpdateMessageSender( const MessageType& messageType ) const
    {
        return getStateUpdateMessageSender(messageType.getContextObj());
    }


    BindingRequestMessageSender* MessagePublicationManager::getBindingRequestMessageSender( const MessageContext& messageContext ) const
    {
        return gCreateMessageSender<BindingRequestMessageSender>(messageContext.getChannelName());
    }


    BindingRequestMessageSender* MessagePublicationManager::getBindingRequestMessageSender( const MessageType& messageType ) const
    {
        return getBindingRequestMessageSender(messageType.getContextObj());
    }

}; // TA_Base_Core

