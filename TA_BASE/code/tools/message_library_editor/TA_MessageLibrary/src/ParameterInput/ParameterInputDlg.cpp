// ParameterInputDlg.cpp : implementation file
//

#include "..\stdafx.h"

#include "..\TA_MessageLibrary.h"
#include "..\MessageGrid\MessageGridDoc.h"
#include "..\UserMessages.h"
#include "..\MessageGrid\GridDefines.h"
#include "..\GridCtrl/CellTypes/GridCellCombo.h"
#include "..\GlobalFunctions.h"

#include "ParameterInputDlg.h"


static const int COLUMN_ENTRY_PARAM = 0;
static const int COLUMN_ENTRY_TYPE = 1;
static const int COLUMN_ENTRY_OBSOLETE = 2;
static const int COLUMN_ENTRY_DESC = 3;

// ParameterInputDlg dialog

IMPLEMENT_DYNAMIC(ParameterInputDlg, cdxCDynamicDialog)

ParameterInputDlg::ParameterInputDlg(CWnd* pParent /*=NULL*/)
	: m_bShowObsolete(true)
{
	m_pParentWnd = pParent;
	m_lpszTemplateName = MAKEINTRESOURCE(ParameterInputDlg::IDD);
	m_nIDHelp = ParameterInputDlg::IDD;
	
	BOOL bLoad = m_menuPopups.LoadMenu(IDM_PARAM_GRID_POPUPS);
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	m_rgsParamTypes.Add(_T("string")); //SP
	m_rgsParamTypes.Add(_T("bool")); //SP
	m_rgsParamTypes.Add(_T("unsignedLong")); //SP
	m_rgsParamTypes.Add(_T("unsignedShort")); //SP
}

ParameterInputDlg::~ParameterInputDlg()
{
	DestroyIcon(m_hIcon);

	m_rgsParamTypes.RemoveAll();
}

void ParameterInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CMenuFrame<cdxCDynamicDialog>::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_PARAMETERS, m_gridCtrl);
}


BEGIN_MESSAGE_MAP(ParameterInputDlg, CMenuFrame<cdxCDynamicDialog>)
	ON_NOTIFY(GVN_BEGINLABELEDIT, IDC_LIST_PARAMETERS, OnGridBeginEdit)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_LIST_PARAMETERS, OnGridEndEdit)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_PARAMETERS, OnGridRightClick)
	ON_COMMAND(IDM_PARAM_SHOW_OBSOLETE, OnGridShowObsolete)
	ON_COMMAND(IDM_PARAM_ADD, OnAddParam)
	ON_COMMAND(IDM_PARAM_OBSOLETE, OnMakeObsolete)
	ON_COMMAND(IDM_PARAM_DELETE, OnDeleteParam)
	ON_MESSAGE(UM_EDIT_CELL, OnEditCell)
	ON_WM_INITMENUPOPUP()
END_MESSAGE_MAP()


// ParameterInputDlg message handlers

