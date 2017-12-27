/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/event_colour/src/EventColourComponent.cpp $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the component the main Configuration Editor framework communicates with.
  * It implements the IConfigurableComponent interface. It will handle all toolbar and menu
  * requests and handle them appropriately.
  */

// EventColourComponent.cpp: implementation of the EventColourComponent class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable:4786)

#include <sstream>

#include "app/configuration/config_plugins/event_colour/src/EventColourComponent.h"
#include "app/configuration/config_plugins/event_colour/src/resource.h"
#include "app/configuration/config_plugins/event_colour/src/EventColourFrameworkView.h"
#include "app/configuration/config_plugins/event_colour/src/EventColourDatabaseAccessor.h"

#include "bus/resources/resource.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{

    // This is the name of the DLL and is used to load resources correctly.
    const CString EventColourComponent::DLL_NAME = "TA_EventColourConfigPlugin.dll";

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

    EventColourComponent::EventColourComponent(const std::string& componentName)
        : m_icon(NULL),
         AbstractComponent(componentName, new EventColourDatabaseAccessor())
    {
        m_icon = LoadIcon(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_MESSAGE_TYPE) );
    }


    EventColourComponent::~EventColourComponent()
    {
        FUNCTION_ENTRY("Destructor");

       /* try
        {
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }*/

        FUNCTION_EXIT;
    }


    HICON EventColourComponent::getIcon()
    {
        FUNCTION_ENTRY("getIcon");
        FUNCTION_EXIT;
        return m_icon;
    }


    CRuntimeClass* EventColourComponent::getComponentView()
    {
        return RUNTIME_CLASS(EventColourFrameworkView);
    }


    std::string EventColourComponent::getDLLName() const
    {
        return std::string(DLL_NAME);
    }


    std::vector<MenuItem> EventColourComponent::getMenuItems()
    {
        return std::vector<MenuItem>();
    }

    std::vector<ToolBarItem> EventColourComponent::getToolBarItems()
    {
        return std::vector<ToolBarItem>();
    }

} // close namespace
