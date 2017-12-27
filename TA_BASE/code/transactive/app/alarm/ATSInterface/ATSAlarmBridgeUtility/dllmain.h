// dllmain.h : Declaration of module class.

class CATSAlarmBridgeUtilityModule : public CAtlDllModuleT< CATSAlarmBridgeUtilityModule >
{
public :
	DECLARE_LIBID(LIBID_ATSAlarmBridgeUtilityLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ATSALARMBRIDGEUTILITY, "{A73E768A-8F58-45EA-AACB-5AF30F066286}")
};

extern class CATSAlarmBridgeUtilityModule _AtlModule;
