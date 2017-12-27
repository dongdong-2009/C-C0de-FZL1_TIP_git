#ifndef DATA_CACHE_HEADER_INCLUDED
#define DATA_CACHE_HEADER_INCLUDED
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include <boost/shared_ptr.hpp>
#include <vector>
#include <ace/Future.h>

namespace TA_Base_Bus
{
    class RightsLibrary;
}

class DataCache
{
public:

    static DataCache& getInstance();
    void run();
    std::string getConsoleName(const std::string& defaultName);
    boost::shared_ptr<TA_Base_Bus::RightsLibrary> getRightsLibrary();
    std::vector<std::string> getAllHostnames();
    TA_Base_Core::IEntityDataPtr getEntity(const std::string& entityName);

private:

    DataCache() {}

    void getConsoleNameTask();
    ACE_Future<std::string> m_consoleName;

    void getRightsLibraryTask();
    ACE_Future<boost::shared_ptr<TA_Base_Bus::RightsLibrary> > m_rightsLibrary;

    void getAllHostnamesTask();
    ACE_Future<std::vector<std::string> > m_allHostnames;

    void getEntityTask(const std::string& entityName);
    std::map<std::string, ACE_Future<TA_Base_Core::IEntityDataPtr> > m_entityMap;
};

#endif
