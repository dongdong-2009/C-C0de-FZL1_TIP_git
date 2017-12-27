// C:\Dev\TA_MessageLibrary\MessageExtraction\ExportToRCDlg.cpp : implementation file
//

#include "..\stdafx.h"

#include "..\TA_MessageLibrary.h"
#include "..\MainFrm.h"
#include "..\MessageGrid\MessageGridDoc.h"
#include "..\GlobalFunctions.h"

#include "ExportToRCDlg.h"


// ExportToRCDlg dialog

IMPLEMENT_DYNAMIC(ExportToRCDlg, CMenuDialog)
ExportToRCDlg::ExportToRCDlg(CWnd* pParent /*=NULL*/)
	: CMenuDialog(ExportToRCDlg::IDD, pParent)
{
}

ExportToRCDlg::~ExportToRCDlg()
{
}

void ExportToRCDlg::DoDataExchange(CDataExchange* pDX)
{
	CMenuDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_ED_EXPORTTORCDLG_TARGET, m_msgExtractor.m_sTargetRCPath);
	DDX_Control(pDX, IDC_COMBO_EXPORTTORCDLG_SEL_LANG, m_comboLanguage);
	
	int iIncludeENA = 0;
	if(pDX->m_bSaveAndValidate)
	{
		DDX_Check(pDX, IDC_CHK_EXPORTTORCDLG_INC_ENA, iIncludeENA);
		m_msgExtractor.m_bIncludeENA = (iIncludeENA == 1);
	}
	else
	{
		iIncludeENA = (m_msgExtractor.m_bIncludeENA) ? 1 : 0;
		DDX_Check(pDX, IDC_CHK_EXPORTTORCDLG_INC_ENA, iIncludeENA);
	}
}


BEGIN_MESSAGE_MAP(ExportToRCDlg, CMenuDialog)
	ON_BN_CLICKED(IDC_BTN_EXPORTTORCDLG_TARGET, OnBnClickedBtnExporttorcdlgTarget)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// ExportToRCDlg message handlers

void ExportToRCDlg::OnBnClickedBtnExporttorcdlgTarget()
{
	CFileDialog fileOpenDlg(FALSE, _T(".rc"), NULL, OFN_SHAREAWARE, _T("Resource Files (*.rc)|*.rc||"));

	if(fileOpenDlg.DoModal() == IDOK)
	{
		m_msgExtractor.m_sTargetRCPath = fileOpenDlg.GetPathName();

		UpdateData(FALSE);
	}
}

void ExportToRCDlg::OnBnClickedOk()
{	
	UpdateData();

	int iIndex = m_comboLanguage.GetCurSel();
	if(iIndex >= 0)
		m_msgExtractor.m_lcidLanguageToExport = m_comboLanguage.GetItemData(iIndex);
	else // only extract the english (Australian) strings
	{
		m_msgExtractor.m_lcidLanguageToExport = 3081;
		m_msgExtractor.m_bIncludeENA = false;
	}
		
	CWaitCursor wait;
	if(m_msgExtractor.ExportMessagesToRC())
		OnOK();
}

bool ExportToRCDlg::InitialiseComboBox()
{
//	MainFrame* pMainFrame = reinterpret_cast<MainFrame*>(AfxGetMainWnd());
	MessageGridDoc* pDoc = reinterpret_cast<MessageGridDoc*>(GetActiveDoc());
	MessageLibraryApp* pApp = GetApp();
	CString sLocaleName;
	int iIndex = 0;

	for(int i = 0, iCount = pDoc->m_messageXMLObj.m_rgSupportedLocales.GetSize(); i < iCount; i++)
	{
		// Don't include english australian
		if(pDoc->m_messageXMLObj.m_rgSupportedLocales[i] == 3081)
			continue;

		if(!pApp->m_mapAvailableLocales.Lookup(pDoc->m_messageXMLObj.m_rgSupportedLocales[i], sLocaleName))
			return false;

		iIndex = m_comboLanguage.AddString(sLocaleName);
		m_comboLanguage.SetItemData(iIndex, pDoc->m_messageXMLObj.m_rgSupportedLocales[i]);
	}

	if(m_comboLanguage.GetCount() == 0)
	{
		if(!pApp->m_mapAvailableLocales.Lookup(3081, sLocaleName))
			return false;

		iIndex = m_comboLanguage.AddString(sLocaleName);
		m_comboLanguage.SetItemData(iIndex, 3081);
		m_comboLanguage.SetCurSel(iIndex);
		m_comboLanguage.EnableWindow(FALSE);
		CButton* pCheck = (CButton*)GetDlgItem(IDC_CHK_EXPORTTORCDLG_INC_ENA);
		pCheck->SetCheck(0);
		pCheck->EnableWindow(FALSE);
	}

	return true;
}

BOOL ExportToRCDlg::OnInitDialog()
{
	CMenuDialog::OnInitDialog();

	InitialiseComboBox();



	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
