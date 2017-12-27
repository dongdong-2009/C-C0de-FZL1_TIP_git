/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/config_plugin_helper/src/IMainView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the main view that the component will use.
  */

#if !defined(AFX_IMAINVIEW_H__1332EF35_428C_4AA3_8B05_008BBA40FA97__INCLUDED_)
#define AFX_IMAINVIEW_H__1332EF35_428C_4AA3_8B05_008BBA40FA97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_App
{
    class IComponent;

    class IMainView
    {

    public:

        virtual ~IMainView(){ };

        
        /**
         * setComponentInterface
         *
         * This is called by the component interface to register itself with this view. This view
         * is the main view so it provides all communication between the various
         * bits and pieces. This method will also perform any final initialisation required on its
         * child views now that it has all the information it requires.
         */
        virtual void setComponentInterface(IComponent* component) =0;


        /**
         * itemSelected
         *
         * This is called when the user has chosen a toolbar or menu item.
         *
         * @param const CString& - The name of the item selected. This should be exactly
         *                         the same as one of the strings passed in the ToolBarItem::name
         *                         structure or MenuItem::menuItem structure.
         */
        virtual void itemSelected(const CString& item) =0;
    };
}

#endif // !defined(AFX_IMAINVIEW_H__1332EF35_428C_4AA3_8B05_008BBA40FA97__INCLUDED_)
