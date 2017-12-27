/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_type/src/AlarmTypeDetailView.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an Alarm Type's details.
  */

#if !defined(AFX_AlarmTypeDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_AlarmTypeDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/alarm_type/src/resource.h"
#include "app/configuration/config_plugins/alarm_type/src/ParameterListCtrl.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"
#include "core/data_access_interface/src/IAlarmData.h" //TD17628


#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigAlarmType;
}

using TA_Base_Core::IAlarmData; //TD17628

namespace TA_Base_App
{
    class AlarmTypeDatabaseAccessor;
    class AlarmTypeListView;


    class AlarmTypeDetailView : public AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    AlarmTypeDetailView();


        /**
         * Destructor
         */
	    virtual ~AlarmTypeDetailView();

        
        DECLARE_DYNCREATE(AlarmTypeDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(AlarmTypeDetailView)
		enum { IDD = IDD_ALARMTYPE_VIEW };
	    CTransparentImage	m_mmsBitmap;
		CComboBox m_MmsStateControl;
		CStatic	m_dateModified;
		CStatic	m_dateCreated;
	    CTransparentImage	m_severityBitmap;
	    //TD18279 zhiqiang-- CTransparentImage	m_printBitmap;
	    CTransparentImage	m_systemBitmap;
		ParameterListCtrl	m_parameterList;
	    CButton	m_type;
	    CEdit	m_description;
	    CEdit	m_name;
	    CEdit	m_severity;
	    //TD18279 zhiqiang-- CButton	m_checkPrint;
		CButton m_checkSystem;
	//}}AFX_DATA


        /**
         * populateItemDetails
         *
         * This will populate all fields in the view using the database object passed in.
         *
         * @param IConfigItem* - The database object for the item we want to display. We will use this to
         *                       set new values and apply. If this is NULL then the details should all be hidden
         */
        void populateItemDetails(TA_Base_Core::IConfigItem* currentItem);


        /**
         * refreshButtonAvailability
         *
         * This should be called after any modification to the fields. This will enable or disable
         * the reset and apply buttons based on whether or not any data has changed.
         */
        virtual void refreshButtonAvailability();


		//TD17628
		 /** 
         * setupMmsStates
         *
         * This should populate the MMS states
         * 
         */
        virtual void setupMmsStates();

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(AlarmTypeDetailView)
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
	    //{{AFX_MSG(AlarmTypeDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
	    afx_msg void onKillfocusEditName();
	    afx_msg void onKillfocusEditDescription();
        afx_msg void onButtonSeverity();
        //TD18279 zhiqiang-- afx_msg void onCheckPrint();
        afx_msg void onCheckSystem();
		//TD17628
		afx_msg void onSelChangeMmsState();
	    //}}AFX_MSG
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
            return MessageSender::ALARMTYPE;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        AlarmTypeDetailView( const AlarmTypeDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        AlarmTypeDetailView& operator=(const AlarmTypeDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the AlarmType details. These controls will be
         * hidden until the user selects an Alarm Type from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);


        /**
         * extractParametersFromText
         *
         * This will return a vector of all the parameters in the provided string. I.e.
         * text that lies between square brackets.
         *
         * @param text The text to extract the parameters from.
         *
         * @return std::vector<std::string> The vector of parameters.
         */
        std::vector<std::string> extractParametersFromText( const std::string& text );


        /**
         * isDescriptionValid
         *
         * This will determine if the description contains all of the required parameters.
         *
         * @param expectedVariables The list of variables the description needs to have.
         * @param actualVariables The list of variables the description actually has.
         * NOTE: Copies of the vectors are used as they need to be modified during the comparison
         *       process.
         *
         * @return std::string The string describing the errors.
         */
        bool isDescriptionValid( std::vector<std::string> expectedVariables,
                                 std::vector<std::string> actualVariables );

		void setSelectedMmsState(IAlarmData::EMmsState mmsState);
		IAlarmData::EMmsState getSelectedMmsState();
    private:

        // These match the restrictions in the database for how long certain fields can be.
        // Hopefully in the future these could be read from the database.
        static const int NAME_FIELD_LENGTH;
        static const int DESCRIPTION_FIELD_LENGTH;


        TA_Base_Core::IConfigAlarmType* m_currentAlarmType;   // The current AlarmType the user is configuring.
                                                   // This is the database object for it.

        AlarmTypeListView* m_listView; // This is the view in the left pane of the splitter
                                    // showing the list of all alarm types. We need to use this to
                                    // tell it when AlarmType details have changed.

        AlarmTypeDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve information
                                                     // from the database.

        std::vector<std::string> m_descriptionParams; // The parameters that must be in the description.

		// This is to register the old IsSystem alarm status for display of parameters
		bool m_oldIsSystemAlarmStatus;
    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_ALARMTYPE}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_AlarmTypeDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
