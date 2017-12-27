/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/event_plan_association/src/EventPlanAssocComponent.cpp $
  * @author:  Brad Cleaver
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

#include "app/configuration/config_plugins/event_plan_association/src/EventPlanAssocComponent.h"
#include "app/configuration/config_plugins/event_plan_association/src/resource.h"
#include "app/configuration/config_plugins/event_plan_association/src/EventPlanAssocFrameworkView.h"
#include "app/configuration/config_plugins/event_plan_association/src/EventPlanAssocDatabaseAccessor.h"

#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    // This is the name of the DLL and is used to load resources correctly.
    const CString EventPlanAssocComponent::DLL_NAME = "TA_EventPlanAssocConfigPlugin.dll";


    EventPlanAssocComponent::EventPlanAssocComponent(const std::string& componentName)
        : m_icon(NULL), AbstractComponent(componentName, new EventPlanAssocDatabaseAccessor())
    {
        // Specify the icon to display in the configuration editor.
        m_icon = LoadIcon( GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_EVENT_PLANS) );
    }


    EventPlanAssocComponent::~EventPlanAssocComponent()
    {
        FUNCTION_ENTRY("Destructor");
        FUNCTION_EXIT;
    }


    HICON EventPlanAssocComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* EventPlanAssocComponent::getComponentView()
    {
        FUNCTION_ENTRY("getComponentView");
        FUNCTION_EXIT;
        return RUNTIME_CLASS(EventPlanAssocFrameworkView);
    }


    std::string EventPlanAssocComponent::getDLLName() const
    {
        FUNCTION_ENTRY("getDLLName");
        FUNCTION_EXIT;
        return std::string(DLL_NAME);
    }
}
