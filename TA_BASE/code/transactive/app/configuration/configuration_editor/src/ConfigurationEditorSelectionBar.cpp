/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ConfigurationEditorSelectionBar.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class manipulates the GfxOutBar used by the CMainFrame class. All manipulations to this
  * selection bar have been put in this class.
  */
#include "app/configuration/configuration_editor/src/stdafx.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorSelectionBar.h"
#include "app/configuration/configuration_editor/src/MainFrm.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/configuration_editor/src/resource.h"
#include "app/configuration/configuration_editor/src/Component.h"
#include "app/configuration/configuration_editor/src/IConfigurableComponent.h"
#include "app/configuration/configuration_editor/src/ComponentLibraryAccessor.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include <boost/foreach.hpp>
#include <map>

#ifdef _DEBUG
    #define new DEBUG_NEW
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;
using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // ConfigurationEditorSelectionBar

    ConfigurationEditorSelectionBar::ConfigurationEditorSelectionBar(CFrameWnd* selectionBarParent,
            ComponentLibraryAccessor& libraryAccessor,
            CSplitterWnd& parentSplitter,
            int parentId)
        : m_selectionBarParent(selectionBarParent),
          m_libraryAccessor(libraryAccessor),
          m_parentSplitWnd(parentSplitter),
          m_parentId(parentId)
    {
        FUNCTION_ENTRY("Constructor");

        TA_ASSERT(m_selectionBarParent != NULL, "Parent for the menu is NULL");

        // Here we create the outbar control; we give as id the parent splitter pane id here
        // For flags we want animation but we do not want renaming, removing or changing of categories.
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Creating the selection bar");
        m_wndSelectionBar.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), &parentSplitter, parentId, CGfxOutBarCtrl::fAnimation | CGfxOutBarCtrl::fSelHighlight);

        // Tell the control to send message to the parent window (the mainframe) and not to its real
        // parent (the splitter)
        m_wndSelectionBar.SetOwner(m_selectionBarParent);

        // Here we create the imagelists for the control
        // Start with an image list of size 10 and grow by 10 each time
        m_imageListLarge.Create(LARGE_ICON_SIZE, LARGE_ICON_SIZE, ILC_COLOR32, 10, 10);
        m_imageListLarge.SetBkColor(GetSysColor(COLOR_APPWORKSPACE));

        m_imageListSmall.Create(SMALL_ICON_SIZE, SMALL_ICON_SIZE, ILC_COLOR32, 10, 10);
        m_imageListSmall.SetBkColor(GetSysColor(COLOR_APPWORKSPACE));

        // and we link them to the control
        m_wndSelectionBar.SetImageList(&m_imageListLarge, CGfxOutBarCtrl::fLargeIcon);
        m_wndSelectionBar.SetImageList(&m_imageListSmall, CGfxOutBarCtrl::fSmallIcon);

        // Set the font in the selection bar
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Setting the font on the selection bar");
        CFont arialBold;
        arialBold.CreatePointFont(FONTSIZE_FOR_CONTROLS, TYPEFACE_FOR_CONTROLS);
        m_wndSelectionBar.SetFont(&arialBold);

        //populateSelectionBar();

        m_wndSelectionBar.AddFolder("╪сть...", -1);//add one temp folder
        FUNCTION_EXIT;
    }

    void ConfigurationEditorSelectionBar::init()
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ConfigurationEditorSelectionBar::init - begin");
        //m_wndSelectionBar.RemoveFolder(0);
        populateSelectionBar();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ConfigurationEditorSelectionBar::init - end");
    }

    void ConfigurationEditorSelectionBar::RemoveDefaultFolder()
    {
        m_wndSelectionBar.RemoveFolder(0);
        m_wndSelectionBar.SetSelFolder(0);
    }

    ConfigurationEditorSelectionBar::~ConfigurationEditorSelectionBar()
    {
    }

    void ConfigurationEditorSelectionBar::populateSelectionBar()
    {
        FUNCTION_ENTRY("populateSelectionBar");

        // This holds the name of each folder in the selection bar as well as the folder id associated
        // with it. This is to ensure we only get a single folder by each name
        std::map<CString, int> foldersAndIds;
        std::vector<Component>& components = m_libraryAccessor.getComponents();
        BOOST_FOREACH(Component& component, components)
        {
            TA_ASSERT(component.componentInterface != NULL, "Stored component interface is NULL");

            // Retrieve the icon for this component and add it to the image lists.
            // We must store where it was added so we can reference it later
            HICON tempIcon = component.componentInterface->getIcon();

            int iconPosition = m_imageListLarge.Add(tempIcon);
            m_imageListSmall.Add(tempIcon);

            // Now check to see if this folder has already been created.
            std::map<CString, int>::iterator matchingFolder = foldersAndIds.find(component.category);

            // This will be set to contain the folder number where the component should be inserted.
            int matchingFolderNumber;

            if (matchingFolder == foldersAndIds.end())
            {
                // This is a new folder so add it and store it in the map
                matchingFolderNumber = m_wndSelectionBar.AddFolder(component.category, -1);
                foldersAndIds.insert(std::map<CString, int>::value_type(component.category, matchingFolderNumber));
            }
            else
            {
                // This folder already exists so we just grab it's index number
                matchingFolderNumber = matchingFolder->second;
            }

            // Now we can insert our new item. -1 indicates we want this item to go at the end of the list.
            // We set the associated data with the item to be a pointer to the Component structure holding all
            // required information for this component.
            int position = m_wndSelectionBar.InsertItem(matchingFolderNumber, -1, component.component, iconPosition, reinterpret_cast<DWORD>(&component));

            //m_wndSelectionBar.DisableItem(position, matchingFolderNumber, !component.isAccessible);
            m_wndSelectionBar.DisableItem(position, matchingFolderNumber - 1, !component.isAccessible); // matchingFolderNumber - 1, for temp folder
        }

        FUNCTION_EXIT;
    }

    Component* ConfigurationEditorSelectionBar::getComponent(int selectedItem)
    {
        FUNCTION_ENTRY("getComponent");

        // The data associated with each item is the Component*
        int data = m_wndSelectionBar.GetItemData(selectedItem);

        if (data == 0)
        {
            return NULL;
        }

        Component* tempComp = reinterpret_cast<Component*>(data);

        FUNCTION_EXIT;
        return tempComp;
    }

    void ConfigurationEditorSelectionBar::setIconSizeForSelectionBar(unsigned long iconSize)
    {
        FUNCTION_ENTRY("setIconSizeForSelectionBar");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Icon size passed is %d", iconSize);

        if (iconSize == LARGE_ICON_SIZE)
        {
            m_wndSelectionBar.SetSmallIconView(false);
        }
        else
        {
            m_wndSelectionBar.SetSmallIconView(true);
        }

        FUNCTION_EXIT;
    }

    unsigned long ConfigurationEditorSelectionBar::getIconSizeForSelectionBar()
    {
        FUNCTION_ENTRY("getIconSizeForSelectionBar");

        if (m_wndSelectionBar.IsSmallIconView())
        {
            FUNCTION_EXIT;
            return SMALL_ICON_SIZE;
        }

        FUNCTION_EXIT;
        return LARGE_ICON_SIZE;
    }

    void ConfigurationEditorSelectionBar::updateSelectionBar()
    {
        FUNCTION_ENTRY("repopulateSelectionBar");

        for (int folder = 0; folder < m_wndSelectionBar.GetFolderCount(); ++folder)
        {
            for (int index = 0; index < m_wndSelectionBar.GetItemCount(folder); ++index)
            {
                Component* component = reinterpret_cast<Component*>(m_wndSelectionBar.GetItemData(index, folder));

                if (component != NULL)
                {
                    m_wndSelectionBar.DisableItem(index, folder, !component->isAccessible);
                }
            }
        }

        m_wndSelectionBar.SetSelFolder(m_wndSelectionBar.GetSelFolder());
        FUNCTION_EXIT;
    }

    void ConfigurationEditorSelectionBar::viewComponent(const Component& comp)
    {
        FUNCTION_ENTRY("viewComponent");

        CString category = comp.category;
        CString component = comp.component;

        int numFolders = m_wndSelectionBar.GetFolderCount();

        // Search through and find a matching folder
        for (int folderIndex = 0; folderIndex < numFolders; folderIndex++)
        {
            CGfxOutBarCtrl::CBarFolder* folder = m_wndSelectionBar.GetFolder(folderIndex);

            if (category == folder->cName)
            {
                m_wndSelectionBar.SetSelFolder(folderIndex);

                // we know the folder... now search and find a matching component...
                int numItems = folder->GetItemCount();

                for (int itemIndex = 0; itemIndex < numItems; itemIndex++)
                {
                    CGfxOutBarCtrl::CBarItem* item = folder->GetItem(itemIndex);

                    if (component == item->cItem)
                    {
                        // we have found the component to load... so now we can simulate as if the
                        // user just clicked on it
                        CMainFrame* mainView = dynamic_cast<CMainFrame*>(m_selectionBarParent);

                        if (mainView != NULL)
                        {
                            mainView->displayComponentDetails(itemIndex);
                        }

                        FUNCTION_EXIT;
                        return;
                    }
                }
            }
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Specified component was not found so the startup screen is not changing");
        FUNCTION_EXIT;
    }
}
