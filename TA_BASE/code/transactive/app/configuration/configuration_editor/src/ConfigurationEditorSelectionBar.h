/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ConfigurationEditorSelectionBar.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class manipulates the GfxOutBar used by the CMainFrame class. All manipulations to this
  * selection bar have been put in this class.
  */

#if !defined(AFX_ConfigurationEditorSelectionBar_H__E81E79BF_8A84_4604_9377_F3A8C1935E5A__INCLUDED_)
#define AFX_ConfigurationEditorSelectionBar_H__E81E79BF_8A84_4604_9377_F3A8C1935E5A__INCLUDED_

#include "bus/mfc_extensions/src/mfc_extensions.h" // for CGfxOutBarCtrl

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_App
{
    // Forward declarations
    class ComponentLibraryAccessor;
    struct Component;

    class ConfigurationEditorSelectionBar
    {
    public:
        /**
          * Constructor
          *
          * @param CFrameWnd* - This is a pointer to the main frame which will ultimately be our parent
          * @param ComponentLibraryAccessor& - This is passed in so we can retrieve the component details
          *                                    and populate the folders. This object must have already had
          *                                    a call to loadComponents() made on it before being passed in
          * @param CSplitterWnd& - This is the splitter window that the Selection Bar control will be
          *                        created in.
          * @param int - This is the parent ID that will need to be used when the selection bar is created.
          */
        ConfigurationEditorSelectionBar(CFrameWnd* selectionBarParent,
                                        ComponentLibraryAccessor& libraryAccessor,
                                        CSplitterWnd& parentSplitter,
                                        int parentId);

        /**
          * Destructor
          */
        virtual ~ConfigurationEditorSelectionBar();

        void init();

        /**
          * getComponent
          *
          * When items are added to the selection bar they have a Component* set as their data.
          * This takes the selected item id passed in and returns the Component object associated
          * with it.
          *
          * @param int - This is the id of the item the user has clicked in the selection bar
          *
          * @return Component* - This is a pointer to the Component object associated with the
          *                      item the user selected.
          */
        Component* getComponent(int selectedItem);

        /**
          * setIconSizeForSelectionBar
          *
          * This is called when the user has changed the size of the icons in the selection bar.
          * They may have changed the size using the menu items or the size may have been loaded from
          * user settings. So we must pass this change onto the control so it can change the actual
          * size of the icons.
          *
          * @param unsigned long - This is the new size of the icons. This should be compared against the
          *                        constants LARGE_ICON_SIZE and SMALL_ICON_SIZE in ConfigurationEditorConstants.h
          */
        void setIconSizeForSelectionBar(unsigned long iconSize);

        /**
          * getIconSizeForSelectionBar
          *
          * This is called to retrieve the current size of the icons. This would be called when the user
          * preferences need to be saved (or similar).
          *
          * @return unsigned long - This is the size of the icon. This will be LARGE_ICON_SIZE or
          *                         SMALL_ICON_SIZE from ConfigurationEditorConstants.h
          */
        unsigned long getIconSizeForSelectionBar();

        /**
          * updateSelectionBar
          *
          * This will update the selection bar to enable or disable items as necessary.
          */
        void updateSelectionBar();

        /**
         * viewComponent
         *
         * This will simulate a user clicking a folder and then on a component to display
         * component details. Called by setStartupView().
         *
         * @param const Component& - a reference to the Component object to view
         */
        void viewComponent(const Component& comp);

        void RemoveDefaultFolder();

    private:

        /**
          * Copy Constructor
          *
          * Hidden as a private method so it is not used.
          */
        ConfigurationEditorSelectionBar(const ConfigurationEditorSelectionBar&);

        /**
          * Operator =
          *
          * Hidden as a private method so it is not used.
          */
        ConfigurationEditorSelectionBar& operator=(const ConfigurationEditorSelectionBar&);

        /**
          * populateSelectionBar
          *
          * This will populate the selection bar using the components retrieved from m_libraryAccessor.
          * It will retrieve the required components and folders from each component and add them to
          * the bar.
          */
        void populateSelectionBar();

        int              m_parentId;
        CSplitterWnd&    m_parentSplitWnd;

        CGfxOutBarCtrl	m_wndSelectionBar;          // The Outlook style selection bar down the side of the
        // application

        CImageList      m_imageListLarge;           // This is collection of large icons to be displayed in
        // the selection bar.

        CImageList      m_imageListSmall;           // This is collection of small icons to be displayed in
        // the selection bar.

        CFrameWnd*      m_selectionBarParent;       // This is the main frame window that is the ultimate
        // parent of this control

        ComponentLibraryAccessor& m_libraryAccessor; // Used to access the loaded components
    };
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ConfigurationEditorSelectionBar_H__E81E79BF_8A84_4604_9377_F3A8C1935E5A__INCLUDED_)
