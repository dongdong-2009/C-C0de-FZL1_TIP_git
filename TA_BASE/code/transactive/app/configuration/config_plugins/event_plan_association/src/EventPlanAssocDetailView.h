/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/event_plan_association/src/EventPlanAssocDetailView.h $
  * @author:  Brad Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an event plan assocation's details.
  */

#if !defined(_EVENT_PLAN_ASSOC_DETAIL_VIEW_H_)
#define _EVENT_PLAN_ASSOC_DETAIL_VIEW_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/event_plan_association/src/resource.h"
#include "bus/mfc_extensions/src/text_colour_edit/TextColourEdit.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Bus
{
    class RightsLibrary;
}

namespace TA_Base_Core
{
    class IConfigEventPlanAssocData;
}


namespace TA_Base_App
{
    class EventPlanAssocDatabaseAccessor;
    class EventPlanAssocListView;


    class EventPlanAssocDetailView : public AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
        EventPlanAssocDetailView();


        /**
         * Destructor
         */
        virtual ~EventPlanAssocDetailView();

        
        DECLARE_DYNCREATE(EventPlanAssocDetailView)


    // Form Data
    public:
        //{{AFX_DATA(EventPlanAssocDetailView)
        enum { IDD = IDD_EVENTPLAN_VIEW };
        TA_Base_Bus::TextColourEdit	m_plan;
        TA_Base_Bus::TextColourEdit	m_entityType;
        TA_Base_Bus::TextColourEdit	m_entity;
        TA_Base_Bus::TextColourEdit	m_eventType;
        CTransparentImage   m_bitmapPlan;
        CTransparentImage	m_bitmapEventType;
        CTransparentImage	m_bitmapEntity;
        CButton m_type;
        CStatic m_modifyDate;
        CStatic m_createDate;
        CButton m_entityTypeRadio;
        CButton m_entityRadio;
        CButton m_enabled;
        //}}AFX_DATA


        /**
         * populateItemDetails
         *
         * This will populate all fields in the view using the database object passed in.
         *
         * @param IConfigItem* - The database object for the item we want to display. We will use this to
         *                       set new values and apply. If this is NULL then the details should all be hidden
         */
        void populateItemDetails(TA_Base_Core::IConfigItem* currentItem);


        /**
         * refreshButtonAvailability
         *
         * This should be called after any modification to the fields. This will enable or disable
         * the reset and apply buttons based on whether or not any data has changed.
         */
        virtual void refreshButtonAvailability();



    // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(EventPlanAssocDetailView)
        public:
            virtual void OnInitialUpdate();
        protected:
            virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

    protected:

    #ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
    #endif


        // Generated message map functions
        //{{AFX_MSG(EventPlanAssocDetailView)
        afx_msg void onButtonReset();
        afx_msg void onButtonApply();
        afx_msg void onButtonEventtype();
        afx_msg void onButtonEntitytype();
        afx_msg void onButtonEntity();
        afx_msg void onButtonPlan();
        afx_msg void onRadioEntity();
        afx_msg void onRadioEntitytype();
        afx_msg void onKillfocusEditEvent();
        afx_msg void onKillfocusEditEntity();
        afx_msg void onKillfocusEditEntitytype();
        afx_msg void onKillfocusEditPlan();
        afx_msg void onCheckEnabled();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()


    protected:

        /**
         * getConfigMessageType
         *
         * This should return the type of configuration update. ie ENTITY, OPERATOR etc.
         *
         * @return ConfigUpdateType - The type of component this update is for
         */
        virtual MessageSender::ConfigUpdateType getConfigMessageType() const
        {
            return MessageSender::EVENT_PLAN_ASSOCIATION;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        EventPlanAssocDetailView( const EventPlanAssocDetailView& );


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        EventPlanAssocDetailView& operator=( const EventPlanAssocDetailView& );


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the event plan association details. These controls will be
         * hidden until the user selects a event plan association from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);


        /**
         * refreshEntityAndEntityType
         *
         * Only one of these items can be enabled at once based on the radio button checked
         * This retrieves the data from the IConfigEventPlanAssocData to determine which is enabled.
         * It then sets the correct check and enabled and disables the fields.
         */
        void refreshEntityAndEntityType();


		/**
		 * setReadOnly
		 *
		 * Will disable/enable all controls except for the "enable" checkbox
		 * and the Apply/Reset buttons
		 *
		 * @param bool - If set to true, will make the view read-only
		 */
		void setReadOnly(bool readOnly);

    private:
        // The titles for the change dialog box
        static const CString CHANGE_ENTITYTYPE_DIALOG_TITLE;
        static const CString CHANGE_EVENTTYPE_DIALOG_TITLE;

        static const int ENTITY_FIELD_LENGTH;
        static const int ENTITYTYPE_FIELD_LENGTH;
        static const int PLAN_FIELD_LENGTH;
        static const int EVENTTYPE_FIELD_LENGTH;

        TA_Base_Core::IConfigEventPlanAssocData* m_currentAssociation; // The current event plan association the user
                                                                       // is configuring. This is the database object
                                                                       // for it.

        EventPlanAssocListView* m_listView; // This is the view in the top pane of the splitter
                                            // showing the list of all event plan associations. We need to
                                            // use this to tell it when event plan assocation details have
                                            // changed.

        EventPlanAssocDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve 
                                                             // information from the database.

		bool m_readOnly;
        TA_Base_Bus::RightsLibrary* m_rightsLibrary;
		unsigned long m_resourceKey;

    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(_EVENT_PLAN_ASSOC_DETAIL_VIEW_H_)
