/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/config_plugin_helper/src/IDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This interface provices the methods needed to populate an item's details.
  */

#if !defined(AFX_IDETAILVIEW_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_IDETAILVIEW_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigItem;
}


namespace TA_Base_App
{
    class IDatabaseAccessor;
    class IAllView;
    class MessageSender;


    class IDetailView
    {
    public:

        virtual ~IDetailView(){ }

       
        /**
         * setListView
         *
         * This method passes us the list view so we can let it know when the user modifies
         * an item. It stores this view in m_listView.
         *
         * @param AbstractListView* - The view which lists all items to be configured
         */
        virtual void setListView( IAllView* listView ) =0;


        /**
         * setDatabaseAccessor
         *
         * This method passes us the object that allows us to retrieve entity data from the database.
         *
         * @param EntityDatabaseAccessor* - The object that accesses the database
         */
        virtual void setDatabaseAccessor( IDatabaseAccessor* databaseAccessor ) =0;


        /**
         * setMessageSender
         *
         * This method passes us the object that allows us to send messages.
         *
         * @param MessageSender& - The object that sends the messages
         */
        virtual void setMessageSender(MessageSender& messageSender) =0;


        /**
         * populateItemDetails
         *
         * This will populate all fields in the view using the database object passed in.
         *
         * @param IConfigItem* - The database object for the item we want to display. We will use this to
         *                         set new values and apply. If this is NULL then the details should all be hidden
         */
        virtual void populateItemDetails(TA_Base_Core::IConfigItem* item) =0;

    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_IDETAILVIEW_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
