/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/DataNodeController/src/DataNodeControllerDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "stdafx.h"
#include "DataNodeController.h"
#include "DataNodeControllerDlg.h"

// displays the Transactive About Box
#include "bus\generic_gui\src\HelpLauncher.h"

#include "bus/scada/scada_tree/src/EntityTreeItem.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

using namespace TA_Base_Core;
using namespace TA_Base_Bus;

namespace TA_Base_App
{
	CDataNodeControllerDlg::CDataNodeControllerDlg(TA_Base_Bus::IGUIAccess& genericGUICallback)
		: TransActiveDialog(genericGUICallback, CDataNodeControllerDlg::IDD)
	{
		// If necessary this code can go in OnInitDialog before the call to TransActiveDialog::OnInitDialog()
		TA_Base_Bus::ResizingProperties properties;
		properties.canMaximise	= false;
		properties.maxHeight	= -1;
		properties.maxWidth		= -1;
		properties.minWidth		= -1;
		properties.minHeight	= -1;
		setResizingProperties(properties);
	}

	void CDataNodeControllerDlg::DoDataExchange(CDataExchange* pDX)
	{
		TransActiveDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CDataNodeControllerDlg)
	DDX_Control(pDX, IDC_TREE, m_ScadaTree);
	DDX_Control(pDX, IDC_DNC_TAB, m_TabControl);
	//}}AFX_DATA_MAP
	}

	BEGIN_MESSAGE_MAP(CDataNodeControllerDlg, TransActiveDialog)
		//{{AFX_MSG_MAP(CDataNodeControllerDlg)
		ON_COMMAND(ID_HELP_ABOUTDATANODECONTROLLER, OnHelpAboutDataNodeController)
	ON_BN_CLICKED(IDC_CLOSE, OnCloseClicked)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE, OnSelchangingTree)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	BOOL CDataNodeControllerDlg::OnInitDialog()
	{
		TransActiveDialog::OnInitDialog();

		HICON appIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_DNC));
		SetIcon(appIcon, TRUE);		

		EnableToolbar(true);

		CButton *wnd = (CButton *)GetDlgItem(IDC_STATUS_EDIT);
		wnd->SetWindowText("Feb 27 11:11AM - CDBG/ECS/ZON successfully Alarm Inhibited");
		
		return TRUE;  
	}


	void CDataNodeControllerDlg::OnHelpAboutDataNodeController() 
	{
		TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();	
	}


	void CDataNodeControllerDlg::OnCloseClicked() 
	{
		::PostQuitMessage(0);
	}

	void CDataNodeControllerDlg::OnSelchangingTree(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
		
		*pResult = 0;
	}


	void CDataNodeControllerDlg::CreateToolbar()
	{
		// Create toolbar at the top of the dialog window
		if (!m_WndToolBar.CreateEx(this, TBSTYLE_FLAT , WS_CHILD | WS_VISIBLE | CBRS_TOP | 
										CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
			!m_WndToolBar.LoadToolBar(IDR_DNC_TOOLBAR))
		{			
			TRACE0("Failed to create toolbar\n");
		}

		// Add text to the buttons
		// Any exception from these functions will fall through and be caught by
		// the calling member
		AddTextToButton(ID_TOOLBAR_INSPECT,"Inspect");
		AddTextToButton(ID_TOOLBAR_HELP,	"Help");
		
		m_WndToolBar.GetToolBarCtrl().SetButtonSize(CSize(69,40));	
	}

	void CDataNodeControllerDlg::AddTextToButton(UINT buttonId, CString buttonLabel)
	{
		int index = m_WndToolBar.CommandToIndex(buttonId);
		if (index >= 0)
			m_WndToolBar.SetButtonText(index, buttonLabel);					
	}
	
	void CDataNodeControllerDlg::EnableToolbar(bool bEnable)
	{
		CPoint ptOffset;

		ptOffset.x = 0;
		
		// Check for initial enabling
		if ((bEnable) && (!m_WndToolBar.IsWindowVisible()))
		{
			// Creating - so move all items down
			ptOffset.y = m_ToolBarHeight;
		}
		else if ((!bEnable) && (m_WndToolBar.IsWindowVisible()))
		{
			// Removing so move all items up
			ptOffset.y = m_ToolBarHeight * -1;
		}
		else
		{
			// No change
			return;
		}

		// Move the child controls
		CRect rcChild;
		CWnd* pwndChild = GetWindow(GW_CHILD);
		while (pwndChild)
		{
			if ((pwndChild != &m_WndToolBar))
			{
				pwndChild->GetWindowRect(rcChild);
				ScreenToClient(rcChild);
				rcChild.OffsetRect(ptOffset);
			
				pwndChild->MoveWindow(rcChild, FALSE);
			}
			pwndChild = pwndChild->GetNextWindow();
		}

		// Adjust the dialog size
		CRect rcWindow;
		GetWindowRect(rcWindow);
		rcWindow.bottom += ptOffset.y;
		MoveWindow(rcWindow, TRUE);

		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	}


	void CDataNodeControllerDlg::CalculateToolbarHeight()
	{
		CRect rcClientStart;
		CRect rcClientNow;

		GetClientRect(rcClientStart);
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, rcClientNow);

		m_ToolBarHeight   = rcClientNow.top;
	}


	int CDataNodeControllerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
	{
		if (TransActiveDialog::OnCreate(lpCreateStruct) == -1)
			return -1;
						
		CreateToolbar();
		CalculateToolbarHeight();	

		return 0;
	}

	void CDataNodeControllerDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
	{		
		TransActiveDialog::OnShowWindow(bShow, nStatus);	
		m_ScadaTree.Expand(m_ScadaTree.GetRootItem(), TVE_EXPAND);
		m_TabControl.ShowWindow(SW_SHOW);
	}



	void CDataNodeControllerDlg::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
		
		if (pNMTreeView->itemNew.hItem)
		{				
			TA_Base_Bus::CEntityTreeItem* selectedItem = dynamic_cast<TA_Base_Bus::CEntityTreeItem*>(m_ScadaTree.getSelectedItem());
			if (selectedItem)
			{			
				CString displayName;
				IEntityData *entityData = selectedItem->getEntityData();
				DataNodeEntityData *dnEntity = NULL;
				if (entityData->getType() == DataNodeEntityData::getStaticType())				
					dnEntity = dynamic_cast<DataNodeEntityData *>(entityData);				
				
				if (dnEntity)
					displayName = dnEntity->getLabel().c_str();				
				
				if (displayName.IsEmpty())
					displayName = "Bad Configuration *";

				displayName.Insert(0, " ");
				
				CWnd *label = GetDlgItem(IDC_DATA_NODE_LABEL);
				label->SetWindowText(displayName);			
			}
		}

		*pResult = 0;
	}

} // TA_Base_App
