/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_rules/src/AlarmRulesDetailView.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This view allows the user to configure an alarm plan assocation's details.
  */

#if !defined(AFX_AlarmRulesDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_AlarmRulesDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/alarm_rules/src/resource.h"
#include "bus/mfc_extensions/src/text_colour_edit/TextColourEdit.h"
#include "app/configuration/config_plugins/alarm_rules/src/AvalancheChildrenListCtrl.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IAlarmRuleData;
}


namespace TA_Base_App
{
    class AlarmRulesDatabaseAccessor;
    class AlarmRulesListView;


    class AlarmRulesDetailView : public AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    AlarmRulesDetailView();


        /**
         * Destructor
         */
	    virtual ~AlarmRulesDetailView();

        
        DECLARE_DYNCREATE(AlarmRulesDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(AlarmRulesDetailView)
	enum { IDD = IDD_ALARMPLAN_VIEW };
        TA_Base_Bus::TextColourEdit  m_entityType;
        TA_Base_Bus::TextColourEdit  m_entity;
        TA_Base_Bus::TextColourEdit  m_alarmType;
        TA_Base_Bus::TextColourEdit  m_plan;
        TA_Base_Bus::TextColourEdit  m_timeout;
        TA_Base_Bus::TextColourEdit  m_timeIn;
        TA_Base_Bus::TextColourEdit  m_minAlarms;
        CComboBox       m_trigger;
		CTransparentImage   m_bitmapAlarmType; //TD18279 zhiqiang++
		CTransparentImage   m_bitmapEntity; //TD18279 zhiqiang++
		CTransparentImage   m_btimapEntityType; //TD18279 zhiqiang++
        CTransparentImage   m_bitmapTrigger;
        CTransparentImage   m_bitmapPlan;
        CTransparentImage   m_bitmapTimeout;
	    CButton	m_type;
        CButton m_applyButton;
        CButton m_resetButton;
        CButton m_planButton;
        CStatic	m_dateCreatedName;
	    CStatic	m_createDate;
        CStatic	m_dateModifiedName;
	    CStatic	m_modifyDate;
        CButton m_entityTypeRadio;
        CButton m_entityRadio;
        AvalancheChildrenListCtrl m_avalancheSuppressionList;
        // TD 11796 No new entity is to be added. "Add" Button removed
		// CButton m_addChildButton;
        CButton m_removeChildButton;
        CButton m_addDataNodeButton;
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


		/**
         * changeTriggerToCh
         *
         * This method is for changing rule Trigger from English to Chinese.
		 *
		 * @param ruleTrigger - the English name (which is saved in database) of rule trigger
		 * 
		 * @return CString - the Chinese name of the rule trigger. Just for show in the GUI.
		 *
         */
		CString changeTriggerToCh(std::string ruleTrigger);

		/**
         * changeTriggerToEn
         *
         * This method is for changing rule Trigger from Chinese to English.
		 *
		 * @param ruleTrigger - the Chinese name (which is shown in the GUI) of rule trigger
		 * 
		 * @return string - the English name of the rule trigger (same as DB value).
		 *
         */
		std::string changeTriggerToEn(CString ruleTrigger);

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(AlarmRulesDetailView)
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
	//{{AFX_MSG(AlarmRulesDetailView)
	afx_msg void onButtonReset();
	afx_msg void onButtonApply();
	afx_msg void onButtonAlarmtype();
	afx_msg void onButtonEntitytype();
	afx_msg void onButtonEntity();
    // TD 11796 No new entity is to be added. All entities should be some form of data nodes/points
	// afx_msg void onButtonAddChild();
    afx_msg void onButtonRemoveChild();
    afx_msg void onButtonAddDataNode();
    afx_msg void onButtonPlan();
	afx_msg void onRadioEntity();
	afx_msg void onRadioEntitytype();
	afx_msg void onKillfocusEditAlarm();
	afx_msg void onKillfocusEditEntity();
	afx_msg void onKillfocusEditEntitytype();
	afx_msg void onKillfocusEditTimeout();
    afx_msg void onKillfocusEditTimeIn();
    afx_msg void onKillfocusEditMinAlarms();
    afx_msg void onKillfocusEditPlan();
	afx_msg void onSelectionChangeComboTrigger();
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
            return MessageSender::ALARM_RULE;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        AlarmRulesDetailView( const AlarmRulesDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        AlarmRulesDetailView& operator=(const AlarmRulesDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the alarm rule details. These controls will be
         * hidden until the user selects a alarm plan association from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);

        void showPlanItemDetails(bool isShown);
        void showAvalancheItemDetails(bool isShown);



        /**
         * refreshEntityAndEntityType
         *
         * Only one of these items can be enabled at once based on the radio button checked
         * This retrieves the data from the IAlarmRule to determine which is enabled.
         * It then sets the correct check and enables and disables the fields.
         */
        void refreshEntityAndEntityType();



        /** 
          * saveItemPositions
          *
          * Saves the original positions of the date fields and the
          * apply/reset buttons so they can be moved back later.
          *
          */
        void saveItemPositions();

        /** 
          * moveDateAndButtonsToDefaultPosition
          *
          * Moves the date fields and apply/reset buttons back to their
          * original positions.
          *
          */
        void moveDateAndButtonsToDefaultPosition();

        /** 
          * layoutAvalancheSuppressionRuleDetails
          *
          * Moves the data/time fields and apply/reset buttons to the correct
          * position for avalanche suppression rule layout, and resizes
          * the border.
          *
          */
        void layoutAvalancheSuppressionRuleDetails();

        /** 
          * layoutPlanExecutionDetails
          *
          * Moves the data/time fields and apply/reset buttons to the correct
          * position for plan execution rule layout, and resizes the border.
          *
          */
        void layoutPlanExecutionDetails();

        /** 
          * layoutCommonDetails
          *
          * Moves the data/time fields and apply/reset buttons to the default
          * position for alarm rule layout, and resizes the border.
          *
          */
        void layoutCommonDetails();

        /** 
          * resizeBorder
          *
          * Moves the bottom of the border to be just below the apply button.
          *
          */
        void resizeBorder();


        /** 
          * getWindowBottom
          *
          * Gets the bottom of the given window in pixels from the top of the
          * client window.
          *
          * @param window  the window whose bottom is found
          *
          * @return  the bottom of the window in pixels
          */
        int getWindowBottom(const CWnd& window);

        /** 
          * moveWindowDown
          *
          * Moves the given window down distancePixels
          *
          * @param window           the window to move
          * @param distancePixels   the number of pixels to move it.
          *                         This can be negative.
          *
          */
        void moveWindowDown(CWnd& window, int distancePixels);


        // The maximum lengths of the various fields in characters
        static const int ENTITY_FIELD_LENGTH;
        static const int ENTITYTYPE_FIELD_LENGTH;
        static const int ALARMTYPE_FIELD_LENGTH;
        static const int TIMEOUT_FIELD_LENGTH;
        static const int TIMEIN_FIELD_LENGTH;
        static const int MINALARMS_FIELD_LENGTH;
        static const int PLAN_FIELD_LENGTH;

        // Indicates which radio button should be selected:
        // entity or entity type
        static const bool SELECTED_RADIO_ENTITY;
        static const bool SELECTED_RADIO_ENTITY_TYPE;


        TA_Base_Core::IAlarmRuleData* m_currentAlarmRule; // The current alarm rule the user
                                                     // is configuring. This is the database object
                                                     // for it.

        AlarmRulesListView* m_listView; // This is the view in the top pane of the splitter
                                        // showing the list of all alarm rules. We need to
                                        // use this to tell it when alarm rule details have
                                        // changed.

        AlarmRulesDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve 
                                                         // information from the database.


        // The radio button that is selected.
        // Either entity or entity type.
        bool m_selectedRadio;

        // The original positions of the date fields and apply/reset buttons
        CRect m_dateCreatedNameRect;
        CRect m_createDateRect;
        CRect m_dateModifiedNameRect;
        CRect m_modifyDateRect;
        CRect m_applyButtonRect;
        CRect m_resetButtonRect;

        // The original position of the bottom of the trigger window.
        int m_originalTriggerBottom;

        // The distance that the date fields and apply/reset buttons are moved
        // down when the plan and avalanche suppression fields are shown.
        int m_planYDistance;
        int m_avalancheYDistance;

        // The position of the date fields and apply/reset buttons needs to be
        // saved the first time they are layed out. This indicates whether
        // their position has been saved since the page was last refreshed
        bool m_windowPositionsSaved;
    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_AlarmRulesDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
