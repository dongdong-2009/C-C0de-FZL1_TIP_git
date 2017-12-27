// dllmain.h : Declaration of module class.

class CATSBridgeUtilModule : public CAtlDllModuleT< CATSBridgeUtilModule >
{
public :
	DECLARE_LIBID(LIBID_ATSBridgeUtilLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ATSBRIDGEUTIL, "{39E17F98-B811-4E03-AA10-15D542D069B5}")
};

extern class CATSBridgeUtilModule _AtlModule;
