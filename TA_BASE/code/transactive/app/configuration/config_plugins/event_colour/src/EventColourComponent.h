/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/event_colour/src/EventColourComponent.h $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the component the main Configuration Editor framework communicates with.
  * It implements the IConfigurableComponent interface. It will handle all toolbar and menu
  * requests and handle them appropriately.
  */

// EventColourComponent.h: interface for the EventColourComponent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENTCOLOURCOMPONENT_H__E845D1FA_754E_484A_A5A1_87A39FB63691__INCLUDED_)
#define AFX_EVENTCOLOURCOMPONENT_H__E845D1FA_754E_484A_A5A1_87A39FB63691__INCLUDED_


#include <string>
#include <map>

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractComponent.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/MessageSender.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"

namespace TA_Base_App
{
	class EventColourComponent : public AbstractComponent 
	{
    
	public:
        // This is the name of the DLL and is used to load resources correctly.
        static const CString DLL_NAME;

	public:
        /** 
         * Constructor
         *
         * @param const string& - This is the name of the component displaying in the selection bar so we
         *                        can use the same name in the DLL for sending audit messages etc.
         */
        EventColourComponent(const std::string& componentName);


        /**
         * Destructor
         */
		virtual ~EventColourComponent();

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
            return TA_Base_Bus::aca_CE_CONFIGURE_EVENT_COLOURS;
        }


    protected:

        /**
         * getDLLName
         *
         * This returns the name of the DLL.
         */
        virtual std::string getDLLName() const;


        /**
         * getMenuItems
         *
         * This returns an empty vector, as this component has no menu items.
         *
         * @return std::vector<MenuItem> - All menu items required
         */
        std::vector<MenuItem> getMenuItems();
        
            
	    /**
         * getToolBarItems
         *
         * This returns an empty vector, as this component has no toolbar
         * items.
         *
         * @return std::vector<ToolBarItem> - All toolbar items required
         */
        std::vector<ToolBarItem> getToolBarItems();


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        EventColourComponent( const EventColourComponent&);


        /**
         * EventColourComponent =
         *
         * Hidden as a private method so it is not used.
         */
        EventColourComponent& operator=(const EventColourComponent&);


    private:


        // The icon for this component
        HICON m_icon;

	};

} // close namespace

#endif // !defined(AFX_EVENTCOLOURCOMPONENT_H__E845D1FA_754E_484A_A5A1_87A39FB63691__INCLUDED_)
