/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/LimitDialog.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the dialog which displays the equiment information tab shown
  * by the inspector panel.  It allows the operator to view the values of the data points.
  */

#include "app/scada/Inspector_Panel/src/stdafx.h"

#include "app/scada/inspector_panel/src/InspectorPanel.h"
#include "app/scada/inspector_panel/src/LimitDialog.h"

#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
	/////////////////////////////////////////////////////////////////////////////
	// CLimitDialog dialog


	CLimitDialog::CLimitDialog(CWnd* pParent /*=NULL*/)
	:	CDialog(CLimitDialog::IDD, pParent), m_limitList (m_limitInfo, m_limitDetailsList), 
		m_heightDifference(0), m_accessAllowed(false), m_originalHeight (0)
	{
		FUNCTION_ENTRY ("CLimitDialog()");

		//{{AFX_DATA_INIT(CLimitDialog)
			// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT

		FUNCTION_EXIT;
	}

	void CLimitDialog::setEquipment(CEquipment* equipment)
	{
		FUNCTION_ENTRY ("setEquipment()");
		m_equipment = equipment;
		m_limitDetailsList.setEquipment(equipment);
		m_limitList.setEquipment(equipment);		
		
		FUNCTION_EXIT;
	}
	void CLimitDialog::displayDataPoints()
	{
	}
	void CLimitDialog::DoDataExchange(CDataExchange* pDX)
	{
		FUNCTION_ENTRY ("DoDataExchange()");

		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CLimitDialog)
		DDX_Control(pDX, IDC_LIMIT_INFO, m_limitInfo);
		//}}AFX_DATA_MAP

		FUNCTION_EXIT;
	}


	BEGIN_MESSAGE_MAP(CLimitDialog, CDialog)
		//{{AFX_MSG_MAP(CLimitDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_INFO_DATAPOINTLIST, OnItemchangedInfoDatapointlist)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CLimitDialog message handlers

	BOOL CLimitDialog::OnInitDialog() 
	{
		FUNCTION_ENTRY ("OnInitDialog()");

		CDialog::OnInitDialog();

		// Subclass the DataPointList
		m_limitList.SubclassDlgItem(IDC_INFO_DATAPOINTLIST,this);
		m_limitDetailsList.SubclassDlgItem(IDC_LIMITS_DETAILSLIST,this);


		FUNCTION_EXIT;
		
		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	//wenching++ (TD12132)
	void CLimitDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
	{
		FUNCTION_ENTRY ("OnShowWindow()");

		CDialog::OnShowWindow(bShow, nStatus);
		
		// TODO: Add your message handler code here

		if (bShow)
		{
			m_limitList.refreshDataPoints();
		}

		FUNCTION_EXIT;
	}
	//++wenching (TD12132)


	void CLimitDialog::updateDataPoint(unsigned long key, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataPoints()");

		m_limitList.updateDataPoint(key, updateType);
		m_limitDetailsList.updateDataPoint(key, updateType);

		FUNCTION_EXIT;
	}

	void CLimitDialog::updateDataNode(TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataNode()");

		m_limitList.updateDataNode(updateType);

		FUNCTION_EXIT;
	}

	bool CLimitDialog::isUnsavedChangeOnTab()
	{
		FUNCTION_ENTRY ("isUnsavedChangeOnTab()");
		FUNCTION_EXIT;

		return false;
	}

	bool CLimitDialog::isOkToChangeTab()
	{
		FUNCTION_ENTRY ("isOkToChangeTab()");
		FUNCTION_EXIT;

		return true;
	}

	void CLimitDialog::getWindowRect(CRect* rect)
	{
		GetWindowRect(rect);
	}

	unsigned int CLimitDialog::getMinimumHeight()
	{
		if (m_heightDifference == 0)
		{
			CRect listRect, dialogRect;
			m_limitList.GetWindowRect(&listRect);
			GetWindowRect(&dialogRect);

			listRect.NormalizeRect();
			dialogRect.NormalizeRect();

			m_heightDifference = dialogRect.Height() - listRect.Height();
		}

		if (m_originalHeight == 0)
		{
			CRect dialogRect;
			GetWindowRect(&dialogRect);
			dialogRect.NormalizeRect();

			m_originalHeight = dialogRect.Height();
		}

		//unsigned int nItem = m_limitList.GetItemCount();
//		unsigned int nItem = m_equipment->getLimitIndex().size();
		unsigned int nItem = 20;

		unsigned int height = m_heightDifference;

		if (nItem > 0)
		{
			if (nItem > 20)
			{
				nItem = 20;
			}

			CRect cellRect;
			m_limitList.GetSubItemRect(0, 0, LVIR_LABEL, cellRect);

			cellRect.NormalizeRect();

//			height += cellRect.Height()*(nItem+3);
			height += cellRect.Height()*(nItem+1)+9; // remove half line
		}
		else
		{
			height = m_originalHeight;
		}

		return height;
	}		

	void CLimitDialog::resize(unsigned int height)
	{
		// TD14391.2
		// When the dialog bottom is extended, the IDC_LIMIT_INFO need not to be moved.
		CRect listRect, detailsListRect, dialogRect;
		m_limitList.GetWindowRect(&listRect);
		m_limitDetailsList.GetWindowRect(&detailsListRect); 
		GetWindowRect(&dialogRect);

		listRect.NormalizeRect();
		detailsListRect.NormalizeRect(); 
		dialogRect.NormalizeRect();

		int diff = height - dialogRect.Height();

		// the 4th parameter of InflateRect can be positive or negative, so need not deal with the signal of diff.
		listRect.InflateRect(0, 0, 0, diff);
		dialogRect.InflateRect(0, 0, 0, diff);
		detailsListRect.InflateRect(0, 0, 0, diff);

		m_limitList.SetWindowPos(NULL, 0, 0, listRect.Width(), listRect.Height(), SWP_NOMOVE|SWP_NOZORDER);
		m_limitDetailsList.SetWindowPos(NULL, 0, 0, detailsListRect.Width(), detailsListRect.Height(), SWP_NOMOVE|SWP_NOZORDER); // TD14391
		SetWindowPos(NULL, 0, 0, dialogRect.Width(), dialogRect.Height(), SWP_NOMOVE|SWP_NOZORDER);
	}

	void CLimitDialog::updatePermissions(/*TA_Base_Bus::RightsLibrary* rightsMgr*/)
	{
		m_limitList.updatePermissions(true/*m_accessAllowed*/);
	}

	LRESULT CLimitDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{				
		if ((message == WM_COMMAND) && ((wParam == IDOK) || (wParam == IDCANCEL)))
		{
			return 0;
		}
		else
		{
			return CDialog::WindowProc(message, wParam, lParam);
		}
	}


	void CLimitDialog::OnItemchangedInfoDatapointlist(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

		// TODO: Add your control notification handler code here

		if ( -1 != pNMListView->iItem )
		{
			m_limitList.GetCellType ( &m_limitList, pNMListView->iItem, pNMListView->iSubItem );
		}
		
		*pResult = 0;
	}

	bool CLimitDialog::isAnyDataPointsDisplayedInTab() const
	{
		// check if any data point is displayed in the list control
		return m_limitList.isAnyDataPointsDisplayedOnScreen();
	}

	void CLimitDialog::resetList()
	{

	}
}
