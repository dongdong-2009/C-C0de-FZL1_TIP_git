///////////////////////////////////////////////////////////
//  CDllComponentMgr.h
//  Implementation of the Class CDllComponentMgr
//  Created on:      20-Nov-2012 5:08:00 PM
//  Original author: Administrator
///////////////////////////////////////////////////////////

#if !defined(EA_1C8E1C31_8B4A_46d9_9574_C5512A2D74D8__INCLUDED_)
#define EA_1C8E1C31_8B4A_46d9_9574_C5512A2D74D8__INCLUDED_

#include "app/system_control/dll_host/src/IDllComponent.h"
#include "core/process_management/IDL/src/IDllHostCorbaDef.h"
#include "core/corba/src/ServantBase.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include <vector>


namespace TA_Base_Bus
{
    class RightsLibrary;
}


namespace TA_Base_App
{

    class CDllComponentMgr : public virtual POA_TA_Base_Core::IDllHostCorbaDef,
                             public virtual TA_Base_Core::ServantBase,
                             public virtual IDllMgrCallBack
    {
    public:

	    CDllComponentMgr();
        virtual ~CDllComponentMgr();

        virtual CORBA::Object_ptr getObject(const char* objectName);

        virtual void registerObject(const std::string& objectName, CORBA::Object_ptr pObject);

        TA_Base_Bus::RightsLibrary* getRightsMgr();

    private:

        CDllComponentMgr( const CDllComponentMgr&);
        CDllComponentMgr& operator =(const CDllComponentMgr&);

        void loadDllInfoFromDB( std::vector<std::string>& dllPathes );
        void initAllDllComponent( const std::vector<std::string>& dllPathes );

    private:

        std::vector<IDllComponentPtr> m_vtDllComponent;
        std::map<std::string, CORBA::Object_var> m_name2ObjectMap;
        TA_Base_Core::NonReEntrantThreadLockable m_name2ObjectMapLock;

        TA_Base_Bus::RightsLibrary* m_rightsMgr;
    };

}

#endif // !defined(EA_1C8E1C31_8B4A_46d9_9574_C5512A2D74D8__INCLUDED_)
