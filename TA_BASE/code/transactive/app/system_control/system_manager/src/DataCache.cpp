#include "DataCache.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/system_controller/src/ISystemControllerData.h"
#include "core/data_access_interface/system_controller/src/SystemControllerAccessFactory.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/threads/src/Thread.h"
#include <boost/foreach.hpp>

using namespace TA_Base_Core;

DataCache& DataCache::getInstance()
{
    static DataCache* instance = new DataCache;
    return *instance;
}

void DataCache::run()
{
    TA_Base_Core::ThreadPoolSingletonManager* threadPool = TA_Base_Core::ThreadPoolSingletonManager::getInstance();

    threadPool->queueWorkItem(TA_Base_Core::IWorkItemPtr(new TA_Base_Core::SimpleWorkItem(boost::bind(&DataCache::getConsoleNameTask, this))));
    threadPool->queueWorkItem(TA_Base_Core::IWorkItemPtr(new TA_Base_Core::SimpleWorkItem(boost::bind(&DataCache::getRightsLibraryTask, this))));
    threadPool->queueWorkItem(TA_Base_Core::IWorkItemPtr(new TA_Base_Core::SimpleWorkItem(boost::bind(&DataCache::getAllHostnamesTask, this))));

    std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
    m_entityMap.insert(std::make_pair(entityName, ACE_Future<TA_Base_Core::IEntityDataPtr>()));
    threadPool->queueWorkItem(TA_Base_Core::IWorkItemPtr(new TA_Base_Core::SimpleWorkItem(boost::bind(&DataCache::getEntityTask, this, entityName))));
}

std::string DataCache::getConsoleName(const std::string& defaultName)
{
    std::string consoleName;
    m_consoleName.get(consoleName);
    return consoleName.empty() ? defaultName : consoleName;
}

boost::shared_ptr<TA_Base_Bus::RightsLibrary> DataCache::getRightsLibrary()
{
    boost::shared_ptr<TA_Base_Bus::RightsLibrary> rightsLibrary;
    m_rightsLibrary.get(rightsLibrary);
    return rightsLibrary;
}

std::vector<std::string> DataCache::getAllHostnames()
{
    std::vector<std::string> allHostnames;
    m_allHostnames.get(allHostnames);
    return allHostnames;
}

TA_Base_Core::IEntityDataPtr DataCache::getEntity(const std::string& entityName)
{
    TA_Base_Core::IEntityDataPtr entity;
    std::map<std::string, ACE_Future<TA_Base_Core::IEntityDataPtr> >::iterator it = m_entityMap.find(entityName);

    if (it != m_entityMap.end())
    {
        it->second.get(entity);
    }

    return entity;
}

void DataCache::getConsoleNameTask()
{
    try
    {
        TA_Base_Core::IConsolePtr console(TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID)));
        m_consoleName.set(console->getName());
    }
    catch (...)
    {
        // If there are any problems, then just log that we couldn't get the console name and move on
        LOG_EXCEPTION_CATCH(SourceInfo, "General Exception", "An exception was caught while attempting to retrieve the console details for this session.  Using the console name instead");
        m_consoleName.set("");
    }
}

void DataCache::getRightsLibraryTask()
{
    try
    {
        while (!(RunParams::getInstance().isSet(RPARAM_LOCATIONKEY) && RunParams::getInstance().isSet(RPARAM_NOTIFYHOSTS)))
        {
            TA_Base_Core::Thread::sleep(10);
        }

        TA_Base_Bus::RightsLibraryFactory factory;
        boost::shared_ptr<TA_Base_Bus::RightsLibrary> rightsLibrary(factory.buildRightsLibrary());
        m_rightsLibrary.set(rightsLibrary);
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Cannot create the rights library. This is because the rights agent configuration has not yet been set up so we go straight to the database login instead");
        m_rightsLibrary.set(boost::shared_ptr<TA_Base_Bus::RightsLibrary>());
    }
}

void DataCache::getAllHostnamesTask()
{
    try
    {
        std::vector<std::string> allHostnames;
        std::vector<TA_Base_Core::ISystemControllerData*> systemControllers = TA_Base_Core::SystemControllerAccessFactory::getInstance().getAllSystemControllers(true);

        BOOST_FOREACH(TA_Base_Core::ISystemControllerData* sc, systemControllers)
        {
            allHostnames.push_back(sc->getName());
            delete sc;
        }

        m_allHostnames.set(allHostnames);
    }
    catch (...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", "Could not retrieve system controller data.  Leaving hostname field blank");
        m_allHostnames.set(std::vector<std::string>());
    }
}

void DataCache::getEntityTask(const std::string& entityName)
{
    try
    {
        TA_Base_Core::IEntityDataPtr entityData(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName));
        m_entityMap[entityName].set(entityData);
    }
    catch (...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", "Could not retrieve entity data for %s", entityName.c_str());
        m_entityMap[entityName].set(TA_Base_Core::IEntityDataPtr());
    }
}
