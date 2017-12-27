#include "app/system_control/dll_host/dll_components/dll_component_helper/src/AbstractDllComponent.h"
#include "app/system_control/dll_host/dll_components/dll_component_helper/src/SingletonHelper.h"


namespace TA_Base_App
{

    void AbstractDllComponent::setupSingletons(const TA_Base_Core::DllHandler& dllHandler)
    {
        SingletonHelper helper;
        helper.initialiseUtilities(dllHandler, getDLLName() );
    }

}
