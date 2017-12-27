/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/DataPointSelectDlg.cpp $
  * @author:  Lizette Lingo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// DataPointSelectDlg.h : header file
//

// DataPointSelectDlg.cpp : implementation file
//

#include "app/trending/trend_viewer/src/StdAfx.h"
#include "app/trending/trend_viewer/src/historyviewer.h"
#include "app/trending/trend_viewer/src/DataPointSelectDlg.h"
#include "app/trending/trend_viewer/src/globals.h"

#include "app/trending/trend_viewer/src/TrendTreeItem.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataPointSelectDlg dialog
using namespace TA_Base_Core;

namespace TA_Base_App
{
	
	CDataPointSelectDlg::CDataPointSelectDlg( const std::string& itemName, const std::map<std::string, TA_Base_Core::ItemInfo>& selectedItems, IItemStore& itemStore, CWnd* pParent /*=NULL*/)
		: CDialog(CDataPointSelectDlg::IDD, pParent),
		m_selectedDpName(itemName),
		m_trendDpTree(&itemStore), // initialize the trend datapoint Tree 
		m_bDataCleared(false)
	{
		//{{AFX_DATA_INIT(CDataPointSelectDlg)
		// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Lizette: datapoint select dialog created");
		std::map<std::string, TA_Base_Core::ItemInfo>::const_iterator it = selectedItems.begin();
        for(; it != selectedItems.end(); ++it)
        {
            m_selectedItems[it->first] = it->second;
        }
	}
		
	void CDataPointSelectDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CDataPointSelectDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here	
		DDX_Control(pDX, IDC_EQUIPMENT_TREE, m_trendDpTree);
		DDX_Control(pDX, IDOK, m_addButton);
		DDX_Control(pDX, IDC_DATAPOINT_EDIT, m_datapointName);
		//}}AFX_DATA_MAP
	}
	
	
	BEGIN_MESSAGE_MAP(CDataPointSelectDlg, CDialog)
	//{{AFX_MSG_MAP(CDataPointSelectDlg)
	// NOTE: the ClassWizard will add message map macros here
	ON_NOTIFY(TVN_SELCHANGED, IDC_EQUIPMENT_TREE, OnSelChangedTreeItem)
	ON_BN_CLICKED(IDC_RESET, OnResetItem)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
		
	/////////////////////////////////////////////////////////////////////////////
	// CDataPointSelectDlg message handlers
		
	BOOL CDataPointSelectDlg::OnInitDialog() 
    {
        CDialog::OnInitDialog();
		
		m_datapointName.SetWindowText(m_selectedDpName.c_str());
		return TRUE;
	}

	void CDataPointSelectDlg::OnSelChangedTreeItem(NMHDR* pNMHDR, LRESULT* pResult)
	{
		FUNCTION_ENTRY( "OnClickEquipmentType" );

		NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
		//enable the OK button if the selected item is a valid datapoint
		bool isDatapointSelected = false;
		std::string tempSelectedName = "";		
		
		CObjectTreeItem* item = m_trendDpTree.getSelectedItem(); 		

		if (NULL != item)
		{
			// if the selected item has no children, then it's a valid datapoint
			if (!(item->hasChildren()))
			{
				//must cast to CTrendTreeItem to retrieve IEntityData info
				CTrendTreeItem* entityItem = dynamic_cast<CTrendTreeItem*>(item);
				if (NULL != entityItem)
				{					
					if (entityItem->isTrendedDatapointType())
					{
						try
						{
							//retrieve the name of the entity data
							tempSelectedName = entityItem->getEntityData()->getName();	
							
							// check if the datapoint name was previously selected
							std::map<std::string, TA_Base_Core::ItemInfo>::iterator selIt = m_selectedItems.find(tempSelectedName);						
							if (selIt == m_selectedItems.end())
							{
								//a valid datapoint for trending is selected
								//update the view
								m_datapointName.SetWindowText(tempSelectedName.c_str());
								isDatapointSelected = true;
								// since a valid datapoint got selected, then the 
								// Reset flag must be set to false.
								m_bDataCleared = false;
							}
						}
						catch (TA_Base_Core::TransactiveException& e)
						{
							LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
								"Transactive Exception: %s caught in AssetDialog::OnOK()!", e.what());    
						}
						catch (...)
						{
							LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
								"Unknown Exception caught in AssetDialog::OnOK()!");    
						}		
					}
				}				
			}
		}	 

		// if a valid datapoint was selected, enable Add
		// if the reset button was clicked prior to selecting an item in the list, enable Add		   
		m_addButton.EnableWindow(isDatapointSelected || m_bDataCleared);
				
		*pResult = 0;
		FUNCTION_EXIT;
	}

	void CDataPointSelectDlg::OnOK()
	{
		FUNCTION_ENTRY( "OnOK" );

		CString selectedDpName;
		m_datapointName.GetWindowText(selectedDpName);

		// save changes of the new datapoint name selected or 
		// if the datapoint to be trended is cleared
		m_selectedDpName = selectedDpName;
		CDialog::OnOK();
		FUNCTION_EXIT;
	}	

	void CDataPointSelectDlg::OnResetItem()
	{
		FUNCTION_ENTRY("OnResetItem");		
		// Reset flag must be set to true 
		// since the item name got cleared.
		m_bDataCleared = true;

		// assign empty string for the item name
		m_datapointName.SetWindowText(EMPTY_STRING);

		m_trendDpTree.SelectItem(m_trendDpTree.GetRootItem()); 

		// enable add button to save changes
		m_addButton.EnableWindow(TRUE);
		FUNCTION_EXIT;
	}
}
