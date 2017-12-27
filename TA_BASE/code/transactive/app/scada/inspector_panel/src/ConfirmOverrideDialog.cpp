/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/ConfirmOverrideDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "app/scada/Inspector_Panel/src/stdafx.h"

#include "app/scada/Inspector_Panel/src/InspectorPanel.h"
#include "app/scada/Inspector_Panel/src/ConfirmOverrideDialog.h"

#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/generic_gui/src/HelpLauncher.h" //TD15124 azenitha

#include "core/exceptions/src/ScadaProxyException.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define NEW_VALUE_CONTROL			5001

namespace TA_Base_App
{
	/////////////////////////////////////////////////////////////////////////////
	// CConfirmOverrideDialog dialog


	
	CConfirmOverrideDialog::CConfirmOverrideDialog(CWnd* pParent, int iPoint, CEquipment *equipment, CString currentValue)
		: CDialog(CConfirmOverrideDialog::IDD, pParent), m_equipment(equipment), m_iPoint(iPoint), m_overrideValue("")
	{
		//{{AFX_DATA_INIT(CConfirmOverrideDialog)
		//}}AFX_DATA_INIT
		m_currentValue = currentValue;
	}

	
	void CConfirmOverrideDialog::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CConfirmOverrideDialog)
		//}}AFX_DATA_MAP
	}

	BEGIN_MESSAGE_MAP(CConfirmOverrideDialog, CDialog)
		//{{AFX_MSG_MAP(CConfirmOverrideDialog)
		//}}AFX_MSG_MAP
		ON_EN_CHANGE(NEW_VALUE_CONTROL, OnChangeNewValue)
		ON_CBN_SELCHANGE(NEW_VALUE_CONTROL, OnChangeNewValue)
		ON_BN_CLICKED(ID_HELP_CO, OnHelpButton) //TD15124 azenitha
	END_MESSAGE_MAP()
	
	/////////////////////////////////////////////////////////////////////////////
	// CConfirmOverrideDialog message handlers

	//TD15124 azenitha++
	void CConfirmOverrideDialog::OnHelpButton() 
	{
		TA_Base_Bus::HelpLauncher::getInstance().displayHelp(true);
	}
	//TD15124 ++azenitha

	BOOL CConfirmOverrideDialog::OnInitDialog() 
	{
		CDialog::OnInitDialog();

		TA_Base_Bus::DataNodeProxySmartPtr& dataNode = m_equipment->getDataNode();

		std::string equipment = dataNode->getAssetName().c_str();
		equipment += "\r\n";
		equipment += dataNode->getDescription().c_str();
		GetDlgItem(IDC_EQUIPMENT)->SetWindowText(equipment.c_str());

		TA_Base_Bus::DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(m_iPoint);

		GetDlgItem(IDC_DATA_POINT)->SetWindowText(dataPoint->getDescription().c_str());
		
		CRect newValueRect;

		GetDlgItem(IDC_NEW_VALUE)->GetWindowRect(&newValueRect);
		ScreenToClient(&newValueRect);

		GetDlgItem(IDOK)->EnableWindow(false);

		if ( CString("") != m_currentValue )
		{
			// Check if boolean, text or combo
			if ( dataPoint->isBoolean()||dataPoint->isDerived()||dataPoint->isNumber() )
			{
				m_valueEdit.Create(WS_VISIBLE | WS_BORDER, newValueRect, this, NEW_VALUE_CONTROL);
				m_valueEdit.SetWindowText( m_currentValue );
				m_valueEdit.SetFont(GetFont(), FALSE);
				m_valueEdit.SetFocus();
				m_valueEdit.ShowWindow(SW_SHOW);

				GetDlgItem(IDOK)->EnableWindow(true);

				return FALSE;
			}
		} 
		else
		{
			// Check if boolean, text or combo
			if (dataPoint->isBoolean())
			{
				// Combo box
				// Give space to scroll
				newValueRect.bottom += 200;

				m_valueComboBox.Create(WS_CHILD | CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL, newValueRect, this, NEW_VALUE_CONTROL);

				try
				{
					m_valueComboBox.AddString(dataPoint->getBooleanLabel(true).c_str());
				}
				catch(...)
				{
				}

				try
				{
					m_valueComboBox.AddString(dataPoint->getBooleanLabel(false).c_str());
				}
				catch(...)
				{
				}

				m_valueComboBox.SetFont(GetFont(), FALSE);
				m_valueComboBox.SetCurSel(-1);
				m_valueComboBox.SetFocus();
				m_valueComboBox.ShowWindow(SW_SHOW);

				return FALSE;
			}
			else if (dataPoint->isDerived())
			{
				// Combo box
				// Give space to scroll
				newValueRect.bottom += 200;

				m_valueComboBox.Create(WS_CHILD | CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL, newValueRect, this, NEW_VALUE_CONTROL);

				try
				{
					
					TA_Base_Bus::DpvEnumLabelsMap derivedStates = dataPoint->getDerivedStatesAsMap();
					
					TA_Base_Bus::DpvEnumLabelsMap::iterator it;

					for (it = derivedStates.begin(); it != derivedStates.end(); it++)
					{
						m_valueComboBox.AddString((*it).second.c_str());
					}	
				}
				catch (TA_Base_Core::ScadaProxyException)
				{
				}

				m_valueComboBox.SetFont(GetFont(), FALSE);
				m_valueComboBox.SetCurSel(-1);
				m_valueComboBox.SetFocus();
				m_valueComboBox.ShowWindow(SW_SHOW);

				return FALSE;
			}
			else if (dataPoint->isNumber())
			{
				// Number edit box
				m_valueNumericEdit.Create(WS_VISIBLE | WS_BORDER, newValueRect, this, NEW_VALUE_CONTROL);
				m_valueNumericEdit.SetFont(GetFont(), FALSE);
				m_valueNumericEdit.SetFocus();
				m_valueNumericEdit.ShowWindow(SW_SHOW);

				return FALSE;
			}
			else if (dataPoint->isText())
			{
				// Text edit box
				m_valueEdit.Create(WS_VISIBLE | WS_BORDER, newValueRect, this, NEW_VALUE_CONTROL);
				m_valueEdit.SetFont(GetFont(), FALSE);
				m_valueEdit.SetFocus();
				m_valueEdit.ShowWindow(SW_SHOW);

					return FALSE;
			}
		}

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	std::string CConfirmOverrideDialog::getOverrideValue()
	{
		return m_overrideValue;
	}

	void CConfirmOverrideDialog::OnOK() 
	{
		// TODO: Add extra validation here
		
		CString str;
		if (CString("") == m_currentValue)
		{
			TA_Base_Bus::DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(m_iPoint);

			// Check if boolean, text or combo
			if (dataPoint->isBoolean() || dataPoint->isDerived())
			{
				// Combo box
				m_valueComboBox.GetLBText(m_valueComboBox.GetCurSel(), str);
			}
			else if (dataPoint->isNumber())
			{
				// Number edit box
				m_valueNumericEdit.GetWindowText(str);
			}
			else if (dataPoint->isText())
			{
				// Text edit box
				m_valueEdit.GetWindowText(str);
			}

			m_overrideValue = str;
		}
		else
		{
			m_valueEdit.GetWindowText(str);
			m_overrideValue = str;
		}
		
		CDialog::OnOK();
	}

	void CConfirmOverrideDialog::OnChangeNewValue() 
	{
		// TODO: If this is a RICHEDIT control, the control will not
		// send this notification unless you override the CDialog::OnInitDialog()
		// function and call CRichEditCtrl().SetEventMask()
		// with the ENM_CHANGE flag ORed into the mask.
		
		CString str;
		if (CString("") == m_currentValue)
		{
			TA_Base_Bus::DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(m_iPoint);

			// Check if boolean, text or combo
			if (dataPoint->isBoolean() || dataPoint->isDerived())
			{
				// Combo box
				GetDlgItem(IDOK)->EnableWindow(m_valueComboBox.GetCurSel() >= 0);
			}
			else if (dataPoint->isNumber())
			{
				// Number edit box
				m_valueNumericEdit.GetWindowText(str);

				GetDlgItem(IDOK)->EnableWindow(str.GetLength() != 0);
			}
			else if (dataPoint->isText())
			{
				// Text edit box
				m_valueEdit.GetWindowText(str);

				GetDlgItem(IDOK)->EnableWindow(str.GetLength() != 0);
		
			}
		}
		else
		{
			GetDlgItem(IDOK)->EnableWindow(TRUE);

		}
		
	}
	
}
