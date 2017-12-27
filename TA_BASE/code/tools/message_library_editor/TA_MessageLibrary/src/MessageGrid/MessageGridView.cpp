// MessageGridView.cpp : implementation of the MessageGridView class
//

#include "..\stdafx.h"

#include "..\TA_MessageLibrary.h"
#include "..\MessageProperties\MessagePropertySheet.h"
#include "..\Message.h"
#include "..\ColumnSelector\ColumnSelector.h"
#include "..\UserMessages.h"
#include "..\MainFrm.h"
#include "..\GridCtrl\CellTypes\GridCellCombo.h"
#include "..\HelpView\HelpDefines.h"

#include "MessageGridDoc.h"
#include "MessageGridView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const DWORD COLUMN_ENTRY_ID = 0;
static const DWORD COLUMN_ENTRY_RESOURCE_ID = 1;
static const DWORD COLUMN_ENTRY_TYPE = 2;
static const DWORD COLUMN_ENTRY_HELP_ID = 3;
static const DWORD COLUMN_ENTRY_STYLE = 4;
static const DWORD COLUMN_ENTRY_OBSOLETE = 5;
static const DWORD COLUMN_ENTRY_AUDIT_EVENT = 6;
// Always leave this entry as the greater value
static const DWORD COLUMN_ENTRY_LAST = 7;

static const UINT UT_START_VALIDATION = 3;

// MessageGridView

IMPLEMENT_DYNCREATE(MessageGridView, cdxCDynamicFormView)

BEGIN_MESSAGE_MAP(MessageGridView, cdxCDynamicFormView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, cdxCDynamicFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, cdxCDynamicFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, cdxCDynamicFormView::OnFilePrintPreview)
	ON_NOTIFY(GVN_SELCHANGED, IDC_MESSAGE_GRID, OnGridSelectionChange)
	ON_NOTIFY(NM_DBLCLK, IDC_MESSAGE_GRID, OnGridDblClick)
	ON_NOTIFY(NM_RCLICK, IDC_MESSAGE_GRID, OnGridRightClick)
	ON_COMMAND(IDM_GRID_PROPERTIES, OnGridProperties)
	ON_COMMAND(IDM_GRID_COLUMNSELECTOR, OnGridColumnselector)
	ON_COMMAND(IDM_GRID_DELETEMESSAGE, OnGridDeletemessage)
	ON_COMMAND(IDM_GRID_INSERTMESSAGE, OnGridInsertmessage)
	ON_COMMAND(IDM_GRID_MAKEOBSOLETE, OnGridMakeobsolete)
	ON_COMMAND(IDM_GRID_SHOWOBSOLETE, OnGridShowobsolete)
	ON_COMMAND(IDM_GRID_NEXT_ERROR, OnGridGotoNextError)
	ON_COMMAND(IDM_GRID_NEXT_WARNING, OnGridGotoNextWarning)
	ON_MESSAGE(UM_REFRESH_GRID_MSG, OnRefreshGridMessage)
	ON_MESSAGE(UM_PARAM_REFRESH, OnRefreshParameters)
	ON_COMMAND(IDM_TEST_MESSAGE, OnTestMessageBox)
	ON_MESSAGE(UM_NEW_GRID_COLUMN, OnNewMessageColumn)
	ON_MESSAGE(UM_REFRESH_GRID, OnRefreshGrid)
	ON_MESSAGE(UM_EMPTY_MSG_GRID, OnEmptyGrid)
	ON_MESSAGE(UM_VALIDATE_MSG_GRID, OnValidateMsgGrid)
	ON_MESSAGE(UM_REFRESH_ERROR_DESC, OnRefreshErrorDescription)
	ON_COMMAND(IDM_GRID_MSG_COPY, OnSendMessageToClipboard)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BEGIN_DYNAMIC_MAP(MessageGridView,cdxCDynamicFormView)
	DYNAMIC_MAP_ENTRY(IDC_MESSAGE_GRID, mdResize, mdResize)
END_DYNAMIC_MAP()

// MessageGridView construction/destruction

MessageGridView::MessageGridView()
:	cdxCDynamicFormView(MessageGridView::IDD, fdAll,flAntiFlicker)
{
	m_menuPopups.LoadMenu(IDM_GRID_POPUPS);
	
	m_bShowObsolete = GetApp()->m_bInternalMode;
}

MessageGridView::~MessageGridView()
{
	for(int i = 0, iCount = m_rgColumnHeaders.GetSize(); i < iCount; i++)
		delete m_rgColumnHeaders[i];
	m_rgColumnHeaders.RemoveAll();

	m_menuPopups.DestroyMenu();
}

void MessageGridView::DoDataExchange(CDataExchange* pDX)
{
	cdxCDynamicFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyFormView)
		DDX_Control(pDX, IDC_MESSAGE_GRID, m_Grid);
	//}}AFX_DATA_MAP
}

BOOL MessageGridView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return cdxCDynamicFormView::PreCreateWindow(cs);
}

// MessageGridView drawing

void MessageGridView::OnDraw(CDC* /*pDC*/)
{
	MessageGridDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}


// MessageGridView printing

BOOL MessageGridView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void MessageGridView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void MessageGridView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// MessageGridView diagnostics

#ifdef _DEBUG
void MessageGridView::AssertValid() const
{
	cdxCDynamicFormView::AssertValid();
}

void MessageGridView::Dump(CDumpContext& dc) const
{
	cdxCDynamicFormView::Dump(dc);
}

MessageGridDoc* MessageGridView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(MessageGridDoc)));
	return (MessageGridDoc*)m_pDocument;
}
#endif //_DEBUG


// MessageGridView message handlers

void MessageGridView::OnInitialUpdate()
{
	cdxCDynamicFormView::OnInitialUpdate();

	m_menuPopups.LoadToolBar(IDR_MAINFRAME);

	if(InitialiseGridCtrl())
	{
		PopulateGrid();
		m_Grid.SortItems(0, TRUE);
	}
}

