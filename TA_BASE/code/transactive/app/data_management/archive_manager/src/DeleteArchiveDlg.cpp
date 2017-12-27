/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/DeleteArchiveDlg.cpp $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Dialog to list archives that have been restored and allow the operator
  * to select restorations to be removed from the active data.
  *
  */

#include <string>
#include <vector>
#include <algorithm>
#include <utility>
#include <afx.h>

#include "app/data_management/archive_manager/src/stdafx.h"
#include "app/data_management/archive_manager/src/BasicHelper.h"
#include "app/data_management/archive_manager/src/DeleteArchiveDlg.h"
#include "app/data_management/archive_manager/src/SelectDeletionDateDlg.h"
#include "app/data_management/archive_manager/src/ChineseInfo.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveInventoryAccessFactory.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveInventoryTable.h"
#include "core/data_access_interface/archive_manager_dai/src/IArchiveInventory.h"
#include "core/exceptions/src/ArchiveException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/generic_gui/src/TransactiveMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    // KT 27 Feb 04: PW #3133: This has been changed so that users can remove
    // an individual days worth of restored data from the database.  The 
    // START_DATE_COL and END_DATE_COL have been replaced with a DATA_DATE_COL.

    // The columns in the inventory list control.
 
    enum EColumns
    {
        TICK_COL                = 0,
        DATA_DATE_COL           = 1,
        RESTORED_BY_COL         = 2,
        RESTORATION_DATE_COL    = 3,
        DELETION_DUE_DATE_COL   = 4,
        STATUS_COL              = 5
    };
    
    // The status' displayed to the user.
/*
    const std::string DELETED       ( "Deleted"   );
    const std::string DELETING      ( "Deleting"  );
    const std::string AVAILABLE     ( "Available" );
    const std::string DELETE_FAILED ( "Failed"    );
    const std::string CANCELLED     ( "Cancelled" );
*/
	const std::string DELETED       ( "已删除"   );
	const std::string DELETING      ( "正删除"  );
	const std::string AVAILABLE     ( "可用的" );
	const std::string DELETE_FAILED ( "失败"    );
	const std::string CANCELLED     ( "取消" );
}

namespace TA_Base_App
{
    DeleteArchiveDlg::DeleteArchiveDlg(CWnd* pParent /*=NULL*/)
        : CDialog(DeleteArchiveDlg::IDD, pParent),
          m_status( IDOK )
    {
        //{{AFX_DATA_INIT(DeleteArchiveDlg)
        // NOTE: the ClassWizard will add member initialization here
        //}}AFX_DATA_INIT
    }
    
    
    DeleteArchiveDlg::~DeleteArchiveDlg()
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


