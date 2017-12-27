/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Andy Parker
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * This class represents a simple dialog displayed to the operator when creating new steps within a plan.
  * It enables the operator to pick the step's type and insert position wrt currently selected step(s),
  * i.e. before or after. If the currently selected step is the End Step, the insert after option is disabled.
  *
  **/

#include "app/response_plans/plan_manager/src/StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "app/response_plans/plan_manager/src/PlanManager.h"
#include "app/response_plans/plan_manager/src/NewPlanStepDlg.h"
#include "app/response_plans/plan_manager/src/Utilities.h"
#include "app/response_plans/plan_manager/src/ApplicationFont.h"
#include "app/response_plans/plan_manager/src/PlanManagerCommonDefs.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataReader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    static const TA_Base_Core::EStep STEP_TYPES[] =
    {
#if defined (STEPTYPE_ABORT)
        TA_Base_Core::ABORT_STEP,
#endif
#if defined (STEPTYPE_ACTIVATE_CAMERA_PRESET)
        TA_Base_Core::ACTIVATE_CAMERA_PRESET_STEP,
#endif
#if defined (STEPTYPE_ACTIVE_PLAN_CHECK)
        TA_Base_Core::ACTIVE_PLAN_CHECK_STEP,
#endif
#if defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)
        TA_Base_Core::ASSIGN_CAMERA_TO_MONITOR_STEP,
#endif
#if defined (STEPTYPE_DATA_POINT_CHECK)
        TA_Base_Core::DATA_POINT_CHECK_STEP,
#endif
#if defined (STEPTYPE_DATA_POINT_SET)
        TA_Base_Core::DATA_POINT_SET_STEP,
#endif
#if defined (STEPTYPE_DECISION)
        TA_Base_Core::DECISION_STEP,
#endif
#if defined (STEPTYPE_EVALUATE_EXPRESSION)
        TA_Base_Core::EVALUATE_EXPRESSION_STEP,
#endif
#if defined (STEPTYPE_JUMP)
        TA_Base_Core::JUMP_STEP,
#endif
#if defined (STEPTYPE_LAUNCH_GUI_APP)
        TA_Base_Core::LAUNCH_GUI_APP_STEP,
#endif
#if defined (STEPTYPE_LAUNCH_SCHEMATIC)
        TA_Base_Core::LAUNCH_SCHEMATIC_STEP,
#endif
#if defined (STEPTYPE_LOG)
        TA_Base_Core::LOG_STEP,
#endif
#if defined (STEPTYPE_PROMPT)
        TA_Base_Core::PROMPT_STEP,
#endif
#if defined (STEPTYPE_RUN_PLAN)
        TA_Base_Core::RUN_PLAN_STEP,
#endif
#if defined (STEPTYPE_SEND_SMS_MESSAGE)
        TA_Base_Core::SEND_SMS_MESSAGE_STEP,
#endif
#if defined (STEPTYPE_STATION_PUBLIC_ADDRESS)
        TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP,
#endif
#if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)
        TA_Base_Core::STATION_TRAVELLER_INFO_SYSTEM_STEP,
#endif
#if defined (STEPTYPE_TERMINATE_PLAN)
        TA_Base_Core::TERMINATE_PLAN_STEP,
#endif
#if defined (STEPTYPE_TRAIN_PUBLIC_ADDRESS)
        TA_Base_Core::TRAIN_PUBLIC_ADDRESS_STEP,
#endif
#if defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM)
        TA_Base_Core::TRAIN_TRAVELLER_INFO_SYSTEM_STEP,
#endif
#if defined (STEPTYPE_VIDEOWALL)
        TA_Base_Core::VIDEO_WALL_STEP,
#endif
#if defined (STEPTYPE_VMS_BLANKOUT)
        TA_Base_Core::VMS_BLANKOUT_STEP,
#endif
#if defined (STEPTYPE_VMS_RESTORE_SCHEDULE)
        TA_Base_Core::VMS_RESTORE_SCHEDULE_STEP,
#endif
#if defined (STEPTYPE_VMS_SET_SCENE)
        TA_Base_Core::VMS_SET_SCENE_STEP,
#endif
#if defined (STEPTYPE_FAILURE_SUMMARY_STEP)
        TA_Base_Core::FAILURE_SUMMARY_STEP,
#endif
//WuZhongyi New Step Fire Alarm Station PA
#if defined (STEPTYPE_STATION_PUBLIC_ADDRESS_BY_ALARM)
		TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP_BY_ALARM,
#endif
#if defined (STEPTYPE_BACKGROUND_LAUNCH_SCHEMATIC)
        TA_Base_Core::BACKGROUND_LAUNCH_SCHEMATIC_STEP,
#endif
#if defined (STEPTYPE_BACKGROUND_LAUNCH_GUI_APP)
        TA_Base_Core::BACKGROUND_LAUNCH_GUI_APP_STEP,
#endif
#if defined (STEPTYPE_BACKGROUND_DECISION)
        TA_Base_Core::BACKGROUND_DECISION_STEP,
#endif
#if defined (STEPTYPE_BACKGROUND_LOG)
        TA_Base_Core::BACKGROUND_LOG_STEP,
#endif
#if defined (STEPTYPE_BACKGROUND_PROMPT)
        TA_Base_Core::BACKGROUND_PROMPT_STEP,
#endif
    };
}

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CNewPlanStepDlg dialog


