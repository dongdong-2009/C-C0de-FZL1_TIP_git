// MsgParamsPage.cpp : implementation file
//

#include "..\stdafx.h"

#include "..\TA_MessageLibrary.h"

#include "AddParamDlg.h"
#include "MsgParamsPage.h"
#include "MessagePropertySheet.h"


// MsgParamsPage dialog

IMPLEMENT_DYNAMIC(MsgParamsPage, MsgPropertyPage)

MsgParamsPage::MsgParamsPage()
	: MsgPropertyPage(MsgParamsPage::IDD)
{
}

MsgParamsPage::~MsgParamsPage()
{
}

void MsgParamsPage::DoDataExchange(CDataExchange* pDX)
{
	MsgPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_MSGPARAMSPAGE_PARAMS, m_listParams);
}


BEGIN_MESSAGE_MAP(MsgParamsPage, MsgPropertyPage)
	ON_BN_CLICKED(IDC_BTN_MSGPARAMSPAGE_ADD, OnAddParam)
	ON_BN_CLICKED(IDC_BTN_MSGPARAMSPAGE_REMOVE, OnRemoveParam)
	ON_BN_CLICKED(IDC_BTN_MSGPARAMSPAGE_MOVE_DOWN, OnMoveUp)
	ON_BN_CLICKED(IDC_BTN_MSGPARAMSPAGE_MOVE_UP, OnMoveDown)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_MSGPARAMSPAGE_PARAMS, OnParamListItemChange)
END_MESSAGE_MAP()


// MsgParamsPage message handlers

BOOL MsgParamsPage::OnInitDialog()
{
	MsgPropertyPage::OnInitDialog();

	AddSzControl(m_listParams, mdResize, mdResize);
	AddSzControl(IDC_GRP_MSGPARAMSPAGE, mdResize, mdResize);
	AddSzControl(IDC_BTN_MSGPARAMSPAGE_MOVE_UP, mdRepos, mdNone);
	AddSzControl(IDC_BTN_MSGPARAMSPAGE_MOVE_DOWN, mdRepos, mdNone);
	AddSzControl(IDC_BTN_MSGPARAMSPAGE_ADD, mdRepos, mdRepos);
	AddSzControl(IDC_BTN_MSGPARAMSPAGE_REMOVE, mdRepos, mdRepos);

	if(InitialiseParameterList())
		PopulateParameterList();

	UpdateButtonState();

	if(!GetApp()->m_bInternalMode)
		DisableControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool MsgParamsPage::InitialiseParameterList()
{
	m_listParams.InsertColumn(0, _T(""));
	m_listParams.InsertColumn(1, _T(""));

	m_listParams.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	return true;
}

bool MsgParamsPage::PopulateParameterList()
{
	MessagePropertySheet* pSheet = (MessagePropertySheet*)GetParent();
	CString sTemp;

	for(int i = 0; i < pSheet->m_pMessage->m_rgsParameters.GetSize(); i++)
	{
		sTemp.Format(_T("{%d}"), i);
		m_listParams.InsertItem(i, sTemp);
		m_listParams.SetItemText(i, 1, pSheet->m_pMessage->m_rgsParameters[i]);
	}

	m_listParams.SetColumnWidth(0, LVSCW_AUTOSIZE);
	m_listParams.SetColumnWidth(1, LVSCW_AUTOSIZE);
	return true;
}

void MsgParamsPage::OnAddParam()
{
	MessagePropertySheet* pSheet = (MessagePropertySheet*)GetParent();

	AddParamDlg dlg(this, &(pSheet->m_pMessage->m_rgsParameters));
	if(dlg.DoModal() == IDOK)
	{
		m_listParams.DeleteAllItems();
		PopulateParameterList();
		m_listParams.SetItemState(m_listParams.GetItemCount() - 1, LVIS_SELECTED, LVIS_SELECTED);
		m_listParams.RedrawWindow();
	}
}

void MsgParamsPage::OnRemoveParam()
{
	MessagePropertySheet* pSheet = (MessagePropertySheet*)GetParent();
	int iIndex = 0, iFirstItem = -1;
	POSITION pos = m_listParams.GetFirstSelectedItemPosition();
	while(pos)
	{
		iIndex = m_listParams.GetNextSelectedItem(pos);
		if(iFirstItem < 0)
			iFirstItem = iIndex;

		if(m_listParams.DeleteItem(iIndex))
			pSheet->m_pMessage->m_rgsParameters.RemoveAt(iIndex);
	}

	if(iFirstItem > 0)
		m_listParams.SetItemState(--iFirstItem, LVIS_SELECTED, LVIS_SELECTED);
}

void MsgParamsPage::OnMoveUp()
{
	MessagePropertySheet* pSheet = (MessagePropertySheet*)GetParent();
	int iIndex = 0;
	POSITION pos = m_listParams.GetFirstSelectedItemPosition();
	if(pos)
	{
		iIndex = m_listParams.GetNextSelectedItem(pos);

		CString sTemp = pSheet->m_pMessage->m_rgsParameters[iIndex];
		pSheet->m_pMessage->m_rgsParameters.RemoveAt(iIndex);
		pSheet->m_pMessage->m_rgsParameters.InsertAt(++iIndex, sTemp);

		m_listParams.DeleteAllItems();
		PopulateParameterList();		
		
		m_listParams.SetItemState(iIndex, LVIS_SELECTED, LVIS_SELECTED);
	}
}

void MsgParamsPage::OnMoveDown()
{
	MessagePropertySheet* pSheet = (MessagePropertySheet*)GetParent();
	int iIndex = 0;
	POSITION pos = m_listParams.GetFirstSelectedItemPosition();
	if(pos)
	{
		iIndex = m_listParams.GetNextSelectedItem(pos);
		
		CString sTemp = pSheet->m_pMessage->m_rgsParameters[iIndex];
		pSheet->m_pMessage->m_rgsParameters.RemoveAt(iIndex);
		pSheet->m_pMessage->m_rgsParameters.InsertAt(--iIndex, sTemp);

		m_listParams.DeleteAllItems();
		PopulateParameterList();		
		
		m_listParams.SetItemState(iIndex, LVIS_SELECTED, LVIS_SELECTED);
	}
}

void MsgParamsPage::OnParamListItemChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if((pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVIS_SELECTED)) 
		UpdateButtonState();
	
	*pResult = 0;
}

