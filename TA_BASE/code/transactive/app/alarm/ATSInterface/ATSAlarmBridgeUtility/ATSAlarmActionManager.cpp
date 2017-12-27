// ATSAlarmActionManager.cpp : Implementation of CATSAlarmActionManager

#include "stdafx.h"
#include "ATSAlarmActionManager.h"
#include "RelatedIP_API.h"
#include "RelatedSchematic_API.h"
#include "ThreadGuard.h"

CATSAlarmActionManager* CATSAlarmActionManager::m_pInstance = NULL;
TA_Base_Bus::NonReEntrantThreadLockable CATSAlarmActionManager::m_callbackLock;

// CATSAlarmActionManager
STDMETHODIMP CATSAlarmActionManager::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IATSAlarmActionManager
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CATSAlarmActionManager::showIP(AlarmActionInfo* pAlarmActionInfo, SHORT* nRetVal)
{	
	TA_Base_Bus::ThreadGuard guard(m_threadLock);

	USES_CONVERSION;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "CATSAlarmActionManager::showIP entry");
	// validate arguments
	if (NULL == pAlarmActionInfo || NULL == nRetVal)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid Arguments passed");
		return E_INVALIDARG;
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Data Received: AlarmID=%s leftPosition=%ld", OLE2A(pAlarmActionInfo->bstrAlarmID), pAlarmActionInfo->lLeftPosition);
	std::string strAlarmID;
	long lScreenOffset;

	*nRetVal = 0; // Initialize to success
	
	strAlarmID = OLE2A(pAlarmActionInfo->bstrAlarmID);
	lScreenOffset = pAlarmActionInfo->lLeftPosition;

	RelatedIP::RelatedIPError _return = RelatedIP::launchRelatedIP(strAlarmID, lScreenOffset);
	if (_return != RelatedIP::REL_IP_OK)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,  "Error calling launchRelatedIP Error Code = %d",_return);
	}

	*nRetVal = (SHORT)_return;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "CATSAlarmActionManager::showIP exit");
	return S_OK;
}

STDMETHODIMP CATSAlarmActionManager::showSchematic(AlarmActionInfo* pAlarmActionInfo, SHORT* nRetVal)
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);

	USES_CONVERSION;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "CATSAlarmActionManager::showSchematic entry");
	// validate arguments
	if (NULL == pAlarmActionInfo || NULL == nRetVal)
	{ 
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,  "Invalid Arguments passed");
		return E_INVALIDARG;
	}
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Data Received: AlarmID=%s leftPosition=%ld", OLE2A(pAlarmActionInfo->bstrAlarmID), pAlarmActionInfo->lLeftPosition);
	
	*nRetVal = 0; // initialize to success
	std::string strAlarmID;
	//long lScreenOffset;

	strAlarmID = OLE2A(pAlarmActionInfo->bstrAlarmID);
	//lScreenOffset = pAlarmActionInfo->lLeftPosition;

	RelatedSchematic::RelatedSchematicError _return = RelatedSchematic::getRelatedSchematic(strAlarmID); //, lScreenOffset);

	if (_return != RelatedSchematic::REL_SCHEM_OK)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,  "Error calling RelatedSchematic::getRelatedSchematic Error Code = %d", _return);
	}

	*nRetVal = (SHORT)_return;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "CATSAlarmActionManager::showSchematic exit");
	return S_OK;
}

HRESULT CATSAlarmActionManager::FinalConstruct()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "CATSAlarmActionManager::FinalConstruct entry");
	m_pInstance = this;
	try
	{
		// Register the callback
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Before Registering Callback");
		ISCSAlarmActionBridge::registerCallBack(&CATSAlarmActionManager::eventSchematicCallback);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "after Registering Callback");
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to register callback address for Show Schematic");
		return E_FAIL;
	}
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "CATSAlarmActionManager::FinalConstruct exit");
	return S_OK;
}

void CATSAlarmActionManager::FinalRelease()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "CATSAlarmActionManager::FinalRelease entry");
	try
	{
		// Register the callback
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Before unRegisterCallBack Callback");
		ISCSAlarmActionBridge::unRegisterCallBack();;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "after unRegisterCallBack Callback");
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to register callback address for Show Schematic");
	}
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "CATSAlarmActionManager::FinalRelease exit");
}

ISCSAlarmActionBridge::ShowSchematicResult CATSAlarmActionManager::eventSchematicCallback(const ISCSAlarmActionBridge::SchematicDisplayInfo & schematicInfo)
{
	TA_Base_Bus::ThreadGuard guard(m_callbackLock);
	USES_CONVERSION;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "CATSAlarmActionManager::FinalRelease entry");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Data received: AlarmID=%s Schematic=%s", schematicInfo.alarmID.c_str(), schematicInfo.schematic.c_str());
	CComBSTR bstrSchematic;
	CComBSTR bstrAlarmID;

	ISCSAlarmActionBridge::ShowSchematicResult retVal = ISCSAlarmActionBridge::RESULT_OK;
	bstrAlarmID = A2OLE(schematicInfo.alarmID.c_str());
	bstrSchematic = A2OLE(schematicInfo.schematic.c_str());
	try
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Before calling Fire_DisplaySchematic");
		m_pInstance->Fire_DisplaySchematic(bstrAlarmID, bstrSchematic, (SHORT)schematicInfo.leftPosition);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "After calling Fire_DisplaySchematic");
	}
	catch(...)
	{
		retVal = ISCSAlarmActionBridge::RESULT_FAILED_TO_LOAD;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception occured while trying to call Fire_DisplaySchematic");
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "CATSAlarmActionManager::FinalRelease exit");
	return retVal;
}