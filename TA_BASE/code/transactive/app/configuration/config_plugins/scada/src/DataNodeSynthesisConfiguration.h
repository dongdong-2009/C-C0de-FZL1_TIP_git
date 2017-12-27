/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/DataNodeSynthesisConfiguration.h $
  * @author:  Hoa Vu
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This property page holds the configuration of alarm synthesis for a DataNode
  */

#if !defined(AFX_DATANODESYNTHESISCONFIGURATION_H__889A1CA7_9C77_4137_BE40_99C314220A29__INCLUDED_)
#define AFX_DATANODESYNTHESISCONFIGURATION_H__889A1CA7_9C77_4137_BE40_99C314220A29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <vector>
#include <string>

#include "app/configuration/config_plugins/scada/src/resource.h"
#include "app/configuration/config_plugins/scada/src/IConfigurationPage.h"


namespace TA_Base_Core
{
	class IEntityData;
}

namespace TA_Base_Core
{
    class IConfigEntity;
}

namespace TA_Base_App
{
	// forward declaration
	class ScadaDatabaseAccessor;
	class AbstractDetailView;

	class DataNodeSynthesisConfiguration : public CPropertyPage, public virtual IConfigurationPage
	{
		// this macro enables this class (derived from CObject) to be created 
		// dynamically by Windows framework at run time.
		DECLARE_DYNCREATE(DataNodeSynthesisConfiguration)

	public:
        /**
         * Constructor
         */
		DataNodeSynthesisConfiguration();

		
        /**
         * Destructor
         */
		virtual ~DataNodeSynthesisConfiguration();


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
		//{{AFX_DATA(DataNodeSynthesisConfiguration)
	enum { IDD = IDD_DATA_NODE_SYNTHESIS_CONFIGURATION };
	CListBox	m_clistBox;
	//}}AFX_DATA


	// Overrides
		// ClassWizard generate virtual function overrides
		//{{AFX_VIRTUAL(DataNodeSynthesisConfiguration)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
	protected:
		// Generated message map functions
		//{{AFX_MSG(DataNodeSynthesisConfiguration)
	afx_msg void OnButtonAddEntity();
	afx_msg void OnButtonRemoveEntity();
	afx_msg void OnRadioHierarchy();
	afx_msg void OnRadioList();
	afx_msg void OnRadioBoth();
	afx_msg void OnRadioNone();
	virtual BOOL OnInitDialog();
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
         * setupSynthesisType
         *
         * This method obtains the synthesis type of the current DataNode entity and check
		 * the radio button accordingly
         *
         * @param	dbEntityData	The reference to the db entity of the current DataNode
         *
         */
		void setupSynthesisType ( TA_Base_Core::IEntityData * dbEntityData );


        /**
         * setupListOfChildEntities
         *
         * This method obtains the child entities of the current DataNode and display them
		 * on screen.
         *
         * @param	dbEntityData	The reference to the db entity of the current DataNode
         *
         */
		void setupListOfChildEntities ( TA_Base_Core::IEntityData * dbEntityData );


        /**
         * parseTheListForEntityName 
         *
         * This method parses the string containing the names of the child entities and put
		 * the individual names into the specified list;
         *
         * @param	listOfChildEntities		The list of entities names
		 * @param	namesOfChildEntities	The individual entities names.  This is
		 *									in/out parameter
         *
         */
		void parseTheListForEntityName ( const std::string & listOfChildEntities, std::vector < std::string > & namesOfChildEntities );


        /**
         * addEntityNameToListOfChildEntities
         *
         * This method adds the specified name to the list of child entities' names
         *
         * @param	selectedName		The name to be added to the list
         *
         */
		bool addEntityNameToListOfChildEntities ( const std::string & selectedName );


        /**
         * removeEntityNameFromListOfChildEntities
         *
         * This method removes the specified name from the list of child entities' names
         *
         * @param	selectedName		The name to be removed from the list
         *
         */
		void removeEntityNameFromListOfChildEntities ( const std::string & selectedName );


		std::string m_synthesisType;
		std::string m_listOfChildEntities;

        TA_Base_Core::IConfigEntity * m_currentEntity;
        ScadaDatabaseAccessor * m_databaseAccessor;
        AbstractDetailView * m_parent;
	};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}

#endif // !defined(AFX_DATANODESYNTHESISCONFIGURATION_H__889A1CA7_9C77_4137_BE40_99C314220A29__INCLUDED_)
