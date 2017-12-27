/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/sound_file/src/SoundFileDetailView.cpp $
  * @author:  Brad Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an sound file's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/sound_file/src/StdAfx.h"
#include "app/configuration/config_plugins/sound_file/src/SoundFileDetailView.h"
#include "app/configuration/config_plugins/sound_file/src/SoundFileDatabaseAccessor.h"
#include "app/configuration/config_plugins/sound_file/src/SoundFileListView.h"
#include "app/configuration/config_plugins/sound_file/src/SoundFileComponent.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/sound_manager/src/ISoundCategory.h"
#include "core/data_access_interface/sound_manager/src/IConfigSoundFile.h"
#include "core/data_access_interface/sound_manager/src/SoundCategoryAccessFactory.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DateFormat.h" // TD12474 

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
    const int SoundFileDetailView::DESCRIPTION_FIELD_LENGTH = 256;
    const int SoundFileDetailView::SOUNDFILE_FIELD_LENGTH = 256;

    
    IMPLEMENT_DYNCREATE(SoundFileDetailView, CFormView)


    SoundFileDetailView::SoundFileDetailView() : 
        m_currentSoundFile(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(SoundFileDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(SoundFileDetailView)
	//}}AFX_DATA_INIT
    }


    SoundFileDetailView::~SoundFileDetailView()
    {
    }


    void SoundFileDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(SoundFileDetailView)
	    DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	    DDX_Control(pDX, IDC_BITMAP_DESCRIPTION, m_bitmapDescription);
	    DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_editDescription);
	    DDX_Control(pDX, IDC_BITMAP_SOUNDFILE, m_bitmapSoundFile);
	    DDX_Control(pDX, IDC_EDIT_SOUNDFILE, m_editSoundFile);
	    DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
	    DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
	    //}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(SoundFileDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(SoundFileDetailView)
	ON_EN_KILLFOCUS(IDC_EDIT_DESCRIPTION, onKillfocusEditDescription)
	ON_BN_CLICKED(IDC_BUTTON_SOUNDFILE, onButtonSoundFile)
	ON_EN_KILLFOCUS(IDC_EDIT_SOUNDFILE, onKillfocusEditSoundFile)
    ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
    ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void SoundFileDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();

        // Set up the controls for editing priorities.
        m_editDescription.SetLimitText( DESCRIPTION_FIELD_LENGTH );

        // Set up the controls for editing sound files.
        m_editSoundFile.SetLimitText( SOUNDFILE_FIELD_LENGTH );

        // Hide all the alarm plan association details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void SoundFileDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view
        GetDlgItem(IDC_STATIC_TYPE)->ShowWindow(isShown);

        GetDlgItem(IDC_STATIC_DESCRIPTION)->ShowWindow(isShown);
        GetDlgItem(IDC_BITMAP_DESCRIPTION)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_DESCRIPTION)->ShowWindow(isShown);

        GetDlgItem(IDC_STATIC_SOUNDFILE)->ShowWindow(isShown);
        GetDlgItem(IDC_BITMAP_SOUNDFILE)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_SOUNDFILE)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_SOUNDFILE)->ShowWindow(isShown);

        GetDlgItem(IDC_STATIC_DATE_CREATED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_CREATEDATE)->ShowWindow(isShown);

        GetDlgItem(IDC_STATIC_DATE_MODIFIED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_MODIFYDATE)->ShowWindow(isShown);

        GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
    }


    void SoundFileDetailView::refreshButtonAvailability()
    {
        if ( m_currentSoundFile == NULL )
        {
            return;
        }

        if ( m_currentSoundFile->isNew() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if ( m_currentSoundFile->hasChanged() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
        }
        else
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(false);
        }

        // Update the status in the list view.
        m_listView->itemDetailsChanged( m_currentSoundFile->getUniqueIdentifier(), "" );
    }

    
    void SoundFileDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if ( m_databaseAccessor == NULL || m_listView == NULL )
        {
            m_databaseAccessor = dynamic_cast<SoundFileDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<SoundFileListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper SoundFileDetailView");
        }


        m_currentSoundFile = dynamic_cast<TA_Base_Core::IConfigSoundFile*>(currentItem);

        if ( m_currentSoundFile == NULL )
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            // Populate description control.
            m_editDescription.SetWindowText( m_currentSoundFile->getDescription().c_str() );

            // Populate sound file control.
            m_editSoundFile.SetWindowText( m_currentSoundFile->getSoundFile().c_str() );

            if( m_currentSoundFile->isNew() )
            {
                // Entity is new, so has no dates.
                m_type.SetWindowText(NEW_LABEL);
                m_modifyDate.SetWindowText(NO_VALUE);
	            m_createDate.SetWindowText(NO_VALUE);
            }
            else
            {
                m_type.SetWindowText( NO_VALUE );

				// TD12474 ++
				TA_Base_Core::DateFormat dateFormat; 
				std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat6);
				// ++ TD12474

                // The create time should always be set so we don't need to perform the same checks as the
                // modify date.
                COleDateTime dateCreated ( m_currentSoundFile->getDateCreated() );
                // TD12474 ++
                //m_createDate.SetWindowText( dateCreated.Format(TA_Configuration::DATE_FORMAT) );
                m_createDate.SetWindowText( dateCreated.Format(dateFormatStr.c_str()) );
                // ++ TD12474

                // Retrieve the modify time and date. If this is zero then one hasn't been set yet
                // so leave the modified field as blank
                time_t modify = m_currentSoundFile->getDateModified();
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
            }
            
            showItemDetails(true);

            refreshButtonAvailability();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve alarm plan association details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve alarm plan association details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        FUNCTION_EXIT;
    }


    void SoundFileDetailView::onButtonSoundFile() 
    {
        FUNCTION_ENTRY("onButtonSoundFile");

        // Get the current sound file
        CString sound;
        m_editSoundFile.GetWindowText(sound);

        OPENFILENAME openFileNameStruct;
        ZeroMemory(&openFileNameStruct, sizeof(OPENFILENAME));
        openFileNameStruct.lStructSize = sizeof(OPENFILENAME);
        openFileNameStruct.hwndOwner = *this;
        openFileNameStruct.lpstrFile = sound.GetBuffer(SOUNDFILE_FIELD_LENGTH);
        openFileNameStruct.nMaxFile = SOUNDFILE_FIELD_LENGTH;
        openFileNameStruct.lpstrFilter = "Sound\0*.WAV\0All\0*.*\0";
        openFileNameStruct.nFilterIndex = 1;
        openFileNameStruct.lpstrFileTitle = NULL;
        openFileNameStruct.nMaxFileTitle = 0;
        openFileNameStruct.lpstrInitialDir = NULL;
        openFileNameStruct.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        
        // Show the open file dialog
        if( GetOpenFileName(&openFileNameStruct) )
        {
            try
            {
                // Update the database
                m_currentSoundFile->setSoundFile( std::string(sound) );
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not set sound file.");
            
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_030001);
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not set sound file.");

                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_010001);
            }

            
            // Update the text field
            m_editSoundFile.SetWindowText(sound);
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "User cancelled selection of a new sound");
        }

		refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void SoundFileDetailView::onKillfocusEditDescription()
    {
        FUNCTION_ENTRY("onKillfocusEditDescription");

        // Get the description from the edit control.
        CString descriptionStr;
        m_editDescription.GetWindowText( descriptionStr );

        // Save it.
        try
        {
            // Update the database.
            m_currentSoundFile->setDescription( std::string(descriptionStr) );
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not set category.");
        
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not set category.");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void SoundFileDetailView::onKillfocusEditSoundFile() 
    {
        FUNCTION_ENTRY("onKillfocusEditDescription");

        // Get the file name from the edit control.
        CString soundFileStr;
        m_editSoundFile.GetWindowText( soundFileStr );

        // Save it.
        try
        {
            // Update the database
            m_currentSoundFile->setSoundFile( std::string(soundFileStr) );
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not set category.");
        
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not set category.");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

		refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    
    void SoundFileDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed( m_currentSoundFile );

        FUNCTION_EXIT;
    }


    void SoundFileDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        applyButtonPressed( m_currentSoundFile );

        // Force this call so our status column updates
        m_listView->itemDetailsChanged( m_currentSoundFile->getUniqueIdentifier(), "" );

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // SoundFileDetailView diagnostics

    #ifdef _DEBUG
    void SoundFileDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void SoundFileDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}

