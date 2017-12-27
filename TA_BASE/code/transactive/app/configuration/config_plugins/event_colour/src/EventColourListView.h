/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/event_colour/src/EventColourListView.h $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all event colours available to be configured. This view must communicate
  * with the EventColourDetailView as this will show the details of the item selected in this view.
  */

// EventColourListView.h: interface for the EventColourListView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENTCOLOURLISTVIEW_H__A2360AA8_73E3_4B49_8966_35439F1A4B59__INCLUDED_)
#define AFX_EVENTCOLOURLISTVIEW_H__A2360AA8_73E3_4B49_8966_35439F1A4B59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxcview.h>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractListView.h"

namespace TA_Base_App
{
    class EventColourDetailView;
    class EventColourDatabaseAccessor;

	class EventColourListView : public AbstractListView  
	{

	public:

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(EventColourListView)
	    protected:
	    virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif

	    // Generated message map functions
    protected:
	    //{{AFX_MSG(EventColourListView)
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

        /**
         * Constructor
         *
         * Protected constructor used by dynamic creation
         */
		EventColourListView();
		virtual ~EventColourListView();

	    DECLARE_DYNCREATE(EventColourListView)

        /**
         * getComponentIcon
         *
         * This should return the component icon for items to be shown in the list
         *
         * @return HICON - The icon to be shown
         */
        virtual HICON getComponentIcon()
        {
            return m_componentIcon;
        }


        /**
         * getConfigMessageType
         *
         * This should return the type of configuration update. ie ENTITY, EVENT_COLOUR etc.
         *
         * @return ConfigUpdateType - The type of component this update is for
         */
        virtual MessageSender::ConfigUpdateType getConfigMessageType() const
        {
            return MessageSender::EVENT_COLOUR;
        }

        
    private:

        HICON m_componentIcon;

	};

}//close namespace

#endif // !defined(AFX_EVENTCOLOURLISTVIEW_H__A2360AA8_73E3_4B49_8966_35439F1A4B59__INCLUDED_)
