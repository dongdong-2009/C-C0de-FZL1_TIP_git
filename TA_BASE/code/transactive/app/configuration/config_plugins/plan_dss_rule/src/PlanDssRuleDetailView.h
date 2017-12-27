/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/configuration/config_plugins/plan_dss_rule/src/PlanDssRuleDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  * 
  * This view allows the user to configure an alarm plan assocation's details.
  */

#if !defined(AFX_PlanDssRuleDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_PlanDssRuleDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/plan_dss_rule/src/resource.h"
#include "bus/mfc_extensions/src/text_colour_edit/TextColourEdit.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigPlanDssDpRule;
}


namespace TA_Base_App
{
    class PlanDssRuleDatabaseAccessor;
    class PlanDssRuleListView;


    class PlanDssRuleDetailView : public AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    PlanDssRuleDetailView();


        /**
         * Destructor
         */
	    virtual ~PlanDssRuleDetailView();

        
        DECLARE_DYNCREATE(PlanDssRuleDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(PlanDssRuleDetailView)
	enum { IDD = IDD_PLAN_DSS_RULE_VIEW };
        TA_Base_Bus::TextColourEdit	m_edtActivePlan;
        TA_Base_Bus::TextColourEdit	m_edtLocation;
        TA_Base_Bus::TextColourEdit	m_edtTriggerEntity;
        TA_Base_Bus::TextColourEdit	m_edtDenyStrategy;
        TA_Base_Bus::TextColourEdit	m_edtMinimumInterval;
		TA_Base_Bus::TextColourEdit m_edtActivedTolerantTime;
        CTransparentImage   m_bitmapPlan;
        CTransparentImage	m_bitmapInterval;
    	CTransparentImage	m_bitmapEntity;
	    CButton	m_type;
	    CStatic	m_modifyDate;
	    CStatic	m_createDate;
		CButton m_activateValueTrue;
		CButton m_activateValueFalse;
        CButton m_entityTypeRadio;
        CButton m_entityRadio;
		CButton m_needDeactivate;
		CButton m_allowMultiple;
		CButton m_checkEnable;
        CButton m_validSunday;
		CButton m_validMonday;
		CButton m_validTuesDay;
		CButton m_validWednesday;
		CButton m_validThursday;
		CButton m_validFriday;
		CButton m_validSaturday;
		CDateTimeCtrl	m_validStartTimeInDay;
		CDateTimeCtrl	m_validEndTimeInDay;

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
	    //{{AFX_VIRTUAL(PlanDssRuleDetailView)
	public:
	virtual void OnInitialUpdate();
	protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

    protected:

	    // Generated message map functions
	    //{{AFX_MSG(PlanDssRuleDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
	    afx_msg void onButtonTriggerEntity();
        afx_msg void onButtonActivePlan();
        afx_msg void onButtonLocation();
        afx_msg void onButtonDenyStrategy();
	    afx_msg void onKillfocusEditMinimumInterval();
		afx_msg void onKillfocusEditActivedTolerantTime();
		afx_msg void onButtonActivateTrue();
		afx_msg void onButtonActivateFalse();
		afx_msg void onButtonNeedDeactivate();
		afx_msg void onButtonAllowMultiple();
		afx_msg void onButtonEnable();
        afx_msg void onButtonSunday();
		afx_msg void onButtonMonday();
		afx_msg void onButtonTuesday();
		afx_msg void onButtonWednesday();
		afx_msg void onButtonThursday();
		afx_msg void onButtonFriday();
		afx_msg void onButtonSaturday();
		afx_msg void OnDatetimechangeStart(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDatetimechangeEnd(NMHDR* pNMHDR, LRESULT* pResult);
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
            return MessageSender::NOT_APPLICABLE;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        PlanDssRuleDetailView( const PlanDssRuleDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        PlanDssRuleDetailView& operator=(const PlanDssRuleDetailView&);


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


    private:
        // The titles for the change dialog box
        TA_Base_Core::IConfigPlanDssDpRule* m_pCurPlanDssRule; // The current alarm plan association the user
                                                              // is configuring. This is the database object
                                                              // for it.

        PlanDssRuleListView* m_listView; // This is the view in the top pane of the splitter
                                            // showing the list of all alarm plan associations. We need to
                                            // use this to tell it when alarm plan assocation details have
                                            // changed.

        PlanDssRuleDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve 
                                                             // information from the database.
    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_PlanDssRuleDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
