/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/AnalogueAlarmConfiguration.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This property page holds the configuration for analogue alarms.
  */


#if !defined(AFX_ANALOGUEALARMCONFIGURATION_H__87E2826D_1B9F_4EBF_A08E_594090DF4949__INCLUDED_)
#define AFX_ANALOGUEALARMCONFIGURATION_H__87E2826D_1B9F_4EBF_A08E_594090DF4949__INCLUDED_

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
    class FieldValidityChecker;
    class AbstractDetailView;

    class AnalogueAlarmConfiguration : public CPropertyPage, virtual public IConfigurationPage
    {
	    DECLARE_DYNCREATE(AnalogueAlarmConfiguration)

    public:

        /**
         * Constructor
         */
	    AnalogueAlarmConfiguration();

        /**
         * Destructor
         */
	    ~AnalogueAlarmConfiguration();


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
	    //{{AFX_DATA(AnalogueAlarmConfiguration)
		enum { IDD = IDD_ALARM_CONFIG_ANALOGUE };
		CEdit	m_alarmThresholdControl;
		CEdit	m_loloLimitControl;
		CEdit	m_loLimitControl;
		CEdit	m_hihiLimitControl;
		CEdit	m_hiLimitControl;
		CEdit	m_loloDelayControl;
		CEdit	m_loDelayControl;
		CEdit	m_hihiDelayControl;
		CEdit	m_hiDelayControl;
		CEdit	m_loloMessageControl;
		CEdit	m_loMessageControl;
		CEdit	m_hiMessageControl;
		CEdit	m_hihiMessageControl;
		CEdit	m_overScaleLoAlarmSeverityControl;
		CEdit	m_overScaleHiAlarmSeverityControl;
		CEdit	m_overScaleLoAlarmMessageControl;
		CEdit	m_overScaleHiAlarmMessageControl;
		CEdit	m_overScaleLoAlarmLimitControl;
		CEdit	m_overScaleHiAlarmLimitControl;
		CEdit	m_overScaleLoAlarmDelayControl;
		CEdit	m_overScaleHiAlarmDelayControl;
	    BOOL	m_highCheck;
	    BOOL	m_highHighCheck;
	    BOOL	m_lowCheck;
	    BOOL	m_lowLowCheck;
		BOOL	m_overScaleHiAlarmChecked;
		BOOL	m_overScaleLoAlarmChecked;
		CString m_alarmThreshold;
	    CString	m_highDelay;
	    CString	m_highHighDelay;
	    CString	m_lowDelay;
	    CString	m_lowLowDelay;
	    CString	m_highMessage;
	    CString	m_highHighMessage;
	    CString	m_lowMessage;
	    CString	m_lowLowMessage;
	    CString	m_highSeverity;
	    CString	m_highHighSeverity;
	    CString	m_lowSeverity;
	    CString	m_lowLowSeverity;
	    CString	m_highLimit;
	    CString	m_highHighLimit;
	    CString	m_lowLimit;
	    CString	m_lowLowLimit;
		CString	m_overScaleHiAlarmDelay;
		CString	m_overScaleLoAlarmDelay;
		CString	m_overScaleHiAlarmLimit;
		CString	m_overScaleLoAlarmLimit;
		CString	m_overScaleHiAlarmMessage;
		CString	m_overScaleLoAlarmMessage;
		CString	m_overScaleHiAlarmSeverity;
		CString	m_overScaleLoAlarmSeverity;
        CComboBox m_hiMmsControl;
        CComboBox m_hihiMmsControl;
        CComboBox m_loMmsControl;
        CComboBox m_loloMmsControl;
		CComboBox	m_overScaleLoAlarmMmsControl;
		CComboBox	m_overScaleHiAlarmMmsControl;
	//}}AFX_DATA


    // Overrides
	    // ClassWizard generate virtual function overrides
	    //{{AFX_VIRTUAL(AnalogueAlarmConfiguration)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:
	    // Generated message map functions
	    //{{AFX_MSG(AnalogueAlarmConfiguration)
	    afx_msg void onChangeSeverityHigh();
	    afx_msg void onChangeSeverityHighHigh();
	    afx_msg void onChangeSeverityLowLow();
	    afx_msg void onChangeSeverityLow();
	    afx_msg void onCheckAnalogueHighAlarm();
	    afx_msg void onCheckAnalogueHighHighAlarm();
	    afx_msg void onCheckAnalogueLowAlarm();
	    afx_msg void onCheckAnalogueLowLowAlarm();
		afx_msg void onKillfocusEditAlarmThreshold();
	    afx_msg void onKillfocusEditDelayHigh();
	    afx_msg void onKillfocusEditDelayHighHigh();
	    afx_msg void onKillfocusEditDelayLowLow();
	    afx_msg void onKillfocusEditDelayLow();
	    afx_msg void onKillfocusEditMessageHigh();
	    afx_msg void onKillfocusEditMessageHighHigh();
	    afx_msg void onKillfocusEditMessageLow();
	    afx_msg void onKillfocusEditMessageLowLow();
	    afx_msg void onKillfocusEditLimitHigh();
	    afx_msg void onKillfocusEditLimitHighHigh();
	    afx_msg void onKillfocusEditLimitLow();
	    afx_msg void onKillfocusEditLimitLowLow();
        afx_msg void onSelChangeHiMms();
	    afx_msg void onSelChangeHiHiMms();
	    afx_msg void onSelChangeLowMms();
	    afx_msg void onSelChangeLowLowMms();
		virtual BOOL OnInitDialog();
		afx_msg void onChangeSeverityOverScaleHi();
		afx_msg void onChangeSeverityOverScaleLo();
		afx_msg void onCheckAnalogueOverScaleHiAlarm();
		afx_msg void onCheckAnalogueOverScaleLoAlarm();
		afx_msg void onSelchangeComboMmsOverScaleHi();
		afx_msg void onSelchangeComboMmsOverScaleLo();
		afx_msg void onKillfocusEditDelayOverScaleHi();
		afx_msg void onKillfocusEditDelayOverScaleLo();
		afx_msg void onKillfocusEditLimitOverScaleHi();
		afx_msg void onKillfocusEditLimitOverScaleLo();
		afx_msg void onKillfocusEditMessageOverScaleHi();
		afx_msg void onKillfocusEditMessageOverScaleLo();
		afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
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

		static const std::string ALARM_THRESHOLD;

        static const std::string LOLO_ALARM_ENABLED;
        static const std::string LO_ALARM_ENABLED;
        static const std::string HIHI_ALARM_ENABLED;
        static const std::string HI_ALARM_ENABLED;

        static const std::string LOLO_ALARM_DELAY;
        static const std::string LO_ALARM_DELAY;
        static const std::string HIHI_ALARM_DELAY;
        static const std::string HI_ALARM_DELAY;

        static const std::string LOLO_ALARM_MESSAGE;
        static const std::string LO_ALARM_MESSAGE;
        static const std::string HIHI_ALARM_MESSAGE;
        static const std::string HI_ALARM_MESSAGE;

        static const std::string LOLO_ALARM_SEVERITY;
        static const std::string LO_ALARM_SEVERITY;
        static const std::string HIHI_ALARM_SEVERITY;
        static const std::string HI_ALARM_SEVERITY;

        static const std::string LOLO_ALARM_LIMIT;
        static const std::string LO_ALARM_LIMIT;
        static const std::string HIHI_ALARM_LIMIT;
        static const std::string HI_ALARM_LIMIT;

        static const std::string HI_ALARM_MMS;
        static const std::string HIHI_ALARM_MMS;
        static const std::string LO_ALARM_MMS;
        static const std::string LOLO_ALARM_MMS;

		//
		// over scale high/low alarms
		//

        static const std::string OH_ALARM_ENABLED;
        static const std::string OH_ALARM_DELAY;
		static const std::string OH_ALARM_MESSAGE;
		static const std::string OH_ALARM_SEVERITY;
		static const std::string OH_ALARM_LIMIT;
		static const std::string OH_ALARM_MMS;

        static const std::string OL_ALARM_ENABLED;
        static const std::string OL_ALARM_DELAY;        
        static const std::string OL_ALARM_MESSAGE;        
        static const std::string OL_ALARM_SEVERITY;
        static const std::string OL_ALARM_LIMIT;     
        static const std::string OL_ALARM_MMS;

		static const std::string ALARM_LIMIT_PRECISION; //TD15446

    private:

        static const CString CHANGE_ALARMSEVERITY_DIALOG_TITLE;
        static const int MESSAGE_FIELD_LENGTH;
        static const int DELAY_FIELD_LENGTH;
        static const int LIMIT_FIELD_LENGTH;
        static const std::string NO_SEVERITY;
		
		std::string m_alarmLimitFormatString; //TD15446

		UINT m_previousScrollBoxPos;


        /**
         * loadEntityIntoPage
         *
         * This will load all the entity details into the page. This checks that the window has been
         * created (ie OnInitDialog has been called) before attempting to populate the controls
         */
        void loadEntityIntoPage();


        /**
         * loadXXXData
         *
         * Each of these methods populates a section of the page with the data from m_currentEntity
         */
        void loadHiHiData();
        void loadHiData();
        void loadLoData();
        void loadLoLoData();
		void loadOverScaleHiData();
		void loadOverScaleLoData();


        /** 
         * refresh
         *
         * This refreshes the enabled and disabled items
         */
        void refresh();


        /**
         * checkValidity
         *
         * This will check a single field to see that it has a valid value entered for it
         *
         * @param string - This is the name of the parameter to check
         * @param int - This is an ID into the string table holding the parameters display name.
         * @param FieldValidityChecker - This can be used to check the validity of the field
         * @param vector<string>& - This is what we add the parameter to if it is mandatory but has not
         *                          been entered.
         */
        void checkValidity(const std::string& parameterName,
                           int displayNameStringId,
                           const FieldValidityChecker& checker,
                           std::vector<std::string>& params );


        TA_Base_Core::IConfigEntity* m_currentEntity;
		TA_Base_Core::DataPointEntityData* m_dataPointEntityData; //TD15446

        AbstractDetailView* m_parent;

        ScadaDatabaseAccessor* m_databaseAccessor;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_ANALOGUEALARMCONFIGURATION_H__87E2826D_1B9F_4EBF_A08E_594090DF4949__INCLUDED_)
