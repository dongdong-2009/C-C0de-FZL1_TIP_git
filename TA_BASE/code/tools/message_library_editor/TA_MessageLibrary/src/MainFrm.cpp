// MainFrm.cpp : implementation of the MainFrame class
//

#include "stdafx.h"

#include "TA_MessageLibrary.h"
#include "MainFrm.h"
#include "UserMessages.h"

#include "MessageExtraction\MessageExtractorDlg.h"
#include "MessageExtraction\ExportToRCDlg.h"
#include "ParameterInput\ParameterInputDlg.h"
#include "NewLang\NewLangDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// MainFrame

IMPLEMENT_DYNCREATE(MainFrame, CMenuFrameWnd)

BEGIN_MESSAGE_MAP(MainFrame, CMenuFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_EXTRACTFROMEXCEL, OnFileExtractfromexcel)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXTRACTFROMEXCEL, OnUpdateFileExtractfromexcel)
	ON_COMMAND(IDM_VIEW_HELPBAR, OnViewHelpbar)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_HELPBAR, OnUpdateViewHelpbar)
	ON_COMMAND(IDM_FILE_EXPORTTORCFILE, OnFileExporttorcfile)
	ON_UPDATE_COMMAND_UI(IDM_FILE_EXPORTTORCFILE, OnUpdateFileExporttorcfile)
	ON_COMMAND(IDM_PARAMETER_INPUT, OnParameterInput)
	ON_UPDATE_COMMAND_UI(IDM_PARAMETER_INPUT, OnUpdateParameterInput)
	ON_COMMAND(IDM_EDIT_NEW_LANG, OnEditNewLang)
	ON_UPDATE_COMMAND_UI(IDM_EDIT_NEW_LANG, OnUpdateEditNewLang)
	ON_COMMAND(IDM_VIEW_MSG_PARAMS, OnViewParams)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_MSG_PARAMS, OnUpdateViewParams)
	ON_MESSAGE(UM_GOTO_HELP_TOPIC, OnHelpTopic)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// MainFrame construction/destruction

MainFrame::MainFrame()
{

}

MainFrame::~MainFrame()
{

}


int MainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CString sText;

	if (CMenuFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar((GetApp()->m_bInternalMode) ? IDR_MAINFRAME : IDR_EXTERNAL_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n"); //SL
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n"); //SL
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);

	sText.LoadString(IDS_MAINFRAME_HELP_CTRLBAR);
	if (!m_wndHelpCtrlBar.Create(sText, this, CSize(140, 100),TRUE, AFX_IDW_CONTROLBAR_FIRST + 33))
	{
		 TRACE0("Failed to create Help View bar\n");
		 return -1;      // fail to create
	}

	m_wndHelpCtrlBar.SetBarStyle(m_wndHelpCtrlBar.GetBarStyle() |
		CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC);

	m_wndHelpCtrlBar.EnableDocking(CBRS_ALIGN_ANY);

	sText.LoadString(IDS_MAINFRAME_MESSAGE_PARAMS_CTRLBAR);
	if (!m_wndParamCtrlBar.Create(sText, this, CSize(140, 100),TRUE, AFX_IDW_CONTROLBAR_FIRST + 34))
	{
		 TRACE0("Failed to create Parameter View bar\n");
		 return -1;      // fail to create
	}

	m_wndParamCtrlBar.SetBarStyle(m_wndParamCtrlBar.GetBarStyle() |
		CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC);

	m_wndParamCtrlBar.EnableDocking(CBRS_ALIGN_ANY);

	EnableDocking(CBRS_ALIGN_ANY);
#ifdef _SCB_REPLACE_MINIFRAME
    m_pFloatingFrameClass = RUNTIME_CLASS(CSCBMiniDockFrameWnd);
#endif

	DockControlBar(&m_wndParamCtrlBar, AFX_IDW_DOCKBAR_RIGHT);
    DockControlBarNextTo(&m_wndHelpCtrlBar, &m_wndParamCtrlBar);
	DockControlBar(&m_wndToolBar);

	m_DefaultNewMenu.LoadToolBar((GetApp()->m_bInternalMode) ? IDR_MAINFRAME : IDR_EXTERNAL_MAINFRAME);

	return 0;
}

BOOL MainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMenuFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;

	return TRUE;
}


// MainFrame diagnostics

#ifdef _DEBUG
void MainFrame::AssertValid() const
{
	CMenuFrameWnd::AssertValid();
}

void MainFrame::Dump(CDumpContext& dc) const
{
	CMenuFrameWnd::Dump(dc);
}