BOOL MessageGridView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (::IsWindow(m_Grid.m_hWnd))
        if (m_Grid.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
            return TRUE;

	return cdxCDynamicFormView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

bool MessageGridView::InitialiseGridCtrl()
{
	m_Grid.SetRowCount(1);
	m_Grid.SetFixedRowCount(1);
	m_Grid.SetHeaderSort();
	m_Grid.EnableHiddenColUnhide(FALSE);
	m_Grid.SetTrackFocusCell(FALSE);
    m_Grid.SetFrameFocusCell(FALSE);
	m_Grid.SetEditable(FALSE);
    m_Grid.SetListMode(TRUE);
	m_Grid.EnableTitleTips(FALSE);
	m_Grid.SetCompareFunction(MessageGridView::pfnCellCompare);
	m_Grid.SetFixedColumnSelection(FALSE);
    m_Grid.SetSingleColSelection(TRUE);
    m_Grid.SetSingleRowSelection(TRUE);

	return InitialiseColumns();
}

bool MessageGridView::PopulateGrid()
{
	MSXML2::IXMLDOMNodeListPtr spMessages = GetDocument()->m_messageXMLObj.m_spMessageRoot->selectNodes(XMLNODE_MSG);

	for(long i = 0, iCount = spMessages->Getlength(); i < iCount; i++)
	{
		if(!InsertNodeIntoGrid(spMessages->Getitem(i)))
			return false;
	}

	InsertAddParamRow();

	SetTimer(UT_START_VALIDATION, 1000, NULL);

	return true;
}

int MessageGridView::InsertAddParamRow()
{
 	int iRow = m_Grid.InsertRow(_T("*")); //SL
 	
 	for(int i = 0; i < m_Grid.GetColumnCount(); i++)
 	{
 		m_Grid.SetItemBkColour(iRow, i, CR_NEW_ROW);
 		m_Grid.SetItemData(iRow, i, ROW_DATA_ADD);
 	}
 
 	return iRow;
}


bool MessageGridView::InsertNodeIntoGrid(MSXML2::IXMLDOMNodePtr& spMessage, int iRow)
{
	MessageGridDoc* pDoc = GetDocument();
	CGridCellCombo *pTypeCell = NULL;
	CString sID, sType, sLocale, sObsolete, sAuditEvent, sResourceID;
	ULONG lID = 0;
	long lIndex = 0, lCount = 0;
	int iCol = 0;
	_variant_t vtValue;
	ColumnHeader* pHeader = NULL;
	GV_ITEM gvItem;
	LCID locale = 0;

	if(iRow < 0)
		gvItem.row = m_Grid.InsertRow(_T(""), iRow); //SL
	else
		gvItem.row = iRow;

	gvItem.nFormat = DT_WORDBREAK;
	gvItem.mask = GVIF_TEXT|GVIF_PARAM|GVIF_FORMAT|GVIF_BKCLR;
	gvItem.crBkClr = GetSysColor(COLOR_WINDOW);
	gvItem.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS;	

	MSXML2::IXMLDOMNodePtr spChild = spMessage->selectSingleNode(_T("@") XMLNODE_MSG_OBSOLETE); //SL
	vtValue = spChild->GetnodeTypedValue();
	sObsolete = vtValue.bstrVal;
	if(sObsolete == XMLVAL_TRUE)
	{
		if(!GetApp()->m_bInternalMode) // don't display obsolete messages in external mode
			return true;
		else
			gvItem.crBkClr = CR_OBSOLETE;
	}

	spChild = spMessage->selectSingleNode(XMLNODE_MSG_ID);
	vtValue = spChild->GetnodeTypedValue();
	sID = vtValue.bstrVal;

	vtValue.Clear();
	spChild = spMessage->selectSingleNode(XMLNODE_MSG_TYPE);
	vtValue = spChild->GetnodeTypedValue();
	sType = vtValue.bstrVal;

	gvItem.lParam = _ttol(sID);
	
	gvItem.col = GetColumnIndex(COLUMN_ENTRY_ID);
	gvItem.strText = sID;
	m_Grid.SetItem(&gvItem); //ID
	
	gvItem.col = GetColumnIndex(COLUMN_ENTRY_TYPE);

	if(sType == _T("1")) //SL
		gvItem.strText = pDoc->m_rgsInfoTypes[0];
	else if(sType == _T("2")) //SL
		gvItem.strText = pDoc->m_rgsInfoTypes[1];
	else if(sType == _T("3")) //SL
		gvItem.strText = pDoc->m_rgsInfoTypes[2];

	m_Grid.SetItem(&gvItem); // Type
	m_Grid.SetCellType(gvItem.row,gvItem.col, RUNTIME_CLASS(CGridCellCombo));
	pTypeCell = (CGridCellCombo*) m_Grid.GetCell(gvItem.row,gvItem.col);
    pTypeCell->SetOptions(pDoc->m_rgsInfoTypes);
    pTypeCell->SetStyle(CBS_DROPDOWNLIST);

	gvItem.col = GetColumnIndex(COLUMN_ENTRY_RESOURCE_ID);
	vtValue.Clear();
	spChild = spMessage->selectSingleNode(XMLNODE_MSG_RESID);
    // TODO remove the following transitional if block
    if (0 != spChild)
    {
	    vtValue = spChild->GetnodeTypedValue();
	    sResourceID = vtValue.bstrVal;
    }
    else
    {
        sResourceID = "NA";
    }

        gvItem.strText = sResourceID;
	    m_Grid.SetItem(&gvItem); // Resource ID
    
    
	
	gvItem.col = GetColumnIndex(COLUMN_ENTRY_HELP_ID);
	vtValue.Clear();
	spChild = spMessage->selectSingleNode(XMLNODE_MSG_HELPID);
	vtValue = spChild->GetnodeTypedValue();
	gvItem.strText = vtValue.bstrVal;
	m_Grid.SetItem(&gvItem); //Help ID
	
	gvItem.col = GetColumnIndex(COLUMN_ENTRY_STYLE);
	vtValue.Clear();
	spChild = spMessage->selectSingleNode(XMLNODE_MSG_STYLE);
	vtValue = spChild->GetnodeTypedValue();
	gvItem.strText = vtValue.bstrVal;
	m_Grid.SetItem(&gvItem); // Style

	gvItem.col = GetColumnIndex(COLUMN_ENTRY_OBSOLETE);
	gvItem.strText = sObsolete;
	m_Grid.SetItem(&gvItem); // Obsolete
	m_Grid.SetCellType(gvItem.row,gvItem.col, RUNTIME_CLASS(CGridCellCombo));
	pTypeCell = (CGridCellCombo*) m_Grid.GetCell(gvItem.row,gvItem.col);
    pTypeCell->SetOptions(pDoc->m_rgsObsolete);
    pTypeCell->SetStyle(CBS_DROPDOWNLIST);

	gvItem.col = GetColumnIndex(COLUMN_ENTRY_AUDIT_EVENT);
	vtValue.Clear();
	spChild = spMessage->selectSingleNode(_T("@") XMLNODE_MSG_AUDIT);
	vtValue = spChild->GetnodeTypedValue();
	gvItem.strText = vtValue.bstrVal;
	m_Grid.SetItem(&gvItem); // Audit Event
	m_Grid.SetCellType(gvItem.row,gvItem.col, RUNTIME_CLASS(CGridCellCombo));
	pTypeCell = (CGridCellCombo*) m_Grid.GetCell(gvItem.row,gvItem.col);
    pTypeCell->SetOptions(pDoc->m_rgsObsolete);
    pTypeCell->SetStyle(CBS_DROPDOWNLIST);

	MSXML2::IXMLDOMElementPtr spString = NULL;
	MSXML2::IXMLDOMNodeListPtr spStrings = spMessage->selectNodes(XMLNODE_MSG_STRING);
	for(lIndex = 0, lCount = spStrings->Getlength(); lIndex < lCount; lIndex++)
	{
		vtValue.Clear();
		spString = spStrings->Getitem(lIndex);
		vtValue = spString->getAttribute(XMLNODE_MSG_LOCALE);
		sLocale = vtValue.bstrVal;
		locale = _ttol(sLocale);

		vtValue.Clear();
		vtValue = spString->GetnodeTypedValue();
		gvItem.strText = vtValue.bstrVal;
		gvItem.col = GetColumnIndex(locale);

		m_Grid.SetItem(&gvItem);
	}

	// Hide the row if it is obsolete
	if(sObsolete == XMLVAL_TRUE && !m_bShowObsolete)
		m_Grid.SetRowHeight(gvItem.row, 0);

	return true;
}


bool MessageGridView::CreateDefaultMessage(int iRow)
{
	MessageGridDoc* pDoc = GetDocument();
    ASSERT(NULL != pDoc);

	Message msg;
	
	msg.m_ulID = pDoc->m_messageXMLObj.GetNextMessageID();
    
    // if the row isn't the first one in the grid (excluding the column headings row)
    if (iRow > 1)
    {
        // get the resource ID of the previous row
        CGridCellBase *pCell = m_Grid.GetCell(iRow -1, GetColumnIndex(COLUMN_ENTRY_RESOURCE_ID));
        ASSERT(pCell);
        CString sPrevID = pCell->GetText();
        // and derive a new id from it
        msg.m_ResID = pDoc->m_messageXMLObj.GetNextResourceID(sPrevID);
    }
            
	for(int i = 0, iCount = pDoc->m_messageXMLObj.m_rgSupportedLocales.GetSize(); i < iCount; i++)
		msg.m_mapMessages.SetAt(pDoc->m_messageXMLObj.m_rgSupportedLocales[i], CString(_T("")));
	
	if(!pDoc->AddNewMessage(msg))
		return false;

	CString sQuery;
	sQuery.Format(XPATH_MSG_USING_ID, msg.m_ulID);
	MSXML2::IXMLDOMNodePtr spMessage = pDoc->m_messageXMLObj.m_spMessageRoot->selectSingleNode((LPCTSTR)sQuery);
	
	if(InsertNodeIntoGrid(spMessage, iRow))
	{
		MessagePropertySheet PropertiesDlg(IDS_MSGPROPERTIES_HEADER, this, m_rgColumnHeaders[0]->m_siPropertiesPage, &msg);
		if (IDOK == PropertiesDlg.DoModal())
        {
    		return true;
        }
        else
        {
            // the user cancelled
            // roll back the changes
            GetDocument()->DeleteMessage(msg.m_ulID);
            m_Grid.DeleteRow(iRow);
            m_Grid.Refresh();
            return false;
        }
	}
	else
		return false;
}

bool MessageGridView::ValidateMessageType(int iRow)
{
	CString sType = m_Grid.GetItemText(iRow, GetColumnIndex(COLUMN_ENTRY_TYPE));

	for(int i = 0; i < GetDocument()->m_rgsInfoTypes.GetSize(); i++)
	{
		if(sType == GetDocument()->m_rgsInfoTypes[i])
			return true;
	}

	return false;
}

bool MessageGridView::ValidateObsolete(int iRow)
{
	CString sObsolete = m_Grid.GetItemText(iRow, GetColumnIndex(COLUMN_ENTRY_OBSOLETE));

	for(int i = 0; i < GetDocument()->m_rgsObsolete.GetSize(); i++)
	{
		if(sObsolete == GetDocument()->m_rgsObsolete[i])
			return true;
	}

	return false;
}

bool MessageGridView::ValidateAuditEvent(int iRow)
{
	CString sAuditEvent = m_Grid.GetItemText(iRow, GetColumnIndex(COLUMN_ENTRY_AUDIT_EVENT));
	for(int i = 0; i < GetDocument()->m_rgsObsolete.GetSize(); i++)
	{
		if(sAuditEvent == GetDocument()->m_rgsObsolete[i])
			return true;
	}

	return false;
}

bool MessageGridView::ValidateHelpID(int iRow)
{
	CString sHelpID = m_Grid.GetItemText(iRow, GetColumnIndex(COLUMN_ENTRY_HELP_ID));
	return (sHelpID != _T("") && _ttol(sHelpID) >= 0);
}

bool MessageGridView::ValidateStyle(int iRow)
{
	CString sStyle = m_Grid.GetItemText(iRow, GetColumnIndex(COLUMN_ENTRY_STYLE));
	LONG lStyle = _ttol(sStyle);
	if(lStyle < 0)
		return false;

	ULONG ulStyle = lStyle;
	int iButtonCount = 1, iDefButton = 1;

	if(ulStyle & MB_OKCANCEL)
	{
		iButtonCount = 2;
		ulStyle ^= MB_OKCANCEL;
	}
	if(ulStyle & MB_YESNO)
	{
		iButtonCount = 2;
		ulStyle ^= MB_YESNO;
	}
	if(ulStyle & MB_YESNOCANCEL)
	{
		iButtonCount = 3;
		ulStyle ^= MB_YESNOCANCEL;
	}
	if(ulStyle & MB_RETRYCANCEL)
	{
		iButtonCount = 2;
		ulStyle ^= MB_RETRYCANCEL;
	}
	if(ulStyle & MB_ABORTRETRYIGNORE)
	{
		iButtonCount = 3;
		ulStyle ^= MB_ABORTRETRYIGNORE;
	}
/*	if(ulStyle & MB_CANCELTRYCONTINUE)
	{
		iButtonCount = 3;
		ulStyle ^= MB_CANCELTRYCONTINUE;
	}*/

	if(ulStyle & MB_HELP)
	{
		iButtonCount++;
		ulStyle ^= MB_HELP;
	}

	if(ulStyle & MB_DEFBUTTON2)
	{
		iDefButton = 2;
		ulStyle ^= MB_DEFBUTTON2;
	}
	if(ulStyle & MB_DEFBUTTON3)
	{
		iDefButton = 3;
		ulStyle ^= MB_DEFBUTTON3;
	}
	if(ulStyle & MB_DEFBUTTON4)
	{
		iDefButton = 4;
		ulStyle ^= MB_DEFBUTTON4;
	}
	
	if(ulStyle & MB_ICONERROR)
		ulStyle ^= MB_ICONERROR;
	if(ulStyle & MB_ICONQUESTION)
		ulStyle ^= MB_ICONQUESTION;
	if(ulStyle & MB_ICONWARNING)
		ulStyle ^= MB_ICONWARNING;
	if(ulStyle & MB_ICONINFORMATION)
		ulStyle ^= MB_ICONINFORMATION;

	if(ulStyle != 0)
		return false;

	if(iDefButton > iButtonCount)
		return false;


	return true;
}

Message::enumMsgErrors MessageGridView::ValidateMessage(int iRow, int iCol)
{
	ULONG ulID = m_Grid.GetItemData(iRow, iCol);
	
	Message::enumMsgErrors error = GetDocument()->ValidateMessage(ulID, m_rgColumnHeaders[iCol]->m_dwColumnID);
	switch(error)
	{
		case Message::enumMsgErrors::errorOK: break;
		case Message::enumMsgErrors::errorMalformed: 
			m_Grid.SetItemBkColour(iRow, iCol, CLR_ERROR_LEVEL_3); 
			break;
		case Message::enumMsgErrors::errorParamCount: 
			m_Grid.SetItemBkColour(iRow, iCol, CLR_ERROR_LEVEL_1); 
			break;
		case Message::enumMsgErrors::errorPlaceMarkerCount: 
			m_Grid.SetItemBkColour(iRow, iCol, CLR_ERROR_LEVEL_3); 
			break;
		case Message::enumMsgErrors::errorSpelling:	
			m_Grid.SetItemBkColour(iRow, iCol, CLR_ERROR_LEVEL_2); 
			break;
	}

	if(error != Message::enumMsgErrors::errorOK)
		m_Grid.RedrawCell(iRow, iCol);
	
	return error;
}

bool MessageGridView::SaveCellChange(int iRow, int iCol)
{
    ASSERT(FALSE);

/*	MessageGridDoc* pDoc = GetDocument();
	ULONG ulMessageID = m_Grid.GetItemData(iRow, iCol);

	switch(m_rgColumnHeaders[iCol]->m_dwColumnID)
	{
		case COLUMN_ENTRY_TYPE:
			{
				CString sResource, sType = m_Grid.GetItemText(iRow, iCol);
				if(!pDoc->ModifyMessageType(ulMessageID, sType))
					return false;

				int iResourceColID = GetColumnIndex(COLUMN_ENTRY_RESOURCE_ID);
				pDoc->FormatResourceID(sType, m_Grid.GetItemText(iRow, GetColumnIndex(COLUMN_ENTRY_ID)), sResource);	
				m_Grid.SetItemText(iRow, iResourceColID, sResource);
				m_Grid.RedrawCell(iRow, iResourceColID);
				
			}break;
		case COLUMN_ENTRY_RESOURCE_ID: break; //read only
		case COLUMN_ENTRY_HELP_ID:
			{
				if(!pDoc->ModifyMessageHelpID(ulMessageID, m_Grid.GetItemText(iRow, iCol)))
					return false;
			}break;
		case COLUMN_ENTRY_STYLE:
			{
				if(!pDoc->ModifyMessageStyle(ulMessageID, m_Grid.GetItemText(iRow, iCol)))
					return false;
			}break;
		case COLUMN_ENTRY_OBSOLETE:
			{
				CString sTemp = m_Grid.GetItemText(iRow, iCol);

				if(!pDoc->ModifyMessageObsolete(ulMessageID, sTemp))
					return false;

				for(int i = 0; i < m_Grid.GetColumnCount(); i++)
					m_Grid.SetItemBkColour(iRow, i, (sTemp == XMLVAL_TRUE) ? CR_OBSOLETE : CLR_DEFAULT);

				m_Grid.RedrawRow(iRow);
			}break;
		case COLUMN_ENTRY_AUDIT_EVENT:
			{
				if(!pDoc->ModifyMessageAuditEvent(ulMessageID, m_Grid.GetItemText(iRow, iCol)))
					return false;
			}break;
		default:
			{
				if(!pDoc->ModifyMessageString(ulMessageID, m_rgColumnHeaders[iCol]->m_dwColumnID, m_Grid.GetItemText(iRow, iCol)))
					return false;
			}break;
	}
*/
	return true;
}

void MessageGridView::OnGridSelectionChange(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

	ULONG ulID = m_Grid.GetItemData(pItem->iRow, pItem->iColumn);

	if(ulID == 0 || pItem->iRow < 0 || pItem->iColumn < 0)
		return;

	PostMessage(UM_REFRESH_ERROR_DESC, pItem->iRow, ulID);

	MainFrame* pMainFrame = (MainFrame*)GetParentFrame();
	pMainFrame->m_wndParamCtrlBar.SendMessageToDescendants(UM_PARAM_REFRESH, NULL, (LPARAM)ulID, TRUE, TRUE);
}

LRESULT MessageGridView::OnRefreshParameters(WPARAM wParam, LPARAM lParam)
{
	MainFrame* pMainFrame = (MainFrame*)GetParentFrame();

	ULONG ulID = (ULONG)lParam;
	if(ulID == NULL)
	{
		CCellRange selection = m_Grid.GetSelectedCellRange();
		ulID = m_Grid.GetItemData(selection.GetMaxRow(), 0);
		if(ulID == 0)
			return FALSE;
	}
	
	pMainFrame->m_wndParamCtrlBar.SendMessageToDescendants(UM_PARAM_REFRESH, NULL, (LPARAM)ulID, TRUE, TRUE);

	return TRUE;
}

void MessageGridView::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

	ULONG ulID = m_Grid.GetItemData(pItem->iRow, pItem->iColumn);

    // if double clicked on the "add data" row,
    if (ROW_DATA_ADD == ulID)
    {
        // create a new message
        OnGridInsertmessage();
    }
    else
    {
        // show the message property sheet dialog
		    
	    Message message;
	    if(GetDocument()->m_messageXMLObj.ConstructMessage(ulID, message))
	    {
		    MessagePropertySheet PropertiesDlg(IDS_MSGPROPERTIES_HEADER, this, m_rgColumnHeaders[pItem->iColumn]->m_siPropertiesPage, &message);
		    PropertiesDlg.DoModal();
	    }
    }

    PostMessage(UM_REFRESH_ERROR_DESC, pItem->iRow, ulID);

	MainFrame* pMainFrame = (MainFrame*)GetParentFrame();
	pMainFrame->m_wndParamCtrlBar.SendMessageToDescendants(UM_PARAM_REFRESH, NULL, (LPARAM)ulID, TRUE, TRUE);


	*pResult = 0;
}

