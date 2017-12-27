/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/ConfirmTagDialog.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the dialog which confirms that the tag is to be updated.
  * It provides details of the tag and the new setting.
  * The return from DoModal() is used to indicate if the update should proceed (IDOK) or
  * be cancelled (IDCANCEL).
  */

#include "app/scada/Inspector_Panel/src/stdafx.h"
#include "app/scada/Inspector_Panel/src/InspectorPanel.h"
#include "app/scada/Inspector_Panel/src/ConfirmTagDialog.h"
#include "bus/generic_gui/src/HelpLauncher.h" //TD15124 azenitha

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
	/////////////////////////////////////////////////////////////////////////////
	// CConfirmTagDialog dialog


	CConfirmTagDialog::CConfirmTagDialog(CWnd* pParent /*=NULL*/)
		: CDialog(CConfirmTagDialog::IDD, pParent)
	{
		//{{AFX_DATA_INIT(CConfirmTagDialog)
		m_equipment = _T("");
		m_tag = _T("");
		m_inhibit = _T("");
		//}}AFX_DATA_INIT
	}


	void CConfirmTagDialog::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CConfirmTagDialog)
		DDX_Text(pDX, IDC_EQUIPMENT, m_equipment);
		DDX_Text(pDX, IDC_TAG, m_tag);
		DDX_Text(pDX, IDC_INHIBIT, m_inhibit);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CConfirmTagDialog, CDialog)
		//{{AFX_MSG_MAP(CConfirmTagDialog)
			// NOTE: the ClassWizard will add message map macros here
		ON_BN_CLICKED(ID_HELP_CT, OnHelpButton) //TD15124 azenitha
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CConfirmTagDialog message handlers

	//TD15124 azenitha++
	void CConfirmTagDialog::OnHelpButton() 
	{
		TA_Base_Bus::HelpLauncher::getInstance().displayHelp(true);
	}
	//TD15124 ++azenitha
}
