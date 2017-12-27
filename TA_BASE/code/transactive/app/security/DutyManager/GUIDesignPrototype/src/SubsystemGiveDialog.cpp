/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/DutyManager/GUIDesignPrototype/src/SubsystemGiveDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// SubsystemGiveDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DutyManager.h"
#include "SubsystemGiveDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSubsystemGiveDialog dialog


CSubsystemGiveDialog::CSubsystemGiveDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSubsystemGiveDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSubsystemGiveDialog)
	//}}AFX_DATA_INIT
}


void CSubsystemGiveDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSubsystemGiveDialog)
	DDX_Control(pDX, IDC_LIST1, m_subsystemList);
	DDX_Control(pDX, IDC_COMBO1, m_siteCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSubsystemGiveDialog, CDialog)
	//{{AFX_MSG_MAP(CSubsystemGiveDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSubsystemGiveDialog message handlers

BOOL CSubsystemGiveDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int   iPos;
	
	// setup station list
	m_siteCombo.InsertString(0,"CVC");
	m_siteCombo.InsertString(0,"DBG");
	m_siteCombo.InsertString(0,"MSM");
	m_siteCombo.SetCurSel(1);

	// Load subsystem list
	m_subsystemList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_subsystemList.InsertColumn(0,"Subsystem",LVCFMT_LEFT,150);
	m_subsystemList.InsertColumn(1,"Operator",LVCFMT_LEFT,200);
	iPos = m_subsystemList.InsertItem(0,"Tunnel Lighting",0);
	       m_subsystemList.SetItemText(iPos,1,"Joe Smith (SO)");
	iPos = m_subsystemList.InsertItem(1,"HV Power",0);
	       m_subsystemList.SetItemText(iPos,1,"Joe Smith (SO)");

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