void MessageGridView::OnGridRightClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	
	if(pItem->iRow <= 0)
	{
		ColumnSelector dlg(this);
		dlg.DoModal();
	}
	else
	{
		POINT p;
		m_Grid.GetCellOrigin(pItem->iRow, pItem->iColumn, &p);
		m_Grid.ClientToScreen(&p);

		CMenu* pMenu = m_menuPopups.GetSubMenu(GetApp()->m_bInternalMode ? 1 : 0);
		pMenu->CheckMenuItem(IDM_GRID_SHOWOBSOLETE, MF_BYCOMMAND|(m_bShowObsolete) ? MF_CHECKED : MF_UNCHECKED);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, p.x, p.y, this);
	}

	*pResult = TRUE;
}

bool MessageGridView::InitialiseColumns()
{
	MessageLibraryApp* pApp = GetApp();
	MessageGridDoc* pDoc = GetDocument();
	CString sHeading, sLocaleID;
	bool bInternalMode = GetApp()->m_bInternalMode, bLangAvailable = false;

	m_rgColumnHeaders.Add(new ColumnHeader(COLUMN_ENTRY_ID, IDS_GRID_COLUMN_ID, 0, 1, true, false, true));
	m_rgColumnHeaders.Add(new ColumnHeader(COLUMN_ENTRY_RESOURCE_ID, IDS_GRID_COLUMN_RESOURCE_ID, 0, 0, bInternalMode, false, bInternalMode));
	m_rgColumnHeaders.Add(new ColumnHeader(COLUMN_ENTRY_TYPE, IDS_GRID_COLUMN_TYPE, 0, 0, bInternalMode, bInternalMode, true));
	m_rgColumnHeaders.Add(new ColumnHeader(COLUMN_ENTRY_HELP_ID, IDS_GRID_COLUMN_HELP_ID, 0, 0, bInternalMode, bInternalMode, bInternalMode));
	m_rgColumnHeaders.Add(new ColumnHeader(COLUMN_ENTRY_STYLE, IDS_GRID_COLUMN_STYLE, 1, 0, bInternalMode, bInternalMode, bInternalMode));
	m_rgColumnHeaders.Add(new ColumnHeader(COLUMN_ENTRY_OBSOLETE, IDS_GRID_COLUMN_OBSOLETE, 0, 0, bInternalMode, bInternalMode, bInternalMode));
	m_rgColumnHeaders.Add(new ColumnHeader(COLUMN_ENTRY_AUDIT_EVENT, IDS_GRID_COLUMN_AUDIT_EVENT, 0, 0, bInternalMode, bInternalMode, bInternalMode));
	
	pApp->m_mapAvailableLocales.Lookup(3081, sLocaleID);
	sHeading.LoadString(IDS_GRID_COLUMN_MESSAGE);
	sHeading += sLocaleID;
	m_rgColumnHeaders.Add(new ColumnHeader(3081, sHeading, 3, 0, true, bInternalMode, true));

	for(int i = 0, iCount = pDoc->m_messageXMLObj.m_rgSupportedLocales.GetSize(); i < iCount; i++)
	{
		if(pDoc->m_messageXMLObj.m_rgSupportedLocales[i] == 3081)
			continue;

		pApp->m_mapAvailableLocales.Lookup(pDoc->m_messageXMLObj.m_rgSupportedLocales[i], sLocaleID);
		sHeading.LoadString(IDS_GRID_COLUMN_MESSAGE);
		sHeading += sLocaleID;

		bLangAvailable = (bInternalMode || pDoc->m_messageXMLObj.m_rgSupportedLocales[i] == GetAppLCID());

		m_rgColumnHeaders.Add(new ColumnHeader(pDoc->m_messageXMLObj.m_rgSupportedLocales[i], sHeading, i + 4, 0, bLangAvailable, bLangAvailable, bLangAvailable));
	}

	for(i = 0, iCount = m_rgColumnHeaders.GetSize(); i < iCount; i++)
	{
		m_Grid.InsertColumn(m_rgColumnHeaders[i]->m_sTitle);
		
		if(!m_rgColumnHeaders[i]->m_bVisible || !m_rgColumnHeaders[i]->m_bAvailable)
			m_Grid.SetColumnWidth(i, 0);

		m_Grid.SetItemFormat(0, i, DT_CENTER|DT_VCENTER|DT_WORDBREAK|DT_END_ELLIPSIS);
	}

	m_Grid.SetRowHeight(0, 40);

    m_Grid.SetItemData(0,0,-1);

	return true;
}

