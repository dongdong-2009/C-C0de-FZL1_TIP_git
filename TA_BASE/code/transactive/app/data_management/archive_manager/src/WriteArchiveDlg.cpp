/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/WriteArchiveDlg.cpp $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Dialog box to list the files that will be written to archive media,
  * and prompt the operator to begin the write process.
  *
  */

#if defined( WIN32 )
#pragma warning ( disable : 4786 4250 4503 ) 
#endif // defined( WIN32 )


#include <map>
#include <sstream>

#include "app/data_management/archive_manager/src/stdafx.h"
#include "app/data_management/archive_manager/src/WriteArchiveDlg.h"
#include "app/data_management/archive_manager/src/BurnCdDlg.h"
#include "app/data_management/archive_manager/src/BurnDvdDlg.h"
#include "app/data_management/archive_manager/src/ChineseInfo.h"
#include "core/exceptions/src/ArchiveException.h"
#include "core/message/types/ArchiveAudit_MessageTypes.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "bus/generic_gui/src/TransactiveMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    //
    // This will be displayed on the dialog with the number of CDs filled in.
    //

    //
    // Enum specifying the columns in the archive list.
    //

    enum EListColumns
    {
        DATE_COL    = 0,
        NAME_COL    = 1,
        SIZE_COL    = 2
    };
}

namespace TA_Base_App
{
    
    /////////////////////////////////////////////////////////////////////////////
    // WriteArchiveDlg dialog
    
    
    WriteArchiveDlg::WriteArchiveDlg( TA_Base_Bus::IGUIAccess& guiAccessor, CWnd* pParent /*=NULL*/)
        : CDialog(WriteArchiveDlg::IDD, pParent),
          m_writeArchiveHelper( guiAccessor ),
          m_isManualArchive( false ),
		  m_fileListColNameWidth(0)
    {        
        //{{AFX_DATA_INIT(WriteArchiveDlg)
	    //}}AFX_DATA_INIT
    }
     
    
    WriteArchiveDlg::WriteArchiveDlg( TA_Base_Bus::IGUIAccess& guiAccessor, const time_t& fromDate,
        const time_t& toDate, CWnd* pParent /*=NULL*/)
        : CDialog(WriteArchiveDlg::IDD, pParent),
          m_writeArchiveHelper( guiAccessor ),
          m_fromDate( fromDate ),
          m_toDate( toDate ),
          m_isManualArchive( true )
    {        
        // Do nothing
    }


