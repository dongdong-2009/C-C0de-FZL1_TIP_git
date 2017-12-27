/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/agent/src/AgentComponent.cpp $
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

#include "app/configuration/config_plugins/agent/src/AgentComponent.h"
#include "app/configuration/config_plugins/agent/src/resource.h"
#include "app/configuration/config_plugins/agent/src/AgentFrameworkView.h"
#include "app/configuration/config_plugins/agent/src/AgentDatabaseAccessor.h"

#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    // This is the name of the DLL and is used to load resources correctly.
    const CString AgentComponent::DLL_NAME = "TA_AgentConfigPlugin.dll";


    AgentComponent::AgentComponent(const std::string& componentName,
                                               const std::vector<std::string>& entityTypes)
        : m_icon(NULL),
         AbstractComponent(componentName, new AgentDatabaseAccessor(entityTypes))
    {
        m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_AGENT) );
    }


    AgentComponent::~AgentComponent()
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


    HICON AgentComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* AgentComponent::getComponentView()
    {
        return RUNTIME_CLASS(AgentFrameworkView);
    }


    std::string AgentComponent::getDLLName() const
    {
        return std::string(DLL_NAME);
    }
}