void MessageGridView::OnGridProperties()
{
	CCellID cell = m_Grid.GetFocusCell();
	if(!cell.IsValid())
		return;

	ULONG ulID = m_Grid.GetItemData(cell.row, cell.col);
	if(ulID > 0)
	{
		Message message;
		if(GetDocument()->m_messageXMLObj.ConstructMessage(ulID, message))
		{
			MessagePropertySheet PropertiesDlg(IDS_MSGPROPERTIES_HEADER, this, 0, &message);
			PropertiesDlg.DoModal();
		}
	}
}

void MessageGridView::OnTestMessageBox()
{
	CCellID cell = m_Grid.GetFocusCell();
	if(!cell.IsValid())
		return;
	

	ULONG ulID = m_Grid.GetItemData(cell.row, cell.col);
	if(ulID > 0)
	{
		Message message;
		if(GetDocument()->m_messageXMLObj.ConstructMessage(ulID, message))
		{
			LCID locale = 3081;
			CString sMessage, sCaption, sError;
			sError.Empty();

			if(m_rgColumnHeaders[cell.col]->m_dwColumnID > COLUMN_ENTRY_LAST)
				locale = m_rgColumnHeaders[cell.col]->m_dwColumnID;

			message.m_mapMessages.Lookup(locale, sMessage);
			if(!ConstructTestMessage(sMessage, message.m_rgsParameters, sError))
			{
				MessageBox(sError, _T("Message Contains Errors"), MB_ICONERROR);
				return;
			}

			int iStart = 0;
			iStart = sMessage.Find(_T("\\n"), iStart);
			while(iStart >= 0)
			{
				sMessage.Delete(iStart, 1);
				sMessage.SetAt(iStart, _T('\n'));

				iStart = sMessage.Find(_T("\\n"), iStart);
			}

			iStart = 0;
			iStart = sMessage.Find(_T("\\t"), iStart);
			while(iStart >= 0)
			{
				sMessage.Delete(iStart, 1);
				sMessage.SetAt(iStart, _T('\t'));

				iStart = sMessage.Find(_T("\\t"), iStart);
			}


			sCaption.Format(_T("%s  -  [%s]"), AfxGetApp()->m_pszAppName, message.m_ResID); //SL

			MessageBox(sMessage, sCaption, message.m_ulMsgBoxStyle);
			
		}
	}
}

