/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_plan_association/src/AlarmPlanAssocDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an alarm plan assocation's details.
  */

#if !defined(AFX_AlarmPlanAssocDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_AlarmPlanAssocDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/alarm_plan_association/src/resource.h"
#include "bus/mfc_extensions/src/text_colour_edit/TextColourEdit.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigAlarmPlanAssoc;
}


namespace TA_Base_App
{
    class AlarmPlanAssocDatabaseAccessor;
    class AlarmPlanAssocListView;


    class AlarmPlanAssocDetailView : public AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    AlarmPlanAssocDetailView();


        /**
         * Destructor
         */
	    virtual ~AlarmPlanAssocDetailView();

        
        DECLARE_DYNCREATE(AlarmPlanAssocDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(AlarmPlanAssocDetailView)
	enum { IDD = IDD_ALARMPLAN_VIEW };
        TA_Base_Bus::TextColourEdit	m_plan;
        TA_Base_Bus::TextColourEdit	m_entityType;
        TA_Base_Bus::TextColourEdit	m_entity;
        TA_Base_Bus::TextColourEdit	m_alarmType;
        CTransparentImage   m_bitmapPlan;
        CTransparentImage	m_bitmapAlarmType;
    	CTransparentImage	m_bitmapEntity;
	    CButton	m_type;
	    CStatic	m_modifyDate;
	    CStatic	m_createDate;
        CButton m_entityTypeRadio;
        CButton m_entityRadio;
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
	    //{{AFX_VIRTUAL(AlarmPlanAssocDetailView)
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
	    //{{AFX_MSG(AlarmPlanAssocDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
	    afx_msg void onButtonAlarmtype();
	    afx_msg void onButtonEntitytype();
	    afx_msg void onButtonEntity();
        afx_msg void onButtonPlan();
	    afx_msg void onRadioEntity();
	    afx_msg void onRadioEntitytype();
	    afx_msg void onKillfocusEditAlarm();
	    afx_msg void onKillfocusEditEntity();
	    afx_msg void onKillfocusEditEntitytype();
	    afx_msg void onKillfocusEditPlan();
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
            return MessageSender::ALARM_PLAN_ASSOCIATION;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        AlarmPlanAssocDetailView( const AlarmPlanAssocDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        AlarmPlanAssocDetailView& operator=(const AlarmPlanAssocDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the alarm plan association details. These controls will be
         * hidden until the user selects a alarm plan association from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);


        /**
         * refreshEntityAndEntityType
         *
         * Only one of these items can be enabled at once based on the radio button checked
         * This retrieves the data from the IConfigAlarmPlanAssoc to determine which is enabled.
         * It then sets the correct check and enabled and disables the fields.
         */
        void refreshEntityAndEntityType();


    private:

        static const int ENTITY_FIELD_LENGTH;
        static const int ENTITYTYPE_FIELD_LENGTH;
        static const int PLAN_FIELD_LENGTH;
        static const int ALARMTYPE_FIELD_LENGTH;

        TA_Base_Core::IConfigAlarmPlanAssoc* m_currentAssociation; // The current alarm plan association the user
                                                              // is configuring. This is the database object
                                                              // for it.

        AlarmPlanAssocListView* m_listView; // This is the view in the top pane of the splitter
                                            // showing the list of all alarm plan associations. We need to
                                            // use this to tell it when alarm plan assocation details have
                                            // changed.

        AlarmPlanAssocDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve 
                                                             // information from the database.


    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_AlarmPlanAssocDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
