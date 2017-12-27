#include "app/system_control/dll_host/src/stdafx.h"
#include "app/system_control/dll_host/src/DllComponentMgr.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_Base_App
{

    CDllComponentMgr::CDllComponentMgr()
        : m_rightsMgr(NULL)
    {
        FUNCTION_ENTRY("CDllComponentMgr");

        std::vector<std::string> dllPathes;

        loadDllInfoFromDB( dllPathes );
        initAllDllComponent( dllPathes );

        TA_Base_Bus::RightsLibraryFactory rightsFactory;
        m_rightsMgr = rightsFactory.buildRightsLibrary();

        TA_Base_Core::CorbaUtil::getInstance().setServantKey(this, "TADllHost");
        activateServant();

        FUNCTION_EXIT;
    }


    CDllComponentMgr::~CDllComponentMgr()
    {
        FUNCTION_ENTRY("~CDllComponentMgr");

        for ( size_t i = 0; i < m_vtDllComponent.size(); ++i )
        {
            m_vtDllComponent[i]->uninit();
        }

        m_vtDllComponent.clear();

        delete m_rightsMgr;
        m_rightsMgr = NULL;

        FUNCTION_EXIT;
    }


    CORBA::Object_ptr CDllComponentMgr::getObject(const char* objectName)
    {
        FUNCTION_ENTRY("getObject");

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Get object with name %s", objectName );

        TA_THREADGUARD(m_name2ObjectMapLock);

        std::map<std::string, CORBA::Object_var>::iterator findIt = m_name2ObjectMap.find( objectName );

        if ( findIt == m_name2ObjectMap.end() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot find object with name %s", objectName );
            throw TA_Base_Core::IDllHostCorbaDef::NameNotFound();
        }

        FUNCTION_EXIT;
        return CORBA::Object::_duplicate( findIt->second );
    }


    void CDllComponentMgr::registerObject(const std::string& objectName, CORBA::Object_ptr pObject)
    {
        FUNCTION_ENTRY("registerObject");

        TA_Assert( false == CORBA::is_nil(pObject) );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Register object with name %s", objectName.c_str() );

        TA_THREADGUARD(m_name2ObjectMapLock);

        m_name2ObjectMap[objectName] = pObject;

        FUNCTION_EXIT;
    }


    TA_Base_Bus::RightsLibrary* CDllComponentMgr::getRightsMgr()
    {
        FUNCTION_ENTRY("getRightsMgr");
        FUNCTION_EXIT;
        return m_rightsMgr;
    }


    void CDllComponentMgr::initAllDllComponent( const std::vector<std::string>& dllPathes )
    {
        FUNCTION_ENTRY("initAllDllComponent");

        for ( size_t i = 0; i < dllPathes.size(); ++i )
        {
            HINSTANCE instance = AfxLoadLibrary(dllPathes[i].c_str());

            if ( NULL == instance )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Could not load the component %s", dllPathes[i].c_str() );

                continue;
            }

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s loaded successfully", dllPathes[i].c_str() );

            // Load the DLL function.
            CreateDllComponentFunPtr createDllComponent = reinterpret_cast<CreateDllComponentFunPtr>( ::GetProcAddress( instance, CREATE_DLL_COMPONENT ) );

            if ( NULL == createDllComponent )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Could not load function in the component %s", dllPathes[i].c_str() );

                FreeLibrary(instance);

                continue;
            }

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "createDllComponent() function loaded successfully for component %s", dllPathes[i].c_str() );

            try
            {
                // Now call the DLL function to get the IBannerPage interface object
                IDllComponentPtr dllComponentPtr( createDllComponent() );
                TA_ASSERT(dllComponentPtr, "The component created is NULL");

                // Construct the DLL handler that we can pass when initialise the singletons in the DLLs.
                TA_Base_Core::DllHandler dllHandler;

                dllComponentPtr->setupSingletons(dllHandler);
                dllComponentPtr->init( this );

                m_vtDllComponent.push_back( dllComponentPtr );

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s initialized successfully", dllPathes[i].c_str() );
            }
            catch ( ... )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Error occurred while attempting initialize dll component");
            }
        }

        FUNCTION_EXIT;
    }


    void CDllComponentMgr::loadDllInfoFromDB( std::vector<std::string>& dllPathes )
    {
        FUNCTION_ENTRY("loadDllInfoFromDB");

        try
        {
            TA_Base_Core::IEntityDataList entities = TA_Base_Core::EntityAccessFactory::getInstance().getChildEntitiesOf(TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME));

            for ( TA_Base_Core::IEntityDataList::iterator it = entities.begin(); it != entities.end(); ++it )
            {
                dllPathes.push_back( (*it)->getAddress() );

                delete *it;
                *it = NULL;
            }
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", "No component will be loaded in the dll host");
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", "No component will be loaded in the dll host");
        }

        FUNCTION_EXIT;
    }

}
