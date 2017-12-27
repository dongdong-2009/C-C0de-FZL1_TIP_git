/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_library/src/RightsLibraryInternal.h $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Logging defines.
  *
  */

#ifndef RIGHTS_LIBRARY_INTERNAL_H_INCLUDED
#define RIGHTS_LIBRARY_INTERNAL_H_INCLUDED

#if defined(_MSC_VER)
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include <functional>
#include <map>
#include <algorithm>
#include <set>

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

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/RightsAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DutyAgentEntityData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/naming/src/NamedObject.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/DataException.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/src/MessageConstants.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/DutyAgentBroadcastComms_MessageTypes.h"
#include "core/message/types/DutyAgentComms_MessageTypes.h"
#include "core/threads/src/IWorkItem.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"

#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/rights_agent/IDL/src/IRightsAgentCorbaDef.h"
#include "bus/security/security_utilities/src/SecurityUtilities.h"

#include "RightsLibrary.h"
#include "RightsQuery.h"
#include "LocSubDbCache.h"
#include "RightsCache.h"
#include "DutyCommsSubscriber.h"
#include "CorbaRightsLibrary.h"


#endif // RIGHTS_LIBRARY_INTERNAL_H_INCLUDED
