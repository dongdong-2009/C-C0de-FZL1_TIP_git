#include "..\stdAfx.h"

#include "..\TA_MessageLibrary.h"
#include "..\UserMessages.h"
#include "..\..\Help\Resource.h"

#include "HelpDefines.h"
#include "HelpControlBar.h"


static LPCTSTR HELP_DLL = _T("TA_MessageLibraryHelp.dll");

//IMPLEMENT_DYNAMIC(HelpControlBar, cdxCDynamicBarDlg);

HelpControlBar::HelpControlBar(CWnd* pParent)
	:cdxCDynamicBarDlg(HelpControlBar::IDD, pParent)
{
	
}

HelpControlBar::~HelpControlBar(void)
{
	UINT uiTopic = 0;
	HelpTopic* pHelpTopic = NULL;
	POSITION pos = m_mapHelpTopics.GetStartPosition();
	while(pos)
	{
		m_mapHelpTopics.GetNextAssoc(pos, uiTopic, pHelpTopic);
		delete pHelpTopic;
	}
}

BEGIN_MESSAGE_MAP(HelpControlBar, cdxCDynamicBarDlg)
	//{{AFX_MSG_MAP(HelpControlBar)
	ON_MESSAGE(UM_GOTO_HELP_TOPIC, OnHelpTopic)
	ON_COMMAND(IDM_HELP_BACK,OnNavigateBack)
	ON_COMMAND(IDM_HELP_FORWARD, OnNavigateForward)
	ON_COMMAND(IDM_HELP_STOP, OnNavigateStop)
	ON_UPDATE_COMMAND_UI(IDM_HELP_STOP, OnUpdateNavigateStop)
	ON_COMMAND(IDM_HELP_REFRESH, OnRefresh)
	ON_COMMAND(IDM_HELP_HOME, OnHome)
	ON_COMMAND(IDM_HELP_PRINT, OnPrint)
	ON_CBN_SELCHANGE(IDC_COMBO_HELP_TOPICS, OnChangeHelpTopic)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void HelpControlBar::DoDataExchange(CDataExchange* pDX)
{
	cdxCDynamicBarDlg::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_HELP_TOPICS, m_comboTopics);
}


