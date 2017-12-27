/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/rights/src/RightsComponent.cpp $
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

#include "app/configuration/config_plugins/rights/src/RightsComponent.h"
#include "app/configuration/config_plugins/rights/src/resource.h"
#include "app/configuration/config_plugins/rights/src/RightsView.h"
#include "app/configuration/config_plugins/rights/src/RightsDatabaseAccessor.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/SingletonHelper.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;
using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{
    // This is the name of the DLL and is used to load resources correctly.
    const CString RightsComponent::DLL_NAME = "TA_RightsConfigPlugin.dll";


    RightsComponent::RightsComponent(const std::string& componentName)
        : m_icon(NULL),
          m_frameworkView(NULL), 
          m_componentName(componentName),
          m_databaseAccessor(new RightsDatabaseAccessor()),
          m_statusBar(NULL)
    {
        m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_RIGHTS) );
    }


    RightsComponent::~RightsComponent()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            if (m_databaseAccessor != NULL)
            {
                delete m_databaseAccessor;
                m_databaseAccessor = NULL;
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }


    void RightsComponent::setUpSingletons(const IDllHandler& dllHandler)
    {
        FUNCTION_ENTRY("setUpSingletons");

        SingletonHelper helper;
        helper.initialiseUtilities(dllHandler, getDLLName() );

        FUNCTION_EXIT;
    }

    
    HICON RightsComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* RightsComponent::getComponentView()
    {
        return RUNTIME_CLASS(RightsView);
    }


    std::string RightsComponent::getDLLName() const
    {
        return std::string(DLL_NAME);
    }


    std::vector<MenuItem> RightsComponent::getMenuItems()
    {
        // We do not want any menu items
        std::vector<MenuItem> menuItems;
        return menuItems;
    }


    std::vector<ToolBarItem> RightsComponent::getToolBarItems()
    {
        // We do not want any toolbar items
        std::vector<ToolBarItem> toolbarItems;
        return toolbarItems;
    }


    void RightsComponent::setCreatedView(CView* ourView)
    {
        FUNCTION_ENTRY("setCreatedView");

        TA_ASSERT(ourView != NULL, "The view passed to us was NULL");

        m_frameworkView = dynamic_cast<RightsView*>(ourView);
        
        TA_ASSERT(m_frameworkView != NULL, "The wrong type of view was passed to us");

        // Before we set the component interface in the view we will set our the component name in the
        // RunParams so the views can retrieve it. We have to do this each time as the component name can
        // change within this DLL.
        TA_Base_Core::RunParams::getInstance().set(RPARAM_COMPONENT.c_str(), m_componentName.c_str());

        m_frameworkView->setComponentInterface(this);

        FUNCTION_EXIT;
    }


    void RightsComponent::setStatusBar(IProgressStatusBar* statusBar)
    {
        FUNCTION_ENTRY("setStatusBar");

        TA_ASSERT(statusBar != NULL, "The status bar passed to us was NULL");

        m_statusBar = statusBar;

        FUNCTION_EXIT;
    }


    void RightsComponent::reload()
    {
        FUNCTION_ENTRY("reload");

        TA_ASSERT(m_databaseAccessor != NULL, "The database accessor was initialised to NULL");

        // This means that next time the user chooses this component the data will be loaded again.
        m_databaseAccessor->invalidateData();

        FUNCTION_EXIT;
    }


    bool RightsComponent::areCurrentChangesPending(std::vector<std::string>& dataNotApplied)
    {
        FUNCTION_ENTRY("canLoseFocus");

        TA_ASSERT(m_databaseAccessor != NULL, "The database accessor was initialised to NULL");
        
        FUNCTION_EXIT;

        return m_databaseAccessor->areCurrentChangesPending(dataNotApplied);
    }


    void RightsComponent::itemSelected(const CString& item)
    {
        FUNCTION_ENTRY("itemSelected");

        // Nothing to be done here as this component has no menu or toolbar items.
        // This should never actually be called.

        FUNCTION_EXIT;
    }

}
