/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/BurnCdDlg.cpp $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Dialog box to handle the CD Burn process.
  * This dialog is specific to CD-R and should be replaced with
  * a similar dialog if an alternative media is required.
  *
  */

#if defined( WIN32 )
#pragma warning ( disable : 4503 4786 ) 
#endif // defined( WIN32 )

#include <sstream>

#include "app/data_management/archive_manager/src/stdafx.h"
#include "app/data_management/archive_manager/src/BasicHelper.h"
#include "app/data_management/archive_manager/src/BurnCdDlg.h"
#include "app/data_management/archive_manager/src/ChineseInfo.h"
#include "app/data_management/archive_manager/src/MediaAccessWrapperFactory.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/generic_gui/src/TransactiveMessage.h"
#include "core/exceptions/src/ArchiveException.h"
#include "core/message/types/ArchiveAudit_MessageTypes.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    const std::string ARTIST = "Transactive";
}

namespace TA_Base_App
{
    
    BurnCdDlg::BurnCdDlg( const std::string& volumeLabel, const std::string& cdLabel, 
        const WriteArchiveHelper::FileDetailsVector& files, CWnd* pParent /*=NULL*/)
        : CDialog(BurnCdDlg::IDD, pParent),
          m_volumeLabel( volumeLabel ),
          m_cdLabel( cdLabel ),
          m_files( files ),
		  iMediaAccess(NULL)
    {
        //{{AFX_DATA_INIT(BurnCdDlg)
        m_csStage = _T("");
        m_csLog = _T("");
        //}}AFX_DATA_INIT
    }
        

    BurnCdDlg::~BurnCdDlg()
    {
        try
        {
            terminateAndWait();
        }
        catch( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Unknown exception caught in destructor." );
        }
    }


