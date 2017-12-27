// configdlg.cpp : implementation file
//

#include "stdafx.h"
#include "AlarmStoreHarness.h"
#include "configdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg dialog


CConfigDlg::CConfigDlg(UINT severity, UINT location, CString asset, CString description, CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDlg)
	m_severityValue = severity;
	m_locationValue = location;
	m_descValue = _T(description);
	m_assetValue = _T(asset);
	//}}AFX_DATA_INIT
}


void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDlg)
	DDX_Text(pDX, IDC_EDIT_SEVERITY, m_severityValue);
	DDX_Text(pDX, IDC_EDIT_LOCATION, m_locationValue);
	DDX_Text(pDX, IDC_EDIT_DESCRIPTION, m_descValue);
	DDX_Text(pDX, IDC_EDIT_ASSET, m_assetValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
	//{{AFX_MSG_MAP(CConfigDlg)
	ON_BN_CLICKED(IDC_BTN_OK, OnBtnOk)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnBtnCancel)
	ON_BN_CLICKED(IDC_CHK_1, OnChk1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg message handlers

void CConfigDlg::OnBtnOk() 
{
	// TODO: Add your control notification handler code here
	
	CDialog::OnOK();
}

void CConfigDlg::OnBtnCancel() 
{
	// TODO: Add your control notification handler code here

	CDialog::OnCancel();
}

void CConfigDlg::OnChk1() 
{
	// TODO: Add your control notification handler code here
}
