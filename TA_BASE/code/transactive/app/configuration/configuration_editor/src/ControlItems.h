/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ControlItems.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This holds several structures that must be used across multiple classes and libraries.
  */


#if !defined(ControlItems_B10291B2_4630_4496_8F9E_987E091FF3A0__INCLUDED_)
#define ControlItems_B10291B2_4630_4496_8F9E_987E091FF3A0__INCLUDED_


namespace TA_Base_App
{
    typedef void (*HandlerFunctionType)();


    /**
     * MenuItem
     *
     * This holds all information required for the Configuration Editor to add a new menu 
     * item and handle it when it is chosen by the user.
     * When the menu item is clicked the method IConfigurableComponent::menuItemSelected() will
     * be called for the current component.
     */
    struct MenuItem
    {
	    // The menu this item should appear in.
	    CString menu;

	    // This is the actual menu item to add to the menu.
	    CString menuItem;
    
        // This is the caption to show in the status bar when the mouse moves over
        // the menu item.
        CString menuItemCaption;
    };


    /**
     * ToolBarItem
     *
     * This holds all information required for the Configuration Editor to add a new toolbar
     * item and handle it when it is chosen by the user
     * When the button is clicked the method IConfigurableComponent::toolBarItemSelected() will
     * be called for the current component.
     */
    struct ToolBarItem
    {
	    // The name of the button
	    CString name;

	    // The icon to display on the button. This should be the hot icon. The other icon
        // will be determined automatically.
	    HICON icon;

        // This is the caption to show in the status bar when the mouse moves over
        // the toolbar button item.
        CString toolBarItemCaption;
    };

}

#endif // !defined(ControlItems_B10291B2_4630_4496_8F9E_987E091FF3A0__INCLUDED_)
