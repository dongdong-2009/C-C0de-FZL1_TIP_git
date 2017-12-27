/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/ArchiveManagerDlg.cpp $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Dialog for the operator to select the archive operation to perform.
  * Inherits from TransActiveDialog.
  */

#if defined( WIN32 )
#pragma warning ( disable : 4503 ) 
#endif // defined( WIN32 )

#include "app/data_management/archive_manager/src/stdafx.h"
#include "app/data_management/archive_manager/src/ArchiveManagerDlg.h"
#include "app/data_management/archive_manager/src/DeleteArchiveDlg.h"
#include "app/data_management/archive_manager/src/WriteArchiveHelper.h"
#include "app/data_management/archive_manager/src/RestoreArchiveHelper.h"
#include "app/data_management/archive_manager/src/ChineseInfo.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "core/exceptions/src/ArchiveException.h"
#include "core/utilities/src/RunParams.h"

#include "bus/generic_gui/src/TransactiveMessage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace 
{
    UINT getIconType( TA_ArchiveManager::EMsgLevel messageLevel )
    {
        switch( messageLevel )
        {
            case TA_ArchiveManager::INFO:
                return MB_ICONINFORMATION;
            case TA_ArchiveManager::WARN:
                return MB_ICONWARNING;
            case TA_ArchiveManager::ERR:
            default:
                return MB_ICONERROR;
        }
    }
}


namespace TA_Base_App
{
    
    /////////////////////////////////////////////////////////////////////////////
    // CArchiveManagerDlg dialog
    
    CArchiveManagerDlg::CArchiveManagerDlg(TA_Base_Bus::IGUIAccess& guiAccess, CWnd* pParent /*=NULL*/)
        : TransActiveDialog(guiAccess, CArchiveManagerDlg::IDD, pParent),
          m_isServerUp( true )
    {
        // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
        m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
            
        // If necessary this code can go in OnInitDialog before the call to TransActiveDialog::OnInitDialog()

        TA_Base_Bus::ResizingProperties properties;
        properties.canMaximise = false;
        properties.maxHeight = -1;
        properties.maxWidth = -1;
        properties.minWidth = -1;
        properties.minHeight = -1;
        setResizingProperties(properties);
    }
    
