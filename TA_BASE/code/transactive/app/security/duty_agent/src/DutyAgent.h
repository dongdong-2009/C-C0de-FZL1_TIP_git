#ifndef DUTYAGENT_H_INCLUDED
#define DUTYAGENT_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/DutyAgent.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2012/06/12 13:35:44 $
  *  Last modified by:  $Author: builder $
  *
  *  Main header file for the DutyAgent. It captures all the includes,
  *  constants and the dependencies between them. It should be the ONLY
  *  header file included by cpp files in this DutyAgent project.
  */

// Pragmas for Windoze
#if defined( WIN32 ) || defined( __WIN32__ )
#pragma warning( disable : 4786 4018 4146 4503 )
#include <crtdbg.h>
#endif

// System includes
#include <string>

// COTS includes
#include "ace/OS.h"
#include "ace/Object_Manager.h"
#include "ace/Thread_Semaphore.h"

// Core includes
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/naming/src/Naming.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DutyAgentEntityData.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/DutyStateUpdate_MessageTypes.h"
#include "core/message/types/DutyAgentBroadcastComms_MessageTypes.h"
#include "core/message/types/DutyAgentAlarms_MessageTypes.h"
#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/synchronisation/src/Semaphore.h"

// Business includes
#include "bus/security/security_utilities/src/SecurityUtilities.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/duty_agent/duty_agent_state/src/DutyAgentState.h"

// Application-wide Constants
namespace DutyAction
{
	const std::string SESSION_BEGIN       = "Operator Login";
	const std::string SESSION_OVR_BEGIN   = "Operator Override Begin";	
	const std::string SESSION_OVR_END     = "Operator Override End";	
	const std::string SESSION_END         = "Operator Logout";

    const std::string HAS_DUTY_QUERY      = "Duty Query";
	const std::string HAS_EXCL_DUTY_QUERY = "Exclusive Duty Query";

    // Duty transfer actions
	const std::string REQUEST_REGION_DUTY_TRANSFER     = "REQUEST_REGION_DUTY_TRANSFER";
	const std::string REQUEST_SUBSYSTEM_DUTY_TRANSFER  = "REQUEST_SUBSYSTEM_DUTY_TRANSFER";
	const std::string REQUEST_SUBSYSTEM_DUTY_ALL_TRANSFER  = "REQUEST_SUBSYSTEM_DUTY_ALL_TRANSFER";

//	const std::string RECEIVE_REGION_DUTY_TRANSFER     = "RECEIVE_REGION_DUTY_TRANSFER";
//	const std::string RECEIVE_SUBSYSTEM_DUTY_TRANSFER  = "RECEIVE_SUBSYSTEM_DUTY_TRANSFER";
//	const std::string RECEIVE_SUBSYSTEM_DUTY_ALL_TRANSFER  = "RECEIVE_SUBSYSTEM_DUTY_ALL_TRANSFER";

	const std::string SYSTEMWIDE_DELEGATION = "SYSTEMWIDE_DELEGATION";
	const std::string AUTOMATIC_DELEGATION = "AUTOMATIC_DELEGATION";
	const std::string MANUAL_DELEGATION="MANUAL_DELEGATION";
    const std::string SUBSYSTEM_DEGRADED = "SUBSYSTEM_DEGRADED";

};

#endif // DUTYAGENT_H_INCLUDED
