/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/DutyManager/GUIDesignPrototype/src/SiteTakeDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// SiteTakeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DutyManager.h"
#include "SiteTakeDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSiteTakeDialog dialog


CSiteTakeDialog::CSiteTakeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSiteTakeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSiteTakeDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSiteTakeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSiteTakeDialog)
	DDX_Control(pDX, IDC_LIST1, m_takeList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSiteTakeDialog, CDialog)
	//{{AFX_MSG_MAP(CSiteTakeDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSiteTakeDialog message handlers

BOOL CSiteTakeDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int  iPos;
	
	m_takeList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	// Setup headings
	m_takeList.InsertColumn(0,"Location",LVCFMT_LEFT,80);
	m_takeList.InsertColumn(1,"Current Operator",LVCFMT_LEFT,200);

	// Load some test data
	iPos = m_takeList.InsertItem(0,"BLV",-1);
	m_takeList.SetItemText(iPos,1,"Bert Reynolds (TCO)");
	iPos = m_takeList.InsertItem(0,"CVC",-1);
	m_takeList.SetItemText(iPos,1,"Bert Reynolds (TCO)");
	iPos = m_takeList.InsertItem(0,"MSM",-1);
	m_takeList.SetItemText(iPos,1,"Bert Reynolds (TCO)");
	iPos = m_takeList.InsertItem(0,"NCH",-1);
	m_takeList.SetItemText(iPos,1,"Bert Reynolds (TCO)");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
