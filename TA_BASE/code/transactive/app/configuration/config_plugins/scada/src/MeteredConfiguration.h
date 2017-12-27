/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/MeteredConfiguration.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This property page holds the configuration for metered data points.
  */

#if !defined(AFX_METEREDCONFIGURATION_H__74FA8462_5B89_453B_8900_93DE374A84EA__INCLUDED_)
#define AFX_METEREDCONFIGURATION_H__74FA8462_5B89_453B_8900_93DE374A84EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>

#include "app/configuration/config_plugins/scada/src/resource.h"
#include "app/configuration/config_plugins/scada/src/IConfigurationPage.h"
#include "app/configuration/config_plugins/scada/src/OutputDataPointListCtrl.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"

namespace TA_Base_Core
{
    class IConfigEntity;
}

namespace TA_Base_App
{
    class AbstractDetailView;
    class FieldValidityChecker;

    class MeteredConfiguration : public CPropertyPage, public virtual IConfigurationPage
    {
	    DECLARE_DYNCREATE(MeteredConfiguration)

    public:
        /**
         * Constructor
         */
	    MeteredConfiguration();

        
        /**
         * Destructor
         */
	    ~MeteredConfiguration();


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
	    //{{AFX_DATA(MeteredConfiguration)
	enum { IDD = IDD_METERED_CONFIG };
	CEdit	m_rolloverConstantControl;
	CEdit	m_meterFactorControl;
	CEdit	m_correctionValueControl;
	    CTransparentImage	m_bitmapRolloverConstant;
	    CTransparentImage	m_bitmapMeterFactor;
	    CTransparentImage	m_bitmapInputDependency;
	    CString	m_correctionValue;
	    CString	m_inputDependency;
	    CString	m_meterFactor;
	    CString	m_rolloverConstant;
	//}}AFX_DATA


    // Overrides
	    // ClassWizard generate virtual function overrides
	    //{{AFX_VIRTUAL(MeteredConfiguration)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:
	    // Generated message map functions
	    //{{AFX_MSG(MeteredConfiguration)
	    virtual BOOL OnInitDialog();
	    afx_msg void onChangeInputDependency();
	    afx_msg void onKillfocusEditCorrectionValue();
	    afx_msg void onKillfocusEditMeterFactor();
	    afx_msg void onKillfocusEditRolloverConstant();
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:

        /**
         * loadEntityIntoPage
         *
         * This will load all the entity details into the page. This checks that the window has been
         * created (ie OnInitDialog has been called) before attempting to populate the controls
         */
        void loadEntityIntoPage();


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

    public:
        // These are the entity parameters to retrieve
        static const std::string INPUT_DEPENDENCY;
        static const std::string ROLLOVER_CONSTANT;
        static const std::string CORRECTION_VALUE;
        static const std::string METER_FACTOR;

    private:
        static const int ROLLOVER_FIELD_LENGTH;
        static const int CORRECTION_FIELD_LENGTH;
        static const int METER_FACTOR_FIELD_LENGTH;

        static const CString CHANGE_INPUT_DIALOG_TITLE;


        TA_Base_Core::IConfigEntity* m_currentEntity;

        ScadaDatabaseAccessor* m_databaseAccessor;

        AbstractDetailView* m_parent;

    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_METEREDCONFIGURATION_H__74FA8462_5B89_453B_8900_93DE374A84EA__INCLUDED_)
