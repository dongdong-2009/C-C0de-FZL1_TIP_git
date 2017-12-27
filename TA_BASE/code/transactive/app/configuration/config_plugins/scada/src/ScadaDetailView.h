/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/ScadaDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a Scada item's details.
  */

#if !defined(AFX_SCADAVIEW_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_SCADAVIEW_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#include "app/configuration/config_plugins/scada/src/resource.h"
#include "app/configuration/config_plugins/scada/src/ParameterListCtrl.h"

// Property Pages
#include "app/configuration/config_plugins/scada/src/DataNodeSynthesisConfiguration.h"
#include "app/configuration/config_plugins/scada/src/AttributeConfiguration.h"
#include "app/configuration/config_plugins/scada/src/BooleanAlarmConfiguration.h"
#include "app/configuration/config_plugins/scada/src/AnalogueAlarmConfiguration.h"
#include "app/configuration/config_plugins/scada/src/BooleanEngUnitsConfiguration.h"
#include "app/configuration/config_plugins/scada/src/AnalogueEngUnitsConfiguration.h"
#include "app/configuration/config_plugins/scada/src/HelpConfiguration.h"
#include "app/configuration/config_plugins/scada/src/StateConfiguration.h"
#include "app/configuration/config_plugins/scada/src/InputAssociationConfiguration.h"
//#include "app/configuration/config_plugins/scada/src/InterlockingConfiguration.h"
#include "app/configuration/config_plugins/scada/src/OutputAssociationConfiguration.h"
#include "app/configuration/config_plugins/scada/src/MeteredConfiguration.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"
#include "boost/shared_ptr.hpp"
//TD18137 - marc_bugfix begin
#include "bus/mathematical_evaluation/src/Token.h"
#include "bus/mathematical_evaluation/src/IBooleanContext.h"
#include "bus/mathematical_evaluation/src/IRealNumberContext.h"
//TD18137 - marc_bugfix end

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif


namespace TA_Base_Core
{
    class IConfigEntity;
}

class CMultiFontPropertySheet;

namespace TA_Base_App
{
    class ScadaDatabaseAccessor;
    class ScadaTreeView;

    class AttributeConfiguration;

    class ScadaDetailView : public AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    ScadaDetailView();


        /**
         * Destructor
         */
	    virtual ~ScadaDetailView();

        
        DECLARE_DYNCREATE(ScadaDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(ScadaDetailView)
	enum { IDD = IDD_SCADA_VIEW };
	CTransparentImage	m_agentBitmap;
	CTransparentImage	m_subsystemBitmap;
	CTransparentImage	m_addressBitmap;
	CTransparentImage	m_descriptionBitmap;
	CTransparentImage	m_parentBitmap;
	CTransparentImage	m_nameBitmap;
	CTransparentImage	m_locationBitmap;
	CEdit	m_agent;
	CEdit	m_parent;
	CEdit	m_subsystem;
	CEdit	m_location;
	CStatic	m_modifyDate;
	CStatic	m_createDate;
	CButton	m_type;
	CEdit	m_name;
	CEdit	m_description;
	CEdit	m_address;
	//}}AFX_DATA


        /**
         * populateItemDetails
         *
         * This will populate all fields in the view using the database object passed in.
         *
         * @param IConfigItem* - The database object for the scada item we want to display. We will use this to
         *                         set new values and apply. If this is NULL then the details should all be hidden
         */
        void populateItemDetails(TA_Base_Core::IConfigItem* currentEntity);


        /**
         * refreshButtonAvailability
         *
         * This should be called after any modification to the fields. This will enable or disable
         * the reset and apply buttons based on whether or not any data has changed.
         */
        virtual void refreshButtonAvailability();

        /**
         * Sets up the input combo box with default states for MMS Alarm modes - text
         *  strings populated in combo are long format (descriptive), there are associated
         *  state value's (shorted) that should be used for database storage/retrieval
         * @see getMmsStateFromCombo, setMmsStateComboSelection
         */
        static void setupMmsCombo(CComboBox& combo);

        /**
         * @return the internal MMS state value as determined by the selection active in 'combo'
         *      Assumes the combo box has been set up by function setupMmsCombo
         */
        static std::string getMmsStateFromCombo(CComboBox& combo);

        /**
         * Updates the selection of the input combo box (set upo with setupMmsCombo), using
         *  the input state which defines the internal MMS state value
         */
        static void setMmsStateComboSelection(CComboBox& combo, const std::string& state);

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ScadaDetailView)
	public:
	virtual void OnInitialUpdate();
	protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

    protected:

    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif


	    // Generated message map functions
	    //{{AFX_MSG(ScadaDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
	    afx_msg void onButtonChangeLocation();
	    afx_msg void onButtonChangeParent();
	    afx_msg void onKillfocusEditName();
	    afx_msg void onKillfocusEditDescription();
	    afx_msg void onKillfocusEditAddress();
	    afx_msg void onButtonChangeSubsystem();
	    afx_msg void onButtonChangeAgent();
	//}}AFX_MSG
        afx_msg LRESULT onWatchedParameterUpdated(WPARAM,LPARAM);
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
            return MessageSender::ENTITY;
        }


        /**
         * applyButtonPressed
         *
         * This can be called to perform an apply. This has been overwritten as it must
         * apply both the datapoint and the derived data point details.
         *
         * @param IConfigItem* - The configuration item to apply changes to.
         */
        virtual void applyButtonPressed(TA_Base_Core::IConfigItem* currentItem);


        /**
         * resetButtonPressed
         *
         * This can be called to perform an apply. This has been overwritten as it must
         * rest both the datapoint and the derived data point details.
         *
         * @param IConfigItem* - The configuration item to reset.
         */
        virtual void resetButtonPressed(TA_Base_Core::IConfigItem* currentItem);


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        ScadaDetailView( const ScadaDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        ScadaDetailView& operator=(const ScadaDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the SCADA item details. These controls will be
         * hidden until the user selects a SCADA item from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        void showItemDetails(bool isShown);


        /**
         * setUpForDataPoint
         *
         * Datapoints require certain pages to be added to the property sheet. The pages to add depends on
         * the datapoint type and some of it's settings. This method will look at all this and determine the
         * pages to add.
         */
        void setUpForDataPoint();


        /**
         * setUpForDataNode
         *
         * DataNode require certain pages to be added to the property sheet. This method will look at this
		 * and determine the pages to add.
         */
		void setUpForDataNode();


    public:
        // The name of the parameter that can be used to retrieve the type of a datapoint
        static const std::string DATAPOINT_TYPE;
        static const std::string ALLOW_WRITE;

        // The different possible SCADA datapoint types
        static const std::string ANALOGUE_TYPE;
        static const std::string TEXT_TYPE;
        static const std::string BOOLEAN_TYPE;
        static const std::string DERIVED_TYPE;
        static const std::string METERED_TYPE;
        static const std::string SAFETY_OUTPUT_TYPE;
        static const std::string COMBINED_OUTPUT_TYPE;
        static const std::string ENUM_TYPE;

        static const std::string TA_FALSE;
        static const std::string TA_TRUE;

    private:

		//TD18137 - marc_bugfix begin
		class BooleanContextImpl : public TA_Base_Bus::IBooleanContext
		{
		public:
		/**
	    * Constructor
	    * 
	    */
			BooleanContextImpl(){}

		/**
	    * Destructor
	    * 
	    */
			virtual ~BooleanContextImpl(){}

			virtual bool getValue(const std::string& variableName) const
			{
				return false;
			}  
		};

		class RealNumberContextImpl : public TA_Base_Bus::IRealNumberContext
		{
		public:
		/**
	    * Constructor
	    * 
	    */
			RealNumberContextImpl(){}

		/**
	    * Destructor
	    * 
	    */
			virtual ~RealNumberContextImpl(){}

			virtual double getValue(const std::string& variableName) const
			{
				return 0.0;
			}  
		};


		static const std::string LAUNCH_CONDITION;
		static const std::string RETURN_CONDITION;

		typedef boost::shared_ptr< TA_Base_Bus::Token > TokenPtr;
        /**
         * checkLCCandRCCCondition
         *
         * This function checks the validity of the LaunchCondition and ReturnCondition expressions
         *
		 */
		bool checkLCCandRCCCondition();
        /**
         * bEvaluateExpression
         *
         * This function performs syntax checking of the given expression
         *
		 * @param const std::string &expression - The given boolean expression
		 * @param const int expression_type - either LCC or RCC expression
		 */

		bool bEvaluateExpression(const std::string &expression, const int expression_type);
		//TD18137 - marc_bugfix end
		


        // The titles for the change dialog box
        static const CString CHANGE_LOCATION_DIALOG_TITLE;
        static const CString CHANGE_SUBSYSTEM_DIALOG_TITLE;
        static const CString CHANGE_AGENT_DIALOG_TITLE;

		// The names of fields.
		static const std::string ADDRESS_FIELD_NAME;

        // These match the restrictions in the database for how long certain fields can be.
        // Hopefully in the future these could be read from the database.
        static const int NAME_FIELD_LENGTH;
        static const int ADDRESS_FIELD_LENGTH;
        static const int DESCRIPTION_FIELD_LENGTH;


        CMultiFontPropertySheet* m_propertySheet;
        
        // The property pages that can be added to the property sheet
        AttributeConfiguration m_attributePage;
        BooleanAlarmConfiguration m_booleanAlarmPage;
        BooleanEngUnitsConfiguration m_booleanEngUnitsPage;
        AnalogueAlarmConfiguration m_analogueAlarmPage;
        AnalogueEngUnitsConfiguration m_analogueEngUnitsPage;
        HelpConfiguration m_helpPage;        
        StateConfiguration m_statePage;
        InputAssociationConfiguration m_inputPage;
        OutputAssociationConfiguration m_outputPage;
        MeteredConfiguration m_meteredPage;
		DataNodeSynthesisConfiguration m_dataNodeSynthesisConfiguration;
        //InterlockingConfiguration m_interlockingConfigurationPage;

        CPropertyPage* m_currentlySelectedPage;     // This is used to remeber what page the user was last
                                                    // viewing so they can be returned to that page.

        TA_Base_Core::IConfigEntity* m_currentEntity;   // The current scada item the user is configuring.
                                                   // This is the database object for it.

        ScadaTreeView* m_listView; // This is the view in the left pane of the splitter
                                    // showing the list of all entities. We need to use this to
                                    // tell it when SCADA item details have changed.

        ScadaDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve information
                                                     // from the database.


    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_SCADAVIEW_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
