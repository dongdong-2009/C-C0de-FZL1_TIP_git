#include "ATSAlarmBridgeWrap.h"
#include "core/exceptions/src/AlarmListCtrlException.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{
	ATSAlarmBridgeWrap::ATSAlarmBridgeWrap(void)
	{
		HRESULT hr = m_pActionManagerUtil.CoCreateInstance(CLSID_ATSAlarmActionManager, NULL, CLSCTX_INPROC_SERVER);
		if(FAILED(hr))
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Could not find ATSAlarmBridgeUtility.dll");
			TA_THROW(AlarmListCtrlException("Could not find ATSAlarmBridgeUtility.dll"));
		}
	}

	ATSAlarmBridgeWrap::~ATSAlarmBridgeWrap(void)
	{

	}

	SHORT ATSAlarmBridgeWrap::showIP(std::string alarmID, long lleftPosition)
	{
		USES_CONVERSION;
		SHORT nRetVal = 0;
		std::auto_ptr<AlarmActionInfo> pObjInfo(new AlarmActionInfo);
		pObjInfo->bstrAlarmID = A2OLE(alarmID.c_str());
		pObjInfo->lLeftPosition = lleftPosition;

		// Call ATS show IP
		HRESULT hr = m_pActionManagerUtil->showIP(pObjInfo.get(), &nRetVal);
		if (FAILED(hr))
		{
			// Clean up
			SysFreeString(pObjInfo->bstrAlarmID);	
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Call to showIP failed.");
			TA_THROW(AlarmListCtrlException("Call to showIP failed."));			
		}

		// Clean up
		SysFreeString(pObjInfo->bstrAlarmID);

		return nRetVal;
	}

	SHORT ATSAlarmBridgeWrap::showSchematic(std::string alarmID, long lleftPosition)
	{
		USES_CONVERSION;
		SHORT nRetVal = 0;

		std::auto_ptr<AlarmActionInfo> pObjInfo(new AlarmActionInfo);
	
		pObjInfo->bstrAlarmID = SysAllocString(A2OLE(alarmID.c_str()));
		pObjInfo->lLeftPosition = lleftPosition;
		
		// Call ATS show schematic
		HRESULT hr = m_pActionManagerUtil->showSchematic( pObjInfo.get(), &nRetVal);
		if (FAILED(hr))
		{
			// Clean up
			SysFreeString(pObjInfo->bstrAlarmID);	
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Call to showSchematic failed.");
			TA_THROW(AlarmListCtrlException("Call to showSchematic failed."));			
		}

		// Clean up
		SysFreeString(pObjInfo->bstrAlarmID);		

		return nRetVal;
	}

	void ATSAlarmBridgeWrap::registerConnectionPoint(IUnknown* pUnkSink, DWORD &dwAdvise)
	{
		HRESULT hr;
		// Query connection point container
		hr = m_pActionManagerUtil->QueryInterface(IID_IConnectionPointContainer,(void **)&m_pCPC);
		if(FAILED(hr))
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to query IConnectionPointContainer for ATSAlarmBridgeUtil");
			TA_THROW(AlarmListCtrlException("Unable to query IConnectionPointContainer for ATSAlarmBridgeUtil"));
		}

		// Find connection point were need
		hr = m_pCPC->FindConnectionPoint(DIID__IATSAlarmActionManagerEvents,&m_pCP);

		if(FAILED(hr))
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to query IConnectionPoint for ATSAlarmBridgeUtil");
			TA_THROW(AlarmListCtrlException("Unable to query IConnectionPoint for ATSAlarmBridgeUtil"));
		}

		hr = m_pCP->Advise(pUnkSink, &dwAdvise);
		if(FAILED(hr))
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to Advise IConnectionPoint for ATSAlarmBridgeUtil");
			TA_THROW(AlarmListCtrlException("Unable to Advise IConnectionPoint for ATSAlarmBridgeUtil"));
		}

	}

	void ATSAlarmBridgeWrap::unRegisterConnectionPoint(DWORD dwAdvise)
	{
		HRESULT hr = m_pCP->Unadvise(dwAdvise);
		if(FAILED(hr))
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to UnAdvise IConnectionPoint for ATSAlarmBridgeUtil");
			TA_THROW(AlarmListCtrlException("Unable to Advise IConnectionPoint for ATSAlarmBridgeUtil"));
		}
	}
}