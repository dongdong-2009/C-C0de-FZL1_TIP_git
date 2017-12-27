/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/DataNodeController/src/InhibitDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "stdafx.h"
#include "DataNodeController.h"
#include "InhibitDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
	CInhibitDialog::CInhibitDialog(CWnd* pParent /*=NULL*/)
		: CDialog(CInhibitDialog::IDD, pParent)
	{
		//{{AFX_DATA_INIT(CInhibitDialog)
			// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT
	}


	void CInhibitDialog::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CInhibitDialog)
			// NOTE: the ClassWizard will add DDX and DDV calls here
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CInhibitDialog, CDialog)
		//{{AFX_MSG_MAP(CInhibitDialog)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	BOOL CInhibitDialog::OnInitDialog() 
	{
		CDialog::OnInitDialog();
		
		CButton *wnd = (CButton *)GetDlgItem(IDC_CHECK2);
		if (wnd)
			wnd->SetCheck(BST_CHECKED);
		
		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

} // TA_Base_App
