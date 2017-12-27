#include "stdafx.h"
#include <atlcom.h>
#include "ControlStation_i.h"
#include "ControlStation2.h"

void test_ControlStation2()
{
    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    CControlStation2* controlStation;// = NULL;
    std::cout << sizeof(CControlStation2) << std::endl;

    //HRESULT hr = CoCreateInstance(CLSID_ControlStation2, NULL, CLSCTX_LOCAL_SERVER, IID_IControlStation2, (void**)&controlStation);
    HRESULT hr = CoCreateInstance(CLSID_ControlStation2, NULL, CLSCTX_LOCAL_SERVER, IID_IControlStation2, (void**)&controlStation);

    if (FAILED(hr))
    {
        std::cout << "Failed" << std::endl;
        return;
    }

    controlStation->AddRef();
    //controlStation->

    if (controlStation)
    {
        controlStation->logout();
    }
}
