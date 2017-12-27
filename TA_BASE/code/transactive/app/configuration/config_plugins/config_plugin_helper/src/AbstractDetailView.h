/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an item's details.
  */

#if !defined(AFX_ABSTRACTDETAILVIEW_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_ABSTRACTDETAILVIEW_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#include "app/configuration/config_plugins/config_plugin_helper/src/IDetailView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/MessageSender.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigItem;
    class IItem;
}

namespace TA_Base_App
{
    class IDatabaseAccessor;
    class IAllView;


    class AbstractDetailView : public CFormView, public IDetailView
    {
    protected:
        /**
         * Constructor
         *
         * @param int - This is the dialog resource ID to link this view to
         */
	    AbstractDetailView(int dialogResource);


        /**
         * Destructor
         */
	    virtual ~AbstractDetailView();

        /**
         * OnActivateView
         *
         * The method is called whenever a view is activated or deactivate. Its normal task is to
         * allocate focus to its child controls based on the state change, however due to how the
         * detail view are being repopulated this default behaviour results in selection state being
         * carried across to different elements. To avoid this we'll just make it do nothing.
         *
         * @param bActivate A flag stating whether the view is being activated or deactivated.
         * @param pActivateView The handle to the view being activated.
         * @param pDeactivateView The handle to teh view being deactivated.
         */
        virtual void OnActivateView( BOOL bActivate, CView* pActivateView,CView* pDeactiveView );
        
    // Form Data
    public:
	    //{{AFX_DATA(AbstractDetailView)
	//}}AFX_DATA


        /**
         * setListView
         *
         * This method passes us the list view so we can let it know when the user modifies
         * an item. It stores this view in m_listView.
         *
         * @param IAllView* - The view which lists all items to be configured
         */
        void setListView( IAllView* listView )
        {
            m_listView = listView;
        }


		IAllView* getListView() const
		{
			return m_listView;
		}

        /**
         * setDatabaseAccessor
         *
         * This method passes us the object that allows us to retrieve entity data from the database.
         *
         * @param EntityDatabaseAccessor* - The object that accesses the database
         */
        void setDatabaseAccessor( IDatabaseAccessor* databaseAccessor )
        {
            m_databaseAccessor = databaseAccessor;
        }


        /**
         * setMessageSender
         *
         * This method passes us the object that allows us to send messages.
         *
         * @param MessageSender& - The object that sends the messages
         */
        void setMessageSender(MessageSender& messageSender)
        {
            m_messageSender = &messageSender;
        }


        /**
         * populateItemDetails
         *
         * This will populate all fields in the view using the database object passed in.
         *
         * @param IConfigItem* - The database object for the item we want to display. We will use this to
         *                         set new values and apply. If this is NULL then the details should all be hidden
         */
        virtual void populateItemDetails(TA_Base_Core::IConfigItem* item) =0;


        /**
         * refreshButtonAvailability
         *
         * This should be called after any modification to the fields. This will enable or disable
         * the reset and apply buttons based on whether or not any data has changed.
         */
        virtual void refreshButtonAvailability() =0;


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(AbstractDetailView)
    	//}}AFX_VIRTUAL

    protected:

    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif


	    // Generated message map functions
	    //{{AFX_MSG(AbstractDetailView)
	    //}}AFX_MSG
        DECLARE_MESSAGE_MAP()


    protected:

        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the item details. These controls will be
         * hidden until the user selects an item from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown) =0;

        /**
         * sendMessagesForTheChange 
         *
         * Created for TES #782
         * Sends the input message out via our message sender
         *  Allows derived classes to intercept the message sending functionality
         *
         * @param updateDetails the data to broadcast
         *
         */
        virtual void sendMessagesForTheChange(MessageSender::UpdateDetails updateDetails);


        IDatabaseAccessor* getDatabaseAccessor() const
        {
            return m_databaseAccessor;
        }


        MessageSender* getMessageSender() const
        {
            return m_messageSender;
        }


        /**
         * getConfigMessageType
         *
         * This should return the type of configuration update. ie ENTITY, OPERATOR etc.
         *
         * @return ConfigUpdateType - The type of component this update is for
         */
        virtual MessageSender::ConfigUpdateType getConfigMessageType() const =0;


        /**
         * applyButtonPressed
         *
         * This can be called by implementations of this class to perform an apply. This ensures all 
         * components perform the same functionality when the user selects 'Apply'
         *
         * @param IConfigItem* - The configuration item to apply changes to.
         */
        virtual void applyButtonPressed(TA_Base_Core::IConfigItem* currentItem);


        /**
         * resetButtonPressed
         *
         * This can be called by implementations of this class to perform a reset. This ensures all 
         * components perform the same functionality when the user selects 'Reset'
         *
         * @param IConfigItem* - The configuration item to reset.
         */
        virtual void resetButtonPressed(TA_Base_Core::IConfigItem* currentItem);


    private:

        /**
         * sendDeleteMessageNotification
         *
         * Sends out a delete notification message for the specified item via
         *  the message sender object
         *
         * @param item the item defining the fields of the delete notification
         * @param keyOfItem defines the key that should be used in notification (overriding
         *  key value present in item)
         */
        void sendDeleteMessageNotification(TA_Base_Core::IItem* item, unsigned long keyOfItem);

        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        AbstractDetailView( const AbstractDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        AbstractDetailView& operator=(const AbstractDetailView&);


    public:
        // The label used to indicate that this is a new item being displayed
        static const CString NEW_LABEL;

        // The string to set in the controls where there is no value.
        static const CString NO_VALUE;


    private:


        IAllView* m_listView; // This is the view in the left pane of the splitter
                               // showing the list of all items. We need to use this to
                               // tell it when item details have changed.

        IDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve information
                                                // from the database.

        MessageSender* m_messageSender; // The object we can use to send messages when an update is made


    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_ABSTRACTDETAILVIEW_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
