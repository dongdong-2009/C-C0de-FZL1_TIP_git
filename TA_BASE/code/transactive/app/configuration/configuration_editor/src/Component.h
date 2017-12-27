/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/Component.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This holds the Component structure that is used throughout the Configuration Editor
  * and the plugins.
  */

#if !defined(Component_2271D6DD_BC63_4274_9F58_EEBB6EC28DCD__INCLUDED_)
#define Component_2271D6DD_BC63_4274_9F58_EEBB6EC28DCD__INCLUDED_

#include <map>
#include "app/configuration/configuration_editor/src/ControlItems.h"


namespace TA_Base_App
{
    class IConfigurableComponent;

    struct Component
    {
        // This is the folder heading this item falls under in the selection bar
        // Populated by the ComponentLibraryAccessor
	    CString category;

        // This is the name of the component to display in the selection bar and 
        // as the title above the view.
        // Populated by the ComponentLibraryAccessor
	    CString component;
        
        // This holds the help file to use for this component. This can be an empty string indicating
        // that now help file has been configured
        std::string helpFile;

        // This is the component interface that will provide access to the component
        // implementation.
        // Populated by the ComponentLibraryAccessor
        IConfigurableComponent* componentInterface;

        // This is the DLL instance. We need to store this so we can close and free the library
        // when we're done.
	    HINSTANCE libraryInstance;

        // This map holds each controlId associated with this component as well as the
        // caption to disply in the status bar when the control is selected.
        // Populated by the CMainFrame
	    std::map<unsigned int,CString> controlIds;

        // This indicates if this item is accessible to the current user.
        // This will be false if it is not available, and true otherwise.
        bool isAccessible;
    };
}

#endif // !defined(Component_2271D6DD_BC63_4274_9F58_EEBB6EC28DCD__INCLUDED_)
