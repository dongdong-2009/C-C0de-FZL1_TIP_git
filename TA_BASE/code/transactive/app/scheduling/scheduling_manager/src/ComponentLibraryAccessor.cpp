/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_manager/src/ComponentLibraryAccessor.cpp $
  * @author:  San Teo
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/08/03 17:02:56 $
  * Last modified by:  $Author: qi.huang $
  * 
  * This class accesses the component libraries to retrieve and set up the Component structure.
  * These can then be retrieved for processing.
  *
  * Adapted from Configuration Editor
  *
  */

#pragma warning(disable:4786)

#include "app/scheduling/scheduling_manager/src/StdAfx.h"
#include "app/scheduling/scheduling_manager/src/ComponentLibraryAccessor.h"
#include "app/scheduling/scheduling_manager/src/IJobDetailsPlugin.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManagerConstants.h"
#include "core/utilities/src/DllHandler.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;
using namespace TA_Base_App::SchedulingManager;

namespace TA_Base_App
{
    ComponentLibraryAccessor::ComponentLibraryAccessor(std::vector<std::string>& jobTypes)
    {
        initDispToComName();

        for (unsigned int i = 0; i < jobTypes.size(); i++)
        {
            Component comp;
            CString temp;
            std::string strComName = dispToComName( jobTypes[i] );

            AfxFormatString1(temp, IDS_DLL_PATH_FORMAT, strComName.c_str());  // set its library path
            comp.path = (LPCTSTR)temp;

            AfxFormatString1(temp, IDS_HELP_PATH_FORMAT, strComName.c_str());  // set its help path
            comp.help = (LPCTSTR)temp;

            comp.plugin = NULL;

            m_componentMap.insert(ComponentMap::value_type(strComName, comp));
        }
    }


    ComponentLibraryAccessor::~ComponentLibraryAccessor()
    {
    }


    IJobDetailsPlugin* ComponentLibraryAccessor::getComponent(std::string& name)
    {
        FUNCTION_ENTRY("getComponent");

        std::string strComName = dispToComName( name );

        // See if we could find the component.  
        TA_ASSERT(m_componentMap.find(strComName) != m_componentMap.end(), "Couldn't find the component");

        Component comp = m_componentMap[strComName];
        // if we've already loaded the component, then just return it.
        if (comp.plugin != NULL)
        {
            return comp.plugin;
        }

        // Load the DLL
        comp.instance = AfxLoadLibrary(comp.path.c_str());
        if (NULL == comp.instance)
	    {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not load the plugin %s", strComName.c_str());
            return NULL;
	    }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "%s plugin loaded successfully", strComName.c_str());

	    // Load the DLL function.
        typedef IJobDetailsPlugin* (*pfnCreateComponent)();

	    pfnCreateComponent DLLCreateComponent = NULL;
	    DLLCreateComponent = reinterpret_cast<pfnCreateComponent>(::GetProcAddress(comp.instance, "createComponent"));
	    if (NULL == DLLCreateComponent)
	    {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not load function in the plugin %s", strComName.c_str());
            FreeLibrary(comp.instance);
		    comp.instance = NULL;
            return NULL;
	    }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "createComponent() function loaded successfully for plugin %s", strComName.c_str());

        // Now call the DLL function to get the IJobDetailsPlugin interface object
        comp.plugin = DLLCreateComponent();

        if ( NULL != comp.plugin )
        {
            // Construct the DLL handler that we can pass when initialise the singletons in the DLLs.
            TA_Base_Core::DllHandler dllHandler;
            comp.plugin->setUpSingletons(dllHandler, strComName);
        }

        FUNCTION_EXIT;
        return comp.plugin;
    }

    std::string ComponentLibraryAccessor::dispToComName( const std::string& strName )
    {
        std::map<std::string, std::string>::iterator itFound = m_mapDispToComName.find( strName );
        if ( itFound != m_mapDispToComName.end() )
        {
            return itFound->second.c_str();
        }

        return strName.c_str();
    }

    void ComponentLibraryAccessor::initDispToComName()
    {
        m_mapDispToComName.insert( std::make_pair( "Ô¤°¸", "Plan" ));
    }
};