#endif //_DEBUG


// MainFrame message handlers


void MainFrame::OnFileExtractfromexcel()
{
	MessageExtractorDlg dlg(this);
	if(dlg.DoModal() == IDOK)
		GetActiveView()->PostMessage(UM_REFRESH_GRID, NULL, NULL);
}

void MainFrame::OnUpdateFileExtractfromexcel(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetApp()->m_bInternalMode);
}

void MainFrame::OnViewHelpbar()
{
	bool bShow = m_wndHelpCtrlBar.IsVisible();
	ShowControlBar(&m_wndHelpCtrlBar, !bShow, FALSE);
}

void MainFrame::OnUpdateViewHelpbar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndHelpCtrlBar.IsVisible() ? 1 : 0);
}

void MainFrame::OnViewParams()
{
	bool bShow = m_wndParamCtrlBar.IsVisible();
	ShowControlBar(&m_wndParamCtrlBar, !bShow, FALSE);
}

void MainFrame::OnUpdateViewParams(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndParamCtrlBar.IsVisible() ? 1 : 0);
}

void MainFrame::OnFileExporttorcfile()
{
	if(GetActiveView()->SendMessage(UM_VALIDATE_MSG_GRID, TRUE, NULL))
	{
		ExportToRCDlg exportDlg(this);
		exportDlg.DoModal();
	}
	else
		AfxMessageBox(IDS_APP_RC_EXPORT_ERROR, MB_ICONERROR);
}

void MainFrame::OnUpdateFileExporttorcfile(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetApp()->m_bInternalMode);
}

void MainFrame::OnParameterInput()
{
	ParameterInputDlg dlg;
	dlg.DoModal();
}

void MainFrame::OnUpdateParameterInput(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetApp()->m_bInternalMode);
}


void MainFrame::OnEditNewLang()
{
	NewLangDlg dlg(this, (MessageGridDoc*)GetActiveDocument());
	if(dlg.DoModal() == IDOK)
		GetActiveView()->PostMessage(UM_NEW_GRID_COLUMN, NULL, NULL);
}

void MainFrame::OnUpdateEditNewLang(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetApp()->m_bInternalMode);
}


BOOL MainFrame::DestroyWindow()
{
	CSizingControlBar::GlobalSaveState(this, AfxGetApp()->m_pszProfileName);

	return CMenuFrameWnd::DestroyWindow();
}

void MainFrame::DockControlBarNextTo(CControlBar* pBar, CControlBar* pTargetBar)
{
    ASSERT(pBar != NULL);
    ASSERT(pTargetBar != NULL);
    ASSERT(pBar != pTargetBar);

    // the neighbour must be already docked
    CDockBar* pDockBar = pTargetBar->m_pDockBar;
    ASSERT(pDockBar != NULL);
    UINT nDockBarID = pTargetBar->m_pDockBar->GetDlgCtrlID();
    ASSERT(nDockBarID != AFX_IDW_DOCKBAR_FLOAT);

    bool bHorz = (nDockBarID == AFX_IDW_DOCKBAR_TOP ||
        nDockBarID == AFX_IDW_DOCKBAR_BOTTOM);

    // dock normally (inserts a new row)
    DockControlBar(pBar, nDockBarID);

    // delete the new row (the bar pointer and the row end mark)
    pDockBar->m_arrBars.RemoveAt(pDockBar->m_arrBars.GetSize() - 1);
    pDockBar->m_arrBars.RemoveAt(pDockBar->m_arrBars.GetSize() - 1);

    // find the target bar
    for (int i = 0; i < pDockBar->m_arrBars.GetSize(); i++)
    {
        void* p = pDockBar->m_arrBars[i];
        if (p == pTargetBar) // and insert the new bar after it
            pDockBar->m_arrBars.InsertAt(i + 1, pBar);
    }

    // move the new bar into position
    CRect rBar;
    pTargetBar->GetWindowRect(rBar);
    rBar.OffsetRect(bHorz ? 1 : 0, bHorz ? 0 : 1);
    pBar->MoveWindow(rBar);
}

LRESULT MainFrame::OnHelpTopic(WPARAM wParam, LPARAM lParam)
{
	m_wndHelpCtrlBar.SendMessageToDescendants(UM_GOTO_HELP_TOPIC, wParam, lParam, TRUE, TRUE);

	return TRUE;
}

void MainFrame::SetMessageText(LPCTSTR lpszText, COLORREF crColour)
{
	m_wndStatusBar.SetPaneText(0, lpszText, TRUE, crColour);
}

