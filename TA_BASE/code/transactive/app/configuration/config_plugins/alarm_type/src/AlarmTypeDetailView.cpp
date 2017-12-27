/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_type/src/AlarmTypeDetailView.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an AlarmType's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/alarm_type/src/StdAfx.h"
#include "app/configuration/config_plugins/alarm_type/src/AlarmTypeDatabaseAccessor.h"
#include "app/configuration/config_plugins/alarm_type/src/AlarmTypeListView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/config_plugins/alarm_type/src/AlarmTypeDetailView.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/IConfigAlarmType.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DateFormat.h" // TD12474 

#include "boost/regex.hpp"
#include "app/configuration/config_plugins/alarm_type/src/ChineseSupport.h"
using namespace alarmType_chinese;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{
    // These match the restrictions in the database for how long certain fields can be.
    // Hopefully in the future these could be read from the database.
    const int AlarmTypeDetailView::NAME_FIELD_LENGTH        = 80;
    const int AlarmTypeDetailView::DESCRIPTION_FIELD_LENGTH = 1024;

    IMPLEMENT_DYNCREATE(AlarmTypeDetailView, CFormView)


    AlarmTypeDetailView::AlarmTypeDetailView() : 
        m_currentAlarmType(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(AlarmTypeDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(AlarmTypeDetailView)
	//}}AFX_DATA_INIT
    }

    AlarmTypeDetailView::~AlarmTypeDetailView()
    {
    }


    void AlarmTypeDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(AlarmTypeDetailView)
	DDX_Control(pDX, IDC_COMBO_MMS, m_MmsStateControl); //TD17628
	DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_dateModified);
	DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_dateCreated);
	DDX_Control(pDX, IDC_ALARMSEVERITY_BITMAP, m_severityBitmap);
	//TD18279 zhiqiang-- DDX_Control(pDX, IDC_PRINT_BITMAP, m_printBitmap);
	DDX_Control(pDX, IDC_SYSTEM_BITMAP, m_systemBitmap);
	DDX_Control(pDX, IDC_LIST_ATTRIBUTES, m_parameterList);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_description);
	DDX_Control(pDX, IDC_EDIT_NAME, m_name);
	DDX_Control(pDX, IDC_EDIT_SEVERITY, m_severity);
	//TD18279 zhiqiang-- DDX_Control(pDX, IDC_CHECK_PRINT, m_checkPrint);
	DDX_Control(pDX, IDC_CHECK_SYSTEM, m_checkSystem);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(AlarmTypeDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(AlarmTypeDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_EN_KILLFOCUS(IDC_EDIT_NAME, onKillfocusEditName)
	ON_EN_KILLFOCUS(IDC_EDIT_DESCRIPTION, onKillfocusEditDescription)
    ON_BN_CLICKED(IDC_BUTTON_SEVERITY, onButtonSeverity)
    //TD18279 zhiqiang-- ON_BN_CLICKED(IDC_CHECK_PRINT, onCheckPrint)
    ON_BN_CLICKED(IDC_CHECK_SYSTEM, onCheckSystem)
	ON_CBN_SELCHANGE(IDC_COMBO_MMS, onSelChangeMmsState)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void AlarmTypeDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        // Limit the number of characters that can be entered into fields so that they match the database schema.
        m_name.SetLimitText(NAME_FIELD_LENGTH);
        m_description.SetLimitText(DESCRIPTION_FIELD_LENGTH);

        // Hide all the AlarmType details until we are told to populate.
	    showItemDetails(false);
		
		m_MmsStateControl.ResetContent();

        FUNCTION_EXIT;
    }


    void AlarmTypeDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view
        GetDlgItem(IDC_STATIC_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DESCRIPTION)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_SEVERITY)->ShowWindow(isShown);
        //TD18279 zhiqiang-- GetDlgItem(IDC_STATIC_PRINT)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_SYSTEM)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_DESCRIPTION)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_SEVERITY)->ShowWindow(isShown);
        //TD18279 zhiqiang-- GetDlgItem(IDC_CHECK_PRINT)->ShowWindow(isShown);
        GetDlgItem(IDC_CHECK_SYSTEM)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_SEVERITY)->ShowWindow(isShown);
        GetDlgItem(IDC_ALARMSEVERITY_BITMAP)->ShowWindow(isShown);
        //TD18279 zhiqiang-- GetDlgItem(IDC_PRINT_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_SYSTEM_BITMAP)->ShowWindow(false);		//TD18279, modified by hongzhi
        GetDlgItem(IDC_STATIC_TYPE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_CREATED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_MODIFIED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_CREATEDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_MODIFYDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_ATTRIBUTES)->ShowWindow(isShown);
        GetDlgItem(IDC_LIST_ATTRIBUTES)->ShowWindow(isShown);
		//TD17628 marc_bugfix support for mms state
        GetDlgItem(IDC_COMBO_MMS)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_MMS_ENABLED)->ShowWindow(isShown);
   }


    void AlarmTypeDetailView::refreshButtonAvailability()
    {
        if (m_currentAlarmType == NULL)
        {
            return;
        }

        if (m_currentAlarmType->isNew() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if (m_currentAlarmType->hasChanged())
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
        }
        else
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(false);
        }
    }

    
    void AlarmTypeDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<AlarmTypeDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<AlarmTypeListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper AlarmTypeDetailView");
        }


        m_currentAlarmType = dynamic_cast<TA_Base_Core::IConfigAlarmType*>(currentItem);

        if (m_currentAlarmType == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigAlarmType* object
            m_name.SetWindowText( m_currentAlarmType->getDisplayName().c_str() );
	        m_description.SetWindowText( m_currentAlarmType->getDescription().c_str() );
            m_severity.SetWindowText( m_currentAlarmType->getSeverityName().c_str() );

            // Save the variables that must be part of the description.
            m_descriptionParams = extractParametersFromText( m_currentAlarmType->getDescription() );
            
            //                       Should the checkbox be checked?       yes  no
            //TD18279 zhiqiang-- m_checkPrint.SetCheck(   m_currentAlarmType->toBePrinted()   ? 1  : 0 );
            m_checkSystem.SetCheck(   m_currentAlarmType->isSystemAlarm()   ? 1  : 0 ); 
			m_oldIsSystemAlarmStatus = m_currentAlarmType->isSystemAlarm();
			//TD17628
			IAlarmData::EMmsState mmsType = m_currentAlarmType->getRelatedMMSType(); 
			//TD17628 marc_bugfix 
			setupMmsStates();
			
			setSelectedMmsState(mmsType);
			if (m_oldIsSystemAlarmStatus == false)
			{
				//If the system alarm checkbox is unchecked , disable the mms state combobox
				m_MmsStateControl.EnableWindow(false);	
			}
			else
			{
				m_MmsStateControl.EnableWindow(true);
			}
			//++TD17628
/*
This functionality is not currently required.
            if( m_currentAlarmType->isNew() )
            {
                // This is a new AlarmType or at least a copy of an AlarmType so we indicate this in the type
                // field and make sure the modify and create dates are blank.
                m_type.SetWindowText(NEW_LABEL);
            }
            else
            {
*/
                m_type.SetWindowText( NO_VALUE );
//            }

			// TD12474 ++
// 			TA_Base_Core::DateFormat dateFormat; 
// 			std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat6);
// 			// ++ TD12474
		    std::string dateFormatStr = DATE_FORMAT_STR;

            // Set the created date.
            COleDateTime dateCreated ( m_currentAlarmType->getDateCreated() );
            // TD12474 ++
            //m_dateCreated.SetWindowText( dateCreated.Format(TA_Configuration::DATE_FORMAT) );
            m_dateCreated.SetWindowText( dateCreated.Format(dateFormatStr.c_str()) );
            // ++ TD12474

            // Set the modified date, if it exists.
            time_t modify = m_currentAlarmType->getDateModified();
            if (modify == 0)
            {
                m_dateModified.SetWindowText(NO_VALUE);
            }
            else
            {
                COleDateTime dateModified( modify );
				// TD12474 ++
				//m_dateModified.SetWindowText( dateModified.Format(TA_Configuration::DATE_FORMAT) );
				m_dateModified.SetWindowText( dateModified.Format(dateFormatStr.c_str()) );
				// ++ TD12474
            }

// 			CFont font;
// 			font.CreatePointFont(FONT_SIZE,FONT_NAME,NULL); // 10pt Arial Bold
			// Only populate the list if it is system alarm
			if (m_currentAlarmType->isSystemAlarm()) {
				m_parameterList.populateControl(m_currentAlarmType);

			}
			else m_parameterList.DeleteAllItems();

            showItemDetails(true);

            refreshButtonAvailability();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve AlarmType details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve AlarmType details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        FUNCTION_EXIT;
    }


    void AlarmTypeDetailView::onKillfocusEditName() 
    {
/*
This functionality is not currently required
        FUNCTION_ENTRY("onKillfocusEditName");

        // When the user changes the name we must update the IConfigAlarmType* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentAlarmType != NULL )
        {
            try
            {
                CString name;
                m_name.GetWindowText(name);
                m_currentAlarmType->setName( std::string(name) );

                if (m_listView!=NULL)
                {
                    m_listView->itemDetailsChanged( m_currentAlarmType->getUniqueIdentifier(), m_currentAlarmType->getName().c_str() );
                }
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve alarm type details to display");
            
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_030001);
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve alarm type details to display");

                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_010001);
            }

        }   

        refreshButtonAvailability();

        FUNCTION_EXIT;
*/
    }


    void AlarmTypeDetailView::onKillfocusEditDescription() 
    {
        FUNCTION_ENTRY("onKillfocusEditDescription");

        // When the user changes the description we must update the IConfigAlarmType* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentAlarmType != NULL )
        {
            try
            {
                CString description;
                m_description.GetWindowText(description);
                m_currentAlarmType->setDescription( std::string(description) );
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve alarm type details to display");
            
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_030001);
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve alarm type details to display");

                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_010001);
            }
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void AlarmTypeDetailView::onButtonSeverity()
    {
	    FUNCTION_ENTRY("onButtonSeverity");
	 
        // Retrieve the current severity this display is linked to
        CString currentSeverity("");
        m_severity.GetWindowText( currentSeverity );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllSeverities();

        // Now construct the dialog and then wait for the user to finish with it
        ChangeDialog dlg(items, CHANGE_ALARMSEVERITY_DIALOG_TITLE, currentSeverity, this);
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of a new severity");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new severity which is %s", dlg.getSelectedName());

        try
        {
            TA_ASSERT(m_currentAlarmType !=  NULL, "Current alarm type is NULL.");

            // If this is a different severity then set it in our IConfigAlarmType*.
            m_currentAlarmType->setSeverityKey( dlg.getSelectedKey() );

            // Now update the severity the details view is showing
            m_severity.SetWindowText( dlg.getSelectedName() );

        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve alarm type details to display");
        
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve alarm type details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }



    void AlarmTypeDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_currentAlarmType);

        FUNCTION_EXIT;
    }


    void AlarmTypeDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        if ( isDescriptionValid( m_descriptionParams, extractParametersFromText( m_currentAlarmType->getDescription() ) ) )
        {
			// reset the parameter list if the IsSystem alarm is changed
			if ( m_oldIsSystemAlarmStatus == true && m_currentAlarmType->isSystemAlarm() == false )
			{
				m_parameterList.resetParameters();
				m_oldIsSystemAlarmStatus = m_currentAlarmType->isSystemAlarm();
			}

			// check for the mandatory parameters' value
			if ( m_currentAlarmType->isSystemAlarm() == true && m_parameterList.validateEmptyValues() )
			{
				return;
			}

            // Allow changes to be saved if description is valid.
            applyButtonPressed(m_currentAlarmType);
        }
        else
        {
            // Otherwise display the relevant error message and don't save the changes.

            // Convert the parameter list to a string.
            std::string paramList;
            if ( 0 < m_descriptionParams.size() )
            {
                paramList = m_descriptionParams[0];
                for ( unsigned int i=1; i<m_descriptionParams.size(); i++ )
                {
                    paramList += ", " + m_descriptionParams[i];
                }
            }
            else
            {
                paramList = "NONE";
            }

            // Display the error message.
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << paramList;
            userMsg.showMsgBox(IDS_UE_030066);
        }

        FUNCTION_EXIT;
    }

    /*TD18279 zhiqiang--begin
    void AlarmTypeDetailView::onCheckPrint() 
    {
	    TA_ASSERT(m_currentAlarmType != NULL, "The current alarm type is NULL");

        try
        {
            int checked = m_checkPrint.GetCheck();
            if (checked == 1)
            {
                m_currentAlarmType->setPrintAlarmStatus(true);
            }
            else
            {
                m_currentAlarmType->setPrintAlarmStatus(false);
            }
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve alarm type details to display");
        
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve alarm type details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }


        refreshButtonAvailability();
    }
    //*///TD18279 zhiqiang--end

    void AlarmTypeDetailView::onCheckSystem() 
    {
	    TA_ASSERT(m_currentAlarmType != NULL, "The current alarm type is NULL");

        try
        {
            int checked = m_checkSystem.GetCheck();
            if (checked == 1)
            {
                m_currentAlarmType->setSystemAlarmStatus(true);
				// show the parameter in the attribute list
				m_parameterList.populateControl(m_currentAlarmType);
				//TD17862 marc_bugfix support for mms state checkbox 
				m_MmsStateControl.EnableWindow(true);				
            }
            else
            {
                m_currentAlarmType->setSystemAlarmStatus(false);
				// hide the parameter in the attribute list and set the param to empty
				m_parameterList.DeleteAllItems();
				m_MmsStateControl.EnableWindow(false);				
            }
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve alarm type details to display");
        
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve alarm type details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }


        refreshButtonAvailability();
    }


    std::vector<std::string> AlarmTypeDetailView::extractParametersFromText( const std::string& text )
    {
        std::vector<std::string> parameters;

        // This method needs to extract all text that lies between square brackets. While a handmade
        // parse could be written to accomplish this its much easier to make use of Boosts regular
        // expression capabilities.

        // The bounds of the string that will be searched.
        std::string::const_iterator start = text.begin();
        std::string::const_iterator end = text.end();

        // The regular expression to use for searching.
        // This expression will match any text that encased in square brackets that doesn't contain
        // square brackets. I.e. [ [test] ] will match [test] not [ [test] ].
        boost::regex expression( "\\[[^\\[\\]]*\\]" );

        // The variables that will store search results.
        boost::match_results<std::string::const_iterator> what;

        // Loop through all matches.
        while( boost::regex_search( start, end, what, expression, boost::match_default ) )
        {
            // Push back the variable name (including the square brackets).
            parameters.push_back( what[0] );

            // Search for the next variable from the end of the last match.
            start = what[0].second;
        }

        return parameters;
    }


    bool AlarmTypeDetailView::isDescriptionValid( std::vector<std::string> expectedParams,
                                                    std::vector<std::string> actualParams )
    {
        // Iterators for comparing the vectors. These are also used when generating the error message.
        std::vector<std::string>::iterator expectedIt;
        std::vector<std::string>::iterator actualIt;

        // Now compare the vectors.
        expectedIt = expectedParams.begin();
        while ( expectedParams.end() != expectedIt )
        {
            bool parameterFound = false;

            // Start search from the beginning of the vector.
            actualIt = actualParams.begin();
            while ( actualParams.end() != actualIt )
            {
                if ( *actualIt == *expectedIt )
                {
                    parameterFound = true;
                    // A match was found so remove the variable, making sure
                    // to save the iterator to the next element.
                    actualIt = actualParams.erase( actualIt );
                    break;
                }
                else
                {
                    // No match, so just move on to the next element.
                    actualIt++;
                }
            }

            if ( parameterFound )
            {
                // A match was found so remove the variable, making sure
                // to save the iterator to the next element.
                expectedIt = expectedParams.erase( expectedIt );
            }
            else
            {
                // No match, so just move on to the next element.
                expectedIt++;
            }
        }

        // At this point the following two conditions are true:
        // 1. 'expectedParams' contains all variables that should exist but are missing.
        // 2. 'actualParams' contains all variables that shouldnt exist but are present.

        return ( ( 0 == expectedParams.size() ) && ( 0 == actualParams.size() ) );
    }

	void AlarmTypeDetailView::setupMmsStates()
	{

		TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

		if (m_MmsStateControl.GetCount() > 0) return;

		std::map<IAlarmData::EMmsState, std::string> data = m_databaseAccessor->getMmsAlarmTypes();
		
        for (std::map<IAlarmData::EMmsState, std::string>::iterator itr = data.begin(); itr != data.end(); ++itr )
        {
            m_MmsStateControl.AddString(itr->second.c_str());
        }		
	}


	void AlarmTypeDetailView::setSelectedMmsState(IAlarmData::EMmsState mmsState)
	{
		TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

		std::map<IAlarmData::EMmsState, std::string> data = m_databaseAccessor->getMmsAlarmTypes();
		std::map<IAlarmData::EMmsState, std::string>::iterator itr = data.find(mmsState);
		if (itr == data.end())
		{
			// As a rule this should never happen (the default selection is "None") -
            // but not a fatal condition if this does happen
			m_MmsStateControl.SetCurSel(-1);
		}
		else
		{
            // Find and set the appropriate mode in combo
            int selIndex = m_MmsStateControl.FindStringExact(-1, itr->second.c_str());
            m_MmsStateControl.SetCurSel(selIndex);
		}

	}

	IAlarmData::EMmsState AlarmTypeDetailView::getSelectedMmsState()
	{
        int curSel = m_MmsStateControl.GetCurSel();
		IAlarmData::EMmsState mmsStateType = IAlarmData::TYPE_NONE;

		TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (curSel >= 0)
        {
            CString selString;
            m_MmsStateControl.GetLBText(curSel, selString);

            // Find the appropriate string in the alarm types map
            std::map<IAlarmData::EMmsState, std::string> data = m_databaseAccessor->getMmsAlarmTypes();
            
            for (std::map<IAlarmData::EMmsState, std::string>::iterator itr = data.begin();
                    itr != data.end(); ++itr)
            {
                if (stricmp(selString.GetBuffer(0), itr->second.c_str()) == 0)
                {
                    // Return the corresponding state value, as description field matches
                    return itr->first;                    
                }
            }
		}
		
		// If item not found, MMS_NONE is returned
        return IAlarmData::TYPE_NONE;
	}

	void AlarmTypeDetailView::onSelChangeMmsState()
	{
		TA_ASSERT(m_currentAlarmType != NULL, "The current alarm type is NULL");
		
		IAlarmData::EMmsState mmsStateType = getSelectedMmsState();
		
		m_currentAlarmType->setRelatedMMSType(mmsStateType);		
		
        refreshButtonAvailability();	   
		
	}
    /////////////////////////////////////////////////////////////////////////////
    // AlarmTypeDetailView diagnostics

    #ifdef _DEBUG
    void AlarmTypeDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void AlarmTypeDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}

