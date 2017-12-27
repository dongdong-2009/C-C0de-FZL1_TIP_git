// DutyChangeNoteDlg.cpp : implementation file
// For TD19075

#include "stdafx.h"
#include "controlstation.h"
#include "DutyChangeNoteDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DutyChangeNoteDlg dialog


DutyChangeNoteDlg::DutyChangeNoteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(DutyChangeNoteDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(DutyChangeNoteDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void DutyChangeNoteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DutyChangeNoteDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DutyChangeNoteDlg, CDialog)
	//{{AFX_MSG_MAP(DutyChangeNoteDlg)
	ON_BN_CLICKED(IDB_DETAIL, OnDetail)
	ON_BN_CLICKED(IDB_CLOSE, OnClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DutyChangeNoteDlg message handlers

//DEL void DutyChangeNoteDlg::OnDetail() 
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL 	
//DEL }

//DEL void DutyChangeNoteDlg::OnClose() 
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL 	
//DEL }

void DutyChangeNoteDlg::OnDetail() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}

void DutyChangeNoteDlg::OnClose() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}
