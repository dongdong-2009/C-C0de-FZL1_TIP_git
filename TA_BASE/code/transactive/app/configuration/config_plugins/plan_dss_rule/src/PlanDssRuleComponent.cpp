/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/configuration/config_plugins/plan_dss_rule/src/PlanDssRuleComponent.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  * 
  * This is the component the main Configuration Editor framework communicates with.
  * It implements the IConfigurableComponent interface. It will handle all toolbar and menu
  * requests and handle them appropriately.
  */

#pragma warning(disable:4786)

#include <sstream>

#include "app/configuration/config_plugins/plan_dss_rule/src/PlanDssRuleComponent.h"
#include "app/configuration/config_plugins/plan_dss_rule/src/resource.h"
#include "app/configuration/config_plugins/plan_dss_rule/src/PlanDssRuleFrameworkView.h"
#include "app/configuration/config_plugins/plan_dss_rule/src/PlanDssRuleDatabaseAccessor.h"

#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    // This is the name of the DLL and is used to load resources correctly.
    const CString PlanDssRuleComponent::DLL_NAME = "TA_PlanDssRuleConfigPlugin.dll";


    PlanDssRuleComponent::PlanDssRuleComponent(const std::string& componentName) : 
    AbstractComponent(componentName, new PlanDssRuleDatabaseAccessor()),
    m_icon(NULL)
    {
        m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_ALARM_PLANS) );
    }


    PlanDssRuleComponent::~PlanDssRuleComponent()
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


    HICON PlanDssRuleComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* PlanDssRuleComponent::getComponentView()
    {
        return RUNTIME_CLASS(PlanDssRuleFrameworkView);
    }


    std::string PlanDssRuleComponent::getDLLName() const
    {
        return std::string(DLL_NAME);
    }
}
