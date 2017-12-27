#pragma warning(disable:4786)
#include "stdafx.h"
#include "DatabaseCache.h"
#include "core/data_access_interface/configuration/src/ComponentAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/Hostname.h"
#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>

namespace TA_Base_App
{
    namespace
    {
        struct ExceptionHandler
        {
            typedef ACE_Future<TA_Base_Core::TransactiveException*> Result;
            boost::function<void()> m_function;
            Result m_result1;
            Result m_result2;

            ExceptionHandler(boost::function<void()> f, Result result1, Result result2 = Result())
                : m_function(f),
                  m_result1(result1),
                  m_result2(result2)
            {
            }

            void operator()()
            {
                try
                {
                    m_function();
                    m_result1.set(NULL);
                    m_result2.set(NULL);
                }
                catch (const TA_Base_Core::DataException& e)
                {
                    m_result1.set(new TA_Base_Core::DataException(e));
                    m_result2.set(new TA_Base_Core::DataException(e));
                }
                catch (const TA_Base_Core::DatabaseException& e)
                {
                    m_result1.set(new TA_Base_Core::DatabaseException(e));
                    m_result2.set(new TA_Base_Core::DatabaseException(e));
                }
                catch (...)
                {
                    m_result1.set(new TA_Base_Core::TransactiveException("Unknown Exception"));
                    m_result2.set(new TA_Base_Core::TransactiveException("Unknown Exception"));
                }
            }
        };
    }

    DatabaseCache& DatabaseCache::getInstance()
    {
        static DatabaseCache* instance = new DatabaseCache;
        return *instance;
    }

    void DatabaseCache::initialize(const std::string& commandLine)
    {
        initDatabaseConnection(commandLine);
        boost::thread(ExceptionHandler(boost::bind(&DatabaseCache::getAllComponentsImpl, this), m_resultMap[&m_allComponents]));
        boost::thread(ExceptionHandler(boost::bind(&DatabaseCache::getGuiEntityImpl, this), m_resultMap[&m_guiEntity], m_resultMap[&m_configurationEditorResourceId]));
        boost::thread(ExceptionHandler(boost::bind(&DatabaseCache::getAllNonSystemOperatorsImpl, this), m_resultMap[&m_allNonSystemOperators]));
        boost::thread(ExceptionHandler(boost::bind(&DatabaseCache::getAllLocationsImpl, this), m_resultMap[&m_allLocations]));
        boost::thread(ExceptionHandler(boost::bind(&DatabaseCache::getAllNonSystemProfilesImpl, this), m_resultMap[&m_allNonSystemProfiles]));
        boost::thread(ExceptionHandler(boost::bind(&DatabaseCache::getConsoleKeyImpl, this), m_resultMap[&m_consoleKey], m_resultMap[&m_locationDescription]));
    }

    void DatabaseCache::initDatabaseConnection(const std::string& commandLine)
    {
        TA_Base_Core::RunParams::getInstance().parseCmdLine(commandLine.c_str());

        try
        {
            std::set<std::string> dbs;
            TA_Base_Core::DbConnectionStrings strings(TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
            TA_Base_Core::DatabaseConnectionMap& map = strings.getConnectionMap();

            for (TA_Base_Core::DatabaseConnectionMap::const_iterator it = map.begin(); it != map.end(); ++it)
            {
                dbs.insert(it->second.front().first);
            }

            BOOST_FOREACH(const std::string& db, dbs)
            {
                TA_Base_Core::RunParams::getInstance().set((RPARAM_DBPREFIX + db).c_str(), RPARAM_DBONLINE);
            }
        }
        catch (TA_Base_Core::TransactiveException& e)
        {
            ::MessageBox(0, e.what(), "≈‰÷√π‹¿Ì∆˜", MB_OK | MB_ICONSTOP);
            ::exit(0);
        }
    }

    std::vector<TA_Base_Core::IComponent*>& DatabaseCache::getAllComponents()
    {
        return genericReturn(m_allComponents);
    }

    TA_Base_Core::IEntityData* DatabaseCache::getGuiEntity()
    {
        return genericReturn(m_guiEntity);
    }

    unsigned long DatabaseCache::getConfigurationEditorResourceId()
    {
        return genericReturn(m_configurationEditorResourceId);
    }

    std::vector<TA_Base_Core::IOperator*>& DatabaseCache::getAllNonSystemOperators()
    {
        return genericReturn(m_allNonSystemOperators);
    }

    std::vector<TA_Base_Core::ILocation*>& DatabaseCache::getAllLocations()
    {
        return genericReturn(m_allLocations);
    }

    std::vector<TA_Base_Core::IProfile*>& DatabaseCache::getAllNonSystemProfiles()
    {
        return genericReturn(m_allNonSystemProfiles);
    }

    unsigned long DatabaseCache::getConsoleKey()
    {
        return genericReturn(m_consoleKey);
    }

    std::string& DatabaseCache::getLocationDescription()
    {
        return genericReturn(m_locationDescription);
    }

    void DatabaseCache::getAllComponentsImpl()
    {
        m_allComponents = TA_Base_Core::ComponentAccessFactory::getInstance().getAllComponents();
    }

    void DatabaseCache::getGuiEntityImpl()
    {
        m_guiEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME));
        TA_Base_Core::IResourcePtr resource(TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(m_guiEntity->getKey()));
        m_configurationEditorResourceId = resource->getKey();
    }

    void DatabaseCache::getAllNonSystemOperatorsImpl()
    {
        m_allNonSystemOperators = TA_Base_Core::OperatorAccessFactory::getInstance().getAllNonSystemOperators(false, false);
    }

    void DatabaseCache::getAllLocationsImpl()
    {
        m_allLocations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations(false);
    }

    void DatabaseCache::getAllNonSystemProfilesImpl()
    {
        m_allNonSystemProfiles = TA_Base_Core::ProfileAccessFactory::getInstance().getAllNonSystemProfiles(false);
    }

    void DatabaseCache::getConsoleKeyImpl()
    {
        TA_Base_Core::IConsolePtr console(TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromAddress(TA_Base_Core::Hostname::getHostname()));
        m_consoleKey = console->getKey();
        TA_Base_Core::ILocationPtr location(TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(console->getLocation()));
        m_locationDescription = location->getDescription();
    }

    void DatabaseCache::checkResult(ACE_Future<TA_Base_Core::TransactiveException*> result)
    {
        TA_Base_Core::TransactiveException* exception = NULL;
        result.get(exception);

        if (exception)
        {
            throw (*exception);
        }
    }
}
