#if !defined(ONLINEPRINTINGAGENT_H_INCLUDED)
#define ONLINEPRINTINGAGENT_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/online_printing_agent/src/OnlinePrintingAgent.h $
 * @author:   Bart Golab
 * @version:  $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/06/18 16:11:24 $
 * Last modified by:  $Author: haijun.li $
 *
 * Consolidates all the required includes in a single header. 
 *
 */

// Platform specific
#if defined(WIN32) || defined(__WIN32__)
#pragma warning(disable : 4786 4018 4146 4503)
#include <crtdbg.h>
#endif

// System includes
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <set>
#include <map>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Cots includes
#include "ace/OS.h"
#include "ace/Task.h"
#include "ace/Future.h"
#include "ace/Time_Value.h"
#include "ace/Activation_Queue.h"
#include "ace/Method_Request.h"
#include "ace/Synch.h"

// Core includes
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/Matrix.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/types/OnlinePrintingAgentStateUpdate_MessageTypes.h"
#include "core/message/types/OnlinePrintingAgentAlarm_MessageTypes.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/synchronisation/src/ReadWriteThreadLockable.h"
#include "core/synchronisation/src/ThreadReadGuard.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/data_access_interface/online_printing_agent/src/PrintableEventAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/OnlinePrintingAgentEntityData.h"

// Bus includes
#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "bus/generic_agent/src/GenericAgent.h"

// App includes

// Online Printing Agent includes
#include "app/event/online_printing_agent/src/OnlinePrintingAgentTypeDefs.h"
#include "app/event/online_printing_agent/src/OnlinePrintingAgentException.h"
#include "app/event/online_printing_agent/src/IConfigDataObserver.h"
#include "app/event/online_printing_agent/src/ConfigData.h"
#include "app/event/online_printing_agent/src/StateUpdateSender.h"
#include "app/event/online_printing_agent/src/PrintAlarmSender.h"
#include "app/event/online_printing_agent/src/PrintableData.h"
#include "app/event/online_printing_agent/src/PrintOutputFormatter.h"
#include "app/event/online_printing_agent/src/IPrintJobSpooler.h"
#include "app/event/online_printing_agent/src/PrintJobSpooler.h"
#include "app/event/online_printing_agent/src/PrintingService.h"
#include "app/event/online_printing_agent/src/PrintableEventGenerator.h"
#include "app/event/online_printing_agent/src/PrintMO.h"
#include "app/event/online_printing_agent/src/OnlinePrintingAgentUser.h"

#endif // !defined(ONLINEPRINTINGAGENT_H_INCLUDED)
