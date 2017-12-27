/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ComponentLibraryAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class accesses the component libraries to retrieve and set up the Component structure.
  * These can then be retrieved for processing.
  */
#pragma warning(disable:4786)
#include "app/configuration/configuration_editor/src/StdAfx.h"
#include "app/configuration/configuration_editor/src/ComponentLibraryAccessor.h"
#include "app/configuration/configuration_editor/src/DatabaseCache.h"
#include "app/configuration/configuration_editor/src/IConfigurableComponent.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/configuration_editor/src/DllHandler.h"
#include "app/configuration/configuration_editor/src/RightsChecker.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/data_access_interface/configuration/src/Component.h"
#include "core/data_access_interface/configuration/src/ComponentAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/SingleThreadBarrier.h"
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include <boost/tuple/tuple.hpp>

using TA_Base_Core::DebugUtil;
using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{
    ComponentLibraryAccessor::ComponentLibraryAccessor()
    {
    }

    ComponentLibraryAccessor::~ComponentLibraryAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            BOOST_FOREACH(Component& component, m_components)
            {
                // Free'ing the library seems to hang the application so we will leave it commented out
                // for now
                //if (component->libraryInstance != NULL)
                //{
                //    FreeLibrary(component->libraryInstance);
                //}

                delete component.componentInterface;
            }
        }
        catch (...)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }

    void ComponentLibraryAccessor::loadComponents(IComponentLibraryAccessorCallback* callback /*=NULL*/)
    {
        FUNCTION_ENTRY("loadComponents");

        CString unavailableComponents;
        std::vector<TA_Base_Core::IComponent*> components;

        try
        {
            components = DatabaseCache::getInstance().getAllComponents();
        }
        catch (const TA_Base_Core::DataException&)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No components will be shown in the Configuration Editor");
            return;
        }
        catch (const TA_Base_Core::DatabaseException&)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No components will be shown in the Configuration Editor");
            return;
        }

        for (std::vector<TA_Base_Core::IComponent*>::iterator iter = components.begin(); iter != components.end(); ++iter)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Loading dynamic link library %s for plugin %s", (*iter)->getLibrary().c_str(), (*iter)->getComponent().c_str());

            // Start setting up the component structure
            Component currentComponent;
            currentComponent.category = (*iter)->getCategory().c_str();
            currentComponent.component = (*iter)->getComponent().c_str();
            currentComponent.isAccessible = false;
            currentComponent.helpFile = (*iter)->getHelpPath();

            if (!isCategorySetInRunPara((*iter)->getCategory()))
            {
                continue;
            }

            // Load the DLL
            currentComponent.libraryInstance = AfxLoadLibrary((*iter)->getLibrary().c_str());

            if (0 == currentComponent.libraryInstance)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not load the plugin %s", (*iter)->getLibrary().c_str());
                unavailableComponents += currentComponent.component + " - " + (*iter)->getLibrary().c_str() + _T("\n");
                continue;
            }

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Dynamic link library %s loaded successfully for plugin %s", (*iter)->getLibrary().c_str(), currentComponent.component);

            // Load the DLL function.

            typedef IConfigurableComponent* (*pfnCreateComponent)(const std::string&, const std::vector<std::string>&);

            pfnCreateComponent DLLCreateComponent;
            DLLCreateComponent = (pfnCreateComponent) ::GetProcAddress(currentComponent.libraryInstance, "createComponent");

            if (NULL == DLLCreateComponent)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not load function in the plugin %s", (*iter)->getLibrary().c_str());
                unavailableComponents += currentComponent.component + " - " + (*iter)->getLibrary().c_str() + _T("\n");

                FreeLibrary(currentComponent.libraryInstance);
                currentComponent.libraryInstance = 0;
                continue;
            }

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "createComponent() function loaded successfully for plugin %s", currentComponent.component);

            std::vector<std::string> types = findOtherTypes(iter, components);

            // Now call the DLL function to get the IConfigurableComponent interface object
            currentComponent.componentInterface = DLLCreateComponent(std::string(currentComponent.component), types);
            TA_ASSERT(currentComponent.componentInterface != NULL, "The component created is NULL");

            // Finally we can add the component to our list
            m_components.push_back(currentComponent);

            // Now update the callback if required.
            if (NULL != callback)
            {
                // A vector iterator is a random access iterator so we can gauge progress by iterator differences.
                unsigned char progress = iter - components.begin();
                progress = (progress * 100) / components.size();
                callback->updateLoadProgress(progress);
            }
        }

        if (!unavailableComponents.IsEmpty())
        {
            showMsgBox(IDS_UW_030016, unavailableComponents);
        }

        FUNCTION_EXIT;
    }

    void ComponentLibraryAccessor::loadComponents2(IComponentLibraryAccessorCallback* callback /*=NULL*/)
    {
#if 0
        FUNCTION_ENTRY("loadComponents");
        //                <component, library>                 typenames
        struct CompenentInfo
        {
            std::string library;
            std::string helpFile;
            std::vector<std::string> typeNames;
        };
        typedef std::map<std::string, CompenentInfo> ComponentMap;
        typedef std::map<std::string, ComponentMap> CategoryMap;
        CategoryMap categoryMap;

        CString unavailableComponents;
        size_t totalComponentNumber = 0;
        std::vector<TA_Base_Core::IComponent*> components;

        try
        {
            components = DatabaseCache::getInstance().getAllComponents();
        }
        catch (const TA_Base_Core::DataException&)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No components will be shown in the Configuration Editor");
            return;
        }
        catch (const TA_Base_Core::DatabaseException&)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No components will be shown in the Configuration Editor");
            return;
        }

        BOOST_FOREACH(TA_Base_Core::IComponent* component, components)
        {
            CompenentInfo& info = categoryMap[component->getCategory()][component->getComponent()];
            info.library = component->getLibrary();
            info.helpFile = component->getHelpPath();
            info.typeNames.push_back(component->getEntityTypeName());
        }

        for (CategoryMap::iterator it = categoryMap.begin(); it != categoryMap.end();)
        {
            if (!isCategorySetInRunPara(it->first))
            {
                categoryMap.erase(it++);
            }
            else
            {
                totalComponentNumber += it->second.size();
                ++it;
            }
        }

        if (0 == totalComponentNumber)
        {
            return;
        }

        TA_Base_Core::SingleThreadBarrier barrier(totalComponentNumber);
        callback->setTotalComponentNumber(totalComponentNumber);

        for (CategoryMap::iterator it = categoryMap.begin(); it != categoryMap.end(); ++it)
        {
            const std::string& category = it->first;
            ComponentMap& componentMap = it->second;

            for (ComponentMap::iterator it2 = componentMap.begin(); it2 != componentMap.end(); ++it2)
            {
                const std::string& component = it2->first;
                CompenentInfo& info = it2->second;
                boost::thread(boost::bind(&ComponentLibraryAccessor::createComponentThread,
                                          this,
                                          boost::ref(category),
                                          boost::ref(component),
                                          boost::ref(info.library),
                                          boost::ref(info.helpFile),
                                          boost::ref(info.typeNames),
                                          boost::ref(unavailableComponents),
                                          boost::ref(barrier)));
            }
        }

        size_t lastCount = 0;
        size_t count = barrier.getCount();

        while (count)
        {
            if (count != lastCount)
            {
                callback->setLoadedComponentNumber(totalComponentNumber - count);
                lastCount = count;
            }

            boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
            count = barrier.getCount();
        }

        barrier.wait();

        if (!unavailableComponents.IsEmpty())
        {
            showMsgBox(IDS_UW_030016, unavailableComponents);
        }
        FUNCTION_EXIT;
