/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/ConfirmAdjustDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// ConfirmAdjustDialog.cpp : implementation file
//

#include "app/scada/Inspector_Panel/src/stdafx.h"
#include "app/scada/Inspector_Panel/src/inspectorpanel.h"
#include "app/scada/Inspector_Panel/src/ConfirmAdjustDialog.h"
#include "bus/generic_gui/src/HelpLauncher.h" //TD15124 azenitha

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfirmAdjustDialog dialog


CConfirmAdjustDialog::CConfirmAdjustDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CConfirmAdjustDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfirmAdjustDialog)
	m_equipment = _T("");
	m_dataPoint = _T("");
	m_newValue = _T("");
	//}}AFX_DATA_INIT
}


void CConfirmAdjustDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfirmAdjustDialog)
	DDX_Text(pDX, IDC_EQUIPMENT, m_equipment);
	DDX_Text(pDX, IDC_DATA_POINT, m_dataPoint);
	DDX_Text(pDX, IDC_NEW_VALUE, m_newValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfirmAdjustDialog, CDialog)
	//{{AFX_MSG_MAP(CConfirmAdjustDialog)
		// NOTE: the ClassWizard will add message map macros here
	ON_BN_CLICKED(ID_HELP_CA, OnHelpButton) //TD15124 azenitha
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfirmAdjustDialog message handlers

//TD15124 azenitha++
void CConfirmAdjustDialog::OnHelpButton() 
{
	TA_Base_Bus::HelpLauncher::getInstance().displayHelp(true);
}
//TD15124 ++azenitha