/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyAgentState.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  <description>
  */
#ifndef DUTYAGENTSTATE_H_INCLUDED
#define DUTYAGENTSTATE_H_INCLUDED

#if defined( WIN32 ) || defined( __WIN32__ )
#pragma warning( push )
#pragma warning( disable : 4786 4503 )
#endif

#include <string>
#include <map>
#include <iostream>
#include <list>
#include <set>
#include <vector>

#if defined( WIN32 ) || defined( __WIN32__ )
#pragma warning( pop )
#endif

#include "ace/Task.h"
#include "ace/Future.h"
#include "ace/Activation_Queue.h"
#include "ace/Method_Request.h"
#include "ace/OS.h"
#include "ace/Singleton.h"

#include "core/types/src/ta_types.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DutyAgentException.h"
#include "core/data_access_interface/src/Profile.h"
#include "core/data_access_interface/src/Subsystem.h"
#include "core/data_access_interface/src/Location.h"
#include "core/data_access_interface/src/Region.h"
#include "core/data_access_interface/src/Operator.h"
#include "core/data_access_interface/src/ActionGroup.h"
#include "core/data_access_interface/src/ConfigRights.h"
#include "core/data_access_interface/src/RightsAccessFactory.h"
#include "core/data_access_interface/src/ActionGroupAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/SubsystemAccessFactory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/RegionAccessFactory.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ControlStation.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/DutyStateUpdate_MessageTypes.h"
#include "core/message/types/DutyAgentAudit_MessageTypes.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/utilities/src/RunParams.h"
#include "core/naming/src/NamedObject.h"
#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/uuid/src/TAuuid.h"
#include "core/synchronisation/src/Condition.h"
#include "core/threads/src/IWorkItem.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"

#include "bus/security/rights_agent/IDL/src/IRightsAgentCorbaDef.h"

#include "bus/security/duty_agent/IDL/src/IDutyAgentCorbaDef.h"

#include "bus/security/security_utilities/src/SecurityUtilities.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"

// Logging defines
#define SOURCE_INFO      SourceInfo

#define GENERIC_LOG_TYPE          TA_Base_Core::DebugUtil::GenericLog
#define EXCEPTION_CATCH_LOG_TYPE  TA_Base_Core::DebugUtil::ExceptionCatch
#define EXCEPTION_DETAIL_LOG_TYPE TA_Base_Core::DebugUtil::ExceptionDetails
#define FUNCTION_ENTRY_LOG_TYPE   TA_Base_Core::DebugUtil::FunctionEntry
#define FUNCTION_EXIT_LOG_TYPE   TA_Base_Core::DebugUtil::FunctionExit

#define FATAL_LOG_LEVEL  TA_Base_Core::DebugUtil::DebugFatal
#define ERROR_LOG_LEVEL  TA_Base_Core::DebugUtil::DebugError
#define WARN_LOG_LEVEL   TA_Base_Core::DebugUtil::DebugWarn
#define INFO_LOG_LEVEL   TA_Base_Core::DebugUtil::DebugInfo
#define DEBUG_LOG_LEVEL  TA_Base_Core::DebugUtil::DebugDebug

// Types
namespace TA_Base_Bus
{
	namespace DASType
	{
		typedef unsigned long DbKey;
	}
}

#include "bus/security/duty_agent/duty_agent_state/src/DutyPrimitive.h"

// More Types
namespace TA_Base_Bus
{
	namespace DASType
	{
		typedef std::set<DbKey> LocationSet;
		typedef std::set<DbKey> RegionSet;
		typedef std::map<DbKey, DbKey> ReverseRegionMap; // region->locations
		typedef std::set<DbKey> SubsystemSet;		
		typedef std::map<DbKey, SubsystemSet > SubsystemMap; // region->subsystems

// 		typedef std::map<std::string, SubsystemSet> SessionSubsystemMap; //session->subsystems
// 		typedef std::map<DbKey, SessionSubsystemMap> RegionSessoinSubsystemMap; //region -> sessionSubsystemMap
		typedef std::set<DutyPrimitive> DutySet;

		typedef TA_Base_Core::NamedObject<IDutyAgentPeerCorbaDef,
									 IDutyAgentPeerCorbaDef_ptr, 
									 IDutyAgentPeerCorbaDef_var> DutyAgentPeer;
	}

}

// Constants
namespace TA_Base_Bus
{
	namespace DASConst
	{
		const DASType::DbKey NO_PROFILE_GROUP = 0xFFFFFFFF;
		const DASType::DbKey NO_REGION        = 0xFFFFFFFF;
		const DASType::DbKey ALL_REGIONS      = 0xFFFFFFFE;
		const DASType::DbKey NO_SUBSYSTEM     = 0xFFFFFFFF;
		const DASType::DbKey ALL_SUBSYSTEMS   = 0xFFFFFFFE;
		const DASType::DbKey NO_LOCATION      = 0xFFFFFFFF;
		const DASType::DbKey ALL_LOCATIONS    = 0xFFFFFFFE;
		const DASType::DbKey NO_ACTION_GROUP  = 0xFFFFFFFF;

		const std::string LOCAL  ("Local"  );
		const std::string CENTRAL("Central");

		enum PolicyType{ POLICY_TYPE_RAIL, POLICY_TYPE_ROAD }; 
		const int ACE_ERROR = -1;
        // duplicate definition as in DutyAgent.h
        const std::string REQUEST_SUBSYSTEM_DUTY_TRANSFER  = "REQUEST_SUBSYSTEM_DUTY_TRANSFER";
        const std::string REQUEST_SUBSYSTEM_DUTY_ALL_TRANSFER = "REQUEST_SUBSYSTEM_DUTY_ALL_TRANSFER";
	}
}

