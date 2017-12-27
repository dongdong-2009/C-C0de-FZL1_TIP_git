/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/system_controller/src/SystemControllerComponent.cpp $
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

#include "app/configuration/config_plugins/system_controller/src/SystemControllerComponent.h"
#include "app/configuration/config_plugins/system_controller/src/resource.h"
#include "app/configuration/config_plugins/system_controller/src/SystemControllerFrameworkView.h"
#include "app/configuration/config_plugins/system_controller/src/SystemControllerDatabaseAccessor.h"

#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    // This is the name of the DLL and is used to load resources correctly.
    const CString SystemControllerComponent::DLL_NAME = "TA_SystemControllerConfigPlugin.dll";


    SystemControllerComponent::SystemControllerComponent(const std::string& componentName)
        : m_icon(NULL),
         AbstractComponent(componentName, new SystemControllerDatabaseAccessor())
    {
        m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_SYS_CONTROLLER) );
    }


    SystemControllerComponent::~SystemControllerComponent()
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


    HICON SystemControllerComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* SystemControllerComponent::getComponentView()
    {
        return RUNTIME_CLASS(SystemControllerFrameworkView);
    }


    std::string SystemControllerComponent::getDLLName() const
    {
        return std::string(DLL_NAME);
    }
}