    void WriteArchiveDlg::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(WriteArchiveDlg)
		DDX_Control(pDX, IDC_STATIC_INSERT_CDDVD, m_insertCDDVD);
	    DDX_Control(pDX, IDC_NUM_CDS_STATIC, m_numCds);
	    DDX_Control(pDX, IDOK, m_okButton);
        DDX_Control(pDX, IDC_FILE_LIST, m_fileList);
	    //}}AFX_DATA_MAP
    }
    
    
    BEGIN_MESSAGE_MAP(WriteArchiveDlg, CDialog)
        //{{AFX_MSG_MAP(WriteArchiveDlg)
	    ON_NOTIFY(LVN_ITEMCHANGED, IDC_FILE_LIST, OnItemChangedFileList)
	    ON_WM_CLOSE()
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()
    
    /////////////////////////////////////////////////////////////////////////////
    // WriteArchiveDlg message handlers
    
    BOOL WriteArchiveDlg::OnInitDialog() 
    {
        CDialog::OnInitDialog();
        	            
        // Give this dialog its parent's icons.

        SetIcon( GetParent()->GetIcon( true ), TRUE );			// Set big icon
        SetIcon( GetParent()->GetIcon( false ), FALSE );		// Set small icon

        // Display an hour-glass cursor.

        HCURSOR hOldCursor = SetCursor( LoadCursor( NULL, IDC_APPSTARTING ) );
        
        // Prepare the CListCtrl columns and fill it up.

        CRect rc;
        m_fileList.GetWindowRect( rc );
        CDC* pDC = m_fileList.GetDC();
        int dateWidth = pDC->GetTextExtent( _T(" 88/88/8888 ")).cx;     //  max date width - for the most common date format.

		int sizeMbWidth = 0;
		if( TA_Base_Core::RunParams::getInstance().isSet( WriteArchiveHelper::CD_ARCHIVE.c_str()) )
		{
			sizeMbWidth = pDC->GetTextExtent( _T(" 888.88 MB ")).cx;    //  max MB size (for a CD)
		}
		else
		{
			sizeMbWidth = pDC->GetTextExtent( _T(" 8888.88 MB ")).cx;    //  max MB size (for a DVD)
		}
        sizeMbWidth+= GetSystemMetrics( SM_CXVSCROLL);
        
		m_fileListColNameWidth = rc.Width()-dateWidth-sizeMbWidth-4;
        m_fileList.InsertColumn( DATE_COL,  _T(DATE_MSG), LVCFMT_CENTER, dateWidth, 0);
        m_fileList.InsertColumn( NAME_COL,  _T(FILENAME_MSG), LVCFMT_LEFT, m_fileListColNameWidth, 1);
        m_fileList.InsertColumn( SIZE_COL,  _T(SIZE_MSG), LVCFMT_RIGHT, sizeMbWidth, 2);

        populateList();

        // Make sure there are files in the list.  If not, abort.
        // This is not done in populateList() because at some stage we
        // may want to use populateList() to perform a refresh, and we may
        // not want the dialog to close if the list is empty.

        if ( 0 == m_fileList.GetItemCount() )
        {
            // The error message will have been displayed in populateList().

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "Archive list is empty. Closing Insert Archive Media dialog." );
            CDialog::EndDialog( IDABORT );
        }

		m_fileList.AutoResizeColumn();

        return TRUE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
    }


    void WriteArchiveDlg::populateList()
    {
        // Get the dates, names and sizes of the files in the archive directory.

        WriteArchiveHelper::FileDetailsVector fileDetails; 
        std::vector<std::string> archiveDirs;
		m_writeArchiveHelper.setDvdVolumeNum();
        try
        {
            // If it is a manual archive, the date range will be limited.

            archiveDirs = m_writeArchiveHelper.getArchiveDirectory( m_isManualArchive );

            // Grab the first set of file details
            if ( m_isManualArchive )
            {
                fileDetails = m_writeArchiveHelper.getAllFileDetails( archiveDirs[0], m_fromDate, m_toDate );
            }
            else
            {
                fileDetails = m_writeArchiveHelper.getAllFileDetails( archiveDirs[0] );
            }

            //  Now if there are any more loop through and append them to the vector
            for (unsigned int i =1; i < archiveDirs.size(); ++i)
            {
                WriteArchiveHelper::FileDetailsVector extraFileDetails = m_writeArchiveHelper.getAllFileDetails( archiveDirs[i] );

                fileDetails.insert( fileDetails.end(), extraFileDetails.begin(), extraFileDetails.end() );
            }
        }
        catch( TA_Base_Core::ArchiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
            TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::CopyArchiveFailed, e.what() ); 
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << e.what();
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_050141,CAPTION_ARCHIVE_MANAGER);

            return;
        }
        if ( fileDetails.empty() )
        {
            // KT 10 Mar 04: PW #3136 - Changed '-' to '_' for operating system compatibility.

            CString error;
            if ( m_isManualArchive )
            {
                CString files;
                for(unsigned int i =0; i < archiveDirs.size(); ++i)
                {
                    files += archiveDirs[i].c_str();
                }

                std::string startTime = TA_ArchiveManager::convertTimeTToStringDate( m_fromDate );
                std::string endTime = TA_ArchiveManager::convertTimeTToStringDate( m_toDate );
                error.FormatMessage( UW050013_NO_MAN_ARCHIVES, files, startTime.c_str(), endTime.c_str() );

                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << files << startTime << endTime;
                UINT selectedButton = userMsg.showMsgBox(IDS_UW_050013,CAPTION_ARCHIVE_MANAGER);
            }
            else
            {
                CString files;
                for(unsigned int i =0; i < archiveDirs.size(); ++i)
                {
                    files += archiveDirs[i].c_str();
                    
                    if (i < archiveDirs.size() - 1)
                    {
                        // For all items but the last one append a comma
                        files += ", ";
                    }
                }

                error.FormatMessage( UW050014_NO_AUTO_ARCHIVES, files );

                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << files;
                UINT selectedButton = userMsg.showMsgBox(IDS_UW_050014,CAPTION_ARCHIVE_MANAGER);
            }
            TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::CopyArchiveFailed, std::string( error ) ); 

            return;
        }
       
        // Split the files into the CDs that they will be burnt to.
        // Then we know how many CDs it will take.

        try
        {
            m_cdsMap = m_writeArchiveHelper.splitIntoCds( fileDetails );
            std::stringstream numCds;
			if( !TA_Base_Core::RunParams::getInstance().isSet( WriteArchiveHelper::CD_ARCHIVE.c_str()) )
			{
				numCds << REQUIRED_MSG<< m_cdsMap.size() << NUM_DVDS_MSG;
				m_insertCDDVD.SetWindowText( INSERT_DVD_MSG.c_str() );
			}
			else
			{
				numCds<< REQUIRED_MSG << m_cdsMap.size() << NUM_CDS_MSG;
				m_insertCDDVD.SetWindowText( INSERT_CD_MSG.c_str() );
			}
            m_numCds.SetWindowText( numCds.str().c_str() );
        }
        catch( TA_Base_Core::ArchiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
            TA_ArchiveManager::sendAuditMessageIgnoreErrors( TA_Base_Core::ArchiveAudit::CopyArchiveFailed, e.what() ); 
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << e.what();
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_050142,CAPTION_ARCHIVE_MANAGER);

            return;
        }

        // Display the information in the list ctrl.
        // Put the file size in the item data, so we can use it when figuring out how much is going on each DVD.

        int item = 0;
        CString strMb;
		CDC *pDC = GetDC();
		UINT uColNameMaxWidth = 0;
        for( WriteArchiveHelper::FileDetailsVector::iterator it = fileDetails.begin(); it != fileDetails.end(); ++it )
        {
            // KT 8 Mar 04: PW #3161 Convert date to local format.

            item = m_fileList.InsertItem( m_fileList.GetItemCount(), 
                TA_ArchiveManager::convertTimeTToStringDate( it->date ).c_str() );
            m_fileList.SetItemText( item, NAME_COL, it->name.c_str() );
			CSize csColName = pDC->GetTextExtent(it->name.c_str());
			uColNameMaxWidth = uColNameMaxWidth < csColName.cx ? csColName.cx : uColNameMaxWidth;
            strMb.Format( "%0.2f MB", it->sizeInMB );
            m_fileList.SetItemText( item, SIZE_COL, strMb );
            //m_fileList.SetItemData( item, (unsigned long)( it->sizeInMB + 1 ) );
			m_fileList.SetItemData( item, item);
        }
		
		if(uColNameMaxWidth > m_fileListColNameWidth)
		{
			UINT uIncrement = uColNameMaxWidth - m_fileListColNameWidth;

			CRect rcFileList;
			m_fileList.GetWindowRect(&rcFileList);
			CRect rcDlg;
			GetWindowRect(&rcDlg);

			
			
			SetWindowPos(NULL, 0, 0, rcDlg.Width()+uIncrement, rcDlg.Height(), SWP_NOZORDER | SWP_NOMOVE);
			m_fileList.SetWindowPos(NULL, 0, 0, rcFileList.Width()+uIncrement, rcFileList.Height(), SWP_NOZORDER | SWP_NOMOVE);
			m_fileList.SetColumnWidth(1, uColNameMaxWidth);
		}
		

		DWORD dwFileListStyle = m_fileList.GetStyle();
		if(dwFileListStyle & WS_VSCROLL)
		{
			CRect rcFileList;
			m_fileList.GetWindowRect(&rcFileList);
			CRect rcDlg;
			GetWindowRect(&rcDlg);
			
			SetWindowPos(NULL, 0, 0, rcDlg.Width()+GetSystemMetrics(SM_CXVSCROLL), rcDlg.Height(), SWP_NOZORDER | SWP_NOMOVE);
			m_fileList.SetWindowPos(NULL, 0, 0, rcFileList.Width()+GetSystemMetrics(SM_CXVSCROLL), rcFileList.Height(), SWP_NOZORDER | SWP_NOMOVE);
		}

        // KT FOR TESTING:showCdBreakUp();
    }


    void WriteArchiveDlg::OnItemChangedFileList(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        // Don't allow items to be selected (highlighted).

        m_fileList.SetItemState(pNMListView->iItem, ~LVIS_SELECTED, LVIS_SELECTED);
	    
	    *pResult = 0;
    }


    void WriteArchiveDlg::OnOK() 
    {
		// Send an audit message.
		
		try
		{
            TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::CopyArchiveRequest, m_cdsMap.begin()->first, m_isManualArchive );
        }
        catch( TA_Base_Core::ArchiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
			return;
        }
		
        // If there are no more CDs to write, clean up and close the dialog.

        if ( m_cdsMap.empty() )
        {
            cleanUp();	    
            CDialog::OnOK();
        }

        // Otherwise, proceed to writing.

        try
        {
            // This will start burning as soon as the dialog is open.

			if( TA_Base_Core::RunParams::getInstance().isSet( WriteArchiveHelper::CD_ARCHIVE.c_str()) )
			{
				BurnCdDlg dlg( m_cdsMap.begin()->first, m_cdsMap.begin()->second.first, m_cdsMap.begin()->second.second );

				if( IDOK != dlg.DoModal() )
				{
					// If we aborted due to an error, don't change anything.

					return;
				}
			}
			else
			{
	            BurnDvdDlg dlg( m_cdsMap.begin()->first, m_cdsMap.begin()->second.first, m_cdsMap.begin()->second.second );

				if( IDOK != dlg.DoModal() )
				{
					// If we aborted due to an error, don't change anything.

					return;
				}
			}

			try
			{
				TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::ArchiveCopied, "", m_isManualArchive ); 
			}
			catch( TA_Base_Core::ArchiveException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
				TA_Base_Bus::TransActiveMessage userMsg;
				std::string messageForUser = e.what();
				TA_ArchiveManager::EMsgLevel messageLevel = TA_ArchiveManager::WARN;
				userMsg << messageForUser.c_str();
				UINT selectedButton = userMsg.showMsgBox(IDS_UW_050018,CAPTION_ARCHIVE_MANAGER);
				
				return;
			}
            
        }
        catch( TA_Base_Core::ArchiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );  
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << e.what();
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_050130,CAPTION_ARCHIVE_MANAGER);
        }
        catch( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << e.what();
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_050130,CAPTION_ARCHIVE_MANAGER);
        }

        // Now we need to delete the files from the first entry in the map,
        // because the files have been burnt.  These should
        // be deleted from the file list also.

        try
        {
            m_writeArchiveHelper.deleteArchiveFiles( m_cdsMap.begin()->second.second );
            for( unsigned int i = 0; i < m_cdsMap.begin()->second.second.size(); i++ )
            {
                m_fileList.DeleteItem( 0 );
            }
            m_cdsMap.erase( m_cdsMap.begin() );
        }
        catch( TA_Base_Core::ArchiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );  
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << e.what();
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_050130,CAPTION_ARCHIVE_MANAGER);
        }


        // Do we have more files to write?

        int count = m_cdsMap.size();
        if ( 0 >= count )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UI_050054,CAPTION_ARCHIVE_MANAGER);

            cleanUp();	    
            CDialog::OnOK();
        }
        else
        {
            std::stringstream numCds;
			if( !TA_Base_Core::RunParams::getInstance().isSet( WriteArchiveHelper::CD_ARCHIVE.c_str()) )
			{
				numCds << REQUIRED_MSG<< m_cdsMap.size() << NUM_DVDS_MSG;
				m_insertCDDVD.SetWindowText( INSERT_DVD_MSG.c_str());
			}
			else
			{
				numCds << REQUIRED_MSG<< m_cdsMap.size() << NUM_CDS_MSG;
				m_insertCDDVD.SetWindowText( INSERT_CD_MSG.c_str());
			}
            m_numCds.SetWindowText( numCds.str().c_str() );
        }
    }


    void WriteArchiveDlg::OnCancel() 
    {
		try
        {
            TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::CopyArchiveCancelled );
        }
        catch( TA_Base_Core::ArchiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
        }
        cleanUp();	    
	    CDialog::OnCancel();
    }


    void WriteArchiveDlg::OnClose() 
    {
        cleanUp();
	    CDialog::OnClose();
    }


    void WriteArchiveDlg::cleanUp()
    {
        // Delete all the files that are left if this is a manual archive.

        if ( m_isManualArchive )
        {
            WriteArchiveHelper::CdsMap::iterator it;
            while( !m_cdsMap.empty() )
            {
                m_writeArchiveHelper.deleteArchiveFiles( m_cdsMap.begin()->second.second );
                m_cdsMap.erase( m_cdsMap.begin() );
            }
        }
    }


    void WriteArchiveDlg::showCdBreakUp()
    {
        // KT 5 Mar 04: THIS IS A METHOD FOR TESTING WRITEARCHIVEHELPER::SPLITINTOCDS().
        // IT IS NOT USED IN THE ACTUAL APPLICATION.

        std::stringstream message;
        message << "This is the break up of files into CDs:";
        for ( WriteArchiveHelper::CdsMap::iterator it = m_cdsMap.begin(); it != m_cdsMap.end(); ++it )
        {
            message << "\n" << it->first;
            message << "\n" << it->second.first;
            for ( WriteArchiveHelper::FileDetailsVector::iterator fit = it->second.second.begin(); fit != it->second.second.end(); ++fit )
            {
                message << "\n    " << (*fit).nameWithPath;
            }
        }

        // AL 17/03/05: Haven't changed this message box to use Transactive Message as it's not used
        // used in the actual application.
        MessageBox( message.str().c_str(), "CD Details", MB_OK | MB_ICONINFORMATION );
    }

} // TA_Base_App
