/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ConfigurationEditorMenu.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manipulates the menu held by the CMainFrame class. All menu manipulations have
  * been put in this class.
  */


#if !defined(AFX_ConfigurationEditorMenu_H__E81E79BF_8A84_4604_9377_F3A8C1935E5A__INCLUDED_)
#define AFX_ConfigurationEditorMenu_H__E81E79BF_8A84_4604_9377_F3A8C1935E5A__INCLUDED_

#include <vector>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_App
{
    // Forward Declarations
    class ComponentLibraryAccessor;
    struct Component;


    class ConfigurationEditorMenu
    {
    public:

        /**
          * Constructor
          *
          * @param CFrameWnd* - This is a pointer to the main frame which will be the parent window
          *                     containing the menu we are interested in.
          * @param ComponentLibraryAccessor& - This is passed in so we can retrieve the component details
          *                                    and populate the menus. This object must have already had
          *                                    a call to loadComponents() made on it before being passed in
          */
	    ConfigurationEditorMenu(CFrameWnd* menuParent, ComponentLibraryAccessor& componentLibraryAccessor);


        /**
          * Destructor
          */
	    virtual ~ConfigurationEditorMenu();


        /**
          * iconSizeChanged
          *
          * This is called when the user changes the icon size of the icons in the selection bar.
          * This means the View menu must be updated to indicate which icon size is currently used.
          *
          * @param unsigned long - This is the size of the icon being used. This can be compared against
          *                        the constants LARGE_ICON_SIZE and SMALL_ICON_SIZE in
          *                        ConfigurationEditorConstants.h
          */
        void iconSizeChanged(unsigned long iconSize);


        /**
          * onMenuItemSelected
          *
          * This is called when a user selects an item from the menu. This will use the id passed
          * to find the correct method to call for the currently selected component.
          *
          * @param UINT - The ID of the menu item selected
          * @param Component* - This is a pointer to the component the user currently has selected.
          *                     This structure will hold a map of control Id's and functions that need
          *                     to be used by this method.
          */
        void onMenuItemSelected(UINT nID, Component* selectedComponent);

        
        /**
          * onMenuItemUpdated
          *
          * This is called before a menu is displayed to the user. It will determine which menu
          * items should be enabled and disabled.
          *
          * @param CCmdUI* - An object holding the menu item's details and allowing certain actions
          *                  to be performed on it such as enabling and disabling.
          * @param Component* - This is a pointer to the component the user currently has selected.
          *                     This structure can be used to determine if the currently selected
          *                     component needs this menu item or not. Then the menu item can be 
          *                     enabled or disabled accordingly.
          */
        void onMenuItemUpdated(CCmdUI* pCmdUI, Component* selectedComponent);


    private:
        /**
          * Copy Constructor
          *
          * Hidden as a private method so it is not used.
          */
        ConfigurationEditorMenu( const ConfigurationEditorMenu&);


        /**
          * Operator =
          *
          * Hidden as a private method so it is not used.
          */
        ConfigurationEditorMenu& operator=(const ConfigurationEditorMenu&);


        /**
          * populateMenu
          *
          * This will populate the menus using the components retrieved from m_libraryAccessor.
          * It will retrieve the required menus from each component and build up a collection of 
          * menus and menu items needed by each. If two components need the same menu item then 
          * this menu item will only appear once and will not be duplicated.
          */
        void populateMenu();


    public:

        static const unsigned int WM_MENU_BEGIN; // Start ID for new menu items
        static const unsigned int WM_MENU_END;   // End ID for new menu items

    private:

        static const int NEW_MENU_POSITION; // Where to insert new menus

        CFrameWnd* m_menuParent;            // The parent window that holds the menu

        std::vector<CMenu*> m_menus;        // The new menus created.

        ComponentLibraryAccessor& m_libraryAccessor; // Used to access the loaded components

    };
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ConfigurationEditorMenu_H__E81E79BF_8A84_4604_9377_F3A8C1935E5A__INCLUDED_)
