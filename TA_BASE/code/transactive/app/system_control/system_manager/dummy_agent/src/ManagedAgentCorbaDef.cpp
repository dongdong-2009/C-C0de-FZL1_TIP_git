/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_manager/dummy_agent/src/ManagedAgentCorbaDef.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "ManagedAgentCorbaDef.h"

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;


ManagedAgentCorbaDef::ManagedAgentCorbaDef()
{
    TA_Base_Core::CorbaUtil::getInstance().setServantKey(this, "ManagedAgent");
    activateServant();

    m_operationMode = TA_Base_Core::Control;
}

ManagedAgentCorbaDef::~ManagedAgentCorbaDef()
{
}

void ManagedAgentCorbaDef::setOperationMode(TA_Base_Core::EOperationMode mode)
{
    m_operationMode = mode;
}

TA_Base_Core::EOperationMode ManagedAgentCorbaDef::getOperationMode()
{
    return m_operationMode;
}

void ManagedAgentCorbaDef::notifyGroupOffline(const char* group)
{
}

void ManagedAgentCorbaDef::notifyGroupOnline(const char* group)
{
}

void ManagedAgentCorbaDef::run()
{
    m_isRunning = true;

    while (m_isRunning)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Debug message");
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Info message");
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Norm message");
        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Warn message");
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error message");
        LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, "Fatal message");

        sleep(1000);
    }
}

void ManagedAgentCorbaDef::terminate()
{
    m_isRunning = false;
}