BOOL ParameterInputDlg::OnInitDialog()
{
	CMenuFrame<cdxCDynamicDialog>::OnInitDialog();
	
	BOOL bRetval = cdxCDynamicDialog::OnInitDialog();
	
	HMENU hMenu = m_SystemNewMenu.Detach();
	HMENU hSysMenu = ::GetSystemMenu(m_hWnd,FALSE);
	if(hMenu!=hSysMenu)
	{
		if(IsMenu(hMenu))
		{
			if(!::DestroyMenu(hMenu))
			{
				WindowsError(GetLastError());
			}
		}
	}
	m_SystemNewMenu.Attach(hSysMenu);
	m_DefaultNewMenu.LoadMenu(::GetMenu(m_hWnd));
	
	if(IsMenu(m_DefaultNewMenu.m_hMenu))
	{
		UpdateMenuBarColor(m_DefaultNewMenu);
	}

	SetIcon(m_hIcon, FALSE);      // Set small icon

	AddSzControl(m_gridCtrl, mdResize, mdResize);
	AddSzControl(IDOK, mdRepos, mdRepos);
	AddSzControl(IDCANCEL, mdRepos, mdRepos);

	InitialiseGridCtrl();

	PopulateGridCtrl(false);

	return bRetval;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

extern void AFXAPI AfxCancelModes(HWND hWndRcvr);

// Most of the code is copied from the MFC code from CFrameWnd::OnInitMenuPopup
void ParameterInputDlg::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
{
	AfxCancelModes(m_hWnd);
	
	// don't support system menu
	if (!bSysMenu)
	{
		ASSERT(pMenu != NULL);
		// check the enabled state of various menu items
		
		CCmdUI state;
		state.m_pMenu = pMenu;
		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pParentMenu == NULL);
		
		// determine if menu is popup in top-level menu and set m_pOther to
		//  it if so (m_pParentMenu == NULL indicates that it is secondary popup)
		HMENU hParentMenu;
		if (AfxGetThreadState()->m_hTrackingMenu == pMenu->m_hMenu)
		{
			state.m_pParentMenu = pMenu;    // parent == child for tracking popup
		}
		else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
		{
			CWnd* pParent = GetTopLevelParent();
			// child windows don't have menus -- need to go to the top!
			if (pParent != NULL &&
				(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
			{
				int nIndexMax = ::GetMenuItemCount(hParentMenu);
				for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
				{
					if (::GetSubMenu(hParentMenu, nIndex) == pMenu->m_hMenu)
					{
						// when popup is found, m_pParentMenu is containing menu
						state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
						break;
					}
				}
			}
		}
		
		state.m_nIndexMax = pMenu->GetMenuItemCount();
		for (state.m_nIndex = 0; state.m_nIndex<state.m_nIndexMax; state.m_nIndex++)
		{
			state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
			if (state.m_nID == 0)
				continue; // menu separator or invalid cmd - ignore it
			
			ASSERT(state.m_pOther == NULL);
			ASSERT(state.m_pMenu != NULL);
			if (state.m_nID == (UINT)-1)
			{
				// possibly a popup menu, route to first item of that popup
				state.m_pSubMenu = pMenu->GetSubMenu(state.m_nIndex);
				if (state.m_pSubMenu == NULL ||
					(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
					state.m_nID == (UINT)-1)
				{
					continue;       // first item of popup can't be routed to
				}
				state.DoUpdate(this, FALSE);    // popups are never auto disabled
			}
			else
			{
				// normal menu item
				// Auto enable/disable if frame window has 'm_bAutoMenuEnable'
				//    set and command is _not_ a system command.
				state.m_pSubMenu = NULL;
				state.DoUpdate(this, state.m_nID < 0xF000);
			}
			
			// adjust for menu deletions and additions
			UINT nCount = pMenu->GetMenuItemCount();
			if (nCount < state.m_nIndexMax)
			{
				state.m_nIndex -= (state.m_nIndexMax - nCount);
				while (state.m_nIndex < nCount &&
					pMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
				{
					state.m_nIndex++;
				}
			}
			state.m_nIndexMax = nCount;
		}
	}
	// Do the work for update
	CMenuFrame<cdxCDynamicDialog>::OnInitMenuPopup(pMenu,nIndex,bSysMenu);
}


bool ParameterInputDlg::InitialiseGridCtrl()
{
	CString sHeader;
	m_gridCtrl.SetTrackFocusCell(FALSE);

	m_gridCtrl.SetRowCount(1);
	m_gridCtrl.SetFixedRowCount(1);
	m_gridCtrl.SetHeaderSort();
	m_gridCtrl.SetCompareFunction(ParameterInputDlg::pfnCellCompare);
	m_gridCtrl.SetFixedColumnSelection(FALSE);

	sHeader.LoadString(IDS_PARAMDLG_COLUMN_PARAMETER);
	m_gridCtrl.InsertColumn(sHeader);
	sHeader.LoadString(IDS_PARAMDLG_COLUMN_TYPE);
	m_gridCtrl.InsertColumn(sHeader);
	sHeader.LoadString(IDS_PARAMDLG_COLUMN_OBSOLETE);
	m_gridCtrl.InsertColumn(sHeader);
	sHeader.LoadString(IDS_PARAMDLG_COLUMN_DESC);
	m_gridCtrl.InsertColumn(sHeader);

	return true;
}

bool ParameterInputDlg::PopulateGridCtrl(bool bRefresh)
{
//	CFrameWnd* pMainFrame = reinterpret_cast<CFrameWnd*>(AfxGetMainWnd());
	MessageGridDoc* pDoc = reinterpret_cast<MessageGridDoc*>(GetActiveDoc());

	MSXML2::IXMLDOMNodeListPtr spParameters = pDoc->m_messageXMLObj.m_spParamRoot->selectNodes(XMLNODE_PARAM);
	for(long i = 0, iCount = spParameters->Getlength(); i < iCount; i++)
	{
		if(!InsertNodeIntoGrid(spParameters->Getitem(i)))
			return false;
	}
	InsertAddParamRow();

	m_gridCtrl.AutoSizeColumns(GVS_BOTH);

	return true;
}

bool ParameterInputDlg::InsertNodeIntoGrid(MSXML2::IXMLDOMNodePtr& spParameter)
{
//	CFrameWnd* pMainFrame = reinterpret_cast<CFrameWnd*>(AfxGetMainWnd());
	MessageGridDoc* pDoc = reinterpret_cast<MessageGridDoc*>(GetActiveDoc());

	int iRow = 1, iCol = 0;
	_variant_t vtValue;
	CGridCellCombo *pTypeCell = NULL;

	iRow = m_gridCtrl.InsertRow(_T("")); //SL

	GV_ITEM gvItem;
	gvItem.row = iRow;
	gvItem.mask = GVIF_TEXT;

	MSXML2::IXMLDOMNodePtr spChild = spParameter->selectSingleNode(XMLNODE_PARAM_NAME);
	vtValue = spChild->GetnodeTypedValue();
	gvItem.col = COLUMN_ENTRY_PARAM;
	gvItem.strText = vtValue.bstrVal;
	m_gridCtrl.SetItem(&gvItem);

	spChild = spParameter->selectSingleNode(_T("@") XMLNODE_PARAM_TYPE); //SL
	vtValue = spChild->GetnodeTypedValue();
	gvItem.col = COLUMN_ENTRY_TYPE;
	gvItem.strText = vtValue.bstrVal;
	m_gridCtrl.SetItem(&gvItem);
	m_gridCtrl.SetCellType(gvItem.row,gvItem.col, RUNTIME_CLASS(CGridCellCombo));
	pTypeCell = (CGridCellCombo*) m_gridCtrl.GetCell(gvItem.row,gvItem.col);
    pTypeCell->SetOptions(m_rgsParamTypes);
    pTypeCell->SetStyle(CBS_DROPDOWNLIST);

	spChild = spParameter->selectSingleNode(_T("@") XMLNODE_PARAM_OBSOLETE); //SL
	vtValue = spChild->GetnodeTypedValue();
	gvItem.col = COLUMN_ENTRY_OBSOLETE;
	gvItem.strText = vtValue.bstrVal;
	m_gridCtrl.SetItem(&gvItem);
	m_gridCtrl.SetCellType(gvItem.row, gvItem.col, RUNTIME_CLASS(CGridCellCombo));
	pTypeCell = (CGridCellCombo*) m_gridCtrl.GetCell(gvItem.row,gvItem.col);
    pTypeCell->SetOptions(pDoc->m_rgsObsolete);
	pTypeCell->SetStyle(CBS_DROPDOWNLIST);
	

	spChild = spParameter->selectSingleNode(XMLNODE_PARAM_DESC);
	vtValue = spChild->GetnodeTypedValue();
	gvItem.col = COLUMN_ENTRY_DESC;
	gvItem.strText = vtValue.bstrVal;
	m_gridCtrl.SetItem(&gvItem);

	return true;
}

int ParameterInputDlg::InsertAddParamRow()
{
//	CFrameWnd* pMainFrame = reinterpret_cast<CFrameWnd*>(AfxGetMainWnd());
	MessageGridDoc* pDoc = reinterpret_cast<MessageGridDoc*>(GetActiveDoc());

	CGridCellCombo *pTypeCell = NULL;
	int iRow = m_gridCtrl.InsertRow(_T("*")); //SL
	
	for(int i = 0; i < m_gridCtrl.GetColumnCount(); i++)
	{
		m_gridCtrl.SetItemBkColour(iRow, i, CR_NEW_ROW);
		m_gridCtrl.SetItemData(iRow, i, ROW_DATA_ADD);
	}
	
	m_gridCtrl.SetCellType(iRow, COLUMN_ENTRY_TYPE, RUNTIME_CLASS(CGridCellCombo));
	m_gridCtrl.SetCellType(iRow, COLUMN_ENTRY_OBSOLETE, RUNTIME_CLASS(CGridCellCombo));

	pTypeCell = (CGridCellCombo*) m_gridCtrl.GetCell(iRow,COLUMN_ENTRY_TYPE);
	pTypeCell->SetOptions(m_rgsParamTypes);
	pTypeCell->SetStyle(CBS_DROPDOWNLIST);

	pTypeCell = (CGridCellCombo*) m_gridCtrl.GetCell(iRow,COLUMN_ENTRY_OBSOLETE);
	pTypeCell->SetOptions(pDoc->m_rgsObsolete);
	pTypeCell->SetStyle(CBS_DROPDOWNLIST);

	return iRow;
}

void ParameterInputDlg::OnGridBeginEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

	if(m_gridCtrl.GetItemData(pItem->iRow, COLUMN_ENTRY_PARAM) == ROW_DATA_ADD)
	{
	
		m_gridCtrl.SetItemText(pItem->iRow, pItem->iColumn, _T(""));

		for(int i = 0; i < m_gridCtrl.GetColumnCount(); i++)
		{
			m_gridCtrl.SetItemBkColour(pItem->iRow, i);
			m_gridCtrl.SetItemData(pItem->iRow, i, ROW_DATA_NEW);
		}

		m_gridCtrl.SetItemText(pItem->iRow, COLUMN_ENTRY_TYPE, _T("string"));
		m_gridCtrl.SetItemText(pItem->iRow, COLUMN_ENTRY_OBSOLETE, _T("false"));

		m_gridCtrl.RedrawRow(pItem->iRow);

		m_gridCtrl.RedrawRow(InsertAddParamRow());
	}

    *pResult = 0;
}

void ParameterInputDlg::SetRowData(int iRow, DWORD dwData)
{
	DWORD dwExistingData = m_gridCtrl.GetItemData(iRow, COLUMN_ENTRY_PARAM);
	dwExistingData |= dwData;

	for(int i = 0; i < m_gridCtrl.GetColumnCount(); i++)
		m_gridCtrl.SetItemData(iRow, i, dwExistingData);
}

void ParameterInputDlg::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	
	switch(pItem->iColumn)
	{
		case COLUMN_ENTRY_PARAM:
			{
				if(ValidateParameter(pItem->iRow, pItem->iColumn))
					*pResult = 0;
				else
				{
					AfxMessageBox(_T("A Parameter with the same name already exists.\nPlease enter a unique parameter name."), MB_ICONERROR);
					*pResult = -1;
					PostMessage(UM_EDIT_CELL, pItem->iRow, pItem->iColumn);
				}
			}break;
		case COLUMN_ENTRY_TYPE:
			{
				*pResult = 0;
			}break;
		case COLUMN_ENTRY_OBSOLETE:
			{
				bool bObsolete = (m_gridCtrl.GetItemText(pItem->iRow, COLUMN_ENTRY_OBSOLETE) == XMLVAL_TRUE);

				for(int i = 0; i < m_gridCtrl.GetColumnCount(); i++)
					m_gridCtrl.SetItemBkColour(pItem->iRow, i, (bObsolete) ? CR_OBSOLETE : CLR_DEFAULT);
				
				m_gridCtrl.RedrawRow(pItem->iRow);
				
				*pResult = 0;
			}break;
		case COLUMN_ENTRY_DESC:
			{
				*pResult = (ValidateParameter(pItem->iRow, pItem->iColumn)) ? 0 : -1;
			}break;
	}

	if(*pResult == 0)
		SetRowData(pItem->iRow, ROW_DATA_MOD);
}

void ParameterInputDlg::OnOK()
{	
	DWORD dwData = 0;
	for(int iRow = 0, iCount = m_gridCtrl.GetRowCount(); iRow < iCount; iRow++)
	{
		dwData = m_gridCtrl.GetItemData(iRow, COLUMN_ENTRY_PARAM);

		if((dwData & ROW_DATA_NEW) && !(dwData & ROW_DATA_DEL))
		{
			ModifyParameter(iRow, true);
		}
		else
		{
			if(dwData & ROW_DATA_DEL)
			{
				DeleteParameter(iRow);
			}
			else if(dwData & ROW_DATA_MOD)
			{
				ModifyParameter(iRow, false);
			}
		}
	}


	PostViewMessage(UM_PARAM_REFRESH);

	CMenuFrame<cdxCDynamicDialog>::OnOK();
}

bool ParameterInputDlg::ModifyParameter(int iRow, bool bNewParam)
{
	//CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetMainWnd();
	MessageGridDoc* pDoc = reinterpret_cast<MessageGridDoc*>(GetActiveDoc());

	return pDoc->ModifyParameter(m_gridCtrl.GetItemText(iRow, COLUMN_ENTRY_PARAM), 
							m_gridCtrl.GetItemText(iRow, COLUMN_ENTRY_TYPE),
							(m_gridCtrl.GetItemText(iRow, COLUMN_ENTRY_OBSOLETE) == XMLVAL_TRUE),
							m_gridCtrl.GetItemText(iRow, COLUMN_ENTRY_DESC), 
							bNewParam);
}

bool ParameterInputDlg::DeleteParameter(int iRow)
{
	MessageGridDoc* pDoc = reinterpret_cast<MessageGridDoc*>(GetActiveDoc());
	
	return pDoc->DeleteParameter(m_gridCtrl.GetItemText(iRow, COLUMN_ENTRY_PARAM));
}

bool ParameterInputDlg::ValidateParameter(int iRow, int iCol)
{
	switch(iCol)
	{
		case COLUMN_ENTRY_PARAM:
			{
				CString sParam = m_gridCtrl.GetItemText(iRow, COLUMN_ENTRY_PARAM);
				for(int i = 0, iRowCount = m_gridCtrl.GetRowCount() - 1; i < iRowCount; i++)
				{
					if(i != iRow && sParam == m_gridCtrl.GetItemText(i, COLUMN_ENTRY_PARAM))
						return false;
				}
			}break;
		case COLUMN_ENTRY_TYPE:
			{
			}break;
		case COLUMN_ENTRY_OBSOLETE:
			{
			
			}break;
		case COLUMN_ENTRY_DESC:
			{
			}break;
	}

	return true;
}

int CALLBACK ParameterInputDlg::pfnCellCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CGridCellBase* pCell1 = (CGridCellBase*) lParam1;
	CGridCellBase* pCell2 = (CGridCellBase*) lParam2;

	if (!pCell1 || !pCell2) 
		return 0;

	int iData1 = pCell1->GetData();
	int iData2 = pCell2->GetData();

	if(iData1 == ROW_DATA_ADD)
		return ((pCell1->GetGrid())->GetSortAscending()) ? 1 : -1;
	else if(iData2 == ROW_DATA_ADD)
		return ((pCell1->GetGrid())->GetSortAscending()) ? -1 : 1;
	else
		return _tcscmp(pCell1->GetText(), pCell2->GetText());
}

void ParameterInputDlg::OnGridRightClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	
	POINT p;
	m_gridCtrl.GetCellOrigin(pItem->iRow, pItem->iColumn, &p);
	m_gridCtrl.ClientToScreen(&p);

	CNewMenu* pMenu = (CNewMenu*)m_menuPopups.GetSubMenu(0);
	pMenu->CheckMenuItem(IDM_PARAM_SHOW_OBSOLETE, MF_BYCOMMAND|(m_bShowObsolete) ? MF_CHECKED : MF_UNCHECKED);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, p.x, p.y, this);

	*pResult = TRUE;
}

