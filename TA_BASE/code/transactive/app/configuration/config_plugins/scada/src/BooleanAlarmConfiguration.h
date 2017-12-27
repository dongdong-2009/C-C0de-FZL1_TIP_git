/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/BooleanAlarmConfiguration.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This property page holds the configuration for boolean alarms.
  */

#if !defined(AFX_BOOLEANALARMCONFIGURATION_H__BFB7DE4F_7F45_48B5_A498_6E5273783624__INCLUDED_)
#define AFX_BOOLEANALARMCONFIGURATION_H__BFB7DE4F_7F45_48B5_A498_6E5273783624__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#include "app/configuration/config_plugins/scada/src/resource.h"
#include "app/configuration/config_plugins/scada/src/IConfigurationPage.h"
#include "core/data_access_interface/src/ConfigChangesTypes.h"

namespace TA_Base_Core
{
    class IConfigEntity;
	class DataPointEntityData;
}


namespace TA_Base_App
{
    class AbstractDetailView;

    class BooleanAlarmConfiguration : public CPropertyPage, virtual public IConfigurationPage
    {
	    DECLARE_DYNCREATE(BooleanAlarmConfiguration)

    public:
        /**
         * Constructor
         */
	    BooleanAlarmConfiguration();


        /**
         * Destructor
         */
        ~BooleanAlarmConfiguration();


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

		//TD15446
        bool getIsAlarmStatesChanged();
        void saveAlarmStates();
		TA_Base_Core::ItemChanges getAllItemChanges();
		
    // Dialog Data
	    //{{AFX_DATA(BooleanAlarmConfiguration)
	enum { IDD = IDD_ALARM_CONFIG_BOOLEAN };
	CEdit	m_trueDelayControl;
	CEdit	m_falseDelayControl;
	CEdit	m_trueMessageControl;
	CEdit	m_falseMessageControl;
	BOOL	m_falseCheck;
	BOOL	m_trueCheck;
	CString	m_falseDelay;
	CString	m_trueDelay;
	CString	m_falseMessage;
	CString	m_trueMessage;
	CString	m_falseSeverity;
	CString	m_trueSeverity;
    CComboBox m_trueMmsCombo;
    CComboBox m_falseMmsCombo;
	//}}AFX_DATA


    // Overrides
	    // ClassWizard generate virtual function overrides
	    //{{AFX_VIRTUAL(BooleanAlarmConfiguration)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:
	    // Generated message map functions
	    //{{AFX_MSG(BooleanAlarmConfiguration)
	    afx_msg void onCheckBoolFalseAlarm();
	    afx_msg void onCheckBoolTrueAlarm();
	virtual BOOL OnInitDialog();
	afx_msg void onChangeSeverityFalse();
	afx_msg void onChangeSeverityTrue();
	afx_msg void onKillfocusEditDelayBoolFalse();
	afx_msg void onKillfocusEditDelayBoolTrue();
	afx_msg void onKillfocusEditMessageBoolFalse();
	afx_msg void onKillfocusEditMessageBoolTrue();
    afx_msg void onSelChangedTrueAlarm();
    afx_msg void onSelChangedFalseAlarm();
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    private:

        /**
         * changeSeverity
         *
         * This provides a dialog to the user to allow them to change the alarm severity. Once the user
         * has selected a new severity this updates the current entity database object and the edit field
         *
         * @param CString& - This is the Cstring for the edit control with the severity we are changing
         * @param string - This is the name of the entity parameter to update in the entity database object
         */
        void changeSeverity(CString& currentSeverity, const std::string& entityParameterName);

    public:
        // These are the entity parameters to retrieve
        static const std::string TRUE_ALARM_ENABLED;
        static const std::string TRUE_ALARM_DELAY;
        static const std::string TRUE_ALARM_MESSAGE;
        static const std::string TRUE_ALARM_SEVERITY;
        static const std::string FALSE_ALARM_ENABLED;
        static const std::string FALSE_ALARM_DELAY;
        static const std::string FALSE_ALARM_MESSAGE;
        static const std::string FALSE_ALARM_SEVERITY;
        static const std::string TRUE_ALARM_MMS;
        static const std::string FALSE_ALARM_MMS;

    private:
        static const CString CHANGE_ALARMSEVERITY_DIALOG_TITLE;

        static const int MESSAGE_FIELD_LENGTH;
        static const int DELAY_FIELD_LENGTH;

        static const std::string NO_SEVERITY;


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


        TA_Base_Core::IConfigEntity* m_currentEntity;
		TA_Base_Core::DataPointEntityData* m_dataPointEntityData; //TD15446
		
        AbstractDetailView* m_parent;

        ScadaDatabaseAccessor* m_databaseAccessor;
    };
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BOOLEANALARMCONFIGURATION_H__BFB7DE4F_7F45_48B5_A498_6E5273783624__INCLUDED_)
