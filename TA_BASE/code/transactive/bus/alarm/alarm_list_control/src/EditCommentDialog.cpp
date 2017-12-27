/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/EditCommentDialog.cpp $
  * @author:  Ben Robbins
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class holds the comment dialog that allows the user to edit alarm comments
  */


#include "bus/alarm/alarm_list_control/src/stdafx.h"
#include "bus/alarm/alarm_list_control/src/EditCommentDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_Bus
{
    const long EditCommentDialog::MAXIMUM_COMMENT_LENGTH = 1024;


    EditCommentDialog::EditCommentDialog(CWnd* pParent /*=NULL*/)
	    : CDialog(EditCommentDialog::IDD, pParent)
    {
	    //{{AFX_DATA_INIT(EditCommentDialog)
	    m_comment = _T("");
	    //}}AFX_DATA_INIT
    }


    void EditCommentDialog::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(EditCommentDialog)
	    DDX_Control(pDX, IDC_EDIT_ALARM_COMMENT, m_commentControl);
	    DDX_Text(pDX, IDC_EDIT_ALARM_COMMENT, m_comment);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(EditCommentDialog, CDialog)
	//{{AFX_MSG_MAP(EditCommentDialog)
	ON_BN_CLICKED(IDOK, onOK)
	ON_BN_CLICKED(IDCANCEL, onCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // EditCommentDialog message handlers

    BOOL EditCommentDialog::OnInitDialog() 
    {
	    CDialog::OnInitDialog();
        m_commentControl.SetLimitText(MAXIMUM_COMMENT_LENGTH);
        UpdateData(FALSE);
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }

    void EditCommentDialog::onOK()
    {
	    UpdateData(TRUE);
        CDialog::OnOK();
    }

    void EditCommentDialog::onCancel()
    {
        CDialog::OnCancel();
    }

	void EditCommentDialog::setEditComment(std::string comment)
	{
		m_comment=comment.c_str();
	}
}