BOOL HelpControlBar::OnInitDialog() 
{
	UINT uiTopic = (GetApp()->m_bInternalMode) ? IDR_HTML_INTERNAL_START : IDR_HTML_EXTERNAL_START;

	cdxCDynamicBarDlg::OnInitDialog();

	m_browserCtrl.CreateFromControl(this, IDC_HELPCTRLBAR_BROWSER);
	
	CreateToolBar();

	AddSzControl(m_hWndToolbar,mdNone,mdResize);
	AddSzControl(m_browserCtrl, mdResize, mdResize);
	AddSzControl(m_comboTopics, mdResize, mdRepos);

	PopulateTopicCombo();
	m_browserCtrl.LoadFromResource(HELP_DLL, uiTopic);
	m_comboTopics.SetCurSel(FindTopicIndex(uiTopic));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT HelpControlBar::OnHelpTopic(WPARAM wParam, LPARAM lParam)
{
	ASSERT(wParam != NULL);

	switch(wParam)
	{
		case IDH_EXTERNAL_START:
		case IDH_INTERNAL_START:
		case IDH_MSG_STRING_ERROR:
			{
				m_comboTopics.SetCurSel(FindTopicIndex(IDR_HTML_STRING_ERRORS));
				return TRUE;
			}break;
		default:
			return FALSE;
	}
}

void  HelpControlBar::CreateToolBar()
{
	// Toolbar buttons
	TBBUTTON tbButtonsSTD [] = 
	{
		{0,	IDM_HELP_BACK,		TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, IDM_HELP_BACK},
		{1,	IDM_HELP_FORWARD,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, IDM_HELP_FORWARD},
		{2,	IDM_HELP_STOP,		TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, IDM_HELP_STOP},
		{3,	IDM_HELP_REFRESH,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, IDM_HELP_REFRESH},
		{4,	IDM_HELP_HOME,		TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, IDM_HELP_HOME},
		{5,	IDM_HELP_PRINT,		TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, IDM_HELP_PRINT},
	};  

	m_hWndToolbar = CreateToolbarEx(m_hWnd, WS_CHILD|WS_VISIBLE|TBSTYLE_TOOLTIPS|TBSTYLE_TRANSPARENT|TBSTYLE_FLAT, 
		IDR_HELP_TOOLBAR, 6, AfxGetResourceHandle(), IDB_HELP_TOOLBAR, (LPCTBBUTTON)&tbButtonsSTD, 6, 
		21, 20, 21, 20, sizeof (TBBUTTON));
}

void HelpControlBar::OnNavigateBack()
{
	m_browserCtrl.GoBack();
}

void HelpControlBar::OnNavigateForward()
{
	m_browserCtrl.GoForward();
}

void HelpControlBar::OnNavigateStop()
{
	m_browserCtrl.Stop();
}

void HelpControlBar::OnUpdateNavigateStop(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((m_browserCtrl.GetBusy()));
}

void HelpControlBar::OnRefresh()
{
	m_browserCtrl.Refresh();
}

void HelpControlBar::OnHome()
{
	m_browserCtrl.LoadFromResource(HELP_DLL, 
		(GetApp()->m_bInternalMode) ? IDR_HTML_INTERNAL_START : IDR_HTML_EXTERNAL_START);
}

void HelpControlBar::OnPrint()
{
	m_browserCtrl.Print();
}

void HelpControlBar::PopulateTopicCombo()
{
	bool bInternalHelp = (GetApp()->m_bInternalMode);
	CString sTopic;

	sTopic.LoadString(IDS_HELPTOPIC_START);
	int iIndex = m_comboTopics.AddString(sTopic);
	m_comboTopics.SetItemData(iIndex, bInternalHelp ? IDR_HTML_INTERNAL_START : IDR_HTML_EXTERNAL_START);

	sTopic.LoadString(IDS_HELPTOPIC_USING);
	iIndex = m_comboTopics.AddString(sTopic);
	m_comboTopics.SetItemData(iIndex, bInternalHelp ? IDR_HTML_INTERNAL_USING : IDR_HTML_EXTERNAL_USING);

	sTopic.LoadString(IDS_HELPTOPIC_TEXT_ERRORS);
	iIndex = m_comboTopics.AddString(sTopic);
	m_comboTopics.SetItemData(iIndex, IDR_HTML_STRING_ERRORS);

	sTopic.LoadString(IDS_HELPTOPIC_MESSAGE_PROPERTIES);
	iIndex = m_comboTopics.AddString(sTopic);
	m_comboTopics.SetItemData(iIndex, IDR_HTML_MESSAGE_PROPERTIES);

	sTopic.LoadString(IDS_HELPTOPIC_MESSAGE_FORMAT);
	iIndex = m_comboTopics.AddString(sTopic);
	m_comboTopics.SetItemData(iIndex, IDR_HTML_MESSAGE_FMT);
	
	if(bInternalHelp)
	{
		sTopic.LoadString(IDS_HELPTOPIC_ADDING_A_MESSAGE);
		iIndex = m_comboTopics.AddString(sTopic);
		m_comboTopics.SetItemData(iIndex, IDR_HTML_ADD_MESSAGE);

		sTopic.LoadString(IDS_HELPTOPIC_REMOVING_MESSAGES);
		iIndex = m_comboTopics.AddString(sTopic);
		m_comboTopics.SetItemData(iIndex, IDR_HTML_REMOVE_MESSAGE);

		sTopic.LoadString(IDS_HELPTOPIC_NEW_LANGUAGE);
		iIndex = m_comboTopics.AddString(sTopic);
		m_comboTopics.SetItemData(iIndex, IDR_HTML_ADD_NEW_LANGUAGE);

		sTopic.LoadString(IDS_HELPTOPIC_COMMAND_LINE_PARAMS);
		iIndex = m_comboTopics.AddString(sTopic);
		m_comboTopics.SetItemData(iIndex, IDR_HTML_COMMAND_LINE);

		sTopic.LoadString(IDS_HELPTOPIC_PARAM_EDITOR);
		iIndex = m_comboTopics.AddString(sTopic);
		m_comboTopics.SetItemData(iIndex, IDR_HTML_PARAMETER_INPUT);

		sTopic.LoadString(IDS_HELPTOPIC_BUILD_INTEGRATION);
		iIndex = m_comboTopics.AddString(sTopic);
		m_comboTopics.SetItemData(iIndex, IDR_HTML_BUILD_INTEGRATION);

		sTopic.LoadString(IDS_HELPTOPIC_EXPORT_TO_RC);
		iIndex = m_comboTopics.AddString(sTopic);
		m_comboTopics.SetItemData(iIndex, IDR_HTML_EXPORT_TO_RC);
	}

}

void HelpControlBar::OnChangeHelpTopic() 
{
	int iIndex = m_comboTopics.GetCurSel();
	if(iIndex >= 0)
		m_browserCtrl.LoadFromResource(HELP_DLL, m_comboTopics.GetItemData(iIndex));
}

int HelpControlBar::FindTopicIndex(UINT uiTopic)
{
	for(int i = 0; i < m_comboTopics.GetCount(); i++)
	{
		if(m_comboTopics.GetItemData(i) == uiTopic)
			return i;
	}

	return -1;
}