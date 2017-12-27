/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/InhibitDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// InhibitDialog.cpp : implementation file
//

#include "app/scada/Inspector_Panel/src/stdafx.h"
#include "app/scada/inspector_panel/src/InspectorPanel.h"
#include "app/scada/inspector_panel/src/InhibitDialog.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/rights_library/src/RightsLibrary.h"

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
	// CInhibitDialog dialog


	CInhibitDialog::CInhibitDialog(CWnd* pParent /*=NULL*/)
		: CDialog(CInhibitDialog::IDD, pParent), m_heightDifference(0), m_originalHeight (0)
	{
		FUNCTION_ENTRY ("CInhibitDialog()");

		//{{AFX_DATA_INIT(CInhibitDialog)
			// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT

		FUNCTION_EXIT;
	}


	void CInhibitDialog::DoDataExchange(CDataExchange* pDX)
	{
		FUNCTION_ENTRY ("DoDataExchange()");

		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CInhibitDialog)
			// NOTE: the ClassWizard will add DDX and DDV calls here
		//}}AFX_DATA_MAP

		FUNCTION_EXIT;
	}


	BEGIN_MESSAGE_MAP(CInhibitDialog, CDialog)
		//{{AFX_MSG_MAP(CInhibitDialog)
		ON_WM_SHOWWINDOW()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CInhibitDialog message handlers

	BOOL CInhibitDialog::OnInitDialog() 
	{
		FUNCTION_ENTRY ("OnInitDialog()");

		CDialog::OnInitDialog();
		
		// TODO: Add extra initialization here

		m_list.SubclassDlgItem(IDC_INHIBIT_DATAPOINTLIST,this);

		FUNCTION_EXIT;
		
		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	void CInhibitDialog::setEquipment(CEquipment* equipment)
	{
		FUNCTION_ENTRY ("setEquipment()");

		m_equipment = equipment;
		m_list.setEquipment(equipment);

		FUNCTION_EXIT;
	}
	void CInhibitDialog::displayDataPoints()
	{
		FUNCTION_ENTRY ("displayDataPoints()");
		m_list.displayDataPoints();
		FUNCTION_EXIT;
	}
	void CInhibitDialog::updateDataPoint(unsigned long key, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataPoints()");

		m_list.updateDataPoint(key, updateType);

		FUNCTION_EXIT;
	}

	void CInhibitDialog::updateDataNode(TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataNode()");

		m_list.updateDataNode(updateType);

		FUNCTION_EXIT;
	}

	bool CInhibitDialog::isUnsavedChangeOnTab()
	{
		FUNCTION_ENTRY ("isUnsavedChangeOnTab()");
		FUNCTION_EXIT;

		return false;
	}

	bool CInhibitDialog::isOkToChangeTab()
	{
		FUNCTION_ENTRY ("isOkToChangeTab()");
		FUNCTION_EXIT;

		return true;
	}

	void CInhibitDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
	{
		FUNCTION_ENTRY ("OnShowWindow()");

		CDialog::OnShowWindow(bShow, nStatus);
		
		// TODO: Add your message handler code here

		if (bShow)
		{
			m_list.refreshDataPoints();
		}

		FUNCTION_EXIT;
	}

	void CInhibitDialog::getWindowRect(CRect* rect)
	{
		GetWindowRect(rect);
	}

	unsigned int CInhibitDialog::getMinimumHeight()
	{
		if (m_heightDifference == 0)
		{
			CRect listRect, dialogRect;
			m_list.GetWindowRect(&listRect);
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

//		unsigned int nItem = m_list.GetItemCount();
		
//		unsigned int nItem = m_equipment->getWriteableIndex().size() + m_equipment->getNotWriteableIndex().size();
		unsigned int nItem = 20;


		unsigned int height = m_heightDifference;

		if (nItem > 0)
		{
			if (nItem > 20)
			{
				nItem = 20;
			}

			CRect cellRect;
			m_list.GetSubItemRect(0, 0, LVIR_LABEL, cellRect);

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

	void CInhibitDialog::resize(unsigned int height)
	{
		CRect listRect, dialogRect;
		m_list.GetWindowRect(&listRect);
		GetWindowRect(&dialogRect);

		listRect.NormalizeRect();
		dialogRect.NormalizeRect();

		int diff = height - dialogRect.Height();

		if (diff > 0)
		{
			listRect.InflateRect(0, 0, 0, diff);
			dialogRect.InflateRect(0, 0, 0, diff);
		}
		else
		{
			listRect.DeflateRect(0, 0, 0, -diff);
			dialogRect.DeflateRect(0, 0, 0, -diff);
		}

		m_list.SetWindowPos(NULL, 0, 0, listRect.Width(), listRect.Height(), SWP_NOMOVE|SWP_NOZORDER);
		SetWindowPos(NULL, 0, 0, dialogRect.Width(), dialogRect.Height(), SWP_NOMOVE|SWP_NOZORDER);
	}

	void CInhibitDialog::updatePermissions(/*TA_Base_Bus::RightsLibrary* rightsMgr*/)
	{
/*		m_accessAllowed = false;

		TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        std::string reason;

        std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

		try
		{
			m_accessAllowed = rightsMgr->isActionPermittedOnResource(
											sessionId, 
											resourceId, 
											TA_Base_Bus::aca_IP_MAINT_TAB_ACCESS, 
											reason, 
											decisionModule);     
		}
		catch (...)
		{
		}
*/
		m_list.updatePermissions(true/*m_accessAllowed*/);
	}

	LRESULT CInhibitDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{				
		if ((message == WM_COMMAND) && ((wParam == IDOK) || (wParam == IDCANCEL)))
		{
			return 0;
		} else
			return CDialog::WindowProc(message, wParam, lParam);
	}

	bool CInhibitDialog::isAnyDataPointsDisplayedInTab() const
	{
		// check if any data point is displayed in the list control
		return m_list.isAnyDataPointsDisplayedOnScreen();
	}

	void CInhibitDialog::resetList()
	{

	}
}
