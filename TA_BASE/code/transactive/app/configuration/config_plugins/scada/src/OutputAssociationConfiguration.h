/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/OutputAssociationConfiguration.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This property page holds the configuration for derived datapoint output associations
  */

#if !defined(AFX_OUTPUTASSOCIATIONCONFIGURATION_H__FEC5C23D_56DA_448E_80A8_6BFC1DC12805__INCLUDED_)
#define AFX_OUTPUTASSOCIATIONCONFIGURATION_H__FEC5C23D_56DA_448E_80A8_6BFC1DC12805__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#include "app/configuration/config_plugins/scada/src/resource.h"
#include "app/configuration/config_plugins/scada/src/IConfigurationPage.h"
#include "app/configuration/config_plugins/scada/src/OutputDataPointListCtrl.h"

namespace TA_Base_Core
{
    class IConfigEntity;
}

namespace TA_Base_App
{
    class AbstractDetailView;

    class OutputAssociationConfiguration : public CPropertyPage, public virtual IConfigurationPage
    {
	    DECLARE_DYNCREATE(OutputAssociationConfiguration)

    public:
        /**
         * Constructor
         */
	    OutputAssociationConfiguration();

        
        /**
         * Destructor
         */
	    ~OutputAssociationConfiguration();


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
	    //{{AFX_DATA(OutputAssociationConfiguration)
	    enum { IDD = IDD_OUTPUT_ASSOCIATION_CONFIG };
	    //}}AFX_DATA
	    OutputDataPointListCtrl	m_outputList;


    // Overrides
	    // ClassWizard generate virtual function overrides
	    //{{AFX_VIRTUAL(OutputAssociationConfiguration)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:
	    // Generated message map functions
	    //{{AFX_MSG(OutputAssociationConfiguration)
	    virtual BOOL OnInitDialog();
	    afx_msg void onButtonAdd();
	    afx_msg void onButtonRemove();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
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


        TA_Base_Core::IConfigEntity* m_currentEntity;

        ScadaDatabaseAccessor* m_databaseAccessor;

        AbstractDetailView* m_parent;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_OUTPUTASSOCIATIONCONFIGURATION_H__FEC5C23D_56DA_448E_80A8_6BFC1DC12805__INCLUDED_)
