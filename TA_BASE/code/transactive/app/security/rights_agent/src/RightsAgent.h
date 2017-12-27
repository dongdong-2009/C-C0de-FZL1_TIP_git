#if !defined(RIGHTSAGENT_H_INCLUDED)
#define RIGHTSAGENT_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/rights_agent/src/RightsAgent.h $
 * @author:   Nick Jardine
 * @version:  $Revision: #3 $
 *
 * Last modification: $DateTime: 2012/07/03 17:57:59 $
 * Last modified by:  $Author: liwei.gong $
 *
 */
#if defined( WIN32 ) || defined( __WIN32__ )
#pragma warning( disable : 4786 4018 4146 4503 )
#include <crtdbg.h>
#endif

#include "ace/Synch.h"
#include "ace/Guard_T.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "bus/security/rights_agent/IDL/src/IRightsAgentCorbaDef.h"
#include "core/corba/src/ServantBase.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/naming/src/Naming.h"
#include "core/naming/src/NamedObject.h"
#include "bus/security/authentication_agent/IDL/src/IAuthenticationAgentCorbaDef.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/DutyAgentBroadcastComms_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/DutyAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/AuthenticationAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/RightsAgentEntityData.h"
#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "bus/generic_agent/src/GenericAgent.h"

#include "bus/security/security_utilities/src/SecurityUtilities.h"
#include "bus/security/rights_agent/xacml_factory/src/XacmlFactory.h"
#include "bus/security/rights_agent/policy_decision_point/src/PolicyDecisionPoint.h"
#include "bus/security/rights_agent/xacml_factory/src/RightsAgentInfo.h"
#include "RightsAgentUser.h"
#include "SessionProfileCache.h"
#include "RightsAgentImpl.h"


#endif // !defined(RIGHTSAGENT_H_INCLUDED)
