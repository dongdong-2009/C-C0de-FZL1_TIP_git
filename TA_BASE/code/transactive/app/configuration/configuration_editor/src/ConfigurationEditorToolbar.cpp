/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ConfigurationEditorToolbar.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manipulates the toolbar for the CMainFrame class. All toolbar manipulations have
  * been put in this class.
  */


#include <map>
#include <vector>
#include <algorithm> // used for std::find

#include "app/configuration/configuration_editor/src/stdafx.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorToolBar.h"
#include "app/configuration/configuration_editor/src/resource.h"
#include "app/configuration/configuration_editor/src/Component.h"
#include "app/configuration/configuration_editor/src/ComponentLibraryAccessor.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/configuration_editor/src/IConfigurableComponent.h"

#include "bus/resources/resource.h"

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
using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{
    const unsigned int ConfigurationEditorToolBar::WM_TOOLBAR_BEGIN = 4001;
    const unsigned int ConfigurationEditorToolBar::WM_TOOLBAR_END   = 5000;

    /////////////////////////////////////////////////////////////////////////////
    // ConfigurationEditorToolbar


    ConfigurationEditorToolBar::ConfigurationEditorToolBar(CFrameWnd* toolBarParent, ComponentLibraryAccessor& libraryAccessor)
        : m_toolBarParent(toolBarParent),
          m_libraryAccessor(libraryAccessor)
    {
        FUNCTION_ENTRY("Constructor");

        TA_ASSERT(m_toolBarParent != NULL, "Parent for the toolbar is NULL");

        DWORD style1 = TBSTYLE_FLAT | TBSTYLE_WRAPABLE;
        DWORD style2 = WS_CHILD | CBRS_TOP | CBRS_FLYBY | CBRS_SIZE_DYNAMIC; // | CBRS_TOOLTIPS;
        // Have turned off tooltips as there isn't much point. They just say the same as the button name.

        // Create and load the toolbar
	    m_toolBar.CreateEx(m_toolBarParent, style1, style2);
        m_toolBar.LoadToolBar(IDR_TOOLBAR);

        CRect temp;
        m_toolBar.GetItemRect(0, &temp);
        m_toolBar.SetSizes( CSize(temp.Width() + SMALL_ICON_SIZE, temp.Height() + SMALL_ICON_SIZE), CSize(SMALL_ICON_SIZE,SMALL_ICON_SIZE) );

        // Create the image lists for the icons
        // Start with an image list of size 10 and grow by 10 each time
        m_toolbarImageListHot.Create(SMALL_ICON_SIZE,SMALL_ICON_SIZE,ILC_COLOR32,10,10);
        m_toolbarImageListHot.SetBkColor( GetSysColor(COLOR_3DFACE) );
        m_toolBar.GetToolBarCtrl().SetHotImageList( &m_toolbarImageListHot );

        m_toolbarImageListEnabled.Create(SMALL_ICON_SIZE,SMALL_ICON_SIZE,ILC_COLOR32,10,10);
        m_toolbarImageListEnabled.SetBkColor( GetSysColor(COLOR_3DFACE) );
        m_toolBar.GetToolBarCtrl().SetImageList( &m_toolbarImageListEnabled );
        m_toolBar.GetToolBarCtrl().SetDisabledImageList( &m_toolbarImageListEnabled );

        m_toolBar.ShowWindow(SW_SHOW);
        m_toolBarParent->RecalcLayout(FALSE);

        populateToolBar();
    }


    ConfigurationEditorToolBar::~ConfigurationEditorToolBar()
    {
    }


    void ConfigurationEditorToolBar::populateToolBar()
    {
        FUNCTION_ENTRY("populateToolBar");

        // Delete the emtpy button off the static toolbar
        m_toolBar.GetToolBarCtrl().DeleteButton(0);

        // Now add any static buttons we require
        addButton("Ë¢ÐÂ", LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CONFIG_MENU_RELOAD) ), ID_APP_RELOAD);
        
        // This holds the name of each toolbar required and the icon and control id of each
        // This is to ensure we only get each toolbar item once.
        typedef std::map<CString, std::pair<HICON,int> > ToolBarSetup;
        ToolBarSetup toolbarSetup;
        unsigned int nextControlId = WM_TOOLBAR_BEGIN;

        // Step through each component and retrieve the toolbar items it requires
        for (std::vector<Component>::iterator iter = m_libraryAccessor.getComponents().begin(); iter != m_libraryAccessor.getComponents().end(); ++iter)
        {
            TA_ASSERT(iter->componentInterface != NULL, "Stored component interface is NULL");

            // Retrieve the toolbar items for this component and add them to the list
            std::vector<ToolBarItem> items = iter->componentInterface->getToolBarItems();

            // Step through each toolbar item in this list and see if we already have it or if it's 
            // new to us.
            for (std::vector<ToolBarItem>::iterator toolbarItem = items.begin(); toolbarItem != items.end(); ++toolbarItem)
            {
                 ToolBarSetup::iterator matchingItem = toolbarSetup.find( toolbarItem->name );

                 if( matchingItem == toolbarSetup.end() )
                 {
                     // This is a new toolbar item
                     // Add it to the map of all toolbar items
                     std::pair<HICON,int> item;
                     item.first = toolbarItem->icon;
                     item.second = nextControlId;
                     toolbarSetup.insert( ToolBarSetup::value_type( toolbarItem->name, item) );

                     // Now store the control id with the component
                     iter->controlIds.insert( std::map<unsigned int,CString>::value_type(nextControlId,toolbarItem->toolBarItemCaption));

                     ++nextControlId;
                 }
                 else
                 {
                     // This toolbar item already exists
                     // So store the control id with the component
                     iter->controlIds.insert( std::map<unsigned int,CString>::value_type(matchingItem->second.second,toolbarItem->toolBarItemCaption));

                } // end if toolbar item is found
            }  // end for each toolbar item
        } // end for each component

        for (ToolBarSetup::iterator item = toolbarSetup.begin(); item != toolbarSetup.end(); ++item)
        {
            // Quick explanation of iterators used below:
            // item->first = Button Name
            // item->second.first = The HICON
            // item->second.second = The control Id
            addButton( item->first, item->second.first, item->second.second );
        }

        FUNCTION_EXIT;
    }


    void ConfigurationEditorToolBar::addButton(const CString& buttonName, const HICON buttonIcon, const int command )
    {
        CToolBarCtrl& toolbar = m_toolBar.GetToolBarCtrl();

    	TBBUTTON tbButton;
        tbButton.fsState = TBSTATE_ENABLED;
        tbButton.fsStyle = TBSTYLE_BUTTON;
        tbButton.dwData = 0;
        tbButton.iString = toolbar.AddStrings(buttonName);

        // We add the coloured icon first so we make sure we're converting the icon of the correct
        // size into grey.
        tbButton.iBitmap = m_toolbarImageListHot.Add( buttonIcon );
        
        // Convert the coloured icon to grey for the enabled icon
        HICON greyIcon = createGreyscaleIcon( m_toolbarImageListHot.ExtractIcon(tbButton.iBitmap) );

        if (greyIcon == NULL)
        {
            // If we didn't create a grey icon then just use the coloured one.
            greyIcon = buttonIcon;
        }

        m_toolbarImageListEnabled.Add( greyIcon );

        tbButton.idCommand = command;
        TA_VERIFY(toolbar.AddButtons(1, &tbButton),"Could not add button to the toolbar control");

        // Now associate the button name with the button so we can retrieve it later. Just adding
        // the name to the string table will ensure the text displays on the button but it is
        // then difficult to retrieve the text later on
        m_toolBar.SetButtonText( m_toolBar.CommandToIndex(command), buttonName );
    }
    

    void ConfigurationEditorToolBar::onToolBarItemSelected(UINT nID, Component* selectedComponent)
    {
        FUNCTION_ENTRY("onToolBarItemSelected");

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

        // Retrieve the button text so we can pass it to the component.
        CString buttonName;
        m_toolBar.GetButtonText(m_toolBar.CommandToIndex(nID), buttonName);
 
        // Now alert the component that this button has been chosen
        IConfigurableComponent* configurable = selectedComponent->componentInterface;
        if (configurable == NULL)
        {
            showMsgBox(IDS_UE_030058, menuItemStr);
            FUNCTION_EXIT;
            return;
        }

        configurable->itemSelected(buttonName);

        FUNCTION_EXIT;
    }


    void ConfigurationEditorToolBar::onToolBarItemUpdated(CCmdUI* pCmdUI, Component* selectedComponent)
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


    CString ConfigurationEditorToolBar::getToolTipText(UINT nID)
    {
        CString buttonName;
        m_toolBar.GetButtonText(m_toolBar.CommandToIndex(nID), buttonName);

        return buttonName;
    }


    HICON ConfigurationEditorToolBar::createGreyscaleIcon(HICON colourIcon)
    {
        FUNCTION_ENTRY("createGreyscaleIcon");

	    ICONINFO	csII;
	    BOOL bRetValue = ::GetIconInfo(colourIcon, &csII);
	    if (bRetValue == FALSE)
        {
            FUNCTION_EXIT;
		    return NULL;
        }

        HDC hMainDC = ::GetDC(m_toolBarParent->m_hWnd);
	    HDC hMemDC1 = ::CreateCompatibleDC(hMainDC);
	    HDC hMemDC2 = ::CreateCompatibleDC(hMainDC);
	    if (hMainDC == NULL || hMemDC1 == NULL || hMemDC2 == NULL)
        {
            FUNCTION_EXIT;
		    return NULL;
        }
  
        ICONINFO    csGrayII;
	    BITMAP		bmp;
        HICON       coldIcon;
	    if (::GetObject(csII.hbmColor, sizeof(BITMAP), &bmp))
	    {
		    csGrayII.hbmColor = ::CreateBitmap(csII.xHotspot*2, csII.yHotspot*2, bmp.bmPlanes, bmp.bmBitsPixel, NULL);
		    if (csGrayII.hbmColor)
		    {
			    HBITMAP hOldBmp1 = (HBITMAP)::SelectObject(hMemDC1, csII.hbmColor);
			    HBITMAP hOldBmp2 = (HBITMAP)::SelectObject(hMemDC2, csGrayII.hbmColor);

			    ::BitBlt(hMemDC2, 0, 0, csII.xHotspot*2, csII.yHotspot*2, hMemDC1, 0, 0, SRCCOPY);

                CString newIcon("\n");

			    for (DWORD dwLoopY = 0; dwLoopY < csII.yHotspot*2; dwLoopY++)
			    {
				    for (DWORD dwLoopX = 0; dwLoopX < csII.xHotspot*2; dwLoopX++)
				    {
                        COLORREF crPixel = ::GetPixel(hMemDC2, dwLoopX, dwLoopY);

                        BYTE byNewPixel = (BYTE)((GetRValue(crPixel) * 0.299) + (GetGValue(crPixel) * 0.587) + (GetBValue(crPixel) * 0.114));
					    if (crPixel)
                        {
                            if (RGB(byNewPixel, byNewPixel, byNewPixel) > RGB(200,200,200))
                            {
                                newIcon += " ";
                            }
                            else if (RGB(byNewPixel, byNewPixel, byNewPixel) <= RGB(200,200,200) && RGB(byNewPixel, byNewPixel, byNewPixel) > RGB(150,150,150))
                            {
                                newIcon += "-";
                            }
                            else if (RGB(byNewPixel, byNewPixel, byNewPixel) <= RGB(150,150,150) && RGB(byNewPixel, byNewPixel, byNewPixel) > RGB(100,100,100))
                            {
                                newIcon += "+";
                            }
                            else if (RGB(byNewPixel, byNewPixel, byNewPixel) <= RGB(100,100,100) && RGB(byNewPixel, byNewPixel, byNewPixel) > RGB(50,50,50))
                            {
                                newIcon += "X";
                            }
                            else
                            {
                                newIcon += "W";
                            }

                            ::SetPixel(hMemDC2, dwLoopX, dwLoopY, RGB(byNewPixel, byNewPixel, byNewPixel));
                        }
				    } // for
                    newIcon += "\n";
			    } // for

//                TRACE(newIcon);
//                TRACE("\n\n");

			    ::SelectObject(hMemDC1, hOldBmp1);
			    ::SelectObject(hMemDC2, hOldBmp2);

			    csGrayII.hbmMask = csII.hbmMask;

			    csGrayII.fIcon = TRUE;
			    coldIcon = ::CreateIconIndirect(&csGrayII);
		    } // if

		    ::DeleteObject(csGrayII.hbmColor);
	    } // if

	    ::DeleteObject(csII.hbmColor);
	    ::DeleteObject(csII.hbmMask);
	    ::DeleteDC(hMemDC1);
	    ::DeleteDC(hMemDC2);
	    ::ReleaseDC(m_toolBarParent->m_hWnd, hMainDC);

        FUNCTION_EXIT;
        return coldIcon;
    }

}
