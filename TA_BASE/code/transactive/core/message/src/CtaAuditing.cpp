/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/CtaAuditing.cpp $
  * @author:  K. Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This wrappers the audit message senders and implements the interface available to outsourcers.
  * One of these objects must be constructed for each message context or message type that needs
  * to be sent.
  */

#ifdef _MSC_VER
	#pragma warning(disable:4786)	
#endif // _MSC_VER

#include "CtaAuditing.h"
#include "AuditMessageSender.h"
#include "MessagePublicationManager.h"
#include "core/message/types/MessageTypes.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"


namespace TA_Base_Core
{
    CtaAuditing::CtaAuditing( unsigned long entityKey,
                              const MessageContext& messageContext )
      : m_appEntityKey(entityKey)
    {
        m_messageSender.reset( MessagePublicationManager::getInstance().getAuditMessageSender(messageContext) );
        TA_ASSERT(m_messageSender.get() != NULL, "The message sender could not be created");
    }


    CtaAuditing::CtaAuditing( unsigned long entityKey,
                              const MessageType& messageType )
      : m_appEntityKey(entityKey)
    {
        m_messageSender.reset( MessagePublicationManager::getInstance().getAuditMessageSender(messageType) );
        TA_ASSERT(m_messageSender.get() != NULL, "The message sender could not be created");
    }


    CtaAuditing::~CtaAuditing()
    {
        // Empty
    }


    std::string CtaAuditing::submitAuditMessage(const MessageType& messageType,
                                                const NameValuePairs& description)
    {
        TA_ASSERT(m_messageSender.get() != NULL, "The message sender has not been created");

        // Take the named pairs passed in and convert them into a set of description parameters
        DescriptionParameters dp;
        for ( NameValuePairs::const_iterator iter = description.begin(); iter != description.end(); ++iter )
        {
            dp.push_back( new NameValuePair(iter->first,iter->second) );
        }


        std::string eventId = m_messageSender->sendAuditMessage( messageType,
                                                                 m_appEntityKey,
                                                                 dp,
                                                                 "", // details
                                                                 RunParams::getInstance().get(RPARAM_SESSIONID),
                                                                 "", // alarmID
                                                                 "", // incidentID
                                                                 "" ); // parentEventID

        // Now clean up the description parameters
        for ( DescriptionParameters::iterator it = dp.begin(); it != dp.end(); ++it )
        {
            if ( (*it) != NULL )
            {
                delete *it;
                *it = NULL;
            }
        }

        return eventId;
    }


    std::string CtaAuditing::submitAdvancedAuditMessage(const MessageType& messageType,
                                                        unsigned long entityKey,
                                                        const NameValuePairs& description,
                                                        const std::string& details,
                                                        const std::string& sessionID,
                                                        const std::string& assetName,
                                                        const timeb& timeStamp)
    {
        TA_ASSERT(m_messageSender.get() != NULL, "The message sender has not been created");

        // Take the named pairs passed in and convert them into a set of description parameters
        DescriptionParameters dp;
        for ( NameValuePairs::const_iterator iter = description.begin(); iter != description.end(); ++iter )
        {
            dp.push_back( new NameValuePair(iter->first,iter->second) );
        }

        // Send the audit message
        std::string eventId = m_messageSender->sendAuditMessageWithAsset(messageType,
                                                                         entityKey,
                                                                         dp,
                                                                         details,
                                                                         sessionID,
                                                                         "",  //alarmID
                                                                         "",  //incidentID
                                                                         "",  //parentEventID
                                                                         assetName,
                                                                         NULL,
                                                                         timeStamp);

        // Now clean up the description parameters
        for ( DescriptionParameters::iterator it = dp.begin(); it != dp.end(); ++it )
        {
            if ( (*it) != NULL )
            {
                delete *it;
                *it = NULL;
            }
        }

        return eventId;
    }


}