/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ConfigurationEditorMenu.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manipulates the menu held by the CMainFrame class. All menu manipulations have
  * been put in this class.
  */

#include <vector>
#include <map>
#include <algorithm> // used for std::find

#include "app/configuration/configuration_editor/src/stdafx.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorMenu.h"

#include "app/configuration/configuration_editor/src/ComponentLibraryAccessor.h"
#include "app/configuration/configuration_editor/src/resource.h"
#include "app/configuration/configuration_editor/src/IConfigurableComponent.h"
#include "app/configuration/configuration_editor/src/Component.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;

namespace TA_Base_App
{

    const unsigned int ConfigurationEditorMenu::WM_MENU_BEGIN = 3001; // Start ID for new menu items
    const unsigned int ConfigurationEditorMenu::WM_MENU_END   = 4000; // End ID for new menu items

    const int ConfigurationEditorMenu::NEW_MENU_POSITION = 1; // After File menu but before View and Help menus


    ConfigurationEditorMenu::ConfigurationEditorMenu(CFrameWnd* menuParent,
                                                     ComponentLibraryAccessor& componentLibraryAccessor)
        : m_menuParent(menuParent), m_libraryAccessor(componentLibraryAccessor)
    {
        FUNCTION_ENTRY("Constructor");

        TA_ASSERT(m_menuParent != NULL, "Parent for the menu is NULL");

        // Change the about menu item to have the correct application name in it.
        CMenu* mainMenu = m_menuParent->GetMenu();
        if (mainMenu != NULL)
        {
            CString aboutString = "&About ";
            aboutString += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            aboutString += "...";
            mainMenu->ModifyMenu(ID_APP_ABOUT,MF_BYCOMMAND|MF_STRING,ID_APP_ABOUT,aboutString);
        }

        populateMenu();

        FUNCTION_EXIT;
    }


    ConfigurationEditorMenu::~ConfigurationEditorMenu()
    {
        std::vector<CMenu*>::iterator iter = m_menus.begin();
        while (iter != m_menus.end() )
        {
            if (*iter != NULL)
            {
                (*iter)->DestroyMenu();
                delete *iter;
                *iter = NULL;
            }
            ++iter;
        }
    }


    void ConfigurationEditorMenu::populateMenu()
    {
        FUNCTION_ENTRY("populateMenu");

        // We use the following maps to ensure we only get each menu and menu item once

        // This holds the name of the menu item and the control id for it
        typedef std::map< CString, int> MenuItemSetup;
        // This holds the names of the new menus and the items in each of them.
        typedef std::map<CString, MenuItemSetup > MenuSetup;
        
        MenuSetup menuSetup;

        UINT nextControlId = WM_MENU_BEGIN;

        // Step through each component and retrieve the menus it requires
        for (std::vector<Component>::iterator iter = m_libraryAccessor.getComponents().begin();
             iter != m_libraryAccessor.getComponents().end();
             ++iter)
        {

            TA_ASSERT(iter->componentInterface != NULL, "Component interface is NULL");

            // Retrieve the menu items for this component and add them to the list
            std::vector<MenuItem> items = iter->componentInterface->getMenuItems();

            // Step through each menu item in this list and see if we already have it or if it's 
            // new to us.
            for (std::vector<MenuItem>::iterator menuItem = items.begin();
                 menuItem != items.end();
                 ++menuItem
                )
            {
                 // Search to see if the menu is already in the map
                 MenuSetup::iterator matchingMenu = menuSetup.find( menuItem->menu );

                 if( matchingMenu == menuSetup.end() )
                 {
                     // This menu isn't in the map so it is all new

                     // Because this is new we need to setup a new map for this menu to hold the
                     // menu items. We will add this menu item to it.
                     MenuItemSetup itemSetup;
                     itemSetup.insert( MenuItemSetup::value_type( menuItem->menuItem, nextControlId) );

                     // Now add this new menu to the full list of menus
                     menuSetup.insert( MenuSetup::value_type(menuItem->menu, itemSetup) );

                     // Now store the control id with the component
                     iter->controlIds.insert( std::map<unsigned int,CString>::value_type(nextControlId,menuItem->menuItemCaption));

                     // Increment so next time we get a new and unique control Id
                     ++nextControlId;
                 }
                 else
                 {
                     // This menu already exists so now check to see if the subitem is in there
                     MenuItemSetup::iterator matchingMenuItem = matchingMenu->second.find( menuItem->menuItem );

                     if ( matchingMenuItem == matchingMenu->second.end() )
                     {
                         // This menu item is new so add it to the map for this menu
                         matchingMenu->second.insert( MenuItemSetup::value_type( menuItem->menuItem, nextControlId) );
                         
                         // Now store the control id with the component
                         iter->controlIds.insert( std::map<unsigned int,CString>::value_type(nextControlId,menuItem->menuItemCaption));

                         // Increment so next time we get a new and unique control Id
                         ++nextControlId;
                     }
                     else
                     {
                         // This menu item already exists
                         // So store the control id with the component
                         iter->controlIds.insert( std::map<unsigned int,CString>::value_type(matchingMenuItem->second,menuItem->menuItemCaption));
                     }
                } // end if menu item is found
            }  // end for each menu item
        } // end for each component


        // Now retrive the existing menu so we can add the new menus to it.
        CMenu* mainMenu = m_menuParent->GetMenu();
        TA_ASSERT( mainMenu != NULL, "Could not retrieve the main menu for this frame");

        // Now we iterate through the maps we've setup and add the real menus
        for (MenuSetup::iterator menuIter = menuSetup.begin();
              menuIter != menuSetup.end();
              ++menuIter)
        {
            // Create the new menu and store it
            CMenu* newMenu = new CMenu();
            m_menus.push_back(newMenu);
            newMenu->CreateMenu();

            // Now add the menu items to that menu
            for (MenuItemSetup::iterator itemIter = menuIter->second.begin();
                 itemIter != menuIter->second.end();
                 ++itemIter
                )
            {
                newMenu->AppendMenu(MF_STRING,itemIter->second,itemIter->first);
            }

            // Now insert the menu into the main menu bar
            mainMenu->InsertMenu(NEW_MENU_POSITION,MF_POPUP|MF_BYPOSITION,(UINT)newMenu->m_hMenu, menuIter->first);
        }

        FUNCTION_EXIT;
    }


