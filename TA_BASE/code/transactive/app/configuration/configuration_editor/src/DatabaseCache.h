/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ComponentLibraryAccessor.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class accesses the component libraries to retrieve and set up the Component structure.
  * These can then be retrieved for processing.
  */
#pragma once
#include <ACE/Future.h>
#include <vector>
#include <map>

namespace TA_Base_Core
{
    class IComponent;
    class IEntityData;
    class IOperator;
    class ILocation;
    class IProfile;
    class TransactiveException;
}

namespace TA_Base_App
{
    struct DatabaseCache
    {
    public:

        static DatabaseCache& getInstance();
        void initialize(const std::string& commandLine);
        std::vector<TA_Base_Core::IComponent*>& getAllComponents();
        TA_Base_Core::IEntityData* getGuiEntity();
        unsigned long getConfigurationEditorResourceId();
        std::vector<TA_Base_Core::IOperator*>& getAllNonSystemOperators();
        std::vector<TA_Base_Core::ILocation*>& getAllLocations();
        std::vector<TA_Base_Core::IProfile*>& getAllNonSystemProfiles();
        unsigned long getConsoleKey();
        std::string& getLocationDescription();

    private:

        void getAllComponentsImpl();
        void getGuiEntityImpl();
        void getAllNonSystemOperatorsImpl();
        void getAllLocationsImpl();
        void getAllNonSystemProfilesImpl();
        void getConsoleKeyImpl();
        void checkResult(ACE_Future<TA_Base_Core::TransactiveException*> result);
        void initDatabaseConnection(const std::string& commandLine);

        template<typename T>
        T& genericReturn(T& x)
        {
            checkResult(m_resultMap[&x]);
            return x;
        }

    private:

        std::vector<TA_Base_Core::IComponent*> m_allComponents;
        TA_Base_Core::IEntityData* m_guiEntity;
        unsigned long m_configurationEditorResourceId;
        std::vector<TA_Base_Core::IOperator*> m_allNonSystemOperators;
        std::vector<TA_Base_Core::ILocation*> m_allLocations;
        std::vector<TA_Base_Core::IProfile*> m_allNonSystemProfiles;
        unsigned long m_consoleKey;
        std::string m_locationDescription;
        std::map< void*, ACE_Future<TA_Base_Core::TransactiveException*> > m_resultMap;
    };
}