void MsgParamsPage::UpdateButtonState()
{
	bool bEnableUp = true, bEnableDown = true, bEnableRemove = true;

	if(!GetApp()->m_bInternalMode)
		return;

	POSITION pos = m_listParams.GetFirstSelectedItemPosition();
	if(pos)
	{
		if(m_listParams.GetSelectedCount() > 1)
		{
			bEnableDown = false;
			bEnableUp = false;
		}
		else
		{
			int iIndex = m_listParams.GetNextSelectedItem(pos);
			if(iIndex == m_listParams.GetItemCount() - 1) //last item selected
				bEnableDown = false;
			
			if(iIndex == 0) // first item selected
				bEnableUp = false;
		}
	}
	else
	{
		bEnableUp = false;
		bEnableDown = false;
		bEnableRemove = false;
	}

	GetDlgItem(IDC_BTN_MSGPARAMSPAGE_MOVE_UP)->EnableWindow(bEnableUp);
	GetDlgItem(IDC_BTN_MSGPARAMSPAGE_MOVE_DOWN)->EnableWindow(bEnableDown);
	GetDlgItem(IDC_BTN_MSGPARAMSPAGE_REMOVE)->EnableWindow(bEnableRemove);
}

void MsgParamsPage::DisableControls()
{
	GetDlgItem(IDC_LIST_MSGPARAMSPAGE_PARAMS)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_MSGPARAMSPAGE_MOVE_UP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_MSGPARAMSPAGE_MOVE_DOWN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_MSGPARAMSPAGE_ADD)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_MSGPARAMSPAGE_REMOVE)->EnableWindow(FALSE);
}