    void BurnCdDlg::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(BurnCdDlg)
	    DDX_Control(pDX, IDCANCEL, m_buttonCancel);
	    DDX_Control(pDX, IDC_STAGE_STATIC, m_stageStatic);
	    DDX_Control(pDX, IDC_LOG_EDIT, m_editLog);
        DDX_Control(pDX, IDC_WRITE_PROGRESS, m_writeProgress);
        DDX_Text(pDX, IDC_STAGE_STATIC, m_csStage);
        DDX_Text(pDX, IDC_LOG_EDIT, m_csLog);
	    //}}AFX_DATA_MAP
    }
    
    
    BEGIN_MESSAGE_MAP(BurnCdDlg, CDialog)
        //{{AFX_MSG_MAP(BurnCdDlg)
	    ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	    ON_MESSAGE(WM_CLOSE_DLG, OnCloseDlg)
    END_MESSAGE_MAP()
        

    void BurnCdDlg::run()
    {
        // Make sure no exceptions escape from the run() method.

        try
        {
			::CoInitializeEx(NULL, COINIT_MULTITHREADED);

			iMediaAccess = MediaAccessWrapperFactory::createIMAPIWrapper();
			iMediaAccess->RegisterBurnStatusCallback(this);

            // Set the state to PROCESSING here. 
            // The state will be set to IDLE in the OnCloseDlg method.

            m_stateManager.setState( StateManager::PROCESSING );

            //  Display an hour-glass cursor.

            HCURSOR hOldCursor = SetCursor( LoadCursor( NULL, IDC_APPSTARTING ) );

            try
            {
				appendStringToLogDisplay(INITIALIZING_MSG);
				if(!iMediaAccess->initialise(true))
				{

					CString reason;

					reason = TA_ArchiveManager::getStringFromMediaStatus(iMediaAccess->GetMediaStatus());

					appendStringToLogDisplay(std::string(reason));
					appendStringToLogDisplay(INITIALIZATION_FAIL_MSG);

					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
						"Failed to initialise hardware for %s ",m_volumeLabel.c_str());

					CString error;
					error.FormatMessage( UE050090_WRITE_FAILED, m_volumeLabel.c_str(), reason );

					TA_Base_Bus::TransActiveMessage userMsg;
					userMsg << m_volumeLabel << reason;
					UINT selectedButton = userMsg.showMsgBox(IDS_UE_050091,CAPTION_ARCHIVE_MANAGER);

					TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::CopyArchiveFailed, 
						std::string( error ) ); 
					PostMessage( WM_CLOSE_DLG, 0, IDABORT );
					return;
				}

				appendStringToLogDisplay(INITIALIZATION_SUCC_MSG);

				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"Initialised Hardware Successfully for %s ",m_volumeLabel.c_str());

				//print the devices found
				appendStringToLogDisplay(FOUND_DEVICES_MSG);

				vector<string> devicesFound = iMediaAccess->GetAllDeviceInfo();

				for(int i =0; i < devicesFound.size(); i++)
				{
					appendStringToLogDisplay(devicesFound[i]);
				}

				//print the selected device

				m_chosenDrive = iMediaAccess->GetSelectedDevice();
				appendStringToLogDisplay(DRIVE_MSG+ m_chosenDrive + SELECTED_ARCHIVE_MSG);


				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"Drive %s selected for archiving", iMediaAccess->GetSelectedDevice().c_str());

				for( WriteArchiveHelper::FileDetailsVector::const_iterator it = m_files.begin(); it != m_files.end(); ++it )
				{
					addFile( (*it).nameWithPath, (*it).name );
				}
              
                // Check that we do actually have files to write.  No point in setting 
                // up the hardware etc if there are no files to burn.  
        
				if(m_FilePathNameVector.size() == 0)
				{
					CString reason;
					reason.LoadString( UE050090_REASON_NO_FILES );
					CString error;
					error.FormatMessage( UE050090_WRITE_FAILED, m_volumeLabel.c_str(), reason );

					TA_Base_Bus::TransActiveMessage userMsg;
					userMsg << m_volumeLabel << reason;
					UINT selectedButton = userMsg.showMsgBox(IDS_UE_050091,CAPTION_ARCHIVE_MANAGER);

					TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::CopyArchiveFailed, 
						std::string( error ) ); 
					PostMessage( WM_CLOSE_DLG, 0, IDABORT );
					return;
				}
       
                // Prepare for the write, includes setting up the hardware and the CD configuration.
              
                // If the user has cancelled - don't go any further.

                if ( StateManager::STOPPING == m_stateManager.getState() )
                {
                    PostMessage( WM_CLOSE_DLG, 0, IDCANCEL );
                    return;
                }

				appendStringToLogDisplay(CONFIGURING_MSG);

				//IMAPI IBurnVerification is only supported in Windows 7 SDK v7.0 onwards
				//The current ArchiveManager does not support  Burn verification,
				//as the default windows 7 SDK version is 6.0
				iMediaAccess->setBurnVerfication(true);
				iMediaAccess->setForceOverwrite(false);
				iMediaAccess->setUnderburn(true);

				//set Label
				iMediaAccess->setVolumeLabel(m_volumeLabel);

				// Write away...

				appendStringToLogDisplay(PREPARE_WRITE_CD_MSG);

				bool writeSuccessful  = iMediaAccess->Burn(m_FilePathNameVector);

				//Eject Media After Write
				/*iMediaAccess->EjectMedia();
				iMediaAccess->Deinitialise();
				m_buttonCancel.SetWindowText( "Close" );*/

				if(!writeSuccessful)
				{
					//Eject Media After Write
					iMediaAccess->EjectMedia();
					iMediaAccess->Deinitialise();
					m_buttonCancel.SetWindowText( "Close" );

					CString reason;
					reason = TA_ArchiveManager::getStringFromMediaStatus(iMediaAccess->GetMediaStatus());

					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
						"Write failed for %s due to: %s",m_volumeLabel.c_str(),reason );

					m_stageStatic.SetWindowText(CD_WRITE_FAILED_MSG );
					appendStringToLogDisplay ( std::string( reason ) );
					appendStringToLogDisplay ( WRITE_FAILED_MSG );
					CString error;
					error.FormatMessage( UE050090_WRITE_FAILED, m_volumeLabel.c_str(), reason );

					TA_Base_Bus::TransActiveMessage userMsg;
					userMsg << m_volumeLabel << reason;
					UINT selectedButton = userMsg.showMsgBox(IDS_UE_050091,CAPTION_ARCHIVE_MANAGER);

					TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::CopyArchiveFailed, 
						std::string( error ) ); 
					PostMessage( WM_CLOSE_DLG, 0, IDABORT );
					return;

				}
				
				m_stageStatic.SetWindowText( CD_WRITE_SUCC_MSG);
				appendStringToLogDisplay (WRITE_COMPLETED_SUCC_MSG );
				
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"Write successful for %s ",m_volumeLabel.c_str());


                // If the user has cancelled - don't go any further.

                if ( StateManager::STOPPING == m_stateManager.getState() )
                {
                    PostMessage( WM_CLOSE_DLG, 0, IDCANCEL );
                    return;
                }

				bool bfound = true;
				std::string strCheck = TA_Base_Core::RunParams::getInstance().get( TA_ArchiveManager::RPARAM_CHECK_BURNED_FILES.c_str() );
				if(!strCheck.empty())
				{  
				//Check whether burned files exist in Drive - 
				std::string chosenDrive = m_chosenDrive + ":\\";
				if(TA_ArchiveManager::isValidDirectory(chosenDrive))
				{
					for( WriteArchiveHelper::FileDetailsVector::const_iterator it = m_files.begin(); it != m_files.end(); ++it )
					{
						//check burned files exits
						std::string burnedfile = chosenDrive + ( *it).name ;
						if(!TA_ArchiveManager::isValidFile(burnedfile))
						{
							LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "The Burned file(%s) is not exists in DVD",burnedfile.c_str() );
							bfound = false;
							break;
						}
					}
				}
					else
					{
						bfound = false;
					}

				}
				//Eject Media After Write
				iMediaAccess->EjectMedia();
				iMediaAccess->Deinitialise();
				m_buttonCancel.SetWindowText( "Close" );

				 if(!bfound)
				 {
					 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Any burned file is not exists in DVD, so returning Abort" );

					 m_stageStatic.SetWindowText( CD_WRITE_FAILED_MSG );
					 appendStringToLogDisplay ( WRITE_FAILED_MSG );

					 CString reason;
					 reason.LoadString( UE050090_REASON_UNKNOWN_ERROR );

					 CString error;
					 error.FormatMessage( UE050090_WRITE_FAILED, m_volumeLabel.c_str(), reason );

					 TA_Base_Bus::TransActiveMessage userMsg;
					 userMsg << m_volumeLabel << reason;
					 UINT selectedButton = userMsg.showMsgBox(IDS_UE_050091,CAPTION_ARCHIVE_MANAGER);

					 TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::CopyArchiveFailed, 
						 std::string( error ) ); 
					 PostMessage( WM_CLOSE_DLG, 0, IDABORT );
					 return;
				 }
				 else
				 {		
					 // Write successful.
					 m_stageStatic.SetWindowText( CD_WRITE_SUCC_MSG );
					 appendStringToLogDisplay ( WRITE_COMPLETED_SUCC_MSG);

					 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
						 "Write successful for %s ",m_volumeLabel.c_str());

					 TA_Base_Bus::TransActiveMessage userMsg;
					 userMsg << m_volumeLabel;
					 UINT selectedButton = userMsg.showMsgBox(IDS_UI_050046,CAPTION_ARCHIVE_MANAGER);
					 PostMessage( WM_CLOSE_DLG, 0, IDOK );
				 }
                return;
            }
            catch ( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "ArchiveException - %s",e.what());
                TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::CopyArchiveFailed, e.what() ); 
                
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << e.what();
                //UINT selectedButton = userMsg.showMsgBox(IDS_UE_050130);
                
                PostMessage( WM_CLOSE_DLG, 0, IDABORT );
                return;
            }
            catch ( TA_Base_Core::TransactiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TransactiveException - %s",e.what());
                CString error;
                error.FormatMessage( UE050090_WRITE_FAILED, m_volumeLabel.c_str(), e.what() );
                TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::CopyArchiveFailed, std::string( error ) ); 
                
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << m_volumeLabel << e.what();
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_050091,CAPTION_ARCHIVE_MANAGER);

                PostMessage( WM_CLOSE_DLG, 0, IDABORT );
                return;
            }
            catch ( ... )
            {
                CString reason;
                reason.LoadString( UE050090_REASON_UNKNOWN_ERROR );
                CString error;
                error.FormatMessage( UE050090_WRITE_FAILED, m_volumeLabel.c_str(), reason );
                
                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", std::string( error ) );
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception - %s",std::string( error ));
                
                TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::CopyArchiveFailed, std::string( error ) ); 
                
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << m_volumeLabel << reason;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_050091,CAPTION_ARCHIVE_MANAGER);

                PostMessage( WM_CLOSE_DLG, 0, IDABORT );
               return;
            }

            // If the user has cancelled - deal with it.

            if ( StateManager::STOPPING == m_stateManager.getState() )
            {
                PostMessage( WM_CLOSE_DLG, 0, IDCANCEL );
                return;
            }
        }
        catch( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() ); 
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TransactiveException - %s",e.what());
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Unknown exception caught in BurnCdDlg::run()" ); 
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception caught in BurnDvdDlg::run()");
        }
    }

    
    void BurnCdDlg::terminate()
    {
        if ( TA_Base_Core::Thread::THREAD_STATE_RUNNING == getCurrentState() )
        {
            // If we are in the middle of writing we have to wait for an appropriate
            // time before we terminate.

            if ( StateManager::PROCESSING == m_stateManager.getState() )
            {
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UI_050055,CAPTION_ARCHIVE_MANAGER);

                m_stateManager.setState( StateManager::STOPPING );
            }
            else if ( StateManager::STOPPING == m_stateManager.getState() )
            {
                // We're still waiting for the current day to finish restoring.

                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UI_050055,CAPTION_ARCHIVE_MANAGER);
            }
        }
        
        // Wait until it has completed.
        // We could put a count in here so we don't get stuck, but how long should we count for?

        while( StateManager::IDLE != m_stateManager.getState() )
        {
            sleep( 1000 );  // 1 sec
        }
    }



	void BurnCdDlg::BurnStatusCallBack (long totalTime, long timeLeft, BurnStatus burnStatus) 
	{
		switch(burnStatus)
		{
		case(BurnStatus::ValidatingMedia):
			m_stageStatic.SetWindowText( _T(VALIDATING_MEDIA) );
			break;
		case(BurnStatus::FormattingMedia):
			m_stageStatic.SetWindowText( _T(FORMAT_MEDIA) );
			break;
		case(BurnStatus::InitialisingHardware):
			m_stageStatic.SetWindowText( _T(INITIALISING_HARWARE) );
			break;
		case(BurnStatus::CalibratingPower):
			m_stageStatic.SetWindowText( _T(CALIBRATING_POWER) );
			break;
		case(BurnStatus::Writing):
			{

				m_stageStatic.SetWindowText( _T(WRITING) );

				int _percentageComplete = 0;
				if(totalTime > 0)
				{
					_percentageComplete = (100 - (timeLeft/totalTime)*100);
				}

				m_writeProgress.SetPos(_percentageComplete );
				break;
			}

		case(BurnStatus::WriteFinalization):
			m_stageStatic.SetWindowText( _T(WRITE_FINALIZATION) );
			break;
		case(BurnStatus::WriteCompleted):
			m_stageStatic.SetWindowText( _T(WRITE_COMPLETED) );
			break;
		case(BurnStatus::WriteVerifying):
			m_stageStatic.SetWindowText( _T(WRITE_VERIFYING) );
			break;

		}
		return;
	}


    /////////////////////////////////////////////////////////////////////////////
    // BurnCdDlg message handlers
    
    BOOL BurnCdDlg::OnInitDialog() 
    {
        CDialog::OnInitDialog();
        
        //  Give this dialog its parent's icons.

        SetIcon( GetParent()->GetIcon( true ), TRUE );			// Set big icon
        SetIcon( GetParent()->GetIcon( false ), FALSE );		// Set small icon

        //  Display an hour-glass cursor.

        HCURSOR hOldCursor = SetCursor( LoadCursor( NULL, IDC_APPSTARTING ) );

        m_writeProgress.SetRange( 0, 100 );
        
        // Start writing.

        start();

        return TRUE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
    }

    
    void BurnCdDlg::OnDestroy() 
    {
	    CDialog::OnDestroy();

		if(iMediaAccess != NULL)
		{
			delete(iMediaAccess);
			iMediaAccess = NULL;
		}
			
    }


    void BurnCdDlg::OnCancel() 
    {
        // If we are in the middle of processing, we have to wait until 
        // it is convenient to cancel.

        if ( StateManager::PROCESSING == m_stateManager.getState() )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UI_050055,CAPTION_ARCHIVE_MANAGER);
            m_stateManager.setState( StateManager::STOPPING );
			
			iMediaAccess->CancelBurn();

            appendStringToLogDisplay( CANCELLING_MSG);

            // Don't let the operator press Cancel again.

            m_buttonCancel.EnableWindow( FALSE );
        }
        else
        {
            // We're not processing, so we can just cancel.

            CDialog::OnCancel();
        }
    }

    
    LRESULT BurnCdDlg::OnCloseDlg(WPARAM, LPARAM lparam)
    {
        // If the user has cancelled - don't go any further.

        if ( StateManager::STOPPING == m_stateManager.getState() )
        {
            CString message;
            try
            {
                std::string operatorName = TA_ArchiveManager::getOperatorNameFromSessionId();
                std::string workstationName = TA_ArchiveManager::getWorkstationNameFromSessionId();

                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << operatorName << workstationName;
                UINT selectedButton = userMsg.showMsgBox(IDS_UI_050030,CAPTION_ARCHIVE_MANAGER);
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UI_050060,CAPTION_ARCHIVE_MANAGER);
            }
        }

        m_stateManager.setState( StateManager::IDLE );
        CDialog::EndDialog( lparam );
	    return NULL;	// This line must be included. Everything above is application specific
    }


    void BurnCdDlg::addFile( const std::string& fileNameWithPath, const std::string& fileName, bool isDirectory )
    {
        //  Call addFile() for every file that will be burnt to CD
      

        // First check that the file/directory exists.

        if ( isDirectory )
        {
            if ( !TA_ArchiveManager::isValidDirectory( fileNameWithPath ) )
            {
                CString reason;
                reason.LoadString( UW050004_REASON_INVALID_DIR );
                CString error;
                error.FormatMessage( UW050004_ADD_DIR_FAILED, fileNameWithPath.c_str(), reason );
                
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					"%s is not a valid directory", fileNameWithPath.c_str());

				TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }
        }
        else // We're expecting a file.
        {
            if ( !TA_ArchiveManager::isValidFile( fileNameWithPath ) )
            {
                CString reason;
                reason.LoadString( UW050016_REASON_INVALID_FILE );
                CString error;
                error.FormatMessage( UW050016_ADD_FILE_FAILED, fileNameWithPath.c_str(), reason );
                
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					"%s is not a valid file", fileNameWithPath.c_str()); 

				TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }
        }
		
		FilePathName _filePathName = std::make_pair(fileNameWithPath, fileName);

		m_FilePathNameVector.push_back(_filePathName);
		std::string s( "File: " + fileName);
		appendStringToLogDisplay( s.c_str() );

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Add File %s", fileName.c_str());
    }
    

    void BurnCdDlg::appendStringToLogDisplay( const std::string& sAdd )
    {
        //  Add the string to the active Windows Edit control if it is usable, otherwise
        //  just add it to the local string that will be DDX'd into the edit control.

        if ( ::IsWindow( m_editLog.m_hWnd) )
        {
            CString sBuffer;
            m_editLog.GetWindowText( sBuffer );     // This method requires a CString.
            if (!sBuffer.IsEmpty())
            {
                sBuffer+= "\r\n";
            }
            sBuffer+= sAdd.c_str();
            m_editLog.SetWindowText( sBuffer );
            m_editLog.LineScroll( m_editLog.GetLineCount(),0 );
        }
        else
        {
            if (!m_csLog.IsEmpty() )
            {
                m_csLog+= "\r\n";
            }
            m_csLog+= sAdd.c_str();
        }
    }

} // TA_Base_App