    void CArchiveManagerDlg::DoDataExchange(CDataExchange* pDX)
    {
        TransActiveDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CArchiveManagerDlg)
	DDX_Control(pDX, IDC_BUTTON_SCHEDULE, m_automaticButton);
	DDX_Control(pDX, IDC_BUTTON_RESTORE, m_restoreButton);
	DDX_Control(pDX, IDC_BUTTON_MANUAL, m_manualButton);
	DDX_Control(pDX, IDC_BUTTON_DISPLAY, m_deleteButton);
	//}}AFX_DATA_MAP
    }
    
    BEGIN_MESSAGE_MAP(CArchiveManagerDlg, TransActiveDialog)
        //{{AFX_MSG_MAP(CArchiveManagerDlg)
	ON_BN_CLICKED(IDC_BUTTON_SCHEDULE, OnButtonSchedule)
	ON_BN_CLICKED(IDC_BUTTON_MANUAL, OnButtonManual)
	ON_BN_CLICKED(IDC_BUTTON_RESTORE, OnButtonRestore)
	ON_BN_CLICKED(IDC_BUTTON_DISPLAY, OnButtonDisplay)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_SERVER_STATUS_CHANGE, OnServerStatusChange)
    ON_MESSAGE(WM_SESSIONID_CHANGE, OnSessionIdChange)
    END_MESSAGE_MAP()
        
    /////////////////////////////////////////////////////////////////////////////
    // CArchiveManagerDlg message handlers
    
    BOOL CArchiveManagerDlg::OnInitDialog()
    {
        TransActiveDialog::OnInitDialog();

        //	Set the icon for this dialog.  The framework does this automatically
        //	when the application's main window is not a dialog

        SetIcon(m_hIcon, TRUE);			// Set big icon
        SetIcon(m_hIcon, FALSE);		// Set small icon
		
		// liborm 1/07/04 - fix for TD defect #3505
		// application icon draws over text in the title bar, so refresh the
		// title text
		char titleText[256];
		this->GetWindowText( titleText, 256 );
		this->SetWindowText( titleText );

        enableButtons();
    
        TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);

        return TRUE;  // return TRUE  unless you set the focus to a control
    }
    
    
    void CArchiveManagerDlg::OnOK() 
    {
        //  Our OK button is displayed as Close
        TransActiveDialog::OnClose();
    }


    void CArchiveManagerDlg::OnButtonSchedule() 
    {
        std::string messageForUser( "" );
        TA_ArchiveManager::EMsgLevel messageLevel = TA_ArchiveManager::ERR;
        WriteArchiveHelper helper( getGenericGUICallbackObject() );
        helper.writeScheduledArchives( messageForUser, messageLevel );
        if( !messageForUser.empty() )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << messageForUser.c_str();
 
            if (messageLevel == TA_ArchiveManager::ERR)
            {
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_050126,CAPTION_ARCHIVE_MANAGER);
            }
            else if (messageLevel == TA_ArchiveManager::WARN)
            {
                UINT selectedButton = userMsg.showMsgBox(IDS_UW_050018,CAPTION_ARCHIVE_MANAGER);
            }
            else if (messageLevel == TA_ArchiveManager::INFO)
            {
                //UINT selectedButton = userMsg.showMsgBox(IDS_UI_050063);
            }
        }
    }


    void CArchiveManagerDlg::OnButtonManual() 
    {
        std::string messageForUser( "" );
        TA_ArchiveManager::EMsgLevel messageLevel = TA_ArchiveManager::ERR;
        WriteArchiveHelper helper( getGenericGUICallbackObject() );
        helper.createManualArchive( messageForUser, messageLevel );
        if( !messageForUser.empty() )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << messageForUser.c_str();

            if (messageLevel == TA_ArchiveManager::ERR)
            {
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_050127,CAPTION_ARCHIVE_MANAGER);
            }
            else if (messageLevel == TA_ArchiveManager::WARN)
            {
                UINT selectedButton = userMsg.showMsgBox(IDS_UW_050018,CAPTION_ARCHIVE_MANAGER);
            }
            else if (messageLevel == TA_ArchiveManager::INFO)
            {
                //UINT selectedButton = userMsg.showMsgBox(IDS_UI_050063);
            }
        }
    }


    void CArchiveManagerDlg::OnButtonRestore() 
    {
        std::string messageForUser( "" );
        TA_ArchiveManager::EMsgLevel messageLevel = TA_ArchiveManager::ERR;
        RestoreArchiveHelper helper( getGenericGUICallbackObject() );
        helper.restoreArchives( messageForUser, messageLevel );
        if( !messageForUser.empty() )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << messageForUser.c_str();
           
            if (messageLevel == TA_ArchiveManager::ERR)
            {
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_050128,CAPTION_ARCHIVE_MANAGER);
            }
            else if (messageLevel == TA_ArchiveManager::WARN)
            {
                UINT selectedButton = userMsg.showMsgBox(IDS_UW_050018,CAPTION_ARCHIVE_MANAGER);
            }
            else if (messageLevel == TA_ArchiveManager::INFO)
            {
                //UINT selectedButton = userMsg.showMsgBox(IDS_UI_050063);
            }
        }
    }


    void CArchiveManagerDlg::OnButtonDisplay() 
    {
        // KT 11 Mar 04: PW #3133. This method has to be changed because the 
        // DeleteArchiveDlg now does both the deletion of the data and the
        // modification of the deletion due date, so the message sending
        // requirements are different.

		// liborm - 26 Oct 04 - Fix for TD #3719
		// Check that the user has permission.
        try
        {
			if( !TA_ArchiveManager::isActionPermitted( TA_Base_Bus::aca_ARCHIVE_DELETE ) )
            {
                CString error;
                try
                {
                    std::string operatorName = TA_ArchiveManager::getOperatorNameFromSessionId();
                    std::string workstationName = TA_ArchiveManager::getWorkstationNameFromSessionId();

                    TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg << operatorName << workstationName;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_050109,CAPTION_ARCHIVE_MANAGER);
				}
                catch( TA_Base_Core::ArchiveException& e )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
					TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg << e.what();
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_050129,CAPTION_ARCHIVE_MANAGER);
                }
            }
			else
			{
                DeleteArchiveDlg dlg;
                dlg.DoModal();
			}
        }
        catch( TA_Base_Core::ArchiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
			TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << e.what();
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_050129,CAPTION_ARCHIVE_MANAGER);
        }
    }


    void CArchiveManagerDlg::setServerStatus( bool isServerUp )
    {
        m_isServerUp = isServerUp;
        PostMessage( WM_SERVER_STATUS_CHANGE, 0, 0 );
    }

    
    LRESULT CArchiveManagerDlg::OnServerStatusChange(WPARAM, LPARAM)
    {
	    enableButtons();
        if ( !m_isServerUp )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UW_050003,CAPTION_ARCHIVE_MANAGER);
        }
	    return NULL;	// This line must be included. Everything above is application specific
    }
    

    void CArchiveManagerDlg::onRunParamChange(const std::string& name, const std::string& value)
    {
        if ( 0 == name.compare(RPARAM_SESSIONID) )
        {
            PostMessage( WM_SESSIONID_CHANGE, 0, 0 );    
        }
    }   

    
    LRESULT CArchiveManagerDlg::OnSessionIdChange(WPARAM, LPARAM)
    {
	    enableButtons();
	    return NULL;	// This line must be included. Everything above is application specific
    }


    void CArchiveManagerDlg::enableButtons()
    {
        // Disable all the buttons if the server is down.
                
        if ( !m_isServerUp )
        {
            m_automaticButton.EnableWindow( FALSE );
            m_manualButton.EnableWindow( FALSE );
            m_restoreButton.EnableWindow( FALSE );
            m_deleteButton.EnableWindow( FALSE );
            return;
        }

        // Only enable the buttons if the operator has sufficient privileges.

        try
        {
            if( TA_ArchiveManager::isActionPermitted( TA_Base_Bus::aca_ARCHIVE_TRANSFER ) )
            {
                m_automaticButton.EnableWindow( TRUE );
            }
            else
            {
                m_automaticButton.EnableWindow( FALSE );
            }

            if( TA_ArchiveManager::isActionPermitted( TA_Base_Bus::aca_ARCHIVE_MANUAL ) )
            {
                m_manualButton.EnableWindow( TRUE );
            }
            else
            {
                m_manualButton.EnableWindow( FALSE );
            }

            if( TA_ArchiveManager::isActionPermitted( TA_Base_Bus::aca_ARCHIVE_RESTORE ) )
            {
                m_restoreButton.EnableWindow( TRUE );
            }
            else
            {
                m_restoreButton.EnableWindow( FALSE );
            }

            if( TA_ArchiveManager::isActionPermitted( TA_Base_Bus::aca_ARCHIVE_DELETE ) )
            {
                m_deleteButton.EnableWindow( TRUE );
            }
            else
            {
                m_deleteButton.EnableWindow( FALSE );
            }
        }
        catch( TA_Base_Core::ArchiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "ArchiveException", e.what() );
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << e.what();
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_050129,CAPTION_ARCHIVE_MANAGER);
        }
    }
	void CArchiveManagerDlg::OnClose() 
	{
		// TODO: Add your message handler code here and/or call default
		//TD18095, jianghp, to fix the performance of showing manager application
		TransActiveDialog::DestroyWindow();
	}
    
} // TA_Base_App


