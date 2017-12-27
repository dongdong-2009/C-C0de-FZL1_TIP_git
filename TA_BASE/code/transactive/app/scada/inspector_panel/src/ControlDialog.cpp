/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/ControlDialog.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/07/05 16:59:46 $
  * Last modified by:  $Author: jinmin.huang $
  * 
  * This class manages the dialog which displays the control information tab shown
  * by the inspector panel.  It allows the operator to set the values of the data points.
  */

#include "app/scada/Inspector_Panel/src/stdafx.h"

#include <sstream>

#include "app/scada/inspector_panel/src/InspectorPanel.h"
#include "app/scada/inspector_panel/src/ConfirmCommandDialog.h"
#include "app/scada/inspector_panel/src/resource.h"
#include "app/scada/inspector_panel/src/ControlDialog.h"

#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"

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
	// CControlDialog dialog


	CControlDialog::CControlDialog(CWnd* pParent /*=NULL*/)
		: CDialog(CControlDialog::IDD, pParent), 
		 m_list(NULL),
		 m_isEoList(false),
		 m_heightDifference(0),
		 m_originalHeight (0),
		 m_equipment(NULL),
		 m_isInitilize(false)
	{
		FUNCTION_ENTRY ("CControlDialog()");

		//{{AFX_DATA_INIT(CControlDialog)
			// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT
		m_ImageList.Create(1,64,ILC_COLOR24,0,0);  
		m_eoImageList.Create(1,16,ILC_COLOR24,0,0);

		FUNCTION_EXIT;
	}


	void CControlDialog::DoDataExchange(CDataExchange* pDX)
	{
		FUNCTION_ENTRY ("DoDataExchange()");

		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CControlDialog)
			// NOTE: the ClassWizard will add DDX and DDV calls here
		//}}AFX_DATA_MAP

		FUNCTION_EXIT;
	}


	BEGIN_MESSAGE_MAP(CControlDialog, CDialog)
		//{{AFX_MSG_MAP(CControlDialog)
		ON_WM_SHOWWINDOW()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CControlDialog message handlers


	BOOL CControlDialog::OnInitDialog() 
	{
		FUNCTION_ENTRY ("OnInitDialog()");

		CDialog::OnInitDialog();
		
		FUNCTION_EXIT;

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	// refresh datapoints when the window was shown
	void CControlDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
	{
		FUNCTION_ENTRY ("OnShowWindow()");

		CDialog::OnShowWindow(bShow, nStatus);
		if(m_list)
		{
			if (bShow)
			{
				m_list->displayDataPoints();
			}
			else
			{
				::SendMessage(m_list->m_hWnd,WM_KILLFOCUS,0,0);
			}
		}
		FUNCTION_EXIT;
	}

	void CControlDialog::setEquipment(CEquipment* equipment)
	{
		FUNCTION_ENTRY ("setEquipment()");
		//first time set valid equipment,set the flag
		if(!m_isInitilize && NULL!=equipment)
		{
			m_isInitilize = true;
		}
		//not the first time,clear,quipment type may be diffrent(pscada and not pscada)
		else if(m_isInitilize && NULL!=equipment && m_equipment!=equipment)
		{
			bool preIsEolist = m_isEoList;
			m_isEoList = shouldUseEOListCtrl(equipment);
			if(m_isEoList != preIsEolist)
			{
				m_list->ShowWindow(SW_HIDE);
				m_list->UnsubclassWindow();
				delete m_list;
				m_list = NULL;
			}
		}
		if(NULL == m_list)
		{
			m_isEoList = shouldUseEOListCtrl(equipment);
			if (m_isEoList)
			{
				m_list = new CDropDownControlListCtrl();
				m_list->SubclassDlgItem(IDC_CONTROLLIST, this);
				
				m_list->SetImageList(&m_eoImageList, LVSIL_SMALL);
			}
			else
			{
				
				m_list = new CButtonControlListCtrl();
				m_list->SubclassDlgItem(IDC_CONTROLLIST,this);
				m_list->SetImageList(&m_ImageList,LVSIL_SMALL); 
			}
		}
		m_equipment = equipment;
		m_list->setEquipment(equipment);	
		if (m_list->m_hWnd)
		{
			m_list->ShowWindow(SW_SHOW);
		}
		FUNCTION_EXIT;
	}
	void CControlDialog::displayDataPoints()
	{
		FUNCTION_ENTRY ("setEquipment()");
		if(m_list)
		{
			m_list->displayDataPoints();
		}
		FUNCTION_EXIT;
	}
	void CControlDialog::updateDataPoint(unsigned long key, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataPoints()");
		if(m_list)
		{
			m_list->updateDataPoint(key, updateType);
		}
		FUNCTION_EXIT;
	}

	void CControlDialog::updateDataNode(TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataNode()");

		if(m_list)
		{
			m_list->updateDataNode(updateType);
		}
		FUNCTION_EXIT;
	}

	bool CControlDialog::isUnsavedChangeOnTab()
	{
		FUNCTION_ENTRY ("isUnsavedChangeOnTab()");
		FUNCTION_EXIT;

		return false;
	}

	bool CControlDialog::isOkToChangeTab()
	{
		FUNCTION_ENTRY ("isOkToChangeTab()");
		FUNCTION_EXIT;

		return true;
	}

	void CControlDialog::getWindowRect(CRect* rect)
	{
		GetWindowRect(rect);
	}

	unsigned int CControlDialog::getMinimumHeight()
	{
		/*if (m_isEoList)
		{
			if (m_heightDifference == 0)
			{
				CRect listRect, dialogRect;
				m_eoList.GetWindowRect(&listRect);
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

			unsigned int nItem = m_eoList.GetItemCount();
			unsigned int height = m_heightDifference;

			if (nItem > 0)
			{
				if (nItem > 20)
				{
					nItem = 20;
				}

				CRect cellRect;
				m_eoList.GetSubItemRect(0, 0, LVIR_LABEL, cellRect);

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
		else
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

			unsigned int nItem = m_list.GetItemCount();
			unsigned int height = m_heightDifference;

			if (nItem > 0)
			{
				if (nItem > 4)
				{
					nItem = 4;
				}

				CRect cellRect;
				m_list.GetSubItemRect(0, 0, LVIR_LABEL, cellRect);

				cellRect.NormalizeRect();

				//			height += cellRect.Height()*(nItem+3);
				height += cellRect.Height()*(nItem); // remove half line
			}
			else
			{
				height = m_originalHeight;
			}

			return height;

		}	*/
		return m_heightDifference;
	}		

	void CControlDialog::resize(unsigned int height)
	{
		CRect listRect, dialogRect;
		m_list->GetWindowRect(&listRect);
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

		m_list->SetWindowPos(NULL, 0, 0, listRect.Width(), listRect.Height(), SWP_NOMOVE|SWP_NOZORDER);
		SetWindowPos(NULL, 0, 0, dialogRect.Width(), dialogRect.Height(), SWP_NOMOVE|SWP_NOZORDER);
	}

	void CControlDialog::updatePermissions()
	{
		if(m_list)
		{
			m_list->updatePermissions(false);
		}
	}

	LRESULT CControlDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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

	bool CControlDialog::isAnyDataPointsDisplayedInTab() const
	{
		// check if any data point is displayed in the list control
		if(m_list)
		{
			return m_list->isAnyDataPointsDisplayedOnScreen();
		}
		return false;
	}

	void CControlDialog::resetList()
	{

	}
	bool CControlDialog::shouldUseEOListCtrl(CEquipment* equipment)
	{
		FUNCTION_ENTRY ("shouldUseEOListCtrl()");
		if (equipment != NULL)
		{
			std::vector<TA_Base_Bus::DataPointProxySmartPtr*>& dataPoints = equipment->getChildDataPointVector();
			std::vector<unsigned long>& vectorIndex = equipment->getWriteableIndex(); 
			std::vector<unsigned long>::iterator it;
			for (it = vectorIndex.begin(); it != vectorIndex.end(); ++it)
			{
				if ((*dataPoints[*it])->getPScadaFlag() || 
					(*dataPoints[*it])->isBoolean() ||
					((*dataPoints[*it])->isNumber() && !((*dataPoints[*it])->isDerived())))
				{
					FUNCTION_EXIT;
					return false;
				}	
			}

			FUNCTION_EXIT;
			return true;
		}
		else
		{
			FUNCTION_EXIT;
			return false;
		}
	}
}
