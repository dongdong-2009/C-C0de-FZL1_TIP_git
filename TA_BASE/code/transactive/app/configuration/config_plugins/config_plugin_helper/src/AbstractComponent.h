/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/config_plugin_helper/src/AbstractComponent.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This inherits from IConfigurableComponent and IComponent and implements the parts of this interface that should be
  * common to all DLL's.
  */

#if !defined(AbstractComponent_A4EEC36D_18C5_4d02_927B_EC8EFFC7172B__INCLUDED_)
#define AbstractComponent_A4EEC36D_18C5_4d02_927B_EC8EFFC7172B__INCLUDED_

#include <string>
#include <map>

#include "app/configuration/configuration_editor/src/IConfigurableComponent.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/IComponent.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/MessageSender.h"


namespace TA_Base_App
{
    class IMainView;
    class IDatabaseAccessor;
    class IDllHandler;
    class IProgressStatusBar;

    class AbstractComponent : public IConfigurableComponent, public IComponent
    {
    public:
	    

        /** 
         * Constructor
         *
         * @param const string& - This is the name of the component displaying in the selection bar so we
         *                        can use the same name in the DLL for sending audit messages etc.
         * @param IDatabaseAccessor* - The database accessor to use, store and pass around. This object will
         *                             delete this object.
         */
        AbstractComponent(const std::string& componentName, IDatabaseAccessor* databaseAccessor);


        /**
         * Destructor
         */
        virtual ~AbstractComponent();


        /** 
         * setUpSingletons
         *
         * This will set up the singletons in the DLL to match those in the main application.
         *
         * @param IDllHandler& - This is a handler from the main application that can be used to
         *                       retrieve the details of it's singletons. This can then be used to
         *                       set up the singletons in the DLL.
         */
        virtual void setUpSingletons(const IDllHandler& dllHandler);

        
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
        virtual void itemSelected(const CString& menuItem);


	    /**
         * getMenuItems
         *
         * This returns the menu items required by this component
         *
         * @return std::vector<MenuItem> - All menu items required
         */
    	virtual std::vector<MenuItem> getMenuItems();


	    /**
         * getToolBarItems
         *
         * This returns the toolbar items required by this component
         *
         * @return std::vector<ToolBarItem> - All toolbar items required
         */
    	virtual std::vector<ToolBarItem> getToolBarItems();

        
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
        virtual void setCreatedView(CView*);


        /**
         * setStatusBar
         *
         * This lets the DLL know about the status bar so that it can use it as a progress bar
         * when loading large amounts of data.
         *
         * @param IProgressStatusBar* - The status bar to use for displaying a progress bar.
         */
        virtual void setStatusBar(IProgressStatusBar* statusBar);

        
        /**
         * reload
         *
         * This will be called if the user wishes to reload all data from the database. It should ensure
         * that the user is given an opportunity to save any work they are currently working on and then
         * reloads all data from the database.
         */
        virtual void reload();


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
        virtual bool areCurrentChangesPending(std::vector<std::string>& dataNotApplied);
             
        
        /**
         * getDatabaseAccessor
         *
         * This returns a reference to an IDatabaseAccessor. This object creates this and it will
         * be used by all other objects requiring database access.
         *
         * @return IDatabaseAccessor* - The object used to access the database.
         */
        IDatabaseAccessor* getDatabaseAccessor()
        {
            return m_databaseAccessor;
        };


        /**
         * getMessageSender
         *
         * This returns a reference to a MessageSender. This object creates this and it will be used
         * by all other objects needing to send audit and online updates.
         *
         * @return MessageSender& - The object to use to send messages.
         */
        MessageSender& getMessageSender()
        {
            return m_messageSender;
        };


        /**
         * getStatusBar
         *
         * This returns a pointer to the status bar. Other objects will need this to display their
         * progress.
         *
         * @return IProgressStatusBar - The status bar object to use to show progress
         */
        IProgressStatusBar* getStatusBar() const
        {
            return m_statusBar;
        }


    protected:

        /**
         * getDLLName
         *
         * This returns the name of the DLL.
         */
        virtual std::string getDLLName() const = 0;



    public:

        // These are all the menu and toolbar items.
        static const CString EDIT_MENU_NAME;

        static const CString MENU_NEW_ITEM;
        static const CString MENU_COPY_ITEM;
        static const CString MENU_DELETE_ITEM;
        static const CString TOOLBAR_NEW_ITEM;
        static const CString TOOLBAR_COPY_ITEM;
        static const CString TOOLBAR_DELETE_ITEM;

        static const CString NEW_ITEM_CAPTION;
        static const CString COPY_ITEM_CAPTION;
        static const CString DELETE_ITEM_CAPTION;

    private:

        // The main view for this component. We will need to communicate to it and it needs to talk
        // to us so we store it here.
        IMainView* m_frameworkView;

        // This is the status bar to use for showing progress
        IProgressStatusBar* m_statusBar;

        // This loads all the entities for this configurable component and stores them for us.
        // This must be stored in this class so it remains persistant and does not get deleted with
        // the views each time the user selects to view a different component.
        IDatabaseAccessor* m_databaseAccessor;

        // This holds the message senders that can be used to send the audit and online update
        // messages. This must be stored in this class so it remains persistant and does not get
        // deleted with the views each time the user selects to view a different component.
        MessageSender m_messageSender;

        // This is the name of this component in the Configuration Editor selection bar. This is
        // needed for some error messages and audit messages.
        std::string m_componentName;

        // These are the icons that will be used for the buttons
        HICON m_copyIcon;
        HICON m_newIcon;
        HICON m_deleteIcon;

    };
}

#endif // !defined(AbstractComponent_A4EEC36D_18C5_4d02_927B_EC8EFFC7172B__INCLUDED_)
