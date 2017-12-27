// AddParamDlg.cpp : implementation file
//

#include "..\stdafx.h"

#include "..\TA_messagelibrary.h"
#include "..\MessageGrid\MessageGridDoc.h"
#include "..\GlobalFunctions.h"

#include "AddParamDlg.h"
#include "..\ParameterInput\ParameterInputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static const int COLUMN_ENTRY_PARAM = 0;
static const int COLUMN_ENTRY_TYPE = 1;
static const int COLUMN_ENTRY_DESC = 2;

/////////////////////////////////////////////////////////////////////////////
// AddParamDlg dialog


AddParamDlg::AddParamDlg(CWnd* pParent, CStringArray* pMsgParams)
	: cdxCDynamicDialog(AddParamDlg::IDD, pParent), m_pMsgParams(NULL)
{
	//{{AFX_DATA_INIT(AddParamDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pMsgParams = pMsgParams;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void AddParamDlg::DoDataExchange(CDataExchange* pDX)
{
	cdxCDynamicDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AddParamDlg)
		DDX_Control(pDX, IDC_LIST_ADDPARAMDLG_PARAMS, m_listParams);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AddParamDlg, cdxCDynamicDialog)
	//{{AFX_MSG_MAP(AddParamDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ADDPARAMDLG_PARAMS, OnListDoubleClick )
    ON_BN_CLICKED(IDC_BUTTON1, OnEditParameters)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AddParamDlg message handlers

void AddParamDlg::OnEditParameters()
{
    ParameterInputDlg dlg;
    dlg.DoModal();

    // refresh the list
    PopulateListCtrl();

}

BOOL AddParamDlg::OnInitDialog() 
{
	cdxCDynamicDialog::OnInitDialog();

	SetIcon(m_hIcon, FALSE);

	AddSzControl(m_listParams, mdResize, mdResize);
	AddSzControl(IDOK, mdRepos, mdRepos);
	AddSzControl(IDCANCEL, mdRepos, mdRepos);
    AddSzControl(IDC_BUTTON1,mdRepos,mdRepos);
	
	if(InitialiseListCtrl())
		PopulateListCtrl();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AddParamDlg::OnOK() 
{	
	if(SaveSelection())
		cdxCDynamicDialog::OnOK();
}

bool AddParamDlg::InitialiseListCtrl()
{
	m_listParams.InsertColumn(COLUMN_ENTRY_PARAM, _T("Parameter"));
	m_listParams.InsertColumn(COLUMN_ENTRY_TYPE, _T("Type"));
	m_listParams.InsertColumn(COLUMN_ENTRY_DESC, _T("Description"));

	m_listParams.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	return true;
}

bool AddParamDlg::PopulateListCtrl()
{
    m_listParams.DeleteAllItems();

//	CFrameWnd* pMainFrame = reinterpret_cast<CFrameWnd*>(AfxGetMainWnd());
	MessageGridDoc* pDoc = reinterpret_cast<MessageGridDoc*>(GetActiveDoc());
	CString sValue;
	_variant_t vtValue;
	int iIndex = 0;

	MSXML2::IXMLDOMNodePtr spParam = NULL;
	MSXML2::IXMLDOMNodePtr spChild = NULL;
	MSXML2::IXMLDOMNodeListPtr spParams = pDoc->m_messageXMLObj.m_spParamRoot->selectNodes(XMLNODE_PARAM);
	for(long l = 0, lCount = spParams->Getlength(); l < lCount; l++)
	{
		spParam = spParams->Getitem(l);
		spChild = spParam->selectSingleNode(XMLNODE_PARAM_NAME);
		vtValue = spChild->GetnodeTypedValue();
		sValue = vtValue.bstrVal;
		
		if(CheckIfParamInMessage(sValue))
			continue;
		
		iIndex = m_listParams.InsertItem(++iIndex, sValue);

		spChild = spParam->selectSingleNode(_T("@") XMLNODE_PARAM_TYPE);
		vtValue.Clear();
		vtValue = spChild->GetnodeTypedValue();
		sValue = vtValue.bstrVal;
		m_listParams.SetItemText(iIndex, COLUMN_ENTRY_TYPE, sValue);

		spChild = spParam->selectSingleNode(XMLNODE_PARAM_DESC);
		vtValue.Clear();
		vtValue = spChild->GetnodeTypedValue();
		sValue = vtValue.bstrVal;
		m_listParams.SetItemText(iIndex, COLUMN_ENTRY_DESC, sValue);
	}

	m_listParams.SetColumnWidth(COLUMN_ENTRY_PARAM, LVSCW_AUTOSIZE);
	m_listParams.SetColumnWidth(COLUMN_ENTRY_TYPE, LVSCW_AUTOSIZE);
	m_listParams.SetColumnWidth(COLUMN_ENTRY_DESC, LVSCW_AUTOSIZE);

	return true;
}

bool AddParamDlg::CheckIfParamInMessage(LPCTSTR pszParam)
{
	for(int i = 0; i < m_pMsgParams->GetSize(); i++)
	{
		if(m_pMsgParams->GetAt(i) == pszParam)
			return true;
	}

	return false;
}

bool AddParamDlg::SaveSelection()
{	
	int iIndex = 0;
	POSITION pos = m_listParams.GetFirstSelectedItemPosition();
	while(pos)
	{
		iIndex = m_listParams.GetNextSelectedItem(pos);

		m_pMsgParams->Add(m_listParams.GetItemText(iIndex, COLUMN_ENTRY_PARAM));
	}

	return true;
}

void AddParamDlg::OnListDoubleClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOK();
	
	*pResult = 0;
}