CNewPlanStepDlg::CNewPlanStepDlg(bool endStepSelected, CWnd* pParent /*=NULL*/) :
    CDialog(CNewPlanStepDlg::IDD, pParent),
    m_endStepSelected(endStepSelected),
    m_insertPosition(IPT_INSERT_AFTER),
    m_selectedStepType(TA_Base_Core::ABORT_STEP)
{
    FUNCTION_ENTRY( "CNewPlanStepDlg" );

    // {{AFX_DATA_INIT(CNewPlanStepDlg)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CNewPlanStepDlg::~CNewPlanStepDlg()
{
    FUNCTION_ENTRY( "~CNewPlanStepDlg" );
    FUNCTION_EXIT;
}


void CNewPlanStepDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CNewPlanStepDlg)
    DDX_Control(pDX, IDC_STEP_TYPE, m_stepTypeCombo);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CNewPlanStepDlg, CDialog)
    // {{AFX_MSG_MAP(CNewPlanStepDlg)
    ON_BN_CLICKED(IDOK, OnOK)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewPlanStepDlg message handlers

BOOL CNewPlanStepDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CDialog::OnInitDialog();

    static std::map<unsigned long, int> stepUseMap = TA_Base_Bus::PlanDataReader::instance()->getStepUseMap();
    
    std::map<unsigned long, int>::const_iterator stepUseMapIt = stepUseMap.begin();

    // Populate the plan step type selection drop-down with the list of supported steps.
    // Filter step population according to step use or not use
    for (int i = 0; i < sizeof(STEP_TYPES) / sizeof(TA_Base_Core::EStep); i++)
    {
        stepUseMapIt = stepUseMap.find(STEP_TYPES[i]);
        if ( stepUseMapIt != stepUseMap.end() &&
            ( Utilities::isPlanManagerConfigurationMode() || 
            TA_Base_App::STEP_USE == stepUseMapIt->second ))
        {
            int stepTypeIdx = m_stepTypeCombo.AddString(Utilities::convertToString(STEP_TYPES[i]).c_str());
            m_stepTypeCombo.SetItemData(stepTypeIdx, STEP_TYPES[i]);
        }
    }
    m_stepTypeCombo.SetCurSel(0);

    GetDlgItem(IDC_INSERT_AFTER_SELECTION_RADIO)->EnableWindow(!m_endStepSelected);
    CheckDlgButton((m_endStepSelected? IDC_INSERT_BEFORE_SELECTION_RADIO : IDC_INSERT_AFTER_SELECTION_RADIO), BST_CHECKED);

    ApplicationFont::getInstance().adjustChildrenFont( GetSafeHwnd() );

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CNewPlanStepDlg::OnOK()
{
    FUNCTION_ENTRY( "OnOK" );

    // Get selected insert position
    m_insertPosition = IsDlgButtonChecked(IDC_INSERT_BEFORE_SELECTION_RADIO)? IPT_INSERT_BEFORE : IPT_INSERT_AFTER;

    // Get selected step type
    int selectedStepTypeIdx = m_stepTypeCombo.GetCurSel();
    TA_ASSERT(selectedStepTypeIdx != CB_ERR, "No current selection in step type combo");

    m_selectedStepType = static_cast<TA_Base_Core::EStep>(m_stepTypeCombo.GetItemData(selectedStepTypeIdx));

    // Normal exit
    EndDialog(IDOK);

    FUNCTION_EXIT;
}