    void ConfigurationEditorMenu::onMenuItemSelected(UINT nID, Component* selectedComponent)
    {
        FUNCTION_ENTRY("onMenuItemSelected");

        CString menuItemStr;
        menuItemStr.LoadString(IDS_MENU_ITEM);

        if (selectedComponent == NULL)
        {
            showMsgBox(IDS_UE_030057, menuItemStr);
            FUNCTION_EXIT;
            return;
        }
        
        std::map<unsigned int, CString>::iterator matching = selectedComponent->controlIds.find( nID );
        if (selectedComponent->controlIds.end() == matching )
        {
            showMsgBox(IDS_UE_030057, menuItemStr);
            FUNCTION_EXIT;
            return;
        }

        CMenu* menu = m_menuParent->GetMenu();
        if (menu == NULL)
        {
            showMsgBox(IDS_UE_030058, menuItemStr);
            FUNCTION_EXIT;
            return;
        }

        // Retrieve the text on this menu item
        CString menuItemName;
        menu->GetMenuString(nID, menuItemName, MF_BYCOMMAND);

        // Now alert the component that this menu item has been chosen
        IConfigurableComponent* configurable = selectedComponent->componentInterface;
        if (configurable == NULL)
        {
            UINT selectedButton = showMsgBox(IDS_UE_030058, menuItemStr);
            FUNCTION_EXIT;
            return;
        }

        configurable->itemSelected(menuItemName);

        FUNCTION_EXIT;
    }


    void ConfigurationEditorMenu::onMenuItemUpdated(CCmdUI* pCmdUI, Component* selectedComponent)
    {
        if (pCmdUI == NULL)
        {
            return;
        }

        if (selectedComponent == NULL)
        {
            // IF this is NULL then no component is selected so we disable the menu item
            pCmdUI->Enable(FALSE);
            return;
        }

        std::map<unsigned int, CString>::iterator matching = selectedComponent->controlIds.find( pCmdUI->m_nID );
        if (selectedComponent->controlIds.end() == matching )
        {
            // If we cannot find this control ID in our list for the selected component then we disable the
            // menu item.
            pCmdUI->Enable(FALSE);
            return;
        }

        // Otherwise this control Id is used for this component so we enable it
        pCmdUI->Enable(TRUE);
    }


    void ConfigurationEditorMenu::iconSizeChanged(unsigned long iconSize)
    {
        FUNCTION_ENTRY("iconSizeChanged");

        CMenu* mainMenu = m_menuParent->GetMenu();
        if (mainMenu == NULL)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Menu retreived from the frame is NULL");
            FUNCTION_EXIT;
            return;
        }

        long menuItemToSelect = ID_VIEW_LARGEICONS;

        switch(iconSize)
        {
            case(TA_Configuration::LARGE_ICON_SIZE):
                menuItemToSelect = ID_VIEW_LARGEICONS;
                break;

            case(TA_Configuration::SMALL_ICON_SIZE):
                menuItemToSelect = ID_VIEW_SMALLICONS;
                break;
        }

        // The first two parameters indicate that ID_VIEW_SMALLICONS and ID_VIEW_LARGEICONS are grouped
        // together (as radio buttons) and only one should be selected at a time.
        mainMenu->CheckMenuRadioItem(ID_VIEW_SMALLICONS,ID_VIEW_LARGEICONS,menuItemToSelect,MF_BYCOMMAND);

        FUNCTION_EXIT;
    }



}
