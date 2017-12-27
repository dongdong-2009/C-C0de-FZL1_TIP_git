/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/DataNodeController/src/NotesDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// NotesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DataNodeController.h"
#include "NotesDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNotesDialog dialog


CNotesDialog::CNotesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CNotesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNotesDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CNotesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNotesDialog)
	DDX_Control(pDX, IDC_NOTES_EDIT, m_NotesEdit);
	DDX_Control(pDX, IDC_UPDATE_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_CLEAR_BUTTON, m_ClearButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNotesDialog, CDialog)
	//{{AFX_MSG_MAP(CNotesDialog)
	ON_BN_CLICKED(IDC_CLEAR_BUTTON, OnClearButton)
	ON_BN_CLICKED(IDC_UPDATE_BUTTON, OnUpdateButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CNotesDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_NotesEdit.SetWindowText("The fan is a little noisy when it first starts, but it Ok after a few minutes when it warms up");
	
	return TRUE;  	              
}

void CNotesDialog::OnClearButton() 
{
	m_NotesEdit.SetWindowText("");
	
}

void CNotesDialog::OnUpdateButton() 
{
	m_NotesEdit.SetWindowText("The fan is a little noisy when it first starts, but it Ok after a few minutes when it warms up");
}
