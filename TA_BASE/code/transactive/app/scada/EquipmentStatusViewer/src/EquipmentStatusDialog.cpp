/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/EquipmentStatusDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "stdafx.h"
#include "equipmentstatusviewer.h"
#include "EquipmentStatusDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{

	CEquipmentStatusDialog::CEquipmentStatusDialog(CWnd* pParent /*=NULL*/)
		: CDialog(CEquipmentStatusDialog::IDD, pParent)
	{
		//{{AFX_DATA_INIT(CEquipmentStatusDialog)
		//}}AFX_DATA_INIT
	}


	void CEquipmentStatusDialog::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CEquipmentStatusDialog)
		DDX_Control(pDX, IDC_REPORT_COMBO, m_reportName);
		DDX_Control(pDX, IDC_EQUIPMENT_LIST, m_equipmentStatusList);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CEquipmentStatusDialog, CDialog)
		//{{AFX_MSG_MAP(CEquipmentStatusDialog)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CEquipmentStatusDialog message handlers

	BOOL CEquipmentStatusDialog::OnInitDialog() 
	{
		CDialog::OnInitDialog();
		
		// TODO: Add extra initialization here

		m_reportName.AddString("Tunnel Ventilation Fans");
		m_reportName.SetCurSel(m_reportName.FindStringExact(-1, "Tunnel Ventilation Fans"));
		
		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

} // TA_Base_App
