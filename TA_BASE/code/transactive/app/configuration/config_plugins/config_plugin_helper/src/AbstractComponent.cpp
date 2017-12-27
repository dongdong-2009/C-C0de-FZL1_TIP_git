/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/config_plugin_helper/src/AbstractComponent.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This inherits from IConfigurableComponent and implements the parts of this interface that should be
  * common to all DLL's.
  */
#pragma warning(disable:4786)

#include <sstream>

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractComponent.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/IMainView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/SingletonHelper.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/IDatabaseAccessor.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/helperresource.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/configuration_editor/src/IDllHandler.h"
#include "app/configuration/configuration_editor/src/IProgressStatusBar.h"

#include "bus/resources/resource.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "ace/ACE.h"


using TA_Base_Core::DebugUtil;
using namespace TA_Base_App::TA_Configuration;


namespace TA_Base_App
{
    // These are all the menu and toolbar items.
    const CString AbstractComponent::EDIT_MENU_NAME = "编辑(&E)";

    const CString AbstractComponent::MENU_NEW_ITEM    = "新建(&N)";
    const CString AbstractComponent::MENU_COPY_ITEM   = "复制(&C)";
    const CString AbstractComponent::MENU_DELETE_ITEM = "删除(&D)";

    const CString AbstractComponent::TOOLBAR_NEW_ITEM    = "新建";
    const CString AbstractComponent::TOOLBAR_COPY_ITEM   = "复制";
    const CString AbstractComponent::TOOLBAR_DELETE_ITEM = "删除";

    const CString AbstractComponent::NEW_ITEM_CAPTION    = "创建新项目";
    const CString AbstractComponent::COPY_ITEM_CAPTION   = "复制所选项目";
    const CString AbstractComponent::DELETE_ITEM_CAPTION = "永久性删除所选项目";


    AbstractComponent::AbstractComponent(const std::string& componentName, IDatabaseAccessor* databaseAccessor)
        : m_frameworkView(NULL), 
          m_componentName(componentName),
          m_databaseAccessor(databaseAccessor),
          m_statusBar(NULL),
          m_copyIcon(NULL),
          m_newIcon(NULL),
          m_deleteIcon(NULL)
    {

    }


    AbstractComponent::~AbstractComponent()
    {
        FUNCTION_ENTRY("Destructor");

        // TD 4631: ACE is initialised (and here destroyed) so that ACE::localtime_r will not
        // throw an exception when called.
        ACE::fini();

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


    void AbstractComponent::setUpSingletons(const IDllHandler& dllHandler)
    {
        FUNCTION_ENTRY("setUpSingletons");

        SingletonHelper helper;
        helper.initialiseUtilities(dllHandler, getDLLName() );

        FUNCTION_EXIT;
    }

    
    std::vector<MenuItem> AbstractComponent::getMenuItems()
    {
        FUNCTION_ENTRY("getMenuItems");
        std::vector<MenuItem> temp;
    
        MenuItem item;
        item.menu = EDIT_MENU_NAME;
        item.menuItem = MENU_DELETE_ITEM;
        item.menuItemCaption = DELETE_ITEM_CAPTION;
        temp.push_back(item);

        item.menu = EDIT_MENU_NAME;
        item.menuItem = MENU_NEW_ITEM;
        item.menuItemCaption = NEW_ITEM_CAPTION;
        temp.push_back(item);

        item.menu = EDIT_MENU_NAME;
        item.menuItem = MENU_COPY_ITEM;
        item.menuItemCaption = COPY_ITEM_CAPTION;
        temp.push_back(item);

        FUNCTION_EXIT;
        return temp;
    }


    std::vector<ToolBarItem> AbstractComponent::getToolBarItems()
    {
        FUNCTION_ENTRY("getToolBarItems");

        if (m_copyIcon == NULL)
        {
            m_copyIcon = LoadIcon(GetModuleHandle(getDLLName().c_str()), MAKEINTRESOURCE(IDI_CONFIG_MENU_COPY) );
        }
        if (m_newIcon == NULL)
        {
            m_newIcon = LoadIcon(GetModuleHandle(getDLLName().c_str()), MAKEINTRESOURCE(IDI_CONFIG_MENU_NEW) );
        }
        if (m_deleteIcon == NULL)
        {
            m_deleteIcon = LoadIcon(GetModuleHandle(getDLLName().c_str()), MAKEINTRESOURCE(IDI_CONFIG_MENU_DELETE) );
        }

        std::vector<ToolBarItem> temp;

        ToolBarItem item;
        
        item.name = TOOLBAR_NEW_ITEM;
        item.toolBarItemCaption = NEW_ITEM_CAPTION;
        item.icon = m_newIcon;
        temp.push_back(item);

        item.name = TOOLBAR_COPY_ITEM;
        item.icon = m_copyIcon;
        item.toolBarItemCaption = COPY_ITEM_CAPTION;
        temp.push_back(item);

        item.name = TOOLBAR_DELETE_ITEM;
        item.toolBarItemCaption = DELETE_ITEM_CAPTION;
        item.icon = m_deleteIcon;
        temp.push_back(item);

        FUNCTION_EXIT;
        return temp;
    }

    
    void AbstractComponent::setCreatedView(CView* ourView)
    {
        FUNCTION_ENTRY("setCreatedView");

        TA_ASSERT(ourView != NULL, "The view passed to us was NULL");

        // TD 4631: ACE is initialised so that ACE::localtime_r will not
        // throw an exception when called.
        ACE::init();

        m_frameworkView = dynamic_cast<IMainView*>(ourView);
        
        TA_ASSERT(m_frameworkView != NULL, "The wrong type of view was passed to us");

        // Before we set the component interface in the view we will set our the component name in the
        // RunParams so the views can retrieve it. We have to do this each time as the component name can
        // change within this DLL.
        TA_Base_Core::RunParams::getInstance().set(RPARAM_COMPONENT.c_str(), m_componentName.c_str());

        m_frameworkView->setComponentInterface(this);

        FUNCTION_EXIT;
    }


    void AbstractComponent::setStatusBar(IProgressStatusBar* statusBar)
    {
        FUNCTION_ENTRY("setStatusBar");

        TA_ASSERT(statusBar != NULL, "The status bar passed to us was NULL");

        m_statusBar = statusBar;

        FUNCTION_EXIT;
    }


    void AbstractComponent::reload()
    {
        FUNCTION_ENTRY("reload");

        TA_ASSERT(m_databaseAccessor != NULL, "The database accessor was initialised to NULL");

        // This means that next time the user chooses this component the data will be loaded again.
        m_databaseAccessor->invalidateData();

        FUNCTION_EXIT;
    }


    bool AbstractComponent::areCurrentChangesPending(std::vector<std::string>& dataNotApplied)
    {
        FUNCTION_ENTRY("canLoseFocus");

        TA_ASSERT(m_databaseAccessor != NULL, "The database accessor was initialised to NULL");
        
        FUNCTION_EXIT;

        return m_databaseAccessor->areCurrentChangesPending(dataNotApplied);
    }


    void AbstractComponent::itemSelected(const CString& item)
    {
        FUNCTION_ENTRY("itemSelected");

        TA_ASSERT(m_frameworkView != NULL, "Cannot perform this action as the view has not yet been set in this class");

        m_frameworkView->itemSelected(item);

        FUNCTION_EXIT;
    }
}
