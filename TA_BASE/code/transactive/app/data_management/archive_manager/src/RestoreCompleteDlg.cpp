/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/RestoreCompleteDlg.cpp $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Displayed if there are errors during the restoration process.
  */

#include "app/data_management/archive_manager/src/stdafx.h"
#include "app/data_management/archive_manager/src/RestoreCompleteDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // RestoreCompleteDlg dialog


    RestoreCompleteDlg::RestoreCompleteDlg(const TA_ArchiveManager::FileStatusMap& fileStatusMap, CWnd* pParent /*=NULL*/)
	    : CDialog(RestoreCompleteDlg::IDD, pParent),
          m_fileStatusMap( fileStatusMap )
    {
	    //{{AFX_DATA_INIT(RestoreCompleteDlg)
	    //}}AFX_DATA_INIT
    }


    void RestoreCompleteDlg::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(RestoreCompleteDlg)
	    DDX_Control(pDX, IDC_ERROR_TREE, m_errorTree);
	    //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(RestoreCompleteDlg, CDialog)
	    //{{AFX_MSG_MAP(RestoreCompleteDlg)
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // RestoreCompleteDlg message handlers

    BOOL RestoreCompleteDlg::OnInitDialog() 
    {
	    CDialog::OnInitDialog();
        
        //  Give this dialog its parent's icons.

        SetIcon( GetParent()->GetIcon( true ), TRUE );			// Set big icon
        SetIcon( GetParent()->GetIcon( false ), FALSE );		// Set small icon

        //  Display an hour-glass cursor.

        HCURSOR hOldCursor = SetCursor( LoadCursor( NULL, IDC_APPSTARTING ) );

        // If there are no errors to report, return yes, because we want to continue.

        if ( m_fileStatusMap.empty() )
        {
            CDialog::EndDialog( IDOK );
        }

		m_treeFont.CreatePointFont(110, "宋体" );
		m_errorTree.SetFont(&m_treeFont,FALSE);  
		
		m_HZScrollBarInfo.cbSize    = sizeof( SCROLLINFO );
		m_HZScrollBarInfo.fMask     = SIF_POS;
		m_HZScrollBarInfo.nMin      = 0;
		m_HZScrollBarInfo.nMax      = 0;
		m_HZScrollBarInfo.nPage     = 0;
		m_HZScrollBarInfo.nPos      = 0;
		m_HZScrollBarInfo.nTrackPos = 0;
/*
        insertItems( TA_ArchiveManager::EX_FAIL, std::string( "File(s) Containing Errors" ) );
        insertItems( TA_ArchiveManager::EX_OKWARN, std::string( "File(s) Containing Warnings" ) );
        insertItems( TA_ArchiveManager::UNKNOWN, std::string( "File(s) Containing An Unknown Status" ) );
		*/
		insertItems( TA_ArchiveManager::EX_FAIL, std::string( "文件包含错误" ) );
		insertItems( TA_ArchiveManager::EX_OKWARN, std::string( "文件包含告警" ) );
		insertItems( TA_ArchiveManager::UNKNOWN, std::string( "文件包含未知状态" ) );      
        // If there are no errors to report, return yes, because we want to continue.
       
		//set scrollbar position
		m_errorTree.SetScrollInfo(SB_HORZ, &m_HZScrollBarInfo,0);

        if ( 0 == m_errorTree.GetCount() )
        {
            CDialog::EndDialog( IDOK );
        }

        return TRUE;  // return TRUE unless you set the focus to a control
	          // EXCEPTION: OCX Property Pages should return FALSE
    }

 
    void RestoreCompleteDlg::insertItems( TA_ArchiveManager::EErrorStatus status, const std::string& rootMsg )
    {
        std::pair< TA_ArchiveManager::FileStatusMap::const_iterator, 
            TA_ArchiveManager::FileStatusMap::const_iterator > range = 
            m_fileStatusMap.equal_range( status ); 
        
        if( ( range.first != range.second ) && ( m_fileStatusMap.end() != range.first ) )
        {
            // Insert a root item using the structure. We must
            // initialize a TVINSERTSTRUCT structure and pass its
            // address to the call. 

			TVINSERTSTRUCT tvInsert;
			tvInsert.hParent = NULL;
			tvInsert.hInsertAfter = NULL;
			tvInsert.item.mask = TVIF_TEXT;
			tvInsert.item.pszText = const_cast< char* >( rootMsg.c_str() );
			HTREEITEM hRoot = m_errorTree.InsertItem(&tvInsert);

            for ( ; range.first != range.second; ++range.first )
            {
                // Insert the files as subitems.

                HTREEITEM hItem = m_errorTree.InsertItem(TVIF_TEXT,
                   range.first->second.c_str(), 0, 0, 0, 0, 0, hRoot, NULL);
				
				m_errorTree.EnsureVisible(hItem);
 
				// TD16728 No HScroll in the ErrorTreeCtrl

				DWORD dwFileListStyle = m_errorTree.GetStyle();
				if(dwFileListStyle & WS_HSCROLL)
				{
					CRect rcErrorTree;
					m_errorTree.GetWindowRect(&rcErrorTree);
					CRect rcDlg;
					GetWindowRect(&rcDlg);

					CDC *pDC = GetDC();
					CSize csColName = pDC->GetTextExtent( range.first->second.c_str() );
					UINT uIncrement = csColName.cx + 50 - rcErrorTree.Width();
					
					SetWindowPos(NULL, 0, 0, rcDlg.Width()+uIncrement, rcDlg.Height(), SWP_NOZORDER | SWP_NOMOVE);
					m_errorTree.SetWindowPos(NULL, 0, 0, rcErrorTree.Width()+uIncrement, rcErrorTree.Height(), SWP_NOZORDER | SWP_NOMOVE);

				}

            }
        }
    }

    
} // TA_Base_App
