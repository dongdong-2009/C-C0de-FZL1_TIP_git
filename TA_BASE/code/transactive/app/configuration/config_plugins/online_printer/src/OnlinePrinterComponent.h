/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/online_printer/src/OnlinePrinterComponent.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the component the main Configuration Editor framework communicates with.
  * It implements the IConfigurableComponent interface. It will handle all toolbar and menu
  * requests and handle them appropriately.
  */

#if !defined(OnlinePrinterComponent_A4EEC36D_18C5_4d02_927B_EC8EFFC7172B__INCLUDED_)
#define OnlinePrinterComponent_A4EEC36D_18C5_4d02_927B_EC8EFFC7172B__INCLUDED_

#include <string>
#include <map>

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractComponent.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/MessageSender.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"

namespace TA_Base_App
{
    class OnlinePrinterComponent : public AbstractComponent
    {

    public:
        /** 
         * Constructor
         *
         * @param const string& - This is the name of the component displaying in the selection bar so we
         *                        can use the same name in the DLL for sending audit messages etc.
         */
        OnlinePrinterComponent(const std::string& componentName);


        /**
         * Destructor
         */
	    virtual ~OnlinePrinterComponent();


    public:
	    
	    /**
         * getIcon
         *
         * This returns the icon to be displayed in the selection bar.
         *
         * @return HICON - A handle to an icon that has been created
         */
	    virtual HICON getIcon();


	    /**
         * getComponentView
         *
         * This returns runtime class for the view to be shown in the bottom-right pane
         * of the application.
         *
         * @return CRuntimeClass* - This will be used to create the view in the correct pane.
         */
	    virtual CRuntimeClass* getComponentView();

        /**
         * getRightsActionForAddingThisComponent
         *
         * This should return the rights action the user must have to be able to view and use this
         * component. This action id will be passed straight to the rights library.
         *
         * @return unsigned long - The action id to use when checking whether to add this component
         *                         to the Configuration Editor.
         */
        virtual unsigned long getRightsActionForAddingThisComponent()
        {
            return TA_Base_Bus::aca_CE_CONFIGURE_ONLINE_PRINTERS;
        }
		//marc_bugfix for now, disable menu /toolbar items
		//Because it has no functionality 
        /**
         * getMenuItems
         *
         * This returns an empty vector, as this component has no menu items.
         *
         * @return std::vector<MenuItem> - All menu items required
         */
        std::vector<MenuItem> OnlinePrinterComponent::getMenuItems();
        
            
	    /**
         * getToolBarItems
         *
         * This returns an empty vector, as this component has no toolbar
         * items.
         *
         * @return std::vector<ToolBarItem> - All toolbar items required
         */
        std::vector<ToolBarItem> OnlinePrinterComponent::getToolBarItems();

        
    protected:

        /**
         * getDLLName
         *
         * This returns the name of the DLL.
         */
        virtual std::string getDLLName() const;


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        OnlinePrinterComponent( const OnlinePrinterComponent&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        OnlinePrinterComponent& operator=(const OnlinePrinterComponent&);



    public:
        // This is the name of the DLL and is used to load resources correctly.
        static const CString DLL_NAME;

    private:


        // The icon for this component
        HICON m_icon;
    };
}

#endif // !defined(OnlinePrinterComponent_A4EEC36D_18C5_4d02_927B_EC8EFFC7172B__INCLUDED_)
