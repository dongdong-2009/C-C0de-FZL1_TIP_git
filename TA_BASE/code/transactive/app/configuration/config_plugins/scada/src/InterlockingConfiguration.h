/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/InterlockingConfiguration.h $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This property page holds the configuration for interlocking.
  */

#if !defined(AFX_INTERLOCKINGCONFIGURATION_H__BFB7DE4F_7F45_48B5_A498_6E5273783624__INCLUDED_)
#define AFX_INTERLOCKINGCONFIGURATION_H__BFB7DE4F_7F45_48B5_A498_6E5273783624__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "app/configuration/config_plugins/scada/src/resource.h"
#include "app/configuration/config_plugins/scada/src/IConfigurationPage.h"

#include "bus/interlocking/InterlockingConfig/src/RuleList.h"

// fix to stop boost (which gets included from RuleList.h) from
// overwritting the standard min and max macros
//#include <minmax.h>

namespace TA_Base_Core
{
    class IConfigEntity;
}

using TA_Base_Bus::RuleList;

namespace TA_Base_App
{
    class AbstractDetailView;

    class InterlockingConfiguration : public CPropertyPage, virtual public IConfigurationPage
    {
	    DECLARE_DYNCREATE(InterlockingConfiguration)

    public:
        /**
         * Constructor
         */
	    InterlockingConfiguration();


        /**
         * Destructor
         */
        ~InterlockingConfiguration();


        /**
         * populatePage
         *
         * This is called to update the page with the new details passed in
         *
         * @param IConfigEntity& - The entity to use to populate the page
         * @param ScadaDatabaseAccessor& - This can be used to retrieve additional information from the database
         *                                 if necessary.
         */
        virtual void populatePage(TA_Base_Core::IConfigEntity& currentEntity,
                                  ScadaDatabaseAccessor& databaseAccessor);


        /**
         * getUsedParameteres
         *
         * This method must determine all entity parameters that this dialogue uses and add them
         * to the set passed in.
         *
         * @param set<string> - This is an in/out parameter. When passed in it contains all the already
         *                      ignored parameters. This method should add all the ones used by this
         *                      class.
         */
        virtual void getUsedParameters(std::set<std::string>& parametersToIgnore);


        /**
         * getMandatoryParametersNotEntered
         *
         * This method must determine if there are any mandatory parameters that the user has not entered.
         * We cannot use the standard method for checking mandatory parameters as mandatory parameters for
         * datapoints change depending on the data point type selected
         *
         * @param vector<string> - This is an in/out parameter. When passed in it contains all the mandatory
         *                         parameters already missing. This method should add all the mandatory params
         *                         not entered in this property page.
         *
         * @exception DatabaseException
         * @exception DataException
         */
        virtual void getMandatoryParametersNotEntered(std::vector<std::string>& params);


        /**
         * getInvalidEnteredParameters
         *
         * This method must determine if there are any parameters that are not valid. We rely on each page
         * to tell us if the parameters are valid so that we can display the correct text to the user.
         *
         * Note: Be careful that this method does not use any resources that require OnInitDialog to have
         *       been called as this may not be the case. Try to use m_currentEntity.
         *
         * @param vector<string> - This is an in/out parameter. When passed in it contains all the invalid
         *                         parameters already found. This method should add all the invalid params
         *                         in this property page.
         *
         * @exception DatabaseException
         * @exception DataException
         */
        virtual void getInvalidEnteredParameters(std::vector<std::string>& params);


    // Dialog Data
	    //{{AFX_DATA(InterlockingConfiguration)
	    enum { IDD = IDD_INTERLOCKING_CONFIG };
        RuleList   	m_ruleList; // interlocking rules list
		CButton		m_editRuleButton;
		CButton		m_deleteRuleButton;
		CButton		m_newRuleButton;
		CButton		m_viewRuleButton;
	//}}AFX_DATA


    // Overrides
	    // ClassWizard generate virtual function overrides
	    //{{AFX_VIRTUAL(InterlockingConfiguration)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:
	    // Generated message map functions
	    //{{AFX_MSG(InterlockingConfiguration)
	    //afx_msg void onCheckBoolFalseAlarm();
        //afx_msg void onCheckBoolTrueAlarm();
	    virtual BOOL OnInitDialog();
        afx_msg void OnEditRule();
	    afx_msg void OnDeleteRule();
	    afx_msg void OnNewRule();
	    afx_msg void OnView();
		afx_msg void OnClickRuleList(NMHDR* pNMHDR, LRESULT* pResult);
	    //afx_msg void onChangeSeverityFalse();
	    //afx_msg void onChangeSeverityTrue();
	    //afx_msg void onKillfocusEditDelayBoolFalse();
	    //afx_msg void onKillfocusEditDelayBoolTrue();
	    //afx_msg void onKillfocusEditMessageBoolFalse();
	    //afx_msg void onKillfocusEditMessageBoolTrue();
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
        static const CString INTERLOCKING_DIALOG_TITLE;


        /**
         * loadEntityIntoPage
         *
         * This will load all the entity details into the page. This checks that the window has been
         * created (ie OnInitDialog has been called) before attempting to populate the controls
         */
        void loadEntityIntoPage();


        /** 
         * refresh
         *
         * This refreshes the enabled and disabled items
         */
        void refresh();

        /**
         * setUpSingletons
         *
         * For preserving singletons between DLLs
         */
        void setUpSingletons();

		/**
         * enableEditDelViewButtons
         *
         * This method is to enable or disable the edit, delete and view buttons
         *
         * @param bool - This is the bool to set the buttons. False is disable while true is enable
         *
		 */
        virtual void enableEditDelViewButtons(bool enable);


        TA_Base_Core::IConfigEntity* m_currentEntity;

        AbstractDetailView* m_parent;
    };
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INTERLOCKINGCONFIGURATION_H__BFB7DE4F_7F45_48B5_A498_6E5273783624__INCLUDED_)
