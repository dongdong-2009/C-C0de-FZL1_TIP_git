/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/NotifyEventProcessor.cpp $
  * @author  J. Welton
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  *
  *	The NotifyEventProcessor thread is responsible 
  * for generating events and submitting
  * them.
  *
  *
  */

#include <ctime>

#include "bus/scada/datapoint_library/src/NotifyEventProcessor.h"

#include "core/corba/src/CorbaUtil.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/NameValuePair.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{

	NotifyEventProcessor::NotifyEventProcessor()
	{
        FUNCTION_ENTRY("Constructor");

        start();

        FUNCTION_EXIT;
	}


	NotifyEventProcessor::~NotifyEventProcessor()
	{
        FUNCTION_ENTRY("Destructor");

        terminateAndWait();

        FUNCTION_EXIT;
	}

	void NotifyEventProcessor::processEvent( NotifyEventRequest * newEvent )
	{
        FUNCTION_ENTRY("processEvent");

		// construct the DescriptionParameters list from the new event
		DescriptionParameters descriptionParameters;
		NameValuePair pair1 ( "AuditMessage", newEvent->getMessage() );
		descriptionParameters.push_back ( &pair1 );

		// get the audit message sender
		AuditMessageSender * auditMessageSender =
		MessagePublicationManager::getInstance().getAuditMessageSender( newEvent->getMessageType() );

		// send the new audit message type
		auditMessageSender->sendAuditMessage ( newEvent->getMessageType(),	// message type
											 newEvent->getEntityKey(),		// entity key
											 descriptionParameters,			// description
											 "",							// details
											 newEvent->getSessionID(),		// session ID
											 "",							// alarm ID
											 "",							// incidentkey
											 "" );							// parent event ID

		// clean up
		delete auditMessageSender;
		auditMessageSender = 0;

        FUNCTION_EXIT;
	}

}

