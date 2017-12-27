/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_plan_association/src/AlarmPlanAssocComponent.cpp $
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

#include "app/configuration/config_plugins/alarm_plan_association/src/AlarmPlanAssocComponent.h"
#include "app/configuration/config_plugins/alarm_plan_association/src/resource.h"
#include "app/configuration/config_plugins/alarm_plan_association/src/AlarmPlanAssocFrameworkView.h"
#include "app/configuration/config_plugins/alarm_plan_association/src/AlarmPlanAssocDatabaseAccessor.h"

#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    // This is the name of the DLL and is used to load resources correctly.
    const CString AlarmPlanAssocComponent::DLL_NAME = "TA_AlarmPlanAssocConfigPlugin.dll";


    AlarmPlanAssocComponent::AlarmPlanAssocComponent(const std::string& componentName)
        : m_icon(NULL),
         AbstractComponent(componentName, new AlarmPlanAssocDatabaseAccessor())
    {
        m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_ALARM_PLANS) );
    }


    AlarmPlanAssocComponent::~AlarmPlanAssocComponent()
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


    HICON AlarmPlanAssocComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* AlarmPlanAssocComponent::getComponentView()
    {
        return RUNTIME_CLASS(AlarmPlanAssocFrameworkView);
    }


    std::string AlarmPlanAssocComponent::getDLLName() const
    {
        return std::string(DLL_NAME);
    }
}
