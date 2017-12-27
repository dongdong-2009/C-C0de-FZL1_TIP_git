/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/ConfirmCommandDialog.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/01/07 14:43:16 $
  * Last modified by:  $Author: yong.liu $
  * 
  * This class manages the dialog which confirms that the data point is to be updated.
  * It provides details of the data point and the new value.
  * The return from DoModal() is used to indicate if the update should proceed (IDOK) or
  * be cancelled (IDCANCEL).
  */

#include "app/scada/Inspector_Panel/src/stdafx.h"
#include "app/scada/Inspector_Panel/src/InspectorPanel.h"
#include "app/scada/Inspector_Panel/src/InputNewValueDialog.h"
#include "bus/generic_gui/src/HelpLauncher.h" //TD15124 azenitha

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
	/////////////////////////////////////////////////////////////////////////////
	// CInputNewValueDialog dialog


	CInputNewValueDialog::CInputNewValueDialog(CWnd* pParent /*=NULL*/)
		: CDialog(CInputNewValueDialog::IDD, pParent)
	{
		//{{AFX_DATA_INIT(CConfirmCommandDialog)
		m_equipment = _T("");
		m_newValue = _T("");
		m_dataPoint = _T("");
		m_unit = _T("");

		m_bListValue = false;
		m_isTextValue = false;
		//}}AFX_DATA_INIT
	}


	void CInputNewValueDialog::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CConfirmCommandDialog)
		DDX_Text(pDX, IDC_EQUIPMENT, m_equipment);
		//DDX_Text(pDX, IDC_INPUT_NEW_VALUE, m_newValue);
		DDX_Text(pDX, IDC_COMBOBOX_VALUE, m_dropListValue);
		DDX_Text(pDX, IDC_DATA_POINT, m_dataPoint);
		DDX_Text(pDX, IDC_VALUE_UNIT, m_unit);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CInputNewValueDialog, CDialog)
		//{{AFX_MSG_MAP(CInputNewValueDialog)
			// NOTE: the ClassWizard will add message map macros here
		ON_BN_CLICKED(IDOK, OnBnClickedOk)
		ON_BN_CLICKED(ID_HELP_CC, OnHelpButton) //TD15124 azenitha
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CConfirmCommandDialog message handlers

	//TD15124 azenitha++
	void CInputNewValueDialog::OnHelpButton() 
	{
		TA_Base_Bus::HelpLauncher::getInstance().displayHelp(true);
	}
	//TD15124 ++azenitha

	void CInputNewValueDialog::OnBnClickedOk() 
	{
		m_newValueEdit.GetWindowText(m_newValue);
		CDialog::OnOK();
	}

	BOOL CInputNewValueDialog::OnInitDialog()
	{
		CDialog::OnInitDialog();

		m_newValueEdit.SubclassDlgItem(IDC_INPUT_NEW_VALUE, (CWnd*)(this));

		if (m_bListValue)
		{
			CComboBox * lBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VALUE);
			for (std::map<unsigned long, std::string>::const_iterator it= m_valueMap.begin();
				m_valueMap.end() != it; ++it)
			{
				lBox->AddString(it->second.c_str());
			}
			m_newValueEdit.ShowWindow(SW_HIDE);
			GetDlgItem(IDC_COMBOBOX_VALUE)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_VALUE_UNIT)->ShowWindow(SW_HIDE);
		}
		else if (m_isTextValue)
		{
			m_newValueEdit.ShowWindow(SW_SHOW);
			m_newValueEdit.setNumeric(false);
			GetDlgItem(IDC_COMBOBOX_VALUE)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_VALUE_UNIT)->ShowWindow(SW_HIDE);
		}
		else
		{
			m_newValueEdit.ShowWindow(SW_SHOW);
			m_newValueEdit.setNumeric(true);
			GetDlgItem(IDC_COMBOBOX_VALUE)->ShowWindow(SW_HIDE);
		}
		return TRUE;

	}
	
	void CInputNewValueDialog::setValueList(std::map<unsigned long, std::string>& valueMap)
	{
		m_bListValue = true;
		m_valueMap = valueMap;
	}
}