bool MessageGridView::ConstructTestMessage(CString& sMessage, CStringArray& rgsParams, CString& sError)
{
	int iArgCount = rgsParams.GetSize();
	int iStart = 0, iFinish = 0, iIndex = 0, iParamIndex = 0, iRightBraceCount = 0;
	CString sFormat, sParamIndex, sSubString, sSubstitution;
	bool bFindRightBrace = true;

	if(iArgCount > 0)
	{
		iStart = sMessage.Find(_T("{"), iStart); //SL
		if(iStart < 0)
		{
			sError.Format(_T("Warning - Expecting message to contain place markers for %d arguments - %s"), rgsParams.GetSize(), sMessage); //SL
			return true;
		}
		while(iStart >= 0)
		{
			// If not at the end of the string check for overriding character
			if(iStart < (sMessage.GetLength() - 1) && sMessage[iStart + 1] == '{') //SL
			{
				iStart += 2;
				if(iStart < sMessage.GetLength())
				{
					iStart = sMessage.Find(_T("{"), iStart); //SL
					continue;
				}
				else
					break;
			}

			iFinish = iStart + 1;
			iRightBraceCount = 0;
			bFindRightBrace = true;

			while(bFindRightBrace)
			{
				iFinish = sMessage.Find(_T("}"), iFinish); //SL
				if(iFinish < 0)
				{
					sError.Format(_T("Error - Malformed String (Missing closing bracket) - %s"), sMessage);
				//	LOG_GENERIC(SourceInfo, DebugUtil::DebugError, sError);
					return false;
				}
				// Check for overriding character
				if(iFinish < (sMessage.GetLength() - 1) && sMessage[iFinish + 1] == '}') //SL
				{
					iRightBraceCount++;
					iFinish++;
				}
				else
					bFindRightBrace = false; // stop loop
			}

			// odd number of right braces (iRightBraceCount is one less than actual count)
			if(iRightBraceCount % 2 == 0)
				iFinish -= iRightBraceCount;
			
			// lose the {} characters
			sSubString = sMessage.Mid(iStart + 1, iFinish - iStart - 1);
			sFormat.Empty();
			int iComma = 0;
			iComma = sSubString.Find(_T(","), iComma); //SL
			if(iComma > 0)
			{
				sFormat = sSubString.Right(sSubString.GetLength() - iComma);
				sParamIndex = sSubString.Left(iComma);
			}
			else
				sParamIndex = sSubString;

			iParamIndex = _ttoi(sParamIndex);
			if(iParamIndex < 0)
			{
				sError.Format(_T("Malformed String (Param Index Error %s) - s"), sParamIndex, sMessage); //SL
				//LOG_GENERIC(SourceInfo, DebugUtil::DebugError, sError);
				return false;
			}

			if(iParamIndex >= iArgCount)
			{
				sError.Format(_T("Malformed String (Param Index not passed in for %s) - s"), sParamIndex, sMessage);
				//LOG_GENERIC(SourceInfo, DebugUtil::DebugError, sError);
				return false;
			}
			// Perform Substitution
			sMessage.Delete(iStart, iFinish - iStart + 1);
			if(sFormat.IsEmpty())
				sSubstitution = rgsParams[iParamIndex];//args[iParamIndex]->GetFormattedValue();
			else
				sSubstitution = rgsParams[iParamIndex];//args[iParamIndex]->GetFormattedValue(sFormat);

			// Perform Substitution
			sMessage.Insert(iStart, sSubstitution);

			// adjust for change in length
			iStart = iStart + sSubstitution.GetLength();

			iStart = sMessage.Find(_T("{"), iStart); //SL
		} //while
	} //if

	return true;
}

