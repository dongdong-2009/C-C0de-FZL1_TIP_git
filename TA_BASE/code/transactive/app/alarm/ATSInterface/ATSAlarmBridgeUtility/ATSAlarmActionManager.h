// ATSAlarmActionManager.h : Declaration of the CATSAlarmActionManager

#pragma once
#include "resource.h"       // main symbols
#include "ATSAlarmBridgeUtility_i.h"
#include "_IATSAlarmActionManagerEvents_CP.h"
#include "app/alarm/ATSInterface/ISCSAlarmActionBridge/ISCSAlarmActionBridge.h"
#include "app/alarm/ATSInterface/ISCSAlarmActionBridge/ISCSAlarmActionBridgeEventHandler.h"

#include "NonReEntrantThreadLockable.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


// include log4cxx header files.
#include "DebugUtil.h"

// CATSAlarmActionManager

class ATL_NO_VTABLE CATSAlarmActionManager :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CATSAlarmActionManager, &CLSID_ATSAlarmActionManager>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CATSAlarmActionManager>,
	public CProxy_IATSAlarmActionManagerEvents<CATSAlarmActionManager>,
	public IDispatchImpl<IATSAlarmActionManager, &IID_IATSAlarmActionManager, &LIBID_ATSAlarmBridgeUtilityLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CATSAlarmActionManager()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ATSALARMACTIONMANAGER)


BEGIN_COM_MAP(CATSAlarmActionManager)
	COM_INTERFACE_ENTRY(IATSAlarmActionManager)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CATSAlarmActionManager)
	CONNECTION_POINT_ENTRY(__uuidof(_IATSAlarmActionManagerEvents))
END_CONNECTION_POINT_MAP()
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);


	DECLARE_PROTECT_FINAL_CONSTRUCT()
	DECLARE_CLASSFACTORY_SINGLETON(CATSAlarmActionManager)

	HRESULT FinalConstruct();
	void FinalRelease();

public:

	STDMETHOD(showIP)(AlarmActionInfo* pAlarmActionInfo, SHORT* nRetVal);
	STDMETHOD(showSchematic)(AlarmActionInfo* pAlarmActionInfo, SHORT* nRetVal);

	static ISCSAlarmActionBridge::ShowSchematicResult eventSchematicCallback(const ISCSAlarmActionBridge::SchematicDisplayInfo & schematicInfo);

private:
	static CATSAlarmActionManager* m_pInstance;
	static TA_Base_Bus::NonReEntrantThreadLockable m_callbackLock;
	TA_Base_Bus::NonReEntrantThreadLockable m_threadLock;
};

OBJECT_ENTRY_AUTO(__uuidof(ATSAlarmActionManager), CATSAlarmActionManager)
