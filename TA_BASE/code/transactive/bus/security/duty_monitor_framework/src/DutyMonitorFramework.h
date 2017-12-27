#if !defined(_DUTYMONITORFRAMEWORK_H__EAB01112_2101_4831_99F1_C72DBB6F90A2__INCLUDED_)
#define _DUTYMONITORFRAMEWORK_H__EAB01112_2101_4831_99F1_C72DBB6F90A2__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/DutyMonitorFramework.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Consolidates all the required includes in a single header.
  *
  */

#ifdef __WIN32__
	#pragma warning(disable: 4250 4503 4786)
#endif

// System includes
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>

#include "ace/Task.h"
#include "ace/Future.h"
#include "ace/Activation_Queue.h"
#include "ace/Method_Request.h"
#include "ace/OS.h"
#include "ace/Singleton.h"
// Core includes
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/MatrixException.h"
#include "core/utilities/src/Matrix.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/configuration_updates/src/EObjectType.h"
#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/message/IDL/src/ConfigUpdateMessageCorbaDef.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/types/AuthenticationAgentStateUpdateBroadcast_MessageTypes.h"
#include "core/message/types/DutyAgentBroadcastComms_MessageTypes.h"
#include "core/naming/src/NamedObject.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/IActionGroup.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/IRegion.h"
#include "core/data_access_interface/src/ISubsystem.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/ActionGroupAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/RegionAccessFactory.h"
#include "core/data_access_interface/src/SubsystemAccessFactory.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ControlStation.h"
#include "core/data_access_interface/entity_access/src/DutyAgentEntityData.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/RightsException.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"

// Bus includes
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/authentication_agent/IDL/src/SessionInfoCorbaDef.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/duty_agent/IDL/src/DutyAgentTypeCorbaDef.h"
#include "bus/security/duty_agent/IDL/src/IDutyAgentMgrCorbaDef.h"
#include "bus/security/security_utilities/src/SeObserver.h"
#include "bus/security/security_utilities/src/SeSubject.h"
#include "bus/security/security_utilities/src/DbCacheLookup.h"

// Duty Monitor Framework includes
#include "bus/security/duty_monitor_framework/src/DMFTypeDefs.h"
#include "bus/security/duty_monitor_framework/src/DutyMonitorFrameworkException.h"
#include "bus/security/duty_monitor_framework/src/DbCacheLookupEx.h"
#include "bus/security/duty_monitor_framework/src/DataCache.h"
#include "bus/security/duty_monitor_framework/src/IConfigDataObserver.h"
#include "bus/security/duty_monitor_framework/src/SessionCache.h"
#include "bus/security/duty_monitor_framework/src/ISessionObserver.h"
#include "bus/security/duty_monitor_framework/src/DutyRightsSingleton.h"
#include "bus/security/duty_monitor_framework/src/DutyRights.h"
#include "bus/security/duty_monitor_framework/src/DutyAgentAccessSingleton.h"
#include "bus/security/duty_monitor_framework/src/DutyAgentAccess.h"
#include "bus/security/duty_monitor_framework/src/DefaultRegionAssignmentMatrix.h"
#include "bus/security/duty_monitor_framework/src/SubsystemDutyMatrixElement.h"
#include "bus/security/duty_monitor_framework/src/SubsystemDutyMatrix.h"
#include "bus/security/duty_monitor_framework/src/ISubsystemDutyObserver.h"
#include "bus/security/duty_monitor_framework/src/RegionDutyMatrixElement.h"
#include "bus/security/duty_monitor_framework/src/RegionDutyMatrix.h"
#include "bus/security/duty_monitor_framework/src/IRegionDutyObserver.h"
#include "bus/security/duty_monitor_framework/src/DutyDataStoreSingleton.h"
#include "bus/security/duty_monitor_framework/src/DutyDataStore.h"
#include "bus/security/duty_monitor_framework/src/RegionDutyMatrixUpdater.h"
#include "bus/security/duty_monitor_framework/src/RegionMatrixDutyUpdateMO.h"
#include "bus/security/duty_monitor_framework/src/RemoveLocationDutyMO.h"
#include "bus/security/duty_monitor_framework/src/RegionMatrixSessionUpdateMO.h"


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


namespace TA_Base_Bus
{
	namespace DASConst
	{
		
		const std::string CENTRAL("Central");
		
		enum PolicyType{ POLICY_TYPE_RAIL, POLICY_TYPE_ROAD }; 
		const int ACE_ERROR = -1;
		
	}
}

#endif // !defined(_DUTYMONITORFRAMEWORK_H__EAB01112_2101_4831_99F1_C72DBB6F90A2__INCLUDED_)