void ParameterInputDlg::OnGridShowObsolete()
{
	if(m_bShowObsolete)
	{
		CWaitCursor wait;
		for(int i = 0, iCount = m_gridCtrl.GetRowCount(); i < iCount; i++)
		{
			// grey rows are obsolete
			if(m_gridCtrl.GetItemBkColour(i, 0) == CR_OBSOLETE)
				m_gridCtrl.SetRowHeight(i, 0);
		}
		m_bShowObsolete = false;
	}
	else
	{
		CWaitCursor wait;
		for(int i = 0, iCount = m_gridCtrl.GetRowCount(); i < iCount; i++)
		{
			// grey rows are obsolete
			if(m_gridCtrl.GetItemBkColour(i, 0) == CR_OBSOLETE)
				m_gridCtrl.SetRowHeight(i, 20);
		}
		m_bShowObsolete = true;
	}

	m_gridCtrl.RedrawWindow();
}

void ParameterInputDlg::OnAddParam()
{
	int iCol = 0, iRow = m_gridCtrl.GetRowCount() - 1;
	m_gridCtrl.EnsureVisible(iRow, iCol);
	m_gridCtrl.SetFocusCell(iRow, iCol);

	EditCell(iRow, iCol);
}

void ParameterInputDlg::EditCell(int iRow, int iCol)
{
	CPoint point;
	m_gridCtrl.GetCellOrigin(iRow, iCol, &point);

	CRect rect;
    if (!m_gridCtrl.GetCellRect(iRow, iCol, rect))
        return;

	NM_GRIDVIEW nmgv;
    nmgv.iRow         = iRow;
    nmgv.iColumn      = iCol;
    nmgv.hdr.hwndFrom = m_gridCtrl.m_hWnd;
    nmgv.hdr.idFrom   = IDC_LIST_PARAMETERS;
    nmgv.hdr.code     = GVN_BEGINLABELEDIT;
	
	SendMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);

	CGridCellBase* pCell = m_gridCtrl.GetCell(iRow, iCol);
	if (pCell)
		pCell->Edit(iRow, iCol, rect, point, IDC_INPLACE_CONTROL, 0);
}

void ParameterInputDlg::OnMakeObsolete()
{
	CCellID selection = m_gridCtrl.GetFocusCell();

	m_gridCtrl.SetItemText(selection.row, COLUMN_ENTRY_OBSOLETE, XMLVAL_TRUE);

	for(int i = 0; i < m_gridCtrl.GetColumnCount(); i++)
		m_gridCtrl.SetItemBkColour(selection.row, i, CR_OBSOLETE);
	
	m_gridCtrl.RedrawRow(selection.row);
}

void ParameterInputDlg::OnDeleteParam()
{
//	CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetMainWnd();
	MessageGridDoc* pDoc = reinterpret_cast<MessageGridDoc*>(GetActiveDoc());

	CCellID selection = m_gridCtrl.GetFocusCell();

	CString sParam = m_gridCtrl.GetItemText(selection.row, COLUMN_ENTRY_PARAM);
	
	CArray<ULONG, ULONG> rgsMessageUsage;

	if(pDoc->m_messageXMLObj.CheckParamUsage(sParam, rgsMessageUsage))
	{
		CString sTemp, sMessage(_T("This parameter cannot be deleted.\nIt is being used by the following messages:\n"));
		for(int i = 0; i < rgsMessageUsage.GetSize(); i++)
		{
			sTemp.Format((i%6) ? _T("%ld, ") : _T("\n%ld, "), rgsMessageUsage[i]);
			sMessage += sTemp;
		}
		
		sMessage.Delete(sMessage.GetLength() - 2, 2); // remove the ', '
		sMessage += _T("\n\nThe parameter may not be deleted but can be made obsolete instead.\nMaking the parameter obsolete will prevent it from being used in any new message");

		AfxMessageBox(sMessage, MB_ICONINFORMATION);
	}
	else
	{
		DWORD dwData = m_gridCtrl.GetItemData(selection.row, COLUMN_ENTRY_PARAM);
		dwData |= ROW_DATA_DEL;
		for(int i = 0; i < m_gridCtrl.GetColumnCount(); i++)
			m_gridCtrl.SetItemData(selection.row, i, dwData);

		m_gridCtrl.SetRowHeight(selection.row, 0);
		m_gridCtrl.RedrawWindow();
	}

}

LRESULT ParameterInputDlg::OnEditCell(WPARAM wParam, LPARAM lParam)
{
	EditCell((int)wParam, (int)lParam);

	return TRUE;
}