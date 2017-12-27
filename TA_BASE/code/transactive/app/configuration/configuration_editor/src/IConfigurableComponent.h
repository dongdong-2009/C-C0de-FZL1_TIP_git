/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/IConfigurableComponent.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This interface must be implemented by each plugin component library. The CMainFrame uses
  * this interface to retrieve information from each component and also to pass other information.
  * 
  */

#if !defined(IConfigurableComponent_4E37AA25_120B_456c_B2D4_25E6A2499C47__INCLUDED_)
#define IConfigurableComponent_4E37AA25_120B_456c_B2D4_25E6A2499C47__INCLUDED_

#include <vector>

#include "app/configuration/configuration_editor/src/StdAfx.h"
#include "app/configuration/configuration_editor/src/ControlItems.h"

namespace TA_Base_App
{
    class IDllHandler;
    class IProgressStatusBar;

    class IConfigurableComponent
    {

    public:
        /**
          * Destructor
          */
        virtual ~IConfigurableComponent(){ };


	    /**
          * getIcon
          *
          * This returns the icon to be displayed in the selection bar.
          *
          * @return HICON - A handle to an icon that has been created
          */
	    virtual HICON getIcon() =0;


	    /**
          * getMenuItems
          *
          * This returns the menu items required by this component
          *
          * @return std::vector<MenuItem> - All menu items required
          */
        virtual std::vector<MenuItem> getMenuItems() =0;


	    /**
          * getToolBarItems
          *
          * This returns the toolbar items required by this component
          *
          * @return std::vector<ToolBarItem> - All toolbar items required
          */
        virtual std::vector<ToolBarItem> getToolBarItems() =0;

        
        /**
          * getRightsActionForAddingThisComponent
          *
          * This should return the rights action the user must have to be able to view and use this
          * component. This action id will be passed straight to the rights library.
          *
          * @return unsigned long - The action id to use when checking whether to add this component
          *                         to the Configuration Editor.
          */
        virtual unsigned long getRightsActionForAddingThisComponent() =0;


	    /**
          * getComponentView
          *
          * This returns runtime class for the view to be shown in the bottom-right pane
          * of the application.
          *
          * @return CRuntimeClass* - This will be used to create the view in the correct pane.
          */
        virtual CRuntimeClass* getComponentView() =0;


        /** 
          * setUpSingletons
          *
          * This will set up the singletons in the DLL to match those in the main application.
          *
          * @param IDllHandler& - This is a handler from the main application that can be used to
          *                       retrieve the details of it's singletons. This can then be used to
          *                       set up the singletons in the DLL.
          */
        virtual void setUpSingletons(const IDllHandler& dllHandler) =0;


	    /**
          * setCreatedView
          *
          * This is called to pass the configurable component the view that has been created for it.
          * Because this only passes out the CRuntimeClass of the view it doesn't have an actual 
          * pointer to the view itself. This method should cast the view to the type it is
          * expecting so it can use it from now on.
          *
          * @param View* - The view that has been created for us
          */
        virtual void setCreatedView(CView*) =0;


        /**
          * setStatusBar
          *
          * This lets the DLL know about the status bar so that it can use it as a progress bar
          * when loading large amounts of data.
          *
          * @param IProgressStatusBar* - The status bar to use for displaying a progress bar.
          */
        virtual void setStatusBar(IProgressStatusBar* statusBar) =0;


        /**
          * itemSelected
          *
          * This is called when the user selects a toolbar of menu item that this object is interested
          * in. The menu item or toolbar button name is passed so this method can determine the function to
          * call.
          *
          * @param const CString& - The name of the toolbar or menu item selected. This should be exactly
          *                         the same as one of the strings passed in the MenuItem::menuItem or the
          *                         ToolBarItem::name structure.
          */
        virtual void itemSelected(const CString& item) =0;


        /**
          * reload
          *
          * This will be called if the user wishes to reload all data from the database. It should ensure
          * that the user is given an opportunity to save any work they are currently working on and then
          * reloads all data from the database.
          */
        virtual void reload() = 0;


        /**
          * areCurrentChangesPending
          *
          * This is called to determine if there are any current changes that have not been applied
          * before reloading or exiting the program. If this returns true then it will populate the
          * vector of strings passed in with all items not applied.
          * 
          * @param vector<string>& - This is an OUT parameter and should be passed in empty. It will
          *                          be populated with names for all data not yet applied
          * 
          * @return bool - This will return true if there are items that have had changes which have  not
          *                been applied.
          */
        virtual bool areCurrentChangesPending(std::vector<std::string>& dataNotApplied) = 0;
    };
}

#endif // !defined(IConfigurableComponent_4E37AA25_120B_456c_B2D4_25E6A2499C47__INCLUDED_)
