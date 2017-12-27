#include "StdAfx.h"
#include "ISCSAlarmActionBridge_impl.h"
#include "ThreadGuard.h"
#include "DebugUtil.h"

#include "tchar.h"
#include <atlconv.h>

// Define Callback pointer
ISCSAlarmActionBridge::ShowSchematicResult (*g_EVSchematicCallback)(const SchematicDisplayInfo & schematicInfo) = NULL;

// Initialize singleton variables
CISCSAlarmActionBridge_impl* CISCSAlarmActionBridge_impl::m_pInstance = NULL;
TA_Base_Bus::NonReEntrantThreadLockable CISCSAlarmActionBridge_impl::m_singletonLock;

CISCSAlarmActionBridge_impl* CISCSAlarmActionBridge_impl::getInstance()
{
	TA_Base_Bus::ThreadGuard guard(m_singletonLock);
	if (NULL == m_pInstance)
	{
		m_pInstance = new CISCSAlarmActionBridge_impl;
	}
	return m_pInstance;
}

void CISCSAlarmActionBridge_impl::releaseInstance()
{
	TA_Base_Bus::ThreadGuard guard(m_singletonLock);
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

CISCSAlarmActionBridge_impl::CISCSAlarmActionBridge_impl(void)
{
}

CISCSAlarmActionBridge_impl::~CISCSAlarmActionBridge_impl(void)
{
}

/**
* showSchematic
*
* This function calls to show schematic. 
**
* @param	SchematicDisplayInfo & schematic display information
*
*/   
ShowSchematicResult CISCSAlarmActionBridge_impl::showSchematic(const SchematicDisplayInfo & schematicInfo)
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	ISCSAlarmActionBridge::ShowSchematicResult showRetVal = RESULT_OK;

	USES_CONVERSION;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "showSchematic entry");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "AlarmID: %s, Schematic: %s, LeftPosition: %ld", schematicInfo.alarmID.c_str(), schematicInfo.schematic.c_str(), schematicInfo.leftPosition);
	
	if (schematicInfo.alarmID.length() == 0 || schematicInfo.schematic.length() == 0 )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid Arguments passed.");
		return RESULT_INVALID_ARG;
	}

	try 
	{
		if (NULL != g_EVSchematicCallback)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Before Calling callback showSchematic at Address: %p", *g_EVSchematicCallback);
			showRetVal = g_EVSchematicCallback(schematicInfo);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "After Calling callback showSchematic result = %d", showRetVal);
		}
		else
		{
			showRetVal = RESULT_FAILED_TO_LOAD;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Callback to Show Schematic not set.");
		}
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception occured while trying to notify callback show schematic.");
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "showSchematic exit");
	return showRetVal;
}

void CISCSAlarmActionBridge_impl::registerCallBack(ShowSchematicResult (*eventshematicCallback)(const SchematicDisplayInfo & schematicInfo) )
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "registerCallBack entry");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Setting callback address to: %p", *eventshematicCallback);
	
	g_EVSchematicCallback = eventshematicCallback;


	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "registerCallBack exit");
}

void CISCSAlarmActionBridge_impl::unRegisterCallBack(void)
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "unRegisterCallBack entry");

	if (NULL != g_EVSchematicCallback)
	{
		g_EVSchematicCallback = NULL;
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace,  "unRegisterCallBack exit");
}