/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/application/src/ApplicationComponent.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the component the main Configuration Editor framework communicates with.
  * It implements the IConfigurableComponent interface. It will handle all toolbar and menu
  * requests and handle them appropriately.
  */

#pragma warning(disable:4786)

#include <sstream>

#include "app/configuration/config_plugins/application/src/ApplicationComponent.h"
#include "app/configuration/config_plugins/application/src/resource.h"
#include "app/configuration/config_plugins/application/src/ApplicationFrameworkView.h"
#include "app/configuration/config_plugins/application/src/ApplicationDatabaseAccessor.h"

#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    // This is the name of the DLL and is used to load resources correctly.
    const CString ApplicationComponent::DLL_NAME = "TA_ApplicationConfigPlugin.dll";


    ApplicationComponent::ApplicationComponent(const std::string& componentName,
                                               const std::vector<std::string>& entityTypes)
        : m_icon(NULL),
         AbstractComponent(componentName, new ApplicationDatabaseAccessor(entityTypes))
    {
        m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_APPLICATION) );
    }


    ApplicationComponent::~ApplicationComponent()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }


    HICON ApplicationComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* ApplicationComponent::getComponentView()
    {
        return RUNTIME_CLASS(ApplicationFrameworkView);
    }


    std::string ApplicationComponent::getDLLName() const
    {
        return std::string(DLL_NAME);
    }
}
