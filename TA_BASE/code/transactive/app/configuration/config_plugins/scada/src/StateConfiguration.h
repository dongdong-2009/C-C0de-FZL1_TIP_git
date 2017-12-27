/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/StateConfiguration.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This property page holds the configuration for derived data point state configuration
  */


#if !defined(AFX_STATECONFIGURATION_H__2A581C0A_DAD1_421B_A6E7_DF3D646009B5__INCLUDED_)
#define AFX_STATECONFIGURATION_H__2A581C0A_DAD1_421B_A6E7_DF3D646009B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#include "app/configuration/config_plugins/scada/src/resource.h"
#include "app/configuration/config_plugins/scada/src/IConfigurationPage.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"

namespace TA_Base_Core
{
    class IConfigEntity;
    class ConfigConfiguredDerivedState;
}

namespace TA_Base_App
{
    class AbstractDetailView;

    class StateConfiguration : public CPropertyPage, public virtual IConfigurationPage
    {
	    DECLARE_DYNCREATE(StateConfiguration)

    // Construction
    public:
        /**
         * Constructor
         */
	    StateConfiguration();


        /**
         * Destructor
         */
        ~StateConfiguration();


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
	    //{{AFX_DATA(StateConfiguration)
	enum { IDD = IDD_DERIVED_CONFIG_STATE };
	CEdit	m_numberOfStatesControl;
	CEdit	m_delayCtrl;
	CEdit	m_messageCtrl;
	CEdit	m_labelCtrl;
	CTransparentImage	m_labelBitmap;
	CListCtrl	m_stateList;
	BOOL	m_isAlarmEnabled;
	CString	m_delay;
	CString	m_label;
	CString	m_message;
	CString	m_severity;
	CString	m_numberOfStates;
    CComboBox m_mmsControl;
	//}}AFX_DATA


    // Overrides
	    // ClassWizard generate virtual function overrides
	    //{{AFX_VIRTUAL(StateConfiguration)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:
	    // Generated message map functions
	    //{{AFX_MSG(StateConfiguration)
	afx_msg void onChangeSeverity();
	afx_msg void onCheckAlarmEnabled();
	afx_msg void onKillfocusEditDelay();
	virtual BOOL OnInitDialog();
	afx_msg void onKillfocusEditLabel();
	afx_msg void onKillfocusEditMessage();
	afx_msg void onItemchangedStateList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void onKillfocusEditStates();
    afx_msg void onSelChangedMms();
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
        // The limit in length for the text fields.
        static const int LABEL_FIELD_LENGTH;
        static const int MESSAGE_FIELD_LENGTH;
        static const int DELAY_FIELD_LENGTH;
        static const int NUMBER_OF_STATES_FIELD_LENGTH;

        static const CString CHANGE_ALARMSEVERITY_DIALOG_TITLE;
        static const std::string NO_SEVERITY;

        /**
         * loadEntityIntoPage
         *
         * This will load all the entity details into the page. This checks that the window has been
         * created (ie OnInitDialog has been called) before attempting to populate the controls
         */
        void loadEntityIntoPage();


        /**
         * changeSeverity
         *
         * This provides a dialog to the user to allow them to change the alarm severity. Once the user
         * has selected a new severity this updates the current entity database object and the edit field
         *
         * @param CString& - This is the Cstring for the edit control with the severity we are changing
         * @param int - The state that this change severity is for
         */
        void changeSeverity(CString& currentSeverity, int state);


        /** 
         * refresh
         *
         * This refreshes the enabled and disabled items
         */
        void refresh();


        TA_Base_Core::IConfigEntity* m_currentEntity;

        ScadaDatabaseAccessor* m_databaseAccessor;

        // This is the state the user currently has selected. This can be NULL
        TA_Base_Core::ConfigConfiguredDerivedState* m_currentState;

        AbstractDetailView* m_parent;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_STATECONFIGURATION_H__2A581C0A_DAD1_421B_A6E7_DF3D646009B5__INCLUDED_)