#include "bus/security/duty_agent/duty_agent_state/src/TransactiveAgentInfo.h"

#include "bus/security/duty_agent/duty_agent_state/src/SubsystemState.h"

#include "bus/security/duty_agent/duty_agent_state/src/IDutyPolicy.h"
#include "bus/security/duty_agent/duty_agent_state/src/DutyPolicyFactory.h"

#include "bus/security/duty_agent/duty_agent_state/src/DutyPolicyCombAlg.h"

#include "bus/security/duty_agent/duty_agent_state/src/IDutyContextBuilder.h"
#include "bus/security/duty_agent/duty_agent_state/src/DutyContextBuilderFactory.h"
#include "bus/security/duty_agent/duty_agent_state/src/DutyContext.h"

#include "bus/security/duty_agent/duty_agent_state/src/EvaluateDutyPolicy.h"

#include "bus/security/duty_agent/duty_agent_state/src/PendingDutyHelper.h"

#include "bus/security/duty_agent/duty_agent_state/src/IDutyStateConverter.h"

#include "bus/security/duty_agent/duty_agent_state/src/DutyState.h"

#include "bus/security/duty_agent/duty_agent_state/src/FrontEndStateChangeSubject.h"
#include "bus/security/duty_agent/duty_agent_state/src/PeerStateChangeSubject.h"
#include "bus/security/duty_agent/duty_agent_state/src/AgentModeSubject.h"
#include "bus/security/duty_agent/duty_agent_state/src/AgentTerminateSubject.h"
#include "bus/security/duty_agent/duty_agent_state/src/DutyResponseSubject.h"
#include "bus/security/duty_agent/duty_agent_state/src/StateUpdateSubject.h"

#include "bus/security/duty_agent/duty_agent_state/src/DutyDbCache.h"
#include "bus/security/duty_agent/duty_agent_state/src/DutyDbCacheUpdater.h"

#include "bus/security/duty_agent/duty_agent_state/src/IDutyQuery.h"
#include "bus/security/duty_agent/duty_agent_state/src/TransferableRegionQuery.h"
#include "bus/security/duty_agent/duty_agent_state/src/TransferableSubsystemQuery.h"
#include "bus/security/duty_agent/duty_agent_state/src/SnapshotQuery.h"
#include "bus/security/duty_agent/duty_agent_state/src/SesSnapshotQuery.h"
#include "bus/security/duty_agent/duty_agent_state/src/SubSnapshotQuery.h"
#include "bus/security/duty_agent/duty_agent_state/src/GetDutyQuery.h"
#include "bus/security/duty_agent/duty_agent_state/src/GetExclusiveDutyQuery.h"

#include "bus/security/duty_agent/duty_agent_state/src/DutySessionCache.h"

#include "bus/security/duty_agent/duty_agent_state/src/Subsystem.h"

#include "bus/security/duty_agent/duty_agent_state/src/IRegSubCommand.h"
#include "bus/security/duty_agent/duty_agent_state/src/RegSubSimpleCommand.h"
#include "bus/security/duty_agent/duty_agent_state/src/RegSubMultiProfCommand.h"
#include "bus/security/duty_agent/duty_agent_state/src/RegSubUniProfCommand.h"
#include "bus/security/duty_agent/duty_agent_state/src/RegSubUniProfCommand2.h"
#include "bus/security/duty_agent/duty_agent_state/src/RegSubQueryCommand.h"

#include "bus/security/duty_agent/duty_agent_state/src/Region.h"

#include "bus/security/duty_agent/duty_agent_state/src/StateUpdateConverter.h"
#include "bus/security/duty_agent/duty_agent_state/src/DutyRequestConverter.h"
#include "bus/security/duty_agent/duty_agent_state/src/DefaultApplyConverter.h"
#include "bus/security/duty_agent/duty_agent_state/src/DutyRequestSender.h"

#include "bus/security/duty_agent/duty_agent_state/src/IDutyPeer.h"
#include "bus/security/duty_agent/duty_agent_state/src/LocalDutyPeer.h"
#include "bus/security/duty_agent/duty_agent_state/src/RemoteDutyPeer.h"

#include "bus/security/duty_agent/duty_agent_state/src/DutyPeerTaskMO.h"
#include "bus/security/duty_agent/duty_agent_state/src/SessionBeginMO.h"
#include "bus/security/duty_agent/duty_agent_state/src/SessionEndMO.h"
#include "bus/security/duty_agent/duty_agent_state/src/SessionOvrBeginMO.h"
#include "bus/security/duty_agent/duty_agent_state/src/SessionOvrEndMO.h"
#include "bus/security/duty_agent/duty_agent_state/src/TransferRegionsMO.h"
#include "bus/security/duty_agent/duty_agent_state/src/TransferSubsystemsMO.h"
#include "bus/security/duty_agent/duty_agent_state/src/DegradeMO.h"
#include "bus/security/duty_agent/duty_agent_state/src/ExecuteQueryMO.h"

#include "bus/security/duty_agent/duty_agent_state/src/DutyPeerTask.h"

#include "bus/security/duty_agent/duty_agent_state/src/IDutyPeerTaskCommand.h"
#include "bus/security/duty_agent/duty_agent_state/src/DutyPeerTaskResultHandler.h"
#include "bus/security/duty_agent/duty_agent_state/src/DutyPeerTaskManager.h"

#include "bus/security/duty_agent/duty_agent_state/src/PeerStateChangeMO.h"
#include "bus/security/duty_agent/duty_agent_state/src/PeerStateChangeTask.h"


#endif //  DUTYAGENTSTATE_H_INCLUDED