#endif
    }

    void ComponentLibraryAccessor::createComponentThread(const std::string& category,
            const std::string& component,
            const std::string& library,
            const std::string& helpFile,
            const std::vector<std::string>& entityTypes,
            CString& unavailableComponents,
            TA_Base_Core::SingleThreadBarrier& barrier)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Loading dynamic link library %s for plugin %s", library.c_str(), component.c_str());

        // Start setting up the component structure
        Component currentComponent;
        currentComponent.category = category.c_str();
        currentComponent.component = component.c_str();
        currentComponent.helpFile = helpFile.c_str();

        // Load the DLL
        currentComponent.libraryInstance = AfxLoadLibrary(library.c_str());

        if (0 == currentComponent.libraryInstance)
        {
            TA_THREADGUARD(m_componentsLock);
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not load the plugin %s", library.c_str());
            unavailableComponents += currentComponent.component + " - " + library.c_str() + _T("\n");
            barrier.post();
            return;
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Dynamic link library %s loaded successfully for plugin %s", library.c_str(), component.c_str());

        // Load the DLL function.

        typedef IConfigurableComponent* (*pfnCreateComponent)(const std::string&, const std::vector<std::string>&);

        pfnCreateComponent DLLCreateComponent;
        DLLCreateComponent = (pfnCreateComponent) ::GetProcAddress(currentComponent.libraryInstance, "createComponent");

        if (NULL == DLLCreateComponent)
        {
            TA_THREADGUARD(m_componentsLock);
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not load function in the plugin %s", library.c_str());
            unavailableComponents += currentComponent.component + " - " + library.c_str() + _T("\n");

            FreeLibrary(currentComponent.libraryInstance);
            currentComponent.libraryInstance = 0;
            barrier.post();
            return;
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "createComponent() function loaded successfully for plugin %s", component.c_str());

        // Now call the DLL function to get the IConfigurableComponent interface object
        currentComponent.componentInterface = DLLCreateComponent(component, entityTypes);
        TA_ASSERT(currentComponent.componentInterface != NULL, "The component created is NULL");

        unsigned long action = currentComponent.componentInterface->getRightsActionForAddingThisComponent();
        currentComponent.isAccessible = RightsChecker::getInstance().canPerformAction(action);

        if (!currentComponent.isAccessible)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "User will not be able to access component %s - %s", category.c_str(), component.c_str());
        }

        // Finally we can add the component to our list
        TA_THREADGUARD(m_componentsLock);
        m_components.push_back(currentComponent);
        barrier.post();
    }

    void ComponentLibraryAccessor::setUpSingletonsInComponents()
    {
        FUNCTION_ENTRY("setUpSingletonsInComponents");

        // Construct the DLL handler that we can pass when initialise the singletons in the DLLs.
        DllHandler dllHandler;

        // Now initialise all the singletons inside the DLLs
        for (std::vector<Component>::iterator iter = m_components.begin(); iter != m_components.end(); ++iter)
        {
            TA_ASSERT(iter->componentInterface != NULL, "Stored component interface is NULL");
            iter->componentInterface->setUpSingletons(dllHandler);
        }

        FUNCTION_EXIT;
    }

    void ComponentLibraryAccessor::determineAvailableComponents(RightsChecker& checker)
    {
        FUNCTION_ENTRY("determineAvailableComponents");

        for (std::vector<Component>::iterator iter = m_components.begin(); iter != m_components.end(); ++iter)
        {
            TA_ASSERT(iter->componentInterface != NULL, "Stored component interface is NULL");

            unsigned long action = iter->componentInterface->getRightsActionForAddingThisComponent();
            iter->isAccessible = checker.canPerformAction(action);

            if (!iter->isAccessible)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                            "User will not be able to access component %s - %s",
                            std::string(iter->category).c_str(),
                            std::string(iter->component).c_str());
            }
        }

        FUNCTION_EXIT;
    }

    std::vector<std::string> ComponentLibraryAccessor::findOtherTypes(const std::vector<TA_Base_Core::IComponent*>::iterator& currentComp,
            std::vector<TA_Base_Core::IComponent*>& components)
    {
        FUNCTION_ENTRY("findOtherTypes");

        TA_ASSERT(*currentComp != NULL, "Iterator passed in contains a NULL pointer");

        std::vector<std::string> types;

        // Add the current component entity type name to the list if it is set to something
        if (!(*currentComp)->getEntityTypeName().empty())
        {
            types.push_back((*currentComp)->getEntityTypeName());
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Finding other types matching: %s, %s, %s", (*currentComp)->getCategory().c_str(), (*currentComp)->getComponent().c_str(), (*currentComp)->getLibrary().c_str());

        // Start our processing one item after the currentComp and add all other strings
        std::vector<TA_Base_Core::IComponent*>::iterator iter = currentComp + 1;

        while (iter != components.end())
        {
            LOGMORE(SourceInfo, "%s %s %s", (*iter)->getCategory().c_str(), (*iter)->getComponent().c_str(), (*iter)->getLibrary().c_str());

            if ((*iter != NULL) &&
                    ((*currentComp)->getCategory() == (*iter)->getCategory()) &&
                    ((*currentComp)->getComponent() == (*iter)->getComponent()) &&
                    ((*currentComp)->getLibrary() == (*iter)->getLibrary()) &&
                    (!(*iter)->getEntityTypeName().empty()))
            {
                types.push_back((*iter)->getEntityTypeName());

                // Now remove this item from the vector so we don't process it again
                delete *iter;
                *iter = NULL;

                // Erase will increment our iterator for us
                iter = components.erase(iter);
                continue;
            }

            ++iter;
        }

        FUNCTION_EXIT;
        return types;
    }

    bool ComponentLibraryAccessor::isCategorySetInRunPara(const std::string& category)
    {
        static const std::string plugins = boost::to_lower_copy(TA_Base_Core::RunParams::getInstance().get(RPARAM_CE_PLUGINS));
        static std::vector<std::string> all_categories;

        if (plugins.empty() || "*" == plugins || "all" == plugins)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "ComponentLibraryAccessor::isCategorySetInRunPara: return true, category=%s, pattern=%s", category.c_str(), plugins.c_str());
            return true;
        }

        if (all_categories.empty())
        {
            typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
            boost::char_separator<char> sep(";|:,");
            tokenizer tokens(plugins, sep);
            std::stringstream log;

            for (tokenizer::iterator it = tokens.begin(); it != tokens.end(); ++it)
            {
                all_categories.push_back("^" + *it);
                log << *it << ", ";
            }

            LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "ComponentLibraryAccessor::isCategorySetInRunPara: all_catergries = %s", log.str().c_str());
        }

        std::string category_lower = "^" + boost::to_lower_copy(category);

        for (size_t i = 0; i < all_categories.size(); ++i)
        {
            if (category_lower.find(all_categories[i]) != std::string::npos)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "ComponentLibraryAccessor::isCategorySetInRunPara: return true, category=%s, pattern=%s", category.c_str(), all_categories[i].substr(1).c_str());
                return true;
            }
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "ComponentLibraryAccessor::isCategorySetInRunPara: return false, category=%s", category.c_str());
        return false;
    }
}
