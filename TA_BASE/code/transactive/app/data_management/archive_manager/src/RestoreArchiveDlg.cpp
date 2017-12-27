/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Alan Brims
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Dialog for operator to select the archive files to be restored.
  *
  */

#include <sstream>
#include <ctype.h>

#include "app/data_management/archive_manager/src/stdafx.h"
#include "app/data_management/archive_manager/src/RestoreArchiveDlg.h"
#include "app/data_management/archive_manager/src/RestoreCompleteDlg.h"
#include "app/data_management/archive_manager/src/ChineseInfo.h"
#include "core/database/src/SimpleDb.h"
#include "core/exceptions/src/ArchiveException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/message/types/ArchiveAudit_MessageTypes.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/generic_gui/src/TransactiveMessage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    //
    // Enum specifying the columns in the archive list.
    //

    enum EListColumns
    {
        TICK_COL    = 0,
        DATE_COL    = 1,
        STATUS_COL  = 2
    };
}

namespace TA_Base_App
{
    
    /////////////////////////////////////////////////////////////////////////////
    // RestoreArchiveDlg dialog
    
    
    RestoreArchiveDlg::RestoreArchiveDlg(TA_Base_Bus::IGUIAccess& guiAccessor, CWnd* pParent /*=NULL*/)
        : CDialog(RestoreArchiveDlg::IDD, pParent),
          m_restoreArchiveHelper( guiAccessor ),
          m_isListPopulated( false ),
          m_returnValue( IDOK ),
		  m_isRestoreInProgress(false)
    {
        //{{AFX_DATA_INIT(RestoreArchiveDlg)
	    //}}AFX_DATA_INIT
    }
    

