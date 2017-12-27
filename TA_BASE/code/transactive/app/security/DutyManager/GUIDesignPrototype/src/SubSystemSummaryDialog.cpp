/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/DutyManager/GUIDesignPrototype/src/SubSystemSummaryDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// SubSystemSummaryDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DutyManager.h"
#include "SubSystemSummaryDialog.h"
#include "SubsystemGiveDialog.h"
#include "SubsystemTakeDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSubSystemSummaryDialog dialog


CSubSystemSummaryDialog::CSubSystemSummaryDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSubSystemSummaryDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSubSystemSummaryDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSubSystemSummaryDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSubSystemSummaryDialog)
	DDX_Control(pDX, IDC_LIST2, m_summaryList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSubSystemSummaryDialog, CDialog)
	//{{AFX_MSG_MAP(CSubSystemSummaryDialog)
	ON_BN_CLICKED(ID_GIVE_DUTY, OnGiveDuty)
	ON_BN_CLICKED(ID_TAKE_DUTY, OnTakeDuty)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void SetListCell(CListCtrl& pList,int iItem,int iSubItem,bool bTick)
{
	LVITEM   item;

	// Set the icon
	item.mask = LVIF_IMAGE;
	item.iItem = iItem;
	if (bTick)
		item.iImage = 0;
	else
		item.iImage = 1;
	item.iSubItem = iSubItem;
	pList.SetItem(&item);
}


/////////////////////////////////////////////////////////////////////////////
// CSubSystemSummaryDialog message handlers

BOOL CSubSystemSummaryDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int	iPos;
	
    m_summaryList.SetExtendedStyle(LVS_EX_SUBITEMIMAGES|LVS_EX_GRIDLINES);

	// Setup the image list
	m_tickIcon  = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_TICK));
	m_crossIcon = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_CROSS));
	m_imageList.Create(16,16,ILC_COLOR8,4,4);
    m_imageList.Add(m_tickIcon);
    m_imageList.Add(m_crossIcon);
	m_summaryList.SetImageList(&m_imageList,LVSIL_SMALL);

	// Setup the grid headings etc.
	m_summaryList.InsertColumn(0,"Subsystem",LVCFMT_LEFT,100);
	m_summaryList.InsertColumn(1,"BLV",LVCFMT_LEFT,40);
	m_summaryList.InsertColumn(2,"CVC",LVCFMT_LEFT,40);
	m_summaryList.InsertColumn(3,"DBG",LVCFMT_LEFT,40);
	m_summaryList.InsertColumn(4,"MLN",LVCFMT_LEFT,40);
	m_summaryList.InsertColumn(5,"MSM",LVCFMT_LEFT,40);
	m_summaryList.InsertColumn(6,"NCH",LVCFMT_LEFT,40);

	// Load test data into the grid
	iPos = m_summaryList.InsertItem(0,"Ventilation",-1);
	SetListCell(m_summaryList,iPos,1,false);
	SetListCell(m_summaryList,iPos,2,false);
	SetListCell(m_summaryList,iPos,3,false);
	SetListCell(m_summaryList,iPos,4,false);
	SetListCell(m_summaryList,iPos,5,false);
	SetListCell(m_summaryList,iPos,6,false);
	iPos = m_summaryList.InsertItem(0,"HV Power",-1);
	SetListCell(m_summaryList,iPos,1,false);
	SetListCell(m_summaryList,iPos,2,true);
	SetListCell(m_summaryList,iPos,3,true);
	SetListCell(m_summaryList,iPos,4,true);
	SetListCell(m_summaryList,iPos,5,false);
	SetListCell(m_summaryList,iPos,6,false);
	iPos = m_summaryList.InsertItem(0,"ATS",-1);
	SetListCell(m_summaryList,iPos,1,false);
	SetListCell(m_summaryList,iPos,2,false);
	SetListCell(m_summaryList,iPos,3,false);
	SetListCell(m_summaryList,iPos,4,false);
	SetListCell(m_summaryList,iPos,5,false);
	SetListCell(m_summaryList,iPos,6,false);
	iPos = m_summaryList.InsertItem(0,"Tunnel Lighting",-1);
	SetListCell(m_summaryList,iPos,1,false);
	SetListCell(m_summaryList,iPos,2,true);
	SetListCell(m_summaryList,iPos,3,true);
	SetListCell(m_summaryList,iPos,4,true);
	SetListCell(m_summaryList,iPos,5,false);
	SetListCell(m_summaryList,iPos,6,false);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSubSystemSummaryDialog::OnGiveDuty() 
{
	CSubsystemGiveDialog    pDlg;
	pDlg.DoModal();
}

void CSubSystemSummaryDialog::OnTakeDuty() 
{
	CSubsystemTakeDialog    pDlg;
	pDlg.DoModal();
}
