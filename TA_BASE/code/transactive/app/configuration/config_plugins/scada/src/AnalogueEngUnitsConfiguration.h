/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/AnalogueEngUnitsConfiguration.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This property page holds the configuration for analogue engineering units.
  */

#if !defined(AFX_ANALOGUEENGUNITSCONFIGURATION_H__86AD6913_4DC0_461D_B78C_A94B44E92100__INCLUDED_)
#define AFX_ANALOGUEENGUNITSCONFIGURATION_H__86AD6913_4DC0_461D_B78C_A94B44E92100__INCLUDED_

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
}

namespace TA_Base_App
{
    class FieldValidityChecker;
    class AbstractDetailView;

    class AnalogueEngUnitsConfiguration : public CPropertyPage, virtual public IConfigurationPage
    {
	    DECLARE_DYNCREATE(AnalogueEngUnitsConfiguration)

    public:
        /**
         * Constructor
         */
	    AnalogueEngUnitsConfiguration();


        /**
         * Destructor
         */
	    ~AnalogueEngUnitsConfiguration();


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
	    //{{AFX_DATA(AnalogueEngUnitsConfiguration)
    	enum { IDD = IDD_ENGUNITS_CONFIG_ANALOGUE };
	    CTransparentImage	m_bitmapUpdateTime;
	    CTransparentImage	m_bitmapUpdateThresh;
	    CTransparentImage	m_bitmapRawLow;
	    CTransparentImage	m_bitmapRawHigh;
	    CTransparentImage	m_bitmapZeroValue;
	    CTransparentImage	m_bitmapUnits;
	    CTransparentImage	m_bitmapEngLow;
	    CTransparentImage	m_bitmapEngHigh;
	    CEdit	m_zeroThresholdControl;
	    CEdit	m_updateTimeLimitControl;
	    CEdit	m_updateThresholdControl;
	    CEdit	m_unitsControl;
	    CEdit	m_rawLowLimitControl;
	    CEdit	m_rawHighLimitControl;
	    CEdit	m_precisionControl;
	    CEdit	m_engLowLimitControl;
	    CEdit	m_engHighLimitControl;
	    BOOL	m_isEngUnitsChecked;
	    CString	m_engHighLimit;
	    CString	m_engLowLimit;
	    CString	m_precision;
	    CString	m_rawHighLimit;
	    CString	m_rawLowLimit;
	    CString	m_units;
	    CString	m_updateThreshold;
	    CString	m_updateTimeLimit;
	    CString	m_zeroThreshold;
	//}}AFX_DATA

    // Overrides
	    // ClassWizard generate virtual function overrides
	    //{{AFX_VIRTUAL(AnalogueEngUnitsConfiguration)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL


	public:

        // These are the entity parameters to retrieve

        static const std::string ENGUNITS_ENABLED;
        static const std::string UNITS;
        static const std::string RAW_HIGH_LIMIT;
        static const std::string RAW_LOW_LIMIT;
        static const std::string ENG_HIGH_LIMIT;
        static const std::string ENG_LOW_LIMIT;
        static const std::string UPDATE_THRESHOLD;
        static const std::string UPDATE_TIME_LIMIT;
        static const std::string ZERO_VAL_THRESHOLD;
        static const std::string PRECISION;


    // Implementation
    protected:
	    // Generated message map functions
	    //{{AFX_MSG(AnalogueEngUnitsConfiguration)
	    afx_msg void onCheckAnalogueEngUnits();
	    afx_msg void onKillfocusEditEngHighLimit();
	    afx_msg void onKillfocusEditEngLowLimit();
	    afx_msg void onKillfocusEditPrecision();
	    afx_msg void onKillfocusEditRawHighLimit();
	    afx_msg void onKillfocusEditRawLowLimit();
	    afx_msg void onKillfocusEditUnits();
	    afx_msg void onKillfocusEditUpdateThresh();
	    afx_msg void onKillfocusEditUpdateTimeLimit();
	    afx_msg void onKillfocusEditZeroThresh();
	    virtual BOOL OnInitDialog();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
        // The limit in length for the text fields.
        static const int RAW_HIGH_FIELD_LENGTH;
        static const int RAW_LOW_FIELD_LENGTH;
        static const int ENG_HIGH_FIELD_LENGTH;
        static const int ENG_LOW_FIELD_LENGTH;
        static const int UPDATE_THRESHOLD_FIELD_LENGTH;
        static const int UNITS_FIELD_LENGTH;
        static const int UPDATE_TIME_LIMIT_FIELD_LENGTH;
        static const int ZERO_VALUE_FIELD_LENGTH;
        static const int PRECISION_FIELD_LENGTH;


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
         * checkMandatory
         *
         * This will check a single mandatory field to see that it has a value entered for it
         *
         * @param string - This is the name of the parameter to check
         * @param int - This is an ID into the string table holding the parameters display name.
         * @param vector<string>& - This is what we add the parameter to if it is mandatory but has not
         *                          been entered.
         */
        void checkMandatory(const std::string& parameterName,
                            int displayNameStringId,
                            std::vector<std::string>& params );


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

		//TD 17818 - marc_bugfix
        /**
         * preProcessUnit
         *
         * This will preprocess the unit string returned by database
         *
         * @param unitString - This is the name of the parameter to check
         */
		void preProcessUnitLabel(CString& unitString);


        TA_Base_Core::IConfigEntity* m_currentEntity;

        AbstractDetailView* m_parent;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_ANALOGUEENGUNITSCONFIGURATION_H__86AD6913_4DC0_461D_B78C_A94B44E92100__INCLUDED_)
