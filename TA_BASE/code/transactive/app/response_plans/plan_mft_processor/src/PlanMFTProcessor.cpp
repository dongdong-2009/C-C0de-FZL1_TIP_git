// PlanMFTProcessor.cpp : Defines the initialization routines for the DLL.
//

#include <afxwin.h>

#include "app/system_control/dll_host/src/IDllComponent.h"
#include "app/response_plans/plan_mft_processor/src/PlanProcessor.h"

namespace TA_Base_App
{

static AFX_EXTENSION_MODULE TApluginDLL = { NULL, NULL };

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    // Remove this if you use lpReserved
    UNREFERENCED_PARAMETER(lpReserved);

    if (dwReason == DLL_PROCESS_ATTACH)
    {
        // Extension DLL one-time initialization
        if (!AfxInitExtensionModule(TApluginDLL, hInstance))
        {
            return 0;
        }

        // Insert this DLL into the resource chain
        // NOTE: If this Extension DLL is being implicitly linked to by
        //  an MFC Regular DLL (such as an ActiveX Control)
        //  instead of an MFC application, then you will want to
        //  remove this line from DllMain and put it in a separate
        //  function exported from this Extension DLL.  The Regular DLL
        //  that uses this Extension DLL should then explicitly call that
        //  function to initialize this Extension DLL.  Otherwise,
        //  the CDynLinkLibrary object will not be attached to the
        //  Regular DLL's resource chain, and serious problems will
        //  result.

        new CDynLinkLibrary(TApluginDLL);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        // Terminate the library before destructors are called
        AfxTermExtensionModule(TApluginDLL);
    }

    return 1;   // ok
}

extern "C" __declspec(dllexport) IDllComponent* createDllComponent()
{
    //AFX_MANAGE_STATE(AfxGetStaticModuleState());
    AFX_MANAGE_STATE(AfxGetAppModuleState());
    return new PlanProcessor;
}

}