    RestoreArchiveDlg::~RestoreArchiveDlg()
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

    
    void RestoreArchiveDlg::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(RestoreArchiveDlg)
	DDX_Control(pDX, IDC_DRIVE_COMBO, m_selectedDriveCombo);
	    DDX_Control(pDX, IDOK, m_closeButton);
	    DDX_Control(pDX, ID_BUTTON_RESTORE, m_restoreButton);
	    DDX_Control(pDX, IDCANCEL, m_cancelButton);
	    DDX_Control(pDX, IDC_BUTTON_SELECT_ALL, m_selectAllButton);
        DDX_Control(pDX, IDC_ARCHIVE_LIST, m_archiveList);
	//}}AFX_DATA_MAP
    }
    
    
    BEGIN_MESSAGE_MAP(RestoreArchiveDlg, CDialog)
    //{{AFX_MSG_MAP(RestoreArchiveDlg)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_ALL, OnButtonSelectAll)
	ON_BN_CLICKED(ID_BUTTON_RESTORE, OnRestore)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ARCHIVE_LIST, OnItemChangedArchiveList)
	ON_BN_CLICKED(IDOK, OnClose)
	ON_CBN_SELCHANGE(IDC_DRIVE_COMBO, OnSelchangeDriveCombo)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_COMPLETE_DLG, OnShowCompleteDlg)
	ON_MESSAGE(WM_TERMINATE_WORKERTHREAD,onTerminateThreadCall)
    END_MESSAGE_MAP()
    

    void RestoreArchiveDlg::run()
    {
        // Make sure no exceptions escape from the run() method.

        try
        {
            m_stateManager.setState( StateManager::PROCESSING );

            //  Display an hour-glass cursor.

            HCURSOR hOldCursor = SetCursor( LoadCursor( NULL, IDC_APPSTARTING));

            // Disable everything except Cancel and change the highlighting from
            // the Restore button to the Cancel button.

            disableAllButCancelButton();

            // Get all the ticked items (the ones to restore).

            RestoreArchiveHelper::DatesVector restoredDates;
            std::string volumeLabel( "" );
            bool isComplete = getTickedItems( restoredDates, volumeLabel );
            if( !isComplete )                                                   // The user cancelled.
            {        
                enableCloseButton();
                m_stateManager.setState( StateManager::IDLE );
                return;
            }

            // If there are no files to restore, display a message box.

            if ( restoredDates.empty() )
            {
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UI_050051,CAPTION_ARCHIVE_MANAGER);

                resetButtons();
                m_stateManager.setState( StateManager::IDLE );
                return;
            }

            // Restore the archive files into the database.  This will be done
            // one complete day at a time.
			
			m_isRestoreInProgress = true;

            try
            {
                isComplete = restoreFiles( restoredDates, volumeLabel );
                if ( !isComplete )                                              // The user cancelled.
                {
                    // Don't return here, because we should still process/clean up the log files so far.
                }
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << e.what();
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_050135,CAPTION_ARCHIVE_MANAGER);

                // Don't return here, because we should still process/clean up the log files so far.
                if( m_returnValue != IDCANCEL )
                {
                    TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::RestoreArchiveFailed, e.what() ); 
                    m_returnValue = IDABORT;
                }
            }   
			
			m_isRestoreInProgress =  false;
            // This next bit HAS to be done after restoreFiles has been called, so disable
            // the cancel button, so the operator can't cancel during this process.
            // (Even if they do somehow, a message box will be displayed, then the cancellation
            // will be ignored until this finishes).

            m_cancelButton.EnableWindow( FALSE );
        
            // This will delete all the log files if every import was successful.
            // It will display the Restore Complete dialog if there were errors/warnings during the restore.
            // Either way, this method will set the state to IDLE and enable the close button.

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

    
    void RestoreArchiveDlg::terminate()
    {
        if ( TA_Base_Core::Thread::THREAD_STATE_RUNNING == getCurrentState() )
        {
            // If we are in the middle of restoring, we have to wait for the
            // day that is currently being restored to be fully restored, before
            // we terminate.

            if ( StateManager::PROCESSING == m_stateManager.getState() )
            {
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UI_050052,CAPTION_ARCHIVE_MANAGER);

                m_stateManager.setState( StateManager::STOPPING );
            }
            else if ( StateManager::STOPPING == m_stateManager.getState() )
            {
                // We're still waiting for the current day to finish restoring.

                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UI_050052,CAPTION_ARCHIVE_MANAGER);
            }
        }

        // Wait until it has completed.
        // We could put a count in here so we don't get stuck, but how long should we count for?
        // Restoration of a single day could take an hour or more... we don't know.

        while( StateManager::IDLE != m_stateManager.getState() )
        {
            sleep( 1000 );  // 1 sec
        }
    }


    /////////////////////////////////////////////////////////////////////////////
    // RestoreArchiveDlg message handlers
    
    BOOL RestoreArchiveDlg::OnInitDialog() 
    {
        CDialog::OnInitDialog();
        
        //  Give this dialog its parent's icons.

        SetIcon( GetParent()->GetIcon( true ), TRUE );			// Set big icon
        SetIcon( GetParent()->GetIcon( false ), FALSE );		// Set small icon

        //  Display an hour-glass cursor.

        HCURSOR hOldCursor = SetCursor( LoadCursor( NULL, IDC_APPSTARTING ) );
        
        // Prepare the CListCtrl columns and fill it up.

        CDC * pDC = m_archiveList.GetDC();
        int dateWidth = pDC->GetTextExtent( _T(" 88/88/8888 ")).cx;                                     //  max date width - for the most common date format.
        int statusWidth = pDC->GetTextExtent( _T( RestoreArchiveHelper::RESTORABLE.c_str() ) ).cx + 15; //  max status width
        
        //  Put checkboxes down the left-hand side

        m_archiveList.SetExtendedStyle( LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT );

		// set font for CListCtrl, otherwise, the chinese column name will not be shown correctly.
		m_listFont.CreatePointFont(110, "ËÎÌå" );
		m_archiveList.SetFont(&m_listFont,FALSE);  

        LVCOLUMNA lvColumn;
        lvColumn.mask = LVCF_FMT | LVCF_IMAGE | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        lvColumn.fmt = LVCFMT_IMAGE; // LVCFMT_COL_HAS_IMAGES
        lvColumn.iImage = 1;
        lvColumn.iSubItem = TICK_COL;
        lvColumn.pszText = 0;
        lvColumn.cchTextMax = 0;
        lvColumn.cx = GetSystemMetrics( SM_CXICON ); 
        m_archiveList.InsertColumn( TICK_COL,  &lvColumn); 
        m_archiveList.InsertColumn( DATE_COL,  _T("Êý¾Ý"), LVCFMT_LEFT, dateWidth, DATE_COL);
        m_archiveList.InsertColumn( STATUS_COL,  _T("×´Ì¬"), LVCFMT_LEFT, statusWidth, STATUS_COL);

        // Populate selected drive combo box
        std::vector<std::string> drives = m_restoreArchiveHelper.getMediaLocations();
        for (std::vector<std::string>::iterator iter = drives.begin(); iter != drives.end(); iter++)
        {
			// marvin++
			// TD13391 
			//must be from cd
			if( GetDriveType((*iter).c_str()) == DRIVE_CDROM)
			{
				m_selectedDriveCombo.AddString((*iter).c_str());
			}
			// ++marvin
			// TD13391 
        }

        m_selectedDriveCombo.EnableWindow( (m_selectedDriveCombo.GetCount() > 1) );
        if (m_selectedDriveCombo.GetCount() > 0)
        {
            m_selectedDriveCombo.SetCurSel(0);
        }

        populateList();

        // Make sure there are files in the list.  If not, abort.
        // This is not done in populateList() because at some stage we
        // may want to use populateList() to perform a refresh, and we may
        // not want the dialog to close if the list is empty.

        if ( 0 == m_archiveList.GetItemCount() )
        {
            // populateList() will have displayed the error message already.

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "Archive list is empty. Closing Restore Archive dialog." );
            CDialog::EndDialog( IDABORT );
        }

		// liborm - 27 Oct 2004 - fix for TD defect #3521
		// focus the first item of the archives list box
		// when the tab key is used to change focus to the list box
		m_archiveList.SetItemState( m_archiveList.GetTopIndex(), LVIS_FOCUSED, LVIS_FOCUSED );

		m_archiveList.AutoResizeColumn();

        return TRUE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
    }


    void RestoreArchiveDlg::OnButtonSelectAll() 
    {
        LVITEM lvItem;
        ZeroMemory( &lvItem, sizeof( LVITEM));
	    for ( int i = 0; i < m_archiveList.GetItemCount(); i++ )
        {
            if ( m_archiveList.GetItemText( i, STATUS_COL ) == RestoreArchiveHelper::RESTORABLE.c_str() ||
				m_archiveList.GetItemText( i, STATUS_COL ) == RestoreArchiveHelper::NOTPROCESSED.c_str() )
            {
                lvItem.iItem = i;
                lvItem.mask = LVIF_STATE;
                lvItem.state = INDEXTOSTATEIMAGEMASK( 2 );
                lvItem.stateMask = LVIS_STATEIMAGEMASK;
                m_archiveList.SetItem( &lvItem );
            }
        }
    }
  

    void RestoreArchiveDlg::OnItemChangedArchiveList(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        // Don't do the updating until the list has been populated.

        if ( !m_isListPopulated )
        {
            *pResult = 0;
            return;
        }

        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        // Don't allow items to be selected (highlighted) except by the tick boxes.

        m_archiveList.SetItemState(pNMListView->iItem, ~LVIS_SELECTED, LVIS_SELECTED);

        // Only bother if the user clicked in the tick column.

        if ( TICK_COL == pNMListView->iSubItem )            
        {
            // Make sure the tick box stays hidden if the data is not restorable.

            if ( m_archiveList.GetItemText( pNMListView->iItem, STATUS_COL ) != RestoreArchiveHelper::RESTORABLE.c_str() &&
				 m_archiveList.GetItemText( pNMListView->iItem, STATUS_COL ) != RestoreArchiveHelper::NOTPROCESSED.c_str() )
            {
                m_archiveList.SetItemState( pNMListView->iItem, INDEXTOSTATEIMAGEMASK(0), LVIS_STATEIMAGEMASK );
            }

            // If any of the items are ticked, enable the Restore button.
            // Otherwise disable it.

            UINT state = 0;
            int itemCount = m_archiveList.GetItemCount();
			int item = 0;
	        for ( ; item < itemCount; item++ )
            {
                state = m_archiveList.GetItemState( item, LVIS_STATEIMAGEMASK );
                state >>= 13;
                if( 1 == state )
                {
                    break;  // At least one item is ticked.
                }
            }
			
			//if restore is in progress do not enable restore button
			if(!m_isRestoreInProgress)
				m_restoreButton.EnableWindow( item != itemCount );
        }		
	    *pResult = 0;
    }

    
    LRESULT RestoreArchiveDlg::OnShowCompleteDlg(WPARAM, LPARAM)
    {
        RestoreCompleteDlg dlg( m_fileStatusMap );
        dlg.DoModal();
        m_stateManager.setState( StateManager::IDLE );
		terminateAndWait();
		//populateList();
		resetButtons();
		m_fileStatusMap.clear();
        return NULL;
    }


    void RestoreArchiveDlg::OnClose() 
    {
	    CDialog::EndDialog( m_returnValue );
    }


    void RestoreArchiveDlg::OnCancel() 
    {
        // If we are in the middle of restoring, we have to wait for the
        // day that is currently being restored to be fully restored, before
        // we cancel.

        if ( StateManager::PROCESSING == m_stateManager.getState() )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UI_050052,CAPTION_ARCHIVE_MANAGER);

            try
            {
                TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::RestoreArchiveCancelled );
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
            }
            m_stateManager.setState( StateManager::STOPPING );

            // Update all selected items that still say RESTORABLE || NOTPROCESSED, to say CANCELLED.
	        for ( int item = 0; item < m_archiveList.GetItemCount(); item++ )
            {
				UINT state = 0;
				state = m_archiveList.GetItemState( item, LVIS_STATEIMAGEMASK );
				state >>= 13;
                if(( m_archiveList.GetItemText( item, STATUS_COL ) == RestoreArchiveHelper::RESTORABLE.c_str() ||
					m_archiveList.GetItemText( item, STATUS_COL ) == RestoreArchiveHelper::NOTPROCESSED.c_str()) &&
					(state == 1))
                {
                    m_archiveList.SetItemText( item, STATUS_COL, RestoreArchiveHelper::CANCELLED.c_str() );
					m_archiveList.SetItemState( item, INDEXTOSTATEIMAGEMASK(0), LVIS_STATEIMAGEMASK );
                }
            }

            // Don't let the operator press Cancel again. Only if Restore operaion is running
			if ( TA_Base_Core::Thread::THREAD_STATE_RUNNING == getCurrentState() )
			{
					m_cancelButton.EnableWindow( FALSE );
			}
            m_returnValue = IDCANCEL;
            try
            {
                TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::ArchiveRestored ); 
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
				std::string messageForUser = e.what();;
				TA_ArchiveManager::EMsgLevel messageLevel = TA_ArchiveManager::WARN;
                TA_Base_Bus::TransActiveMessage userMsg;
				userMsg << messageForUser.c_str();
				UINT selectedButton = userMsg.showMsgBox(IDS_UW_050018,CAPTION_ARCHIVE_MANAGER);
                return;
            }
        }
        else
        {
            // We're not processing, so we can just cancel.

            CDialog::OnCancel();
        }
    }


    void RestoreArchiveDlg::OnRestore() 
    {
        // Restoration is run in a separate thread, so we don't block the UI.

        try
        {
            TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::RestoreArchiveRequest ); 
        }
        catch( TA_Base_Core::ArchiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
			std::string messageForUser = e.what();
			TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << messageForUser.c_str();
			UINT selectedButton = userMsg.showMsgBox(IDS_UE_050128,CAPTION_ARCHIVE_MANAGER);           
            return;
        }
        start();
    }


    void RestoreArchiveDlg::populateList()
    {
        // Get the dates and status' of the files that are on the DVD.
        // We don't need to know the filenames here, because we only show the dates, because
        // the user has to restore all the data for a particular date.

        CWaitCursor wait;

        try
        {
            // Get the currently selected drive letter.
            CString drive;
            int selection = m_selectedDriveCombo.GetCurSel();
            if (selection != CB_ERR)
            {
                m_selectedDriveCombo.GetLBText(selection, drive);
                m_fileDetails = m_restoreArchiveHelper.getAllFileDetails( (LPCTSTR)drive );
				if( (!m_fileDetails.empty()) && m_restoreArchiveHelper.hasInvalidArchiveFile())
				{
					return;
				}
            }
			else
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"Restore Archive: No restore archive driver path is selected!");
			}
        }
        catch( TA_Base_Core::ArchiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
            TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::RestoreArchiveFailed, e.what() );
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << e.what();
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_050136,CAPTION_ARCHIVE_MANAGER);

            return;
        }
        if ( m_fileDetails.empty() )
        {
            // KT 10 Mar 04: PW #3136 - Changed '-' to '_' for operating system compatibility.

            CString error;
            error.LoadString( UW050001_NO_VALID_FILENAMES );
            TA_ArchiveManager::sendAuditMessageIgnoreErrors( 
                TA_Base_Core::ArchiveAudit::RestoreArchiveFailed, std::string( error ) ); 
            
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UW_050001,CAPTION_ARCHIVE_MANAGER);
			//td19031
            return;
        }

        // Display the dates and status' in the list ctrl.

        int item = 0;
        bool areSomeRestorable = false;
        m_archiveList.DeleteAllItems();
        for( RestoreArchiveHelper::FileDetailsVector::iterator it = m_fileDetails.begin(); it != m_fileDetails.end(); ++it )
        {
            item = m_archiveList.InsertItem( m_archiveList.GetItemCount(), NULL );
            m_archiveList.SetItem( item, TICK_COL, LVIF_IMAGE, 0, 1, 0, 0, 0);

            // KT 9 Mar 04: PW #3161 Converted to use time_t to remove dependence on date formatting.
			m_archiveList.SetItemText( item, DATE_COL, TA_ArchiveManager::convertTimeTToStringDate( it->date ).c_str() );
			m_archiveList.SetItemText( item, STATUS_COL, it->status.c_str() );
			m_archiveList.SetItemData(item, item);
			if ( it->status != RestoreArchiveHelper::RESTORABLE &&
				it->status != RestoreArchiveHelper::NOTPROCESSED )
            {
                m_archiveList.SetItemState( item, INDEXTOSTATEIMAGEMASK(0), LVIS_STATEIMAGEMASK );
            }
            else
            {
                m_archiveList.SetItemState( item, INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK );
                areSomeRestorable = true;
            }
        }

        // Only enable the SelectAll button if there are some files that are restorable.

        if( areSomeRestorable )
        {
            m_selectAllButton.EnableWindow( TRUE );
        }
		else
		{
			m_selectAllButton.EnableWindow( FALSE );
		}

        m_isListPopulated = true;
    }


    void RestoreArchiveDlg::disableAllButCancelButton()
    {
        m_archiveList.EnableWindow( FALSE );
        m_selectAllButton.EnableWindow( FALSE );
        m_restoreButton.EnableWindow( FALSE );
        m_selectedDriveCombo.EnableWindow( FALSE );
        m_cancelButton.SetFocus();
        m_selectAllButton.SetButtonStyle( BS_PUSHBUTTON );
        m_restoreButton.SetButtonStyle( BS_PUSHBUTTON );
        m_cancelButton.SetButtonStyle( BS_DEFPUSHBUTTON );
    }


    void RestoreArchiveDlg::resetButtons()
    {
        m_archiveList.EnableWindow( TRUE );
       // m_selectAllButton.EnableWindow( TRUE );
        // If any of the items are ticked, enable the Restore button.
        // Otherwise disable it.

        UINT state = 0;
        int itemCount = m_archiveList.GetItemCount();
		int item = 0;
		bool bFoundSelectable = false;
		bool bFoundTicked = false;
	    for ( ; item < itemCount; item++ )
        {

			std::string status = m_archiveList.GetItemText( item, STATUS_COL );
			if(status == RestoreArchiveHelper::RESTORABLE ||
				status == RestoreArchiveHelper::NOTPROCESSED)
			{
				bFoundSelectable = true;
			}
			else
			{
				m_archiveList.SetItemState( item, INDEXTOSTATEIMAGEMASK(0), LVIS_STATEIMAGEMASK );
				continue;
			}

			state = m_archiveList.GetItemState( item, LVIS_STATEIMAGEMASK );
            state >>= 13;
            if( 1 == state )
            {
				bFoundTicked = true;
            }

			if(bFoundSelectable && bFoundTicked)
			{
                break;  // At least one item is ticked.
            }
        }
		m_cancelButton.EnableWindow( TRUE );
        if (!bFoundTicked )
        {
            m_restoreButton.EnableWindow( FALSE );
            m_cancelButton.SetFocus();
        }
        else
        {
            m_restoreButton.EnableWindow( TRUE );
            m_cancelButton.SetFocus();
        }

		if(bFoundSelectable)
		{
			m_selectAllButton.EnableWindow( TRUE );
		}
		else
		{
			enableCloseButton();
		}
    }


    void RestoreArchiveDlg::enableCloseButton()
    {
        m_closeButton.ShowWindow( TRUE );
        m_closeButton.SetFocus();
        m_cancelButton.ShowWindow( FALSE );
        m_restoreButton.ShowWindow( FALSE );
        m_selectAllButton.ShowWindow( FALSE );
        m_closeButton.EnableWindow( TRUE );
        m_closeButton.SetButtonStyle( BS_DEFPUSHBUTTON );
    }


    bool RestoreArchiveDlg::getTickedItems( RestoreArchiveHelper::DatesVector& restoredDates, std::string& volumeLabel )
    {
        // Find the items that are ticked.

        UINT state = 0;
        int itemCount = m_archiveList.GetItemCount();
	    for ( int item = 0; item < itemCount; item++ )
        {
            if ( StateManager::STOPPING == m_stateManager.getState() )
            {
                return false;
            }

            // Start setting up the volume label to be passed to updateInventory later on.

            if ( 0 == item )
            {
                volumeLabel = m_archiveList.GetItemText( item, DATE_COL );
            }
            else if ( itemCount == item + 1 )
            {
                volumeLabel += " To ";
                volumeLabel += m_archiveList.GetItemText( item, DATE_COL );
            }

            //  Determine the ticked items.
            
            state = m_archiveList.GetItemState( item, LVIS_STATEIMAGEMASK );
            state >>= 13;
            if( ( 1 == state ) && ( (m_archiveList.GetItemText( item, STATUS_COL ) == RestoreArchiveHelper::RESTORABLE.c_str() ) ||
											(m_archiveList.GetItemText( item, STATUS_COL ) == RestoreArchiveHelper::NOTPROCESSED.c_str())))
            {
				 restoredDates.push_back( m_fileDetails[ item ].date );
            }
        }

        return true;
    }

    bool RestoreArchiveDlg::restoreFiles( const RestoreArchiveHelper::DatesVector& restoredDates,
        const std::string& volumeLabel )
    {
        if( StateManager::STOPPING == m_stateManager.getState() )
        {
            return false;
        }        

        // Restore the files.
        
	    int itemIndex = 0;
        int itemCount = m_archiveList.GetItemCount();
        for( RestoreArchiveHelper::DatesVector::const_iterator it = restoredDates.begin(); it != restoredDates.end(); ++it )
        {
            if( StateManager::STOPPING == m_stateManager.getState() )
            {
                return false;
            }

			//try
			//{
				//First check the selected date is already exists in the database based on ar_tables query column search
				std::string status = m_restoreArchiveHelper.getArchiveStatusFromTables(*it);
			//}
			//catch( TA_Base_Core::ArchiveException& e )
           // {
          //      LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );                
          //      TA_Base_Bus::TransActiveMessage userMsg;
          //      userMsg << e.what();
          //      UINT selectedButton = userMsg.showMsgBox(IDS_UE_050135);

           // }   

            // Find the restored date in the list.  Note: Both the restoredDates vector 
            // and the list are in date order.
            
            while( m_archiveList.GetItemText( itemIndex, DATE_COL ) != TA_ArchiveManager::convertTimeTToStringDate( *it ).c_str() )
            {
                itemIndex++;
            }
            
            if ( itemIndex == itemCount )
            {
                CString error;
                error.FormatMessage( UW050011_DATE_NOT_FOUND, 
                    TA_ArchiveManager::convertTimeTToStringDate( *it ).c_str() );
                TA_THROW( TA_Base_Core::ArchiveException( std::string( error ) ) );
            }
			if(status == RestoreArchiveHelper::EXISTS)
			{
				 m_archiveList.SetItemText( itemIndex, STATUS_COL, RestoreArchiveHelper::EXISTS.c_str() );
				 m_archiveList.SetItemState( itemIndex, INDEXTOSTATEIMAGEMASK(0), LVIS_STATEIMAGEMASK );
				 continue;
			}

            m_archiveList.SetItemText( itemIndex, STATUS_COL, RestoreArchiveHelper::IMPORTING.c_str() );
        
            try
            {
                // KT 27 Feb 04: PW #3133: This section has been changed so that users can remove
                // an individual days worth of restored data from the database.
                // KT 9 Mar 04: PW #3161 Converted to use time_t to remove dependence on date formatting.

                m_restoreArchiveHelper.restoreFilesForDate( *it, volumeLabel, m_fileStatusMap );
                m_archiveList.SetItemText( itemIndex, STATUS_COL, RestoreArchiveHelper::COMPLETE.c_str() );
				m_archiveList.SetItemState( itemIndex, INDEXTOSTATEIMAGEMASK(0), LVIS_STATEIMAGEMASK );
            }
            catch( TA_Base_Core::ArchiveException& )
            {
                // Don't remove the AR_INVENTORY inventory table entry for this date, 
                // because we may have got partway through the restoration already.

                // This will be further dealt with outside this method.

                m_archiveList.SetItemText( itemIndex, STATUS_COL, RestoreArchiveHelper::RESTORE_FAILED.c_str() );
				m_archiveList.SetItemState( itemIndex, INDEXTOSTATEIMAGEMASK(0), LVIS_STATEIMAGEMASK );
                throw;
            }
        }

        return true;
    }
        

    void RestoreArchiveDlg::processLogFiles()
    {
        if ( m_fileStatusMap.empty() )
        {
            m_stateManager.setState( StateManager::IDLE );
            //enableCloseButton();
			resetButtons();
			PostMessage( WM_TERMINATE_WORKERTHREAD, 0, 0 );
            return;
        }

        // We don't check if the status goes to STOPPING in here, because we want to complete
        // this whether the user has cancelled or not.

        // If there were no failures found in the log files, delete them.
        // Otherwise display the Restore Complete to show which files contained warnings/errors/non-successes.
    
        std::pair< TA_ArchiveManager::FileStatusMap::iterator, 
            TA_ArchiveManager::FileStatusMap::iterator > range(  
            m_fileStatusMap.equal_range( TA_ArchiveManager::EX_SUCC ) );           

        // Check if the imports were all successful.

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
            m_stateManager.setState( StateManager::IDLE );
            //enableCloseButton();
			resetButtons();
			m_fileStatusMap.clear();
			PostMessage( WM_TERMINATE_WORKERTHREAD, 0, 0 );
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


    void RestoreArchiveDlg::OnSelchangeDriveCombo() 
    {
        populateList();	 
    }


	LRESULT RestoreArchiveDlg::onTerminateThreadCall(WPARAM, LPARAM)
    {
		terminateAndWait();
        return NULL;
    }

} // TA_Base_App

