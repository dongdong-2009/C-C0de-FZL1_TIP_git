/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/message_type/src/MessageTypeDetailView.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an MessageType's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/message_type/src/StdAfx.h"
#include "app/configuration/config_plugins/message_type/src/MessageTypeDatabaseAccessor.h"
#include "app/configuration/config_plugins/message_type/src/MessageTypeListView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/config_plugins/message_type/src/MessageTypeDetailView.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/entity_access/src/ConfigurationEditor.h"
#include "core/data_access_interface/src/IConfigMessageType.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DateFormat.h" // TD12474 

#include "boost/regex.hpp"

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
    IMPLEMENT_DYNCREATE(MessageTypeDetailView, CFormView)


    MessageTypeDetailView::MessageTypeDetailView() : 
        m_currentMessageType(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(MessageTypeDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(MessageTypeDetailView)
	//}}AFX_DATA_INIT
    }


    MessageTypeDetailView::~MessageTypeDetailView()
    {
    }


    void MessageTypeDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(MessageTypeDetailView)
	DDX_Control(pDX, IDC_CHECK_LOG_MESSAGE, m_checkLogMessage);
	DDX_Control(pDX, IDC_EDIT_VIEW_LEVEL, m_viewLevel);
	DDX_Control(pDX, IDC_SPIN_VIEW_LEVEL, m_viewLevelSpin);
	DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
	DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
	DDX_Control(pDX, IDC_VIEW_LEVEL_BITMAP, m_viewLevelBitmap);
	DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_description);
	DDX_Control(pDX, IDC_EDIT_NAME, m_name);
	DDX_Control(pDX, IDC_CHECK_PRINT, m_checkPrint);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(MessageTypeDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(MessageTypeDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
    ON_BN_CLICKED(IDC_CHECK_PRINT, onCheckPrint)
	ON_EN_UPDATE(IDC_EDIT_VIEW_LEVEL, onUpdateEditViewLevel)
	ON_BN_CLICKED(IDC_CHECK_LOG_MESSAGE, onCheckLogMessage)
	ON_EN_KILLFOCUS(IDC_EDIT_DESCRIPTION, onKillfocusEditDescription)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void MessageTypeDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();

        // Configure the spin control.
        m_viewLevelSpin.SetBuddy( &m_viewLevel );
        m_viewLevelSpin.SetRange( 0, TA_Base_Core::ConfigurationEditor::getEventLevels() );
	    
        // Hide all the MessageType details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void MessageTypeDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view
        GetDlgItem(IDC_STATIC_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DESCRIPTION)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_PRINT)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_DESCRIPTION)->ShowWindow(isShown);
        GetDlgItem(IDC_CHECK_PRINT)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
        GetDlgItem(IDC_PRINT_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_TYPE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_LOG_MESSAGE)->ShowWindow(isShown);
        GetDlgItem(IDC_CHECK_LOG_MESSAGE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_VIEW_LEVEL)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_VIEW_LEVEL)->ShowWindow(isShown);
        GetDlgItem(IDC_SPIN_VIEW_LEVEL)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_CREATED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_MODIFIED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_CREATEDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_MODIFYDATE)->ShowWindow(isShown);        
    }


    void MessageTypeDetailView::refreshButtonAvailability()
    {
        if (m_currentMessageType == NULL)
        {
            return;
        }

        if (m_currentMessageType->isNew() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if (m_currentMessageType->hasChanged())
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

    
    void MessageTypeDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<MessageTypeDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<MessageTypeListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper MessageTypeDetailView");
        }


        m_currentMessageType = dynamic_cast<TA_Base_Core::IConfigMessageType*>(currentItem);

        if (m_currentMessageType == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigMessageType* object
            m_name.SetWindowText( m_currentMessageType->getDisplayName().c_str() );
	        m_description.SetWindowText( m_currentMessageType->getDescription().c_str() );

            // Save the variables that must be part of the description.
            m_descriptionParams = extractParametersFromText( m_currentMessageType->getDescription() );

            // Set whether the message should be logged.
            m_checkLogMessage.SetCheck( m_currentMessageType->toBeLogged() ? 1 : 0  );

			// TD14169 azenitha++
			if (m_checkLogMessage.GetCheck() == 1)
			// If Log Event is checked, enable Print Event
			{
				m_checkPrint.EnableWindow( true ); 

				//                     Should the checkbox be checked?       yes  no
				m_checkPrint.SetCheck( m_currentMessageType->toBePrinted() ? 1  : 0 );
			}
			else
			// If Log Event is unchecked, uncheck and disable Print Event
			{
				m_checkPrint.SetCheck( false );
				m_checkPrint.EnableWindow( false ); 
			}
			// TD14169 ++azenitha
			
            // Set the level of the message.
            m_viewLevelSpin.SetPos( m_currentMessageType->getViewLevel() );

			// TD12474 ++
			TA_Base_Core::DateFormat dateFormat; 
			std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1);
			// ++ TD12474

            // Set the created date.
            COleDateTime dateCreated ( m_currentMessageType->getDateCreated() );
            // TD12474 ++
            //m_createDate.SetWindowText( dateCreated.Format(TA_Configuration::DATE_FORMAT) );
            m_createDate.SetWindowText( dateCreated.Format(dateFormatStr.c_str()) );
            // ++ TD12474 

            // Set the modified date, if it exists.
            time_t modify = m_currentMessageType->getDateModified();
            if (modify == 0)
            {
                m_modifyDate.SetWindowText(NO_VALUE);
            }
            else
            {
                COleDateTime dateModified( modify );
                // TD12474 ++
                //m_modifyDate.SetWindowText( dateModified.Format(TA_Configuration::DATE_FORMAT) );
                m_modifyDate.SetWindowText( dateModified.Format(dateFormatStr.c_str()) );
                // ++ TD12474
            }

            showItemDetails(true);

            refreshButtonAvailability();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve MessageType details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve MessageType details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        FUNCTION_EXIT;
    }


    void MessageTypeDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_currentMessageType);

        FUNCTION_EXIT;
    }


    void MessageTypeDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        if ( isDescriptionValid( m_descriptionParams, extractParametersFromText( m_currentMessageType->getDescription() ) ) )
        {
            // Allow changes to be saved if description is valid.
            applyButtonPressed(m_currentMessageType);
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


    void MessageTypeDetailView::onCheckPrint() 
    {
	    TA_ASSERT(m_currentMessageType != NULL, "The current message type is NULL");

        try
        {
            int checked = m_checkPrint.GetCheck();
            if (checked == 1)
            {
                m_currentMessageType->setPrintMessageStatus(true);
            }
            else
            {
                m_currentMessageType->setPrintMessageStatus(false);
            }
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);

            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);

            return;
        }

        refreshButtonAvailability();
    }


    void MessageTypeDetailView::onUpdateEditViewLevel() 
    {
	    TA_ASSERT(m_currentMessageType != NULL, "The current message type is NULL");

        // Convert the current level text into an int.
        CString levelStr;
        m_viewLevel.GetWindowText( levelStr );

        // Allow empty strings at this point to make manual editing easier, just don't save
        // the change.
        if ( 0 < levelStr.GetLength() )
        {
            unsigned int level;
            std::stringstream strToInt;
            strToInt << std::string( levelStr );
            strToInt >> level;

            // Update the spin control - this will also clamp out of range values!
            m_viewLevelSpin.SetPos( level );

            try
            {
                // Save the change.
                m_currentMessageType->setViewLevel( level );

                // Update button states.
                refreshButtonAvailability();
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_010001);

                return;
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_030001);

                return;
            }
        }
    }


    void MessageTypeDetailView::onCheckLogMessage() 
    {
	    TA_ASSERT(m_currentMessageType != NULL, "The current message type is NULL");

        try
        {
            int checked = m_checkLogMessage.GetCheck();
            if (checked == 1)
            {
				// TD14169 azenitha++
				// If Log Event is checked, enable Print Event
				m_checkPrint.EnableWindow( true ); 
				// TD14169 ++azenitha

                m_currentMessageType->setLogMessageStatus(true);
            }
            else
            {
				// TD14169 azenitha++
				// If Log Event is unchecked, disable and uncheck Print Event
				m_checkPrint.EnableWindow( false ); 
				m_checkPrint.SetCheck( false );
				// TD14169 ++azenitha

				m_currentMessageType->setPrintMessageStatus(false);
                m_currentMessageType->setLogMessageStatus(false);
            }
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);

            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);

            return;
        }

        // Update button states.
        refreshButtonAvailability();
    }

    void MessageTypeDetailView::onKillfocusEditDescription() 
    {
        // Get the text from the description edit control.
        CString text;
        m_description.GetWindowText( text );

        if ( NULL != m_currentMessageType )
        {
            try
            {
                // Save the description.
                m_currentMessageType->setDescription( std::string(text) );
                refreshButtonAvailability();
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_010001);

                return;
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_030001);

                return;
            }
        }
    }

    std::vector<std::string> MessageTypeDetailView::extractParametersFromText( const std::string& text )
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

    bool MessageTypeDetailView::isDescriptionValid( std::vector<std::string> expectedParams,
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

    /////////////////////////////////////////////////////////////////////////////
    // MessageTypeDetailView diagnostics

    #ifdef _DEBUG
    void MessageTypeDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void MessageTypeDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}