ColumnHeader* MessageGridView::GetColumnHeader(DWORD dwColumnID)
{
	ColumnHeader* pColumn = NULL;
	for(int i = 0, iCount = m_rgColumnHeaders.GetSize(); i < iCount; i++)
	{
		pColumn = m_rgColumnHeaders[i];
		if(pColumn->m_dwColumnID == dwColumnID)
			return pColumn;
	}
	return NULL;
}

ColumnHeader* MessageGridView::GetColumnHeader(LPCTSTR pszHeader)
{
	ColumnHeader* pColumn = NULL;
	for(int i = 0, iCount = m_rgColumnHeaders.GetSize(); i < iCount; i++)
	{
		pColumn = m_rgColumnHeaders[i];
		if(pColumn->m_sTitle == pszHeader)
			return pColumn;
	}

	return NULL;
}

int MessageGridView::GetColumnIndex(DWORD dwColumnID)
{
	ColumnHeader* pColumn = NULL;
	for(int i = 0, iCount = m_rgColumnHeaders.GetSize(); i < iCount; i++)
	{
		pColumn = m_rgColumnHeaders[i];
		if(pColumn->m_dwColumnID == dwColumnID)
			break;
	}

	return i;
}

void MessageGridView::MakeColumnVisible(DWORD dwColumnID, bool bVisible)
{
	m_Grid.SetColumnWidth(GetColumnIndex(dwColumnID), (bVisible) ? 100 : 0);
}

void MessageGridView::MoveColumn(int iPrevIndex, int iNewIndex)
{
	if(m_Grid.MoveColumn(iPrevIndex, iNewIndex))
	{
		m_rgColumnHeaders.InsertAt(iNewIndex, m_rgColumnHeaders.GetAt(iPrevIndex));
		m_rgColumnHeaders.RemoveAt(iNewIndex < iPrevIndex ? iPrevIndex + 1 : iPrevIndex);
		m_Grid.RedrawWindow();
	}
}
void MessageGridView::OnGridColumnselector()
{
	ColumnSelector dlg(this);
	dlg.DoModal();
}

void MessageGridView::OnGridDeletemessage()
{
	CCellID cell = m_Grid.GetFocusCell();
	if(cell.IsValid())
	{
		CString sMsg, sHeader;
		sMsg.LoadString(IDS_GRID_CONFIRM_DELETE);
		sHeader.LoadString(IDS_GRID_CONFIRM_DELETE_HEADER);
		if(MessageBox(sMsg, sHeader, MB_YESNO) != IDYES)
			return;

		ULONG ulID = m_Grid.GetItemData(cell.row, 0);
		if(!GetDocument()->DeleteMessage(ulID))
			return;

		m_Grid.DeleteRow(cell.row);
		m_Grid.Refresh();
	}
}

void MessageGridView::OnGridInsertmessage()
{
	// get the currently selected cell    
    int iRow = -1;
    CCellID cell = m_Grid.GetFocusCell();
	if(cell.IsValid())
    {
		iRow = m_Grid.InsertRow(_T(""),cell.row);
    }

	m_Grid.Refresh();

	if(!CreateDefaultMessage(iRow))
		return;

	for(int i = 0; i < m_Grid.GetColumnCount(); i++)
	{
		m_Grid.SetItemBkColour(iRow, i, CR_INSERTED_ROW);
	}

	
}

void MessageGridView::OnGridMakeobsolete()
{
	CCellID cell = m_Grid.GetFocusCell();
	if(cell.IsValid())
	{
		Message msg;
		ULONG ulID = m_Grid.GetItemData(cell.row, 0);
		
		if(ulID == 0 || !GetDocument()->ModifyMessageObsolete(ulID, XMLVAL_TRUE))
			return;
		
		for(int i = 0; i < m_Grid.GetColumnCount(); i++)
			m_Grid.SetItemBkColour(cell.row, i, CR_OBSOLETE);

		m_Grid.RedrawRow(cell.row);
	}
}

void MessageGridView::OnGridShowobsolete()
{
	CWaitCursor wait;
    
    if(m_bShowObsolete)
	{
		for(int i = 0, iCount = m_Grid.GetRowCount(); i < iCount; i++)
		{
			// grey rows are obsolete
			if(m_Grid.GetItemBkColour(i, 0) == CR_OBSOLETE)
				m_Grid.SetRowHeight(i, 0);
		}
		m_bShowObsolete = false;
	}
	else
	{
		for(int i = 0, iCount = m_Grid.GetRowCount(); i < iCount; i++)
		{
			// grey rows are obsolete
			if(m_Grid.GetItemBkColour(i, 0) == CR_OBSOLETE)
				m_Grid.SetRowHeight(i, 20);
		}
		m_bShowObsolete = true;
	}

	m_Grid.RedrawWindow();
}

