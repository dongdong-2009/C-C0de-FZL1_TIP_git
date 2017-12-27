/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/RetrievingDataDlg.cpp $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Retrieves the data from the database for a manual archive.
  */

#if defined( WIN32 )
#pragma warning ( disable : 4503 ) 
#endif // defined( WIN32 )

#include <sstream>

#include "app/data_management/archive_manager/src/stdafx.h"
#include "app/data_management/archive_manager/src/RetrievingDataDlg.h"
#include "app/data_management/archive_manager/src/RetrieveCompleteDlg.h"
#include "app/data_management/archive_manager/src/ChineseInfo.h"

#include "core/exceptions/src/ArchiveException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/message/types/ArchiveAudit_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/generic_gui/src/TransactiveMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // RetrievingDataDlg dialog


    RetrievingDataDlg::RetrievingDataDlg(TA_Base_Bus::IGUIAccess& guiAccessor, const time_t& fromDate, 
        const time_t& toDate, CWnd* pParent /*=NULL*/)
	    : CDialog(RetrievingDataDlg::IDD, pParent),
          m_guiAccessor( guiAccessor ),
          m_writeArchiveHelper( guiAccessor ),
          m_fromDate( fromDate ),
          m_toDate( toDate )
    {
        TA_ASSERT( fromDate <= toDate, "The From date is before or the same as the To date." );

	    //{{AFX_DATA_INIT(RetrievingDataDlg)
	    m_range = _T("");
	    //}}AFX_DATA_INIT
    }

            
    
    RetrievingDataDlg::~RetrievingDataDlg()
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


    void RetrievingDataDlg::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(RetrievingDataDlg)
	    DDX_Control(pDX, IDC_DATE_STATIC, m_date);
	    DDX_Control(pDX, IDSTOP, m_stopButton);
	    DDX_Text(pDX, IDC_RANGE_STATIC, m_range);
	    //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(RetrievingDataDlg, CDialog)
	    //{{AFX_MSG_MAP(RetrievingDataDlg)
	    ON_BN_CLICKED(IDSTOP, OnStop)
	    //}}AFX_MSG_MAP
	    ON_MESSAGE(WM_CLOSE_DLG, OnCloseDlg)
	    ON_MESSAGE(WM_COMPLETE_DLG, OnShowCompleteDlg)
        ON_MESSAGE(WM_DISABLE_DLG, OnDisableDlg)
        ON_WM_CLOSE()
    END_MESSAGE_MAP()


    void RetrievingDataDlg::run()
    {
        // Make sure no exceptions escape from the run() method.

        try
        {
            m_stateManager.setState( StateManager::PROCESSING );

            // Determine all the dates in the range fromDate to toDate.

            time_t currentTime( m_fromDate );
            do
            {
                if ( StateManager::STOPPING == m_stateManager.getState() )
                {
                    m_toDate = currentTime - TA_ArchiveManager::ONE_DAY;
                    break;
                }
            
                // KT 9 Mar 04: PW #3161 Convert date to local format.

                m_date.SetWindowText( TA_ArchiveManager::convertTimeTToStringDate( currentTime ).c_str() );
                try
                {
                    m_writeArchiveHelper.retrieveDataForDate( currentTime, m_fileStatusMap, m_archiveFiles );            
                }
                catch( TA_Base_Core::ArchiveException& e )
                {
                    TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg << e.what();
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_050139,CAPTION_ARCHIVE_MANAGER);

                    try
                    {
                        TA_ArchiveManager::deleteLogFiles( m_fileStatusMap );
                    }
                    catch( TA_Base_Core::ArchiveException& e )
                    {
                        // The error message contains a list of the files that couldn't be deleted and the reasons.

                        LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                        TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::CopyArchiveFailed, e.what() ); 
                        TA_Base_Bus::TransActiveMessage userMsg;
                        userMsg << e.what();
                        UINT selectedButton = userMsg.showMsgBox(IDS_UE_050138);
                    }
                    PostMessage( WM_CLOSE_DLG, 0, IDABORT );
                    return;
                }
                currentTime += TA_ArchiveManager::ONE_DAY;

            } while( currentTime <= m_toDate );

            // This next bit HAS to be done after retrieveDataForDate has been called, so disable
            // the stop button, so the operator can't stop during this process.
            // (Even if they do somehow, a message box will be displayed, then the stop
            // will be ignored until this finishes).
            
            PostMessage( WM_DISABLE_DLG, 0, 0 );
            
            
            // This will delete all the log files if every export was successful.
            // It will display the Restore Complete dialog if there were errors/warnings during the restore.
            // Either way, this method will set the state to IDLE and close the dialog.
            
            processLogFiles();
        }
        catch( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() ); 
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Unknown exception caught in RestoreArchiveDlg::run()" ); 
        }
    }


    void RetrievingDataDlg::terminate()
    {
        if ( TA_Base_Core::Thread::THREAD_STATE_RUNNING == getCurrentState() )
        {
	        // If we are in the middle of retrieving, we have to wait for the
            // day that is currently being restored to be fully retrieved, before
            // we stop.

            if ( StateManager::PROCESSING == m_stateManager.getState() )
            {
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UI_050053,CAPTION_ARCHIVE_MANAGER);

                m_stateManager.setState( StateManager::STOPPING );
            }
            else if ( StateManager::STOPPING == m_stateManager.getState() )
            {
                // We're still waiting for the current day to finish retrieving.

                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UI_050053,CAPTION_ARCHIVE_MANAGER);
            }
        }

        // Wait until it has completed.
        // We could put a count in here so we don't get stuck, but how long should we count for?
        // Retrieval of a single day could take an hour or more... we don't know.

        while( StateManager::IDLE != m_stateManager.getState() )
        {
            sleep( 1000 );  // 1 sec
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    // RetrievingDataDlg message handlers

    BOOL RetrievingDataDlg::OnInitDialog() 
    {
	    CDialog::OnInitDialog();

        //  Give this dialog its parent's icons.

        SetIcon( GetParent()->GetIcon( true ), TRUE );			// Set big icon
        SetIcon( GetParent()->GetIcon( false ), FALSE );		// Set small icon

        //  Display an hour-glass cursor.

        HCURSOR hOldCursor = SetCursor( LoadCursor( NULL, IDC_APPSTARTING ) );

        // KT 8 Mar 04: PW #3161 Converted dates to local format for display.

        std::stringstream range;
        range << ARCHIVE_DATA_FROM_MSG;
        range << TA_ArchiveManager::convertTimeTToStringDate( m_fromDate );
        if ( m_fromDate != m_toDate )
        {
            range << ARCHIVE_DATA_TO_MSG;
            range << TA_ArchiveManager::convertTimeTToStringDate( m_toDate );
        }
	    m_range = range.str().c_str();
        UpdateData( FALSE );
	            
        // This is in a separate thread, so we don't block the UI.
        
        start();
    
        return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void RetrievingDataDlg::OnStop() 
    {
	    // If we are in the middle of retrieving, we have to wait for the
        // day that is currently being restored to be fully retrieved, before
        // we stop.

        if ( StateManager::PROCESSING == m_stateManager.getState() )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UI_050053,CAPTION_ARCHIVE_MANAGER);
            m_stateManager.setState( StateManager::STOPPING );
            PostMessage( WM_DISABLE_DLG, 0, 0 );
        }
    }

    
    LRESULT RetrievingDataDlg::OnShowCompleteDlg(WPARAM, LPARAM)
    {
        RetrieveCompleteDlg dlg( m_fileStatusMap );
        if ( IDCANCEL == dlg.DoModal() )
        {
            try
            {
                m_writeArchiveHelper.deleteArchiveFiles( m_archiveFiles );
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );  
                TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::CopyArchiveFailed, e.what() ); 
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << e.what();
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_050138,CAPTION_ARCHIVE_MANAGER);
            }
            PostMessage( WM_CLOSE_DLG, 0, IDCANCEL );
        }
        else
        {
            PostMessage( WM_CLOSE_DLG, 0, IDOK );
        }
        return NULL;
    }


    LRESULT RetrievingDataDlg::OnCloseDlg(WPARAM, LPARAM lparam)
    {
        m_stateManager.setState( StateManager::IDLE );
        CDialog::EndDialog( lparam );
	    return NULL;	// This line must be included. Everything above is application specific
    }


    LRESULT RetrievingDataDlg::OnDisableDlg(WPARAM, LPARAM lparam)
    {
        // Disable the stop button
        m_stopButton.EnableWindow(FALSE);
    
        // Disable the close button
        CMenu* mnu = this->GetSystemMenu(FALSE);
        mnu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);

        return NULL;
    }

        
    void RetrievingDataDlg::OnClose()
    {
        // User attempted to close the dialog. Behaviour should be identical to clicking 'Stop'.
        OnStop();
    }

    
    void RetrievingDataDlg::processLogFiles()
    {
        if ( m_fileStatusMap.empty() )
        {
            return;
        }

        // We don't check if the status goes to STOPPING in here, because we want to complete
        // this whether the user has cancelled or not.

        // If there were no failures found in the log files, delete them.
        // Otherwise display the Retrieve Complete to show which files contained warnings/errors/non-successes.
    
        std::pair< TA_ArchiveManager::FileStatusMap::iterator, 
            TA_ArchiveManager::FileStatusMap::iterator > range(  
            m_fileStatusMap.equal_range( TA_ArchiveManager::EX_SUCC ) );           

        // Check if the exports were all successful.

        if( ( m_fileStatusMap.begin() == range.first ) && ( m_fileStatusMap.end() == range.second ) )
        {
            // If so, delete all the log files to stop them from accumulating.

            try
            {
                TA_ArchiveManager::deleteLogFiles( m_fileStatusMap );
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                // The error message contains a list of the files that couldn't be deleted and the reasons.

                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << e.what();
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_050138,CAPTION_ARCHIVE_MANAGER);
            }
            PostMessage( WM_CLOSE_DLG, 0, IDOK );
        }
        else
        {
            // They don't all contain successes, so show the dialog.
            // Do this by sending a message, so the windows thread handles the dialog.
            // If you don't do this, the thread shows the dialog and then aborts and
            // it debug asserts.

            PostMessage( WM_COMPLETE_DLG, 0, 0 );
        }
    }
}
