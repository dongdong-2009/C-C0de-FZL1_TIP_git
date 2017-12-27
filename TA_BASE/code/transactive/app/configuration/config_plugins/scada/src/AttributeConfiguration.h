/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/AttributeConfiguration.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This property page holds the attribute list used for all entities. In this case it is put in a property
  * page so the general attributes can be configured with the other attributes
  */

#if !defined(AFX_ATTRIBUTECONFIGURATION_H__1E4A37D7_2457_415C_943E_64DC43F3897A__INCLUDED_)
#define AFX_ATTRIBUTECONFIGURATION_H__1E4A37D7_2457_415C_943E_64DC43F3897A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//
#include "app/configuration/config_plugins/scada/src/resource.h"
#include "app/configuration/config_plugins/scada/src/IConfigurationPage.h"
#include "app/configuration/config_plugins/scada/src/ParameterListCtrl.h"

namespace TA_Base_Core
{
    class IConfigEntity;
}

namespace TA_Base_App
{
    class AttributeConfiguration : public CPropertyPage, virtual public IConfigurationPage
    {
	    DECLARE_DYNCREATE(AttributeConfiguration)

    // Construction
    public:
        /**
         * Constructor
         */
	    AttributeConfiguration();

        /**
         * Destructor
         */
	    ~AttributeConfiguration();


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
        virtual void getUsedParameters(std::set<std::string>& parametersToIgnore)
        {
            // Don't do anything as we don't use any specific parameters.
            return;
        }


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


        /**
         * setParametersToIgnore
         *
         * This class allows configuration of all entity parameters that are visible. Because some parameters
         * are used in other classes we do not want to display all visible parameters. This method receives
         * the parameters that we are to ignore.
         *
         * @param set<string> - The parameters we can ignore
         */
        void setParametersToIgnore(std::set<std::string>& parametersToIgnore)
        {
            m_parametersToIgnore = parametersToIgnore;
        }


        /**
         * setWatchParameters
         *
         * This method is called to set some parameters that must be monitored. When any of these parameters
         * are changed then the callback window must be notified with the passed-in message.
         *
         * @param set<string> - The names of the parameters to look for. When one of these changes the window
         *                      must be notified.
         * @param CWnd* - This is the callback window to notify when one of the parameters change.
         * @param UINT - This is the message to send when one of the parameters changes
         */
        void setWatchParameters(std::set<std::string> paramsToWatch, CWnd* callbackWindow, UINT messageToPost);


	    //{{AFX_DATA(AttributeConfiguration)
	enum { IDD = IDD_ATTRIBUTE_CONFIG };
	ParameterListCtrl	m_attributeList;
	//}}AFX_DATA


    // Overrides
	    // ClassWizard generate virtual function overrides
	    //{{AFX_VIRTUAL(AttributeConfiguration)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:
	    // Generated message map functions
	    //{{AFX_MSG(AttributeConfiguration)
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
        std::set<std::string> m_parametersToIgnore;

        TA_Base_Core::IConfigEntity* m_currentEntity;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_ATTRIBUTECONFIGURATION_H__1E4A37D7_2457_415C_943E_64DC43F3897A__INCLUDED_)
