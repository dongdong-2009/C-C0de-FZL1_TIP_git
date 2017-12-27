/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/SelectDeletionDateDlg.cpp $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Allows the operator to change the deletion date for a set of restored data.
  * Added for PW #3133.
  */

#include "app/data_management/archive_manager/src/StdAfx.h"
#include "app/data_management/archive_manager/src/SelectDeletionDateDlg.h"
#include "app/data_management/archive_manager/src/BasicHelper.h"
#include "app/data_management/archive_manager/src/ChineseInfo.h"
#include "core/message/types/ArchiveAudit_MessageTypes.h"

#include "bus/generic_gui/src/TransactiveMessage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    // The columns in the list control.
 
    enum EColumns
    {
        DATA_DATE_COL           = 1,
        DELETION_DUE_DATE_COL   = 2
    };

   
    //
    // Tomorrow's date
    //

    const COleDateTime TOMORROWS_DATE( COleDateTime::GetCurrentTime() + COleDateTimeSpan( 1,0,0,0 ) );
}

namespace TA_Base_App
{
    SelectDeletionDateDlg::SelectDeletionDateDlg(CWnd* pParent /*=NULL*/)
	    : CDialog(SelectDeletionDateDlg::IDD, pParent)
    {
        // Do nothing.
    }


    time_t SelectDeletionDateDlg::getDeletionDueDate()
    {
        return TA_ArchiveManager::convertCOleDateTimeToTimeT( m_date );
    }


    void SelectDeletionDateDlg::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(SelectDeletionDateDlg)
	    DDX_DateTimeCtrl(pDX, IDC_DATEPICKER, m_date);
	    //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(SelectDeletionDateDlg, CDialog)
	    //{{AFX_MSG_MAP(SelectDeletionDateDlg)
		//}}AFX_MSG_MAP
		ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATEPICKER, OnDateChanged)
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // SelectDeletionDateDlg message handlers

    BOOL SelectDeletionDateDlg::OnInitDialog() 
    {
	    CDialog::OnInitDialog();
	    
        // Give this dialog its parent's icons.

        SetIcon( GetParent()->GetIcon( true ), TRUE );			// Set big icon
        SetIcon( GetParent()->GetIcon( false ), FALSE );		// Set small icon
            
        // Select tomorrow's date.

        m_date = TOMORROWS_DATE;
        UpdateData( FALSE );

	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void SelectDeletionDateDlg::OnOK() 
    {
		// Send an audit message.

		TA_ArchiveManager::sendAuditMessage( TA_Base_Core::ArchiveAudit::ChangeDeletionDateRequest ); 

	    UpdateData( TRUE );

        if ( m_date < TOMORROWS_DATE )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UI_050059,CAPTION_ARCHIVE_MANAGER);

            return;
        }
	    CDialog::OnOK();
    }

	// liborm 05/07/04 fix for TD defect #3536
	void SelectDeletionDateDlg::OnDateChanged(NMHDR *,LRESULT *)
	{
		UpdateData( TRUE );

		if ( m_date < TOMORROWS_DATE )
		{
			// Set Cancel button to be default
			this->SetDefID( IDCANCEL );
		}
		else
		{
			// Set OK button to be default
			this->SetDefID( IDOK );
		}
	}

} // TA_Base_App
