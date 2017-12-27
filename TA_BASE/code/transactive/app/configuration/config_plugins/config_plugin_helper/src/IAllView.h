/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/config_plugin_helper/src/IAllView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all items available to be configured. The term 'List' is used generically and could
  * mean a list control, a tree control etc. Anything that lists all the items.
  */

#if !defined(AFX_IALLVIEW_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_)
#define AFX_IALLLISTVIEW_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_

#include <afxcview.h>
#include <string>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_App
{
    class IDetailView;
    class IDatabaseAccessor;
    class MessageSender;
    class IProgressStatusBar;


    class IAllView
    {
    public:

        virtual ~IAllView() { };


        /**
         * setDetailView
         *
         * This method passes us the detail view so we can let it know when the user selects
         * different items. It stores this view in m_detailView.
         *
         * @param AbstractDetailView* - The view which allows the user to configure a single item.
         */
        virtual void setDetailView( IDetailView* detailView ) =0;


        /**
         * setDatabaseAccessor
         *
         * This method passes us the object that allows us to retrieve entity data from the database.
         *
         * @param IDatabaseAccessor* - The object that accesses the database
         */
        virtual void setDatabaseAccessor( IDatabaseAccessor* databaseAccessor ) =0;


        /**
         * setMessageSender
         *
         * This method passes us the object that allows us to send messages.
         *
         * @param MessageSender* - The object that sends the messages
         */
        virtual void setMessageSender(MessageSender* messageSender) =0;

        
        /**
         * setStatusBar
         *
         * This sets the status bar to use to show the progress of loading etc.
         *
         * @param IProgressStatusBar* - The status bar to use for displaying a progress bar.
         */
        virtual void setStatusBar(IProgressStatusBar* statusBar) =0;

        
        /**
         * itemDetailsChanged
         *
         * This method is called when an items details change. If the name of the item changes then
         * we must update our list with the new name.
         *
         * @param unsigned long - The id of the object that has changed
         * @param CString - The name of the object. This may be new
         */
        virtual void itemDetailsChanged(unsigned long id, const CString& name) =0;


        /**
         * loadAllItems
         *
         * This retrieves all the items and populates the display list for the user.
         * PreCondition: setDatabaseAccessor has been called
         */
        virtual void loadAllItems( ) =0;
    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_IALLVIEW_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_)
