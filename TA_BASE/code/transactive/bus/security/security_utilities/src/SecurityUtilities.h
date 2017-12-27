#ifndef SECURITYUTILITIES_H_INCLUDED
#define SECURITYUTILITIES_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/security_utilities/src/SecurityUtilities.h $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * An overarching header file to simplify includes.
  *
  */
// system includes
#include <set>
#include <time.h>

// core includes
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/AuthenticationAgentStateUpdateBroadcast_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DutyAgentEntityData.h"
#include "core/data_access_interface/src/MessageAccessFactory.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/MessageData.h"
#include "core/message/types/DutyAgentBroadcastComms_MessageTypes.h"
#include "core/message/src/MessageConfig.h"
#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"

// bus includes
#include "bus/security/authentication_agent/IDL/src/SessionInfoCorbaDef.h"
#include "bus/security/duty_agent/IDL/src/IDutyAgentCorbaDef.h"

// local includes
#include "bus/security/security_utilities/src/DbCacheLookup.h"
#include "bus/security/security_utilities/src/SeObserver.h"
#include "bus/security/security_utilities/src/SeSubject.h"
#include "bus/security/security_utilities/src/SeDutyStateObserver.h"
#include "bus/security/security_utilities/src/SeDutyStateSubject.h"
#include "bus/security/security_utilities/src/SecurtyCorbaMacros.h"


namespace TA_Base_Bus
{
	class ISessionUpdateObserver;
    class IDutyUpdateObserver;
}

#include "bus/security/security_utilities/src/SessionUpdateSubject.h"
#include "bus/security/security_utilities/src/ISessionUpdateObserver.h"
#include "bus/security/security_utilities/src/DutyUpdateSubject.h"
#include "bus/security/security_utilities/src/IDutyUpdateObserver.h"
		
#include "bus/security/security_utilities/src/TimePair.h" 
#include "bus/security/security_utilities/src/TimePairMapBuilder.h" 
#include "bus/security/security_utilities/src/SessionDetailsCache.h" 

#endif // SECURITYUTILITIES_H_INCLUDED
