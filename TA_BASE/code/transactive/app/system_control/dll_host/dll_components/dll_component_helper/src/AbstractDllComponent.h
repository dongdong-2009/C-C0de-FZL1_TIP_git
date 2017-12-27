#if !defined(ABSTRACT_IDLL_COMPONENT_H)
#define ABSTRACT_IDLL_COMPONENT_H

#include "app/system_control/dll_host/src/IDllComponent.h"


namespace TA_Base_App
{

    class AbstractDllComponent : public IDllComponent
    {
    public:

        virtual void setupSingletons(const TA_Base_Core::DllHandler& dllHandler);

    protected:

        virtual std::string getDLLName() const = 0;
    };

}

#endif // !defined(ABSTRACT_IDLL_COMPONENT_H)