    void DeleteArchiveDlg::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(DeleteArchiveDlg)
	    DDX_Control(pDX, IDC_MODIFY_BUTTON, m_modifyButton);
	    DDX_Control(pDX, IDCANCEL, m_cancelButton);
	    DDX_Control(pDX, IDOK, m_closeButton);
	    DDX_Control(pDX, IDC_BUTTON_REMOVE_DATA, m_removeButton);
        DDX_Control(pDX, IDC_ARCHIVE_LIST, m_archiveList);
	    //}}AFX_DATA_MAP
    }
    
    
    BEGIN_MESSAGE_MAP(DeleteArchiveDlg, CDialog)
        //{{AFX_MSG_MAP(DeleteArchiveDlg)
	    ON_BN_CLICKED(IDC_BUTTON_REMOVE_DATA, OnButtonRemoveData)
	    ON_NOTIFY(LVN_ITEMCHANGED, IDC_ARCHIVE_LIST, OnItemChangedArchiveList)
		ON_BN_CLICKED(IDC_MODIFY_BUTTON, OnModifyButton)
	//}}AFX_MSG_MAP
		ON_MESSAGE(WM_FORCE_CANCEL, OnForceCancel)
    END_MESSAGE_MAP()
    

	LRESULT DeleteArchiveDlg::OnForceCancel(WPARAM wp, LPARAM lp)
	{
		CString message;
		message.LoadString( UI050050_DELETE_IN_PROG );
		//-- td19009 bgn
		TA_Base_Bus::TransActiveMessage userMsg;
		userMsg << TA_ArchiveManager::getOperatorNameFromSessionId();
		userMsg << TA_ArchiveManager::getWorkstationNameFromSessionId();
		UINT selectedButton = userMsg.showMsgBox(IDS_UI_050023,CAPTION_ARCHIVE_MANAGER);
		
		// Update all the items that still say AVAILABLE, to say CANCELLED.
		UINT state = 0;
		std::vector<int> cancelled;
		for ( int item = 0; item < m_archiveList.GetItemCount(); item++ )
		{
			state = m_archiveList.GetItemState( item, LVIS_STATEIMAGEMASK );
			state >>= 13;
			if( 1 == state )
			{
				if( m_archiveList.GetItemText( item, STATUS_COL ) == DELETED.c_str() )
				{
					m_archiveList.SetItemState( item, INDEXTOSTATEIMAGEMASK(0), LVIS_STATEIMAGEMASK );
				}
				else
				{
					m_archiveList.SetItemText( item, STATUS_COL, CANCELLED.c_str());
				}
			}
		}
		//-- td19009 end
		// Don't let the operator press Cancel again.
		m_status = IDCANCEL;
		/*
		try
		{
		CString message;
		message.FormatMessage( UI050023_DELETE_ABORTED, 
		TA_ArchiveManager::getOperatorNameFromSessionId().c_str(),
		TA_ArchiveManager::getWorkstationNameFromSessionId().c_str() );
		m_statusMessage.str( "" );
		m_statusMessage << message;
		}
		catch( TA_Base_Core::ArchiveException& e )
		{
		LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
		CString statusMsg;
		statusMsg.LoadString( UI050061_DELETE_ABORTED );
		m_statusMessage.str( "" );
		m_statusMessage << statusMsg;
		}
		*/  
		//-- td19009 bgn
		terminateAndWait();
		m_modifyButton.EnableWindow( TRUE ); 
		m_removeButton.EnableWindow( TRUE );
		m_archiveList.EnableWindow( TRUE );
		m_cancelButton.EnableWindow( TRUE );
		//-- td19009 end
		return 0;
	}

    void DeleteArchiveDlg::run()
    {
        // Make sure no exceptions escape from the run() method.

        try
        {
            // Disable the Change Date, Remove Data and Close buttons and the Archive List.
            // POSSIBLE TODO In the future, you should be able to close the Archive 
            // Manager without stopping the deletion.  For now, just leave it open.

            m_modifyButton.EnableWindow( FALSE );       // KT 2 Mar 04, PW #3133
            m_removeButton.EnableWindow( FALSE );
            m_archiveList.EnableWindow( FALSE );
            m_stateManager.setState( StateManager::PROCESSING );
			m_deletedCompleted = false;

            // KT 11 Mar 04: PW #3133. This method has to be changed because the 
            // DeleteArchiveDlg now does both the deletion of the data and the
            // modification of the deletion due date, so the message sending
            // requirements are different.
            
	          DeleteArchiveHelper helper;
            try
            {
                helper.initiateDelete();
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << e.what();
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_050131,CAPTION_ARCHIVE_MANAGER);
                
                helper.completeDelete( IDABORT, e.what() );
                m_stateManager.setState( StateManager::IDLE );
                enableCloseButton();
                return;
            }

            // Find the items that are ticked.

            UINT state = 0;
            unsigned long key = 0;
            TA_Base_Core::IArchiveInventory::ArchiveInventoryItemMap::iterator it;
            int itemCount = m_archiveList.GetItemCount();
	          for ( int item = 0; item < itemCount; item++ )
            {
                if( m_stateManager.getState() == StateManager::STOPPING )
                {
					this->PostMessage(WM_FORCE_CANCEL,0,0);
                    break;
                }
				
				//set m_deletedCompleted to true if processing last time.
				if(item == itemCount-1)
				{
					m_deletedCompleted = true;
				}
                //  For each of the ticked items, delete the restored data.
    
                state = m_archiveList.GetItemState( item, LVIS_STATEIMAGEMASK );
                state >>= 13;
                if( 1 == state )
                {
                    m_archiveList.SetItemText( item, STATUS_COL, DELETING.c_str() );
                    try
                    {
                        // KT 27 Feb 04: PW #3133: This has been changed so that users can remove
                        // an individual days worth of restored data from the database.  The 
                        // date range columns (start date to end date) have been replaced with a single 
                        // date column.

                        // KT 9 Mar 04: PW #3161 Converted to use time_t to remove dependence on date formatting.

                        key = m_archiveList.GetItemData( item );
                        it = m_archives.find( key );
                        if ( it == m_archives.end() )
                        {
                            CString reason;
                            reason.LoadString( UE050054_REASON_FIND_FAILED );
                            CString error;
                            error.FormatMessage( UE050054_REMOVE_FAILED, "selected date", reason );
                            
                            std::string date("selected date");
                            TA_Base_Bus::TransActiveMessage userMsg;
                            userMsg << date << reason;
                            UINT selectedButton = userMsg.showMsgBox(IDS_UE_050054,CAPTION_ARCHIVE_MANAGER);

                            m_archiveList.SetItemText( item, STATUS_COL, DELETE_FAILED.c_str() );
                        }

                        m_deleteArchiveHelper.deleteRestoredData( key, it->second.dataDate );
                        m_archiveList.SetItemText( item, STATUS_COL, DELETED.c_str() );
                    }
                    catch( TA_Base_Core::ArchiveException& e )
                    {
                        LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                        
                        TA_Base_Bus::TransActiveMessage userMsg;
                        userMsg << e.what();
                        UINT selectedButton = userMsg.showMsgBox(IDS_UE_050132,CAPTION_ARCHIVE_MANAGER);

                        m_archiveList.SetItemText( item, STATUS_COL, DELETE_FAILED.c_str() );
                    }
                }
            }
			//td19009 bgn

			 //if user has cancelled while deleting the last item, the cancel operation is ignored.
			if(m_deletedCompleted || m_stateManager.getState() == StateManager::PROCESSING ) //already delete all 
            {
				enableCloseButton(); 
            }
            //td19009 end
            m_stateManager.setState( StateManager::IDLE );

            // KT 11 Mar 04: PW #3133. This method has to be changed because the 
            // DeleteArchiveDlg now does both the deletion of the data and the
            // modification of the deletion due date, so the message sending
            // requirements are different.
            
            helper.completeDelete( m_status, m_statusMessage.str() );
        }
        catch( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() ); 
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Unknown exception caught in DeleteArchiveDlg::run()" ); 
        }
    }


    void DeleteArchiveDlg::terminate()
    {
        //-- td19009 bgn do this maybe dangerous
	/*	if(m_forceCancel && m_delthrd != NULL)
		{
			TerminateThread(m_delthrd, -1);
			m_forceCancel = false;
			CloseHandle(m_delthrd);
			m_delthrd = NULL;
			return;
		}
		if(m_delthrd != NULL)
		{
			CloseHandle(m_delthrd);
			m_delthrd = NULL;
		}*/
		//-- td19009 end
		// If the thread is still processing, notify the user, then tell it to stop.
        if ( TA_Base_Core::Thread::THREAD_STATE_RUNNING == getCurrentState() )
        {
            if ( m_stateManager.getState() == StateManager::PROCESSING )
            {
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UI_050050,CAPTION_ARCHIVE_MANAGER);

                m_stateManager.setState( StateManager::STOPPING );
            }
        }

        // Wait until it has completed.
        // We could put a count in here so we don't get stuck, but how long should we count for?
        // Deletion of a single day could take an hour or more... we don't know.

        while( m_stateManager.getState() != StateManager::IDLE )
        {
            sleep( 1000 );  // 1 second
        }
    }


    /////////////////////////////////////////////////////////////////////////////
    // DeleteArchiveDlg message handlers
    
    BOOL DeleteArchiveDlg::OnInitDialog() 
    {
        CDialog::OnInitDialog();
        
        //  Give this dialog its parent's icons.

        SetIcon( GetParent()->GetIcon( true ), TRUE );			// Set big icon
        SetIcon( GetParent()->GetIcon( false ), FALSE );		// Set small icon

        //  Display an hour-glass cursor.

        HCURSOR hOldCursor = SetCursor( LoadCursor( NULL, IDC_APPSTARTING ) );

        //  Put checkboxes down the left-hand side

        m_archiveList.SetExtendedStyle( LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT );

        CDC * pDC = m_archiveList.GetDC();
        int dateWidth = pDC->GetTextExtent( _T(" 88/88/8888 ")).cx;                 //  max date width - for the most common date format.
        int dateTimeWidth = pDC->GetTextExtent( _T(" 88/88/8888 88:88:88 AM ")).cx - 30; //  max date+time  width - for the most common date format.
        int whoWidth =  pDC->GetTextExtent( _T(" Restored By ")).cx;
        int deletionWidth =  pDC->GetTextExtent( _T(" Deletion Due ")).cx + 10;
        int statusWidth =  pDC->GetTextExtent( CANCELLED.c_str() ).cx + 15;
        //TD16728++
        //Hanhao
        // Prepare the listCtrl columns and fill it up
        
        LVCOLUMNA lvColumn;
        lvColumn.mask = LVCF_FMT | LVCF_IMAGE | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        lvColumn.fmt = LVCFMT_IMAGE; // LVCFMT_COL_HAS_IMAGES
        lvColumn.iImage = 1;
        lvColumn.iSubItem = TICK_COL;
        lvColumn.pszText = 0;
        lvColumn.cchTextMax = 0;
        lvColumn.cx = GetSystemMetrics( SM_CXICON ); 
        
        // KT 27 Feb 04: PW #3133: This has been changed so that users can remove
        // an individual days worth of restored data from the database.  
        // START_DATE_COL and END_DATE_COL have been replaced with DATA_DATE_COL.
        m_archiveList.InsertColumn( TICK_COL,  &lvColumn); 
        m_archiveList.InsertColumn( DATA_DATE_COL,        _T(DATA_DATE_MSG),        LVCFMT_LEFT, dateWidth, -1);
        m_archiveList.InsertColumn( RESTORED_BY_COL,      _T(RESTORED_BY_MSG),      LVCFMT_LEFT, whoWidth,  -1);
        m_archiveList.InsertColumn( RESTORATION_DATE_COL, _T(RESTORED_DATE_MSG), LVCFMT_LEFT, dateTimeWidth, -1);
        m_archiveList.InsertColumn( DELETION_DUE_DATE_COL,_T(DELETION_DATE_MSG),    LVCFMT_LEFT, deletionWidth, -1);
        m_archiveList.InsertColumn( STATUS_COL,           _T(STATUS_MSG),           LVCFMT_LEFT, statusWidth, -1);

        populateList();

        // Make sure there are files in the list.  If not, abort.
        // This is not done in populateList() because at some stage we
        // may want to use populateList() to perform a refresh, and we may
        // not want the dialog to close if the list is empty.

        if ( 0 == m_archiveList.GetItemCount() )
        {
            // populateList() will have displayed the error message already.

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "Archive list is empty. Closing Delete Archive dialog." );
            SendMessage( WM_CLOSE, 0, 0 );
        }

		// liborm 06/07/2004 - fix for TD defect #3521, #3530
		// focus the first item of the restored archives list box
		// when the tab key is used to change focus to the list box
		m_archiveList.SetItemState( m_archiveList.GetTopIndex(), LVIS_FOCUSED, LVIS_FOCUSED );

		m_archiveList.AutoResizeColumn();

        return TRUE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
    }


    void DeleteArchiveDlg::populateList()
    {
        // Get the list of restored data from the AR_INVENTORY table.

        try
        {
            m_archives = m_deleteArchiveHelper.getArchiveInventoryItems();
        }
        catch( TA_Base_Core::ArchiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "", e.what() );
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << e.what();
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_050053,CAPTION_ARCHIVE_MANAGER);//IDS_UE_050133); td19370
            enableCloseButton();
            return;
        }
        
        if( m_archives.empty() )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << TA_Base_Core::TA_ArchiveManagerDAI::ARCHIVE_INVENTORY_TABLE_NAME;
            UINT selectedButton = userMsg.showMsgBox(IDS_UW_050012,CAPTION_ARCHIVE_MANAGER);

            enableCloseButton();
            return;
        }

        std::string dataDate( "" );
        std::string restorationDate( "" );
        std::string deletionDueDate( "" );
        int itemIndex = 0;
        for ( TA_Base_Core::IArchiveInventory::ArchiveInventoryItemMap::iterator it = m_archives.begin(); it != m_archives.end(); ++it )
        {   
            // Don't show data that is marked as already being deleted.

            if ( it->second.isDeleting )
            {
                continue;
            }
            
            // KT 27 Feb 04: PW #3133: This has been changed so that users can remove
            // an individual days worth of restored data from the database.  The 
            // date range columns (start date to end date) have been replaced with a single 
            // "DataDate" date column.
                 
            // KT 8 Mar 04: PW #3161 Converted dates to local format.

            dataDate = TA_ArchiveManager::convertTimeTToStringDate( it->second.dataDate );
            restorationDate = TA_ArchiveManager::convertTimeTToStringDate( it->second.restoredDate );
            deletionDueDate = TA_ArchiveManager::convertTimeTToStringDate( it->second.deletionDueDate );

            itemIndex = m_archiveList.InsertItem( m_archiveList.GetItemCount(), NULL );
            m_archiveList.SetItemText(itemIndex, DATA_DATE_COL, dataDate.c_str() );
            m_archiveList.SetItemText(itemIndex, RESTORED_BY_COL, it->second.restoredBy.c_str() );
            m_archiveList.SetItemText(itemIndex, RESTORATION_DATE_COL, restorationDate.c_str() );
            m_archiveList.SetItemText(itemIndex, DELETION_DUE_DATE_COL, deletionDueDate.c_str() );
            m_archiveList.SetItemText(itemIndex, STATUS_COL, AVAILABLE.c_str() );  // We only show available items, not data that is being deleted.

            // Now store the id of the archive associated with the list control item.
            
            m_archiveList.SetItemData(itemIndex, it->first);
        }

        if ( 0 == m_archiveList.GetItemCount() )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << TA_Base_Core::TA_ArchiveManagerDAI::ARCHIVE_INVENTORY_TABLE_NAME;
            UINT selectedButton = userMsg.showMsgBox(IDS_UW_050012,CAPTION_ARCHIVE_MANAGER);

            enableCloseButton();
            return;
        }

        // Sort the list by START_DATE.

        m_archiveList.SortItems( compareItems, (unsigned long)(&m_archives) );
    }
    

    void DeleteArchiveDlg::OnItemChangedArchiveList(NMHDR* pNMHDR, LRESULT* pResult) 
    {
      	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
        
        // Don't allow items to be selected (highlighted) except by the tick boxes.

        m_archiveList.SetItemState(pNMListView->iItem, ~LVIS_SELECTED, LVIS_SELECTED);

        // Only bother if the user clicked in the tick column.

        if ( TICK_COL == pNMListView->iSubItem )            
        {
            // If any of the items are ticked, enable the Remove button.
            // Otherwise disable it.

            UINT state = 0;
            int itemCount = m_archiveList.GetItemCount();
			int item = 0;
	        for (; item < itemCount; item++ )
            {
                state = m_archiveList.GetItemState( item, LVIS_STATEIMAGEMASK );
                state >>= 13;
                if( 1 == state )
                {
                    break;  // At least one item is ticked.
                }
            }

            m_modifyButton.EnableWindow( item != itemCount );       // KT 2 Mar 04, PW #3133
            m_removeButton.EnableWindow( item != itemCount );
        }
        
	    *pResult = 0;
    }


    void DeleteArchiveDlg::OnButtonRemoveData() 
    {
        // Deletion is run in a separate thread, so we don't block the UI.
		UINT state = 0;
		for ( int item = 0; item < m_archiveList.GetItemCount(); item++ )
		{
			state = m_archiveList.GetItemState( item, LVIS_STATEIMAGEMASK );
			state >>= 13;
			if( 1 == state )
			{
				m_archiveList.SetItemText(item, STATUS_COL, AVAILABLE.c_str());
			}
		}
		start();
    }


    void DeleteArchiveDlg::OnModifyButton() 
    {
        // KT 3 Mar 04, PW #3133 Method added to allow user to change the deletion due date.

        setEnabledProperty( false );  // This is a short process and you can't cancel during it.

        // Send the audit message and check the user privileges.

        DeleteArchiveHelper helper;
        try
        {
            helper.initiateChangeDeletionDate();
        }
        catch( TA_Base_Core::ArchiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << e.what();
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_050134,CAPTION_ARCHIVE_MANAGER);

            helper.completeChangeDeletionDate( IDCANCEL, e.what() );
            setEnabledProperty( true );
            return;
        }

        // Open the dialog that allows the user to change the deletion date for the given items.

        SelectDeletionDateDlg dlg;
        int status = dlg.DoModal();
        if ( IDCANCEL == status )
        {
            // Handle if the user cancels.

            CString message;
            try
            {
                std::string operatorName = TA_ArchiveManager::getOperatorNameFromSessionId();
                std::string workstationName = TA_ArchiveManager::getWorkstationNameFromSessionId();

                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << operatorName << workstationName;
                UINT selectedButton = userMsg.showMsgBox(IDS_UI_050057,CAPTION_ARCHIVE_MANAGER);

                message.FormatMessage( UI050057_CHANGE_DATE_ABORTED, operatorName.c_str(), workstationName.c_str());
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                message.LoadString( UI050060_TRANSFER_ABORTED );

                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UI_050060,CAPTION_ARCHIVE_MANAGER);
            }
            
            helper.completeChangeDeletionDate( IDCANCEL, std::string( message ) );
            setEnabledProperty( true );
            return;
        }
        else if ( IDOK == status )
        {
            // If the user selected OK, get the selected date.
            // KT 8 Mar 04: PW #3161 Converted newDeletionDate to time_t.

            time_t newDeletionDate;
            try
            {
                newDeletionDate = dlg.getDeletionDueDate();
            }
            catch( TA_Base_Core::ArchiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
                CString error;
                error.FormatMessage( UE050117_SELECT_FAILED, e.what() );
                
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << e.what();
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_050117,CAPTION_ARCHIVE_MANAGER);

                helper.completeChangeDeletionDate( IDABORT, std::string( error ) );
                setEnabledProperty( true );
                return;
            }

            std::auto_ptr< TA_Base_Core::IArchiveInventory > inventory(
                TA_Base_Core::ArchiveInventoryAccessFactory::getInstance().getArchiveInventory() );
            if( 0 == inventory.get() )
            {
                CString reason;
                reason.LoadString( UE050118_REASON_INV_ACCESS_FAILED );
                CString error;
                error.FormatMessage( UE050118_UPDATE_FAILED, reason );
                
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << error;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_050118,CAPTION_ARCHIVE_MANAGER);

                helper.completeChangeDeletionDate( IDABORT, std::string( error ) );
                setEnabledProperty( true );
                return;
            }
            
            // If the user selected a date successfully, change the deletion due date for  
            // all the selected items to the new date.
            // If any of the items can't be updated, don't process any more dates.

            TA_Base_Core::IArchiveInventory::ArchiveInventoryItem inventoryItem;
            UINT state = 0;
	          for ( int item = 0; item < m_archiveList.GetItemCount(); item++ )
            {
                state = m_archiveList.GetItemState( item, LVIS_STATEIMAGEMASK );
                state >>= 13;
                if( 1 == state )
                {
                    // Update the database and the GUI.

                    try
                    {
                        inventoryItem = inventory->getArchiveInventoryItem( m_archiveList.GetItemData( item ) );
                        inventoryItem.deletionDueDate = newDeletionDate;
                        inventory->setArchiveInventoryItem( inventoryItem );
                        
                        // KT 8 Mar 04: PW #3161 Converted newDeletionDate to local format for display.

                        m_archiveList.SetItemText( item, DELETION_DUE_DATE_COL, 
                            TA_ArchiveManager::convertTimeTToStringDate( newDeletionDate ).c_str() );
                    }
                    catch( TA_Base_Core::DatabaseException& e )
                    {
                        LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
                        CString error;
                        error.FormatMessage( UE050118_UPDATE_FAILED, e.what() );
                        
                        TA_Base_Bus::TransActiveMessage userMsg;
                        userMsg << error;
                        UINT selectedButton = userMsg.showMsgBox(IDS_UE_050118,CAPTION_ARCHIVE_MANAGER);

                        helper.completeChangeDeletionDate( IDABORT, std::string( error ) );
                        setEnabledProperty( true );
                        return;
                    }
                    catch( TA_Base_Core::DataException& e )
                    {
                        LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
                        CString error;
                        error.FormatMessage( UE050118_UPDATE_FAILED, e.what() );

                        TA_Base_Bus::TransActiveMessage userMsg;
                        userMsg << error;
                        UINT selectedButton = userMsg.showMsgBox(IDS_UE_050118,CAPTION_ARCHIVE_MANAGER);

                        helper.completeChangeDeletionDate( IDABORT, std::string( error ) );
                        setEnabledProperty( true );
                        return;
                    }
                }
            }
        }

        TA_ASSERT( IDOK == status, "SelectDeletionDateDlg::doModal() returned an unexpected value" );

        helper.completeChangeDeletionDate( IDOK, "" );
        setEnabledProperty( true );
    }


    void DeleteArchiveDlg::OnCancel() 
    {
        // If we are in the middle of deleting, we have to wait for the
        // day that is currently being restored to be fully deleted, before
        // we cancel.

        if ( StateManager::PROCESSING == m_stateManager.getState() )
        {
			m_cancelButton.EnableWindow( FALSE );
            m_stateManager.setState( StateManager::STOPPING );
        }
        else
        {
            // We're not processing, so we can just cancel.

            CDialog::OnCancel();
        }
    }


    void DeleteArchiveDlg::enableCloseButton()
    {
        m_closeButton.ShowWindow( TRUE );
        m_closeButton.SetFocus();
        m_modifyButton.ShowWindow( FALSE );         // KT 2 Mar 04, PW #3133

		//default close button in dialog should always be enabled
		m_cancelButton.EnableWindow( TRUE );
        m_cancelButton.ShowWindow( FALSE );
        m_removeButton.ShowWindow( FALSE );
        m_closeButton.EnableWindow( TRUE );
        m_closeButton.SetFocus();
        m_closeButton.SetButtonStyle( BS_DEFPUSHBUTTON );
    }

    
    void DeleteArchiveDlg::setEnabledProperty( bool isEnabled )
    {
        m_modifyButton.EnableWindow( isEnabled );
        m_removeButton.EnableWindow( isEnabled );
        m_archiveList.EnableWindow( isEnabled );
        m_cancelButton.EnableWindow( isEnabled );
    }

    
    int DeleteArchiveDlg::compareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
    {
        // KT 27 Feb 04: PW #3133: startDate has been replaced with dataDate.

        // Use the map of archives to sort the archive list.

        TA_Base_Core::IArchiveInventory::ArchiveInventoryItemMap* archives = 
            reinterpret_cast< TA_Base_Core::IArchiveInventory::ArchiveInventoryItemMap* >( lParamSort );
        if ( archives == 0 )
        {
            return 0;
        }

        TA_Base_Core::IArchiveInventory::ArchiveInventoryItemMap::iterator it = archives->find( lParam1 );
        if ( archives->end() == it )
        {
            return 0;
        }
        time_t date1 = it->second.dataDate;
                
        it = archives->find( lParam2 );
        if ( archives->end() == it )
        {
            return 0;
        }
        time_t date2 = it->second.dataDate;

        if ( date1 < date2 )
        {
            return -1;
        }
        else if ( date1 > date2 )
        {
            return 1;
        }
        return 0;
    }
} // TA_Base_App



