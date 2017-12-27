#if !defined(IDLL_COMPONENT_H)
#define IDLL_COMPONENT_H

#include "core/utilities/src/DllHandler.h"
#include "omniORB4/CORBA.h"
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>


namespace TA_Base_App
{

    class IDllMgrCallBack
    {
    public:

        virtual ~IDllMgrCallBack() {}
        virtual void registerObject(const std::string& objectName, CORBA::Object_ptr pObject) = 0;
    };


    class IDllComponent
    {
    public:

        virtual ~IDllComponent() {}
        virtual void init( IDllMgrCallBack* callback ) = 0;
        virtual void uninit() = 0;
        virtual void setupSingletons(const TA_Base_Core::DllHandler& dllHandler) = 0;
    };

    typedef boost::shared_ptr<TA_Base_App::IDllComponent> IDllComponentPtr;

}

// the type of dllexport function
typedef TA_Base_App::IDllComponent* (*CreateDllComponentFunPtr)();
// the name of dllexport function
#define CREATE_DLL_COMPONENT "createDllComponent"


#endif // !defined(IDLL_COMPONENT_H)
