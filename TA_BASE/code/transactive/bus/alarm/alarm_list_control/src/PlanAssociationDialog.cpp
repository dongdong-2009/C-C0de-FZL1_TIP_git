/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/PlanAssociationDialog.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class holds the plan association dialog that allows the user to run an associated
  * plan for an alarm
  */

#include "bus/alarm/alarm_list_control/src/stdafx.h"
#include "bus/alarm/alarm_list_control/src/PlanAssociationDialog.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/plans/src/AlarmPlanAssocData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_Bus
{
    PlanAssociationDialog::PlanAssociationDialog(TA_Base_Core::PlanDetailsList& plans,
                                                 CWnd* pParent /*=NULL*/)
	    : m_plans(plans),
          m_selectedPlanPath(""),
          CDialog(PlanAssociationDialog::IDD, pParent)
    {
	    //{{AFX_DATA_INIT(PlanAssociationDialog)
		    // NOTE: the ClassWizard will add member initialization here
	    //}}AFX_DATA_INIT
    }


    void PlanAssociationDialog::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(PlanAssociationDialog)
	    DDX_Control(pDX, IDC_ALARMLISTBOX_OK, m_okButton);
	    DDX_Control(pDX, IDC_ALARMPLANLISTBOX, m_planList);
	    //}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(PlanAssociationDialog, CDialog)
	//{{AFX_MSG_MAP(PlanAssociationDialog)
	ON_BN_CLICKED(IDC_ALARMLISTBOX_OK, onPlanlistboxOk)
	ON_LBN_SELCHANGE(IDC_ALARMPLANLISTBOX, onSelchangeAlarmplanlistbox)
	ON_BN_CLICKED(IDC_ALARMLISTBOX_CANCEL, onPlanlistboxCancel)
	ON_LBN_DBLCLK(IDC_ALARMPLANLISTBOX, onDblclkAlarmplanlistbox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    /////////////////////////////////////////////////////////////////////////////
    // PlanAssociationDialog message handlers

    BOOL PlanAssociationDialog::OnInitDialog() 
    {
        FUNCTION_ENTRY("OnInitDialog");
	    CDialog::OnInitDialog();
	    
        // Insert all the plan paths into the list. Determine the extent of the longest path.
        int maxExtent(0);
        CDC* listDC = m_planList.GetDC();

        m_planList.ResetContent();
        for (TA_Base_Core::PlanDetailsList::iterator iter = m_plans.begin(); iter != m_plans.end(); ++iter)
        {
	        m_planList.AddString(iter->c_str());

            CSize itemSize(listDC->GetTextExtent(iter->c_str()));
            maxExtent = __max(itemSize.cx, maxExtent);
        }

        m_planList.ReleaseDC(listDC);

        // Set the horizontal extent so that the end of the longest path can be scrolled to.
        m_planList.SetHorizontalExtent(maxExtent);

        // OK button remains disabled until a plan has been selected.
        m_okButton.EnableWindow(FALSE);

        FUNCTION_EXIT;
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void PlanAssociationDialog::onSelchangeAlarmplanlistbox() 
    {
	    int selected = m_planList.GetCurSel();
        if (selected == LB_ERR)
        {
            m_okButton.EnableWindow(FALSE);
        }
        else
        {
            m_okButton.EnableWindow(TRUE);
        }	    
    }


    void PlanAssociationDialog::onDblclkAlarmplanlistbox() 
    {
	    onPlanlistboxOk();
    }


    void PlanAssociationDialog::onPlanlistboxCancel() 
    {
        CDialog::OnCancel();
    }


    void PlanAssociationDialog::onPlanlistboxOk() 
    {
        FUNCTION_ENTRY("onPlanlistboxOk");

	    int selected = m_planList.GetCurSel();
        if (selected == LB_ERR)
        {
            return;
        }

        CString actionName;
        actionName.LoadString(IDS_RUN_SELECTED_PLAN);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UW_010003);
		if(selectedButton == IDNO)
        {
            return;
        }

        CString planPath;
        m_planList.GetText(selected, planPath);

        m_selectedPlanPath = static_cast<LPCTSTR>(planPath);
        
        CDialog::OnOK();

        FUNCTION_EXIT;
    }
}

