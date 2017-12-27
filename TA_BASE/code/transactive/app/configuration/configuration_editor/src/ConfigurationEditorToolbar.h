/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ConfigurationEditorToolbar.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manipulates the toolbar for the CMainFrame class. All toolbar manipulations have
  * been put in this class.
  */


#if !defined(AFX_CONFIGURATIONEDITORTOOLBAR_H__E81E79BF_8A84_4604_9377_F3A8C1935E5A__INCLUDED_)
#define AFX_CONFIGURATIONEDITORTOOLBAR_H__E81E79BF_8A84_4604_9377_F3A8C1935E5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_App
{
    // Forward Declarations
    class ComponentLibraryAccessor;
    struct Component;

    class ConfigurationEditorToolBar
    {
    public:
        /**
          * Constructor
          *
          * @param CFrameWnd* - This is a pointer to the main frame which will ultimately be our parent
          * @param ComponentLibraryAccessor& - This is passed in so we can retrieve the component details
          *                                    and populate the toolbar. This object must have already had
          *                                    a call to loadComponents() made on it before being passed in
          */
	    ConfigurationEditorToolBar(CFrameWnd* toolBarParent, ComponentLibraryAccessor& libraryAccessor);


        /** 
          * Destructor
          */
	    virtual ~ConfigurationEditorToolBar();


        /**
          * onToolBarItemSelected
          *
          * This is called when a user selects an item from the toolbar. This will use the id passed
          * to find the correct method to call for the currently selected component.
          *
          * @param UINT - The ID of the toolbar item selected
          * @param Component* - This is a pointer to the component the user currently has selected.
          *                     This structure will hold a map of control Id's and functions that need
          *                     to be used by this method.
          */
        void onToolBarItemSelected(UINT nID, Component* selectedComponent);


        /**
          * onToolBarItemUpdated
          *
          * This is called before the toolbar is drawn. It will determine which toolbar
          * items should be enabled and disabled.
          *
          * @param CCmdUI* - An object holding the toolbar item's details and allowing certain actions
          *                  to be performed on it such as enabling and disabling.
          * @param Component* - This is a pointer to the component the user currently has selected.
          *                     This structure can be used to determine if the currently selected
          *                     component needs this menu item or not. Then the menu item can be 
          *                     enabled or disabled accordingly.
          */
        void onToolBarItemUpdated(CCmdUI* pCmdUI, Component* selectedComponent);


        /**
          * getToolTipText
          *
          * If tool tips are turned on (in the constructor) then when the user moves over a button
          * this method must return the tooltip to display for the chosen button.
          *
          * @param UINT - The ID of the toolbar item moved over
          *
          * @return CString - The text to display in the tool tip
          */
        CString getToolTipText(UINT nID);
         /* populateToolBar
          *
          * This will populate the toolbar using the components retrieved from m_libraryAccessor.
          * It will retrieve the required toolbar items from each component and build up a collection of 
          * toolbar items needed by each. If two components need the same toolbar item then 
          * this toolbar item will only appear once and will not be duplicated.
          */
        void populateToolBar();

    private:
        /**
          * Copy Constructor
          *
          * Hidden as a private method so it is not used.
          */
        ConfigurationEditorToolBar( const ConfigurationEditorToolBar&);


        /**
          * Operator =
          *
          * Hidden as a private method so it is not used.
          */
        ConfigurationEditorToolBar& operator=(const ConfigurationEditorToolBar&);





        /**
          * addButton
          *
          * This will add a button to the toolbar.
          *
          * @param const CString& - This is the name of the button and will be displayed under the icon
          * @param HICON - The hot icon for the button.
          * @param int - The command id for this button.
          */
        void addButton(const CString& buttonName, const HICON buttonIcon, const int command );


        /**
	     * CreateGreyscaleIcon()
	     *
	     * This function will greate a grey version of the hot icon passed in automatically.
          * 
          * @param HICON - A coloured icon to convert to grey-scale
          *
          * @return HICON - A grey-scale icon matching the passed-in hot icon. If an error
          *                 occurs this will be NULL.
	     */
        HICON createGreyscaleIcon(HICON colourIcon);


    public:

        static const unsigned int WM_TOOLBAR_BEGIN;  // Start ID for new toolbar items
        static const unsigned int WM_TOOLBAR_END;    // End ID for new toolbar items

    private:

        CToolBar m_toolBar;     // This is the actual toolbar showing in the window

        CFrameWnd* m_toolBarParent;  // This is the parent of the toolbar. The main frame window

        ComponentLibraryAccessor& m_libraryAccessor;  // Used to access the loaded components

        CImageList m_toolbarImageListHot;       // This image list holds icons for when the toolbar
                                                // buttons are hot (ie the user has their mouse over them)

        CImageList m_toolbarImageListEnabled;   // This image list holds the icons for when the toolbar
                                                // buttons are enabled (ie the button is accessible but
                                                // the user does not have their mouse over it.

    };
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGURATIONEDITORTOOLBAR_H__E81E79BF_8A84_4604_9377_F3A8C1935E5A__INCLUDED_)