LRESULT MessageGridView::OnRefreshGridMessage(WPARAM wParam, LPARAM lParam)
{
	int iRow = GetMessageRow((ULONG)lParam);
	if(iRow < 0)
		return FALSE;
	CString sQuery;
	sQuery.Format(XPATH_MSG_USING_ID, (ULONG)lParam);
	MSXML2::IXMLDOMNodePtr spMessage = GetDocument()->m_messageXMLObj.m_spMessageRoot->selectSingleNode((LPCTSTR)sQuery);
	if(InsertNodeIntoGrid(spMessage, iRow))
	{
		m_Grid.RedrawRow(iRow);
		return TRUE;
	}
	return FALSE;
}

int MessageGridView::GetMessageRow(ULONG ulMessageID)
{
	// FIXME: This is very inefficient! use a map instead

    for(int i = 0, iCount = m_Grid.GetRowCount(); i < iCount; i++)
	{
		if(m_Grid.GetItemData(i, 0) == (long)ulMessageID)
			return i;
	}

	return -1;
}

int CALLBACK MessageGridView::pfnCellCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CGridCellBase* pCell1 = (CGridCellBase*) lParam1;
	CGridCellBase* pCell2 = (CGridCellBase*) lParam2;

	if (!pCell1 || !pCell2) 
		return 0;

	int iData1 = pCell1->GetData();
	int iData2 = pCell2->GetData();

	// if the "add data" row is involved, make sure it remains the last row...
    if(iData1 == ROW_DATA_ADD)
    {
        return ((pCell1->GetGrid())->GetSortAscending()) ? 1 : -1;
    }
    else if(iData2 == ROW_DATA_ADD)
    {
     	return ((pCell1->GetGrid())->GetSortAscending()) ? -1 : 1;
    }
    else
    {
        // if sorting on ID
        if (COLUMN_ENTRY_ID == pCell1->GetGrid()->GetSortColumn())
        {
            return CGridCtrl::pfnCellNumericCompare(lParam1, lParam2, lParamSort);
        }
        else
        {
            return CGridCtrl::pfnCellTextCompare(lParam1, lParam2, lParamSort);
        }
    }

}


LRESULT MessageGridView::OnNewMessageColumn(WPARAM wParam, LPARAM lParam)
{
	DWORD dwData = 0;
	int i = 0, iCount = 0;
	MessageGridDoc* pDoc = (MessageGridDoc*)GetDocument();
	MessageLibraryApp* pApp = GetApp();

	for(i = 0, iCount = pDoc->m_messageXMLObj.m_rgSupportedLocales.GetSize(); i < iCount; i++)
	{
		ColumnHeader* pHeader = GetColumnHeader(pDoc->m_messageXMLObj.m_rgSupportedLocales[i]);
		if(!pHeader)
			break;
	}

	CString sHeading, sLocaleID;

	pApp->m_mapAvailableLocales.Lookup(pDoc->m_messageXMLObj.m_rgSupportedLocales[i], sLocaleID);
	sHeading.LoadString(IDS_GRID_COLUMN_MESSAGE);
	sHeading += sLocaleID;

	m_rgColumnHeaders.Add(new ColumnHeader(pDoc->m_messageXMLObj.m_rgSupportedLocales[i], sHeading, iCount + 2, 
											0, pApp->m_bInternalMode, pApp->m_bInternalMode, pApp->m_bInternalMode));

	int iColumn = m_Grid.InsertColumn(sHeading);
	m_Grid.SetColumnWidth(iColumn, 150);

	// Set row data and background colour if obsolete
	for(i = 0, iCount = m_Grid.GetRowCount(); i < iCount; i++)
	{
		m_Grid.SetItemBkColour(i, iColumn, CR_NEW_ROW);
	}

	m_Grid.RedrawColumn(iColumn);

	return TRUE;
}

LRESULT MessageGridView::OnRefreshGrid(WPARAM wParam, LPARAM lParam)
{
	CWaitCursor wait;
	if(m_Grid.DeleteAllItems())
	{
		for(int i = 0, iCount = m_rgColumnHeaders.GetSize(); i < iCount; i++)
			delete m_rgColumnHeaders[i];
		m_rgColumnHeaders.RemoveAll();

		if(InitialiseGridCtrl())
		{
			PopulateGrid();
			return TRUE;
		}
	}

	return FALSE;
}

LRESULT	MessageGridView::OnEmptyGrid(WPARAM wParam, LPARAM lParam)
{
	CWaitCursor wait;
	if(m_Grid.DeleteAllItems())
	{
		for(int i = 0, iCount = m_rgColumnHeaders.GetSize(); i < iCount; i++)
			delete m_rgColumnHeaders[i];
		m_rgColumnHeaders.RemoveAll();

		return TRUE;
	}

	return FALSE;
}

LRESULT MessageGridView::OnValidateMsgGrid(WPARAM wParam, LPARAM lParam)
{
    bool bScrollToError(wParam != 0), bWarnings = false, bErrors = false;
	CString sProgress;
	int i = 0, j = 0, iRowCount = m_Grid.GetRowCount() - 1, 
	int iLocaleCount = GetDocument()->m_messageXMLObj.m_rgSupportedLocales.GetSize();

	CArray<int, int> rgStringCols;
	for(i = 0; i < iLocaleCount; i++)
		rgStringCols.Add(GetColumnIndex(GetDocument()->m_messageXMLObj.m_rgSupportedLocales[i]));

	MainFrame* pMainFrame = reinterpret_cast<MainFrame*>(AfxGetMainWnd());
	
	CWaitCursor wait;
	for(i = 1; i < iRowCount; i++)
	{
		for(j = 0; j < iLocaleCount; j++)
		{
			switch(ValidateMessage(i, rgStringCols[j]))
			{
				case Message::enumMsgErrors::errorOK: 
					break;
				case Message::enumMsgErrors::errorMalformed: 
					bErrors = true;
					break;
				case Message::enumMsgErrors::errorParamCount: 
					bWarnings = true; 
					break;
				case Message::enumMsgErrors::errorPlaceMarkerCount: 
					bErrors = true;
					break;
				case Message::enumMsgErrors::errorSpelling: 
					bWarnings = true; 
					break;
			}
		}

		sProgress.Format(_T("Please wait while the messages are being validated [Message %d of %d]"), i, iRowCount);
		pMainFrame->SetMessageText(sProgress);
	}

	if(bErrors)
		sProgress = _T("Library contains Errors");
	else if(bWarnings)
		sProgress = _T("Library contains Warnings");
	else
		sProgress = _T("Validation complete successfully");

	pMainFrame->SetMessageText(sProgress);

	// Goto the error with the highest priority
	if(!GotoNextAnomoly(CLR_ERROR_LEVEL_3))
		if(!GotoNextAnomoly(CLR_ERROR_LEVEL_2))
			GotoNextAnomoly(CLR_ERROR_LEVEL_1);

	return !bErrors;

}

bool MessageGridView::GotoNextAnomoly(const COLORREF clr)
{
	bool bOnceAround = false;
	CCellID current = m_Grid.GetFocusCell();
	if(!current.IsValid())
	{
		current.col = 0;
		current.row = 1;
	}
		
	int iRow = 0, iCol = 0, iRowCount = m_Grid.GetRowCount();
	
	// finish off the row
	for(iCol = current.col + 1; iCol < m_Grid.GetColumnCount(); iCol++)
	{
		if(m_Grid.GetItemBkColour(current.row, iCol) == clr)
		{
			m_Grid.EnsureVisible(current.row, iCol);
			m_Grid.SetFocusCell(current.row, iCol);
			PostMessage(UM_REFRESH_ERROR_DESC, iRow, m_Grid.GetItemData(iRow, iCol));
			return true;
		}
	}

	// go to the next row and continue
	for(iRow = current.row + 1; iRow < iRowCount; iRow++)
	{
		for(iCol = 0; iCol < m_Grid.GetColumnCount(); iCol++)
		{
			if(m_Grid.GetItemBkColour(iRow, iCol) == clr)
			{
				if(iRow == current.row && iCol == current.col) // back at the start
					return false;

				m_Grid.EnsureVisible(iRow, iCol);
				m_Grid.SetFocusCell(iRow, iCol);
				LPARAM lMessageID = m_Grid.GetItemData(iRow, iCol);
				PostMessage(UM_REFRESH_ERROR_DESC, iRow, lMessageID);
				PostMessage(UM_PARAM_REFRESH, NULL, lMessageID);
				return true;
			}
		}

		if(!bOnceAround && iRow == iRowCount - 1)
		{
			iRow = 1;
			bOnceAround = true;
		}
	}

	return false; // no anomolies found
}

LRESULT MessageGridView::OnRefreshErrorDescription(WPARAM wParam, LPARAM lParam)
{
	MainFrame* pMainFrame = reinterpret_cast<MainFrame*>(AfxGetMainWnd());
	MessageGridDoc* pDoc = GetDocument();
	int iRow = (int)wParam;
	ULONG ulMessageID = (ULONG)lParam;
	CString sError;
	COLORREF cl = NULL;
	Message::enumMsgErrors error = Message::enumMsgErrors::errorOK;

	int iCount = pDoc->m_messageXMLObj.m_rgSupportedLocales.GetSize();
	for(int i = 0; i < iCount; i++)
	{
		cl = m_Grid.GetItemBkColour(iRow, GetColumnIndex(pDoc->m_messageXMLObj.m_rgSupportedLocales[i]));
		
		if(cl == CLR_DEFAULT)
			continue;

		error =pDoc->ValidateMessage(ulMessageID, pDoc->m_messageXMLObj.m_rgSupportedLocales[i]);

		switch(error)
		{
			case Message::enumMsgErrors::errorMalformed: 
				{
					cl = RGB(255, 0, 0);
					sError = _T("Error - The message is not formatted correctly");
				}break;
			case Message::enumMsgErrors::errorParamCount:
				{
					cl = RGB(255, 255, 0);
					sError = _T("Warning - There are more parameters than place markers");
				}break;
			case Message::enumMsgErrors::errorPlaceMarkerCount:
				{
					cl = RGB(255, 0, 0);
					sError = _T("Error - There are not enough parameters supplied for the message");
				}break;
			case Message::enumMsgErrors::errorSpelling:
				{
					cl = RGB(255, 128, 0);
					sError = _T("Error - There is a spelling mistake in the message");
				}break;
			default:
				continue;
		}

		if(error != Message::enumMsgErrors::errorOK)
		{
			pMainFrame->SetMessageText(sError, cl);
			return TRUE;
		}
	}

	if(error == Message::enumMsgErrors::errorOK)
		pMainFrame->SetMessageText(_T("Ready"));

	return FALSE;
}

void MessageGridView::OnSendMessageToClipboard()
{
	MessageGridDoc* pDoc = GetDocument();
	CString sText;
	ULONG ulMessageID = 0;
	
	CCellID current = m_Grid.GetFocusCell();
	if(!current.IsValid())
		return;
	
	ulMessageID = m_Grid.GetItemData(current.row, current.col);
	
	if(!pDoc->CreateMessageForClipboard(ulMessageID, sText))
		return;
	
	// Create a shared memory file
    CSharedFile sf(GMEM_MOVEABLE | GMEM_SHARE | GMEM_ZEROINIT);
	
	// Convert to narrow text for use in cpp code
	int iLength = sText.GetLength();
	char* szText = new char[iLength + 1];
	wcstombs(szText, sText, iLength);
	
    // Place clipboard data in the shared memory file
	sf.Write((const void*)szText, iLength);
	
    if (sf.GetLength() > 0) 
	{
        // Put the data on the clipboard
        OpenClipboard();
        EmptyClipboard();
        SetClipboardData(CF_TEXT, sf.Detach());
        CloseClipboard();
    }
	
	delete [] szText;
}

void MessageGridView::EditCell(int iRow, int iCol)
{
	if(!m_rgColumnHeaders[iCol]->m_bVisible)
	{
		for(int i = 0; i < m_rgColumnHeaders.GetSize(); i++)
		{
			if(m_rgColumnHeaders[i]->m_bEditable && m_rgColumnHeaders[i]->m_bVisible)
			{
				iCol = i;
				break;
			}
		}

		if(i == m_rgColumnHeaders.GetSize())
			return;
	}

	CPoint point;
	m_Grid.GetCellOrigin(iRow, iCol, &point);

	CRect rect;
    if (!m_Grid.GetCellRect(iRow, iCol, rect))
        return;

	m_Grid.EnsureVisible(iRow, iCol);

	NM_GRIDVIEW nmgv;
    nmgv.iRow         = iRow;
    nmgv.iColumn      = iCol;
    nmgv.hdr.hwndFrom = m_Grid.m_hWnd;
    nmgv.hdr.idFrom   = IDC_MESSAGE_GRID;
    nmgv.hdr.code     = GVN_BEGINLABELEDIT;
	
	SendMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);

	CGridCellBase* pCell = m_Grid.GetCell(iRow, iCol);
	if (pCell)
		pCell->Edit(iRow, iCol, rect, point, IDC_INPLACE_CONTROL, 0);
}

void MessageGridView::OnTimer(UINT nIDEvent)
{
	if(nIDEvent == UT_START_VALIDATION)
	{
		PostMessage(UM_VALIDATE_MSG_GRID, NULL, NULL);

		KillTimer(nIDEvent);
	}
	else
		CView::OnTimer(nIDEvent);
}