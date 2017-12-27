/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/DataNodeController/src/SummaryListCtrl.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */

#include "stdafx.h"
#include "DataNodeController.h"
#include "SummaryListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CSummaryListCtrl::CSummaryListCtrl()
{
}

CSummaryListCtrl::~CSummaryListCtrl()
{
}


BEGIN_MESSAGE_MAP(CSummaryListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CSummaryListCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CSummaryListCtrl::PreSubclassWindow() 
{	
	SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES);

	// Column Headings
   	InsertColumn(0,	"Asset");
   	InsertColumn(1,	"Summary");
	
	// Column Widths
	SetColumnWidth(0, 110);
	SetColumnWidth(1, 280);	

	CListCtrl::PreSubclassWindow();
}

void CSummaryListCtrl::InitialiseList()
{
	int nItem = 0;
	LV_ITEM item;

	item.pszText	= "CDBG/ECS/TVS";
	item.mask		= LVIF_TEXT;
	item.iItem		= nItem;
	item.iSubItem	= 0;

	this->InsertItem(&item);
	this->SetItemText(nItem, 1, "Tagged, Control Inhibited, Scan Inhibited");

	nItem++;

	item.pszText	= "CDBG/ECS/SES";
	item.mask		= LVIF_TEXT;
	item.iItem		= nItem;
	item.iSubItem	= 0;

	this->InsertItem(&item);
	this->SetItemText(nItem, 1, "Tagged with Ptw");

	nItem++;

	item.pszText	= "CDBG/ECS/STN";
	item.mask		= LVIF_TEXT;
	item.iItem		= nItem;
	item.iSubItem	= 0;

	this->InsertItem(&item);
	this->SetItemText(nItem, 1, "Un-Acknowledged Alarm Raised");

	nItem++;

	item.pszText	= "CDBG/ECS/ZON";
	item.mask		= LVIF_TEXT;
	item.iItem		= nItem;
	item.iSubItem	= 0;

	this->InsertItem(&item);
	this->SetItemText(nItem, 1, "Tagged with Ptw, Alarm Inhibited");

}
