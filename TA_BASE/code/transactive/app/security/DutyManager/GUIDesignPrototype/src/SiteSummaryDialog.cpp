/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/DutyManager/GUIDesignPrototype/src/SiteSummaryDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// SiteSummaryDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DutyManager.h"
#include "SiteSummaryDialog.h"
#include "DelegateConfirmDialog.h"
#include "SiteTakeDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSiteSummaryDialog dialog


CSiteSummaryDialog::CSiteSummaryDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSiteSummaryDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSiteSummaryDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSiteSummaryDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSiteSummaryDialog)
	DDX_Control(pDX, IDC_LIST2, m_summaryList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSiteSummaryDialog, CDialog)
	//{{AFX_MSG_MAP(CSiteSummaryDialog)
	ON_BN_CLICKED(ID_DELEGATE, OnDelegate)
	ON_BN_CLICKED(ID_TAKE_DUTY, OnTakeDuty)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define PSC_IMAGE  0
#define OCC_IMAGE  1


void SetListCell(CListCtrl& pList,int iItem,int iSubItem,char* label,bool bCentral)
{
	LVITEM   item;

	// Display the text
	pList.SetItemText(iItem,iSubItem,label);

	// Set the icon
	item.mask = LVIF_IMAGE;
	item.iItem = iItem;
	if (bCentral)
		item.iImage = OCC_IMAGE;
	else
		item.iImage = PSC_IMAGE;
	item.iSubItem = iSubItem;
	pList.SetItem(&item);
}


/////////////////////////////////////////////////////////////////////////////
// CSiteSummaryDialog message handlers

BOOL CSiteSummaryDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int		 iPos;

    m_summaryList.SetExtendedStyle(LVS_EX_SUBITEMIMAGES|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

	// Setup the image list
	m_OCCIcon  = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_OCC));
	m_PSCIcon  = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_PSC));
	m_imageList.Create(16,16,ILC_COLOR8,4,4);
    m_imageList.Add(m_OCCIcon);
    m_imageList.Add(m_PSCIcon);
	m_summaryList.SetImageList(&m_imageList,LVSIL_SMALL);

	// Setup the grid headings etc.
	m_summaryList.InsertColumn(0,"Operator",LVCFMT_LEFT,100);
	m_summaryList.InsertColumn(1,"BLV",LVCFMT_LEFT,70);
	m_summaryList.InsertColumn(2,"CVC",LVCFMT_LEFT,70);
	m_summaryList.InsertColumn(3,"DBG",LVCFMT_LEFT,70);
	m_summaryList.InsertColumn(4,"MLN",LVCFMT_LEFT,70);
	m_summaryList.InsertColumn(5,"MSM",LVCFMT_LEFT,70);
	m_summaryList.InsertColumn(6,"NCH",LVCFMT_LEFT,70);
	m_summaryList.InsertColumn(7,"DCC",LVCFMT_LEFT,70);

	// Load test data into the grid
	iPos = m_summaryList.InsertItem(0,"Bert Reynolds",OCC_IMAGE);
	SetListCell(m_summaryList,iPos,1,"TCO", true);
	SetListCell(m_summaryList,iPos,2,"TCO", true);
	SetListCell(m_summaryList,iPos,5,"TCO", true);
	SetListCell(m_summaryList,iPos,6,"TCO", true);

	iPos = m_summaryList.InsertItem(1,"Sid James",OCC_IMAGE);
	SetListCell(m_summaryList,iPos,3,"TCO", true);
	SetListCell(m_summaryList,iPos,4,"TCO", true);

	iPos = m_summaryList.InsertItem(2,"Nick Brown",OCC_IMAGE);
	SetListCell(m_summaryList,iPos,1,"PSCO", true);
	SetListCell(m_summaryList,iPos,2,"PSCO", true);
	SetListCell(m_summaryList,iPos,3,"PSCO", true);
	SetListCell(m_summaryList,iPos,4,"PSCO", true);
	SetListCell(m_summaryList,iPos,5,"PSCO", true);
	SetListCell(m_summaryList,iPos,6,"PSCO", true);

	iPos = m_summaryList.InsertItem(3,"Napoleon Bonaparte",PSC_IMAGE);
	SetListCell(m_summaryList,iPos,7,"DCO", false);

	iPos = m_summaryList.InsertItem(4,"Joe Smith",PSC_IMAGE);
	SetListCell(m_summaryList,iPos,3,"SO", false);

	iPos = m_summaryList.InsertItem(5,"Fred Jones",PSC_IMAGE);
	SetListCell(m_summaryList,iPos,4,"SO", false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CSiteSummaryDialog::OnDelegate() 
{
	CDelegateConfirmDialog   pDlg;
	pDlg.DoModal();
}

void CSiteSummaryDialog::OnTakeDuty() 
{
	CSiteTakeDialog pDlg;
	pDlg.DoModal();	
}
