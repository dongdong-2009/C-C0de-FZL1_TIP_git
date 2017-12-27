/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_type/src/AlarmTypeComponent.cpp $
  * @author:  Dirk McCormick
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

#include "app/configuration/config_plugins/alarm_type/src/AlarmTypeComponent.h"
#include "app/configuration/config_plugins/alarm_type/src/resource.h"
#include "app/configuration/config_plugins/alarm_type/src/AlarmTypeFrameworkView.h"
#include "app/configuration/config_plugins/alarm_type/src/AlarmTypeDatabaseAccessor.h"

#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    // This is the name of the DLL and is used to load resources correctly.
    const CString AlarmTypeComponent::DLL_NAME = "TA_AlarmTypeConfigPlugin.dll";


    AlarmTypeComponent::AlarmTypeComponent(const std::string& componentName)
        : m_icon(NULL),
         AbstractComponent(componentName, new AlarmTypeDatabaseAccessor())
    {
        m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_ALARM_TYPE) );
    }


    AlarmTypeComponent::~AlarmTypeComponent()
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


    HICON AlarmTypeComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* AlarmTypeComponent::getComponentView()
    {
        return RUNTIME_CLASS(AlarmTypeFrameworkView);
    }


    std::string AlarmTypeComponent::getDLLName() const
    {
        return std::string(DLL_NAME);
    }

    std::vector<MenuItem> AlarmTypeComponent::getMenuItems()
    {
        return std::vector<MenuItem>();
    }

    std::vector<ToolBarItem> AlarmTypeComponent::getToolBarItems()
    {
        return std::vector<ToolBarItem>();
    }
}
