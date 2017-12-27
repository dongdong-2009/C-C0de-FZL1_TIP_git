/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/banner/src/ComponentLibraryAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class accesses the component libraries to retrieve and set up the Component structure.
  * These can then be retrieved for processing.
  */

#pragma warning(disable:4786)

#include "app/system_control/banner/src/ComponentLibraryAccessor.h"
#include "app/system_control/banner/src/Component.h"
#include "bus/banner_framework/src/IBannerPage.h"
#include "bus/banner_framework/src/BannerFrameworkDialog.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/DllHandler.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include <vector>
#include <string>
#include <algorithm>

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{

    ComponentLibraryAccessor::ComponentLibraryAccessor()
    {

    }


    ComponentLibraryAccessor::~ComponentLibraryAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            for (std::vector<Component>::iterator thisComponent = m_components.begin(); thisComponent != m_components.end(); ++thisComponent )
            {
                // Free'ing the library seems to hang the application so we will leave it commented out
                // for now
                //if (thisComponent->libraryInstance != NULL)
                //{
                    //FreeLibrary( thisComponent->libraryInstance );
                //}

                if (thisComponent->bannerPage != NULL)
                {
                    delete thisComponent->bannerPage;
                    thisComponent->bannerPage = NULL;
                }
            }            
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }


    std::vector<Component> ComponentLibraryAccessor::getComponents(int xPosition, int yPosition, 
            int width, int height, TA_Base_Bus::BannerFrameworkDialog* parentDialog)
    {
        FUNCTION_ENTRY("getComponents");

        CString unavailableComponents = "";

        std::vector<TA_Base_Core::IEntityData*> pages;

        // Get the banner's child entities
        try
        {
            pages = TA_Base_Core::EntityAccessFactory::getInstance().getChildEntitiesOf(
                TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME));

            // Sort them by pkey so that they can be displayed in that order.
            std::sort(pages.begin(), pages.end(), EntitySort());
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No pages will be shown in the Banner");
            return m_components;
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No pages will be shown in the Banner");
            return m_components;
        }
  
        for (std::vector<TA_Base_Core::IEntityData*>::iterator iter = pages.begin(); iter != pages.end(); ++iter)
        {
            // Load the DLL. The entity's address field should contain the DLL name.
            Component currentComponent;
            currentComponent.entityName = (*iter)->getName();
            currentComponent.path = (*iter)->getAddress();
            currentComponent.instance = AfxLoadLibrary(currentComponent.path.c_str());

            if ( 0 == currentComponent.instance )
		    {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Could not load the plugin %s", currentComponent.path.c_str() );
                unavailableComponents += _T(currentComponent.path.c_str());
                unavailableComponents += _T("\n");
                continue;
		    }

            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Dynamic link library %s loaded successfully", currentComponent.path.c_str() );

		    // Load the DLL function.
			    
            typedef TA_Base_Bus::IBannerPage* (*pfnCreateComponent)(const std::string&, int, int, int, int, 
                                                                    TA_Base_Bus::BannerFrameworkDialog*);
                                                                    

		    pfnCreateComponent DLLCreateComponent;
		    DLLCreateComponent = (pfnCreateComponent) ::GetProcAddress( currentComponent.instance, "createComponent" );
		    if ( NULL == DLLCreateComponent )
		    {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Could not load function in the plugin %s", currentComponent.path.c_str() );
                unavailableComponents += _T(currentComponent.path.c_str());
                unavailableComponents += _T("\n");

                FreeLibrary( currentComponent.instance );
			    currentComponent.instance = 0;
                continue;
		    }

            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "createComponent() function loaded successfully for plugin %s", currentComponent.path.c_str() );

            // Now call the DLL function to get the IBannerPage interface object
            currentComponent.bannerPage = DLLCreateComponent( currentComponent.entityName, xPosition, yPosition, width, height, parentDialog);
            TA_ASSERT(currentComponent.bannerPage != NULL, "The component created is NULL");

            // Construct the DLL handler that we can pass when initialise the singletons in the DLLs.
            TA_Base_Core::DllHandler dllHandler;
            currentComponent.bannerPage->setUpSingletons(dllHandler, currentComponent.entityName,
                                                         currentComponent.path.c_str());

            // Finally we can add the component to our list
            m_components.push_back(currentComponent);
        }

        if ( !unavailableComponents.IsEmpty() )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << unavailableComponents;
            userMsg.showMsgBox(IDS_UW_020011);
        }

        return m_components;

        FUNCTION_EXIT;
    }

} // namespace TA_Base_App
