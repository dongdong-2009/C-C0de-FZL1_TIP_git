// MessageExtractorDlg.cpp : implementation file
//

#include "..\stdafx.h"
#include "..\Resource.h"
#include "MsgExtractor.h"
#include "MessageExtractorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// MessageExtractorDlg dialog

MessageExtractorDlg::MessageExtractorDlg(CWnd* pParent /*=NULL*/)
	: CMenuDialog(MessageExtractorDlg::IDD, pParent)
{
}

void MessageExtractorDlg::DoDataExchange(CDataExchange* pDX)
{
	CMenuDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_ED_SOURCE, m_msgExtractor.m_sSourcePath);
//	DDX_Text(pDX, IDC_ED_TARGET, m_msgExtractor.m_sTargetPath);
}

BEGIN_MESSAGE_MAP(MessageExtractorDlg, CMenuDialog)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_SOURCE_BROWSE, OnBnClickedBtnSourceBrowse)
END_MESSAGE_MAP()


// MessageExtractorDlg message handlers

BOOL MessageExtractorDlg::OnInitDialog()
{
	CMenuDialog::OnInitDialog();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void MessageExtractorDlg::OnBnClickedOk()
{
	if(!UpdateData())
		return;

	CWaitCursor wait;
	if(m_msgExtractor.ExtractMsgsFromExcel(GetDlgItem(IDC_TXT_PROGRESS)))
	{
		AfxMessageBox(IDS_APP_EXCEL_PROGRESS_POST_COMMENTS, MB_ICONINFORMATION);

	}

	CMenuDialog::OnOK();
}

void MessageExtractorDlg::OnBnClickedBtnSourceBrowse()
{
	CFileDialog fileOpenDlg(TRUE, _T(".xls"), NULL, OFN_FILEMUSTEXIST|OFN_NOTESTFILECREATE, 
								_T("Worksheet Files (*.xls)|*.xls||"));

	if(fileOpenDlg.DoModal() == IDOK)
	{
		m_msgExtractor.m_sSourcePath = fileOpenDlg.GetPathName();
		UpdateData(FALSE);
	}
}


