//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditAssignCameraToMonitorPage.cpp $
// @author:  Katherine Thomson
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
//

#include "stdafx.h"

#if defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)

#include "core/utilities/src/TAAssert.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "PlanManager.h"
#include "PlanStepEditAssignCameraToMonitorPage.h"
#include "AssignCameraToMonitorStep.h"
#include "MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditAssignCameraToMonitorPage dialog

IMPLEMENT_DYNAMIC(CPlanStepEditAssignCameraToMonitorPage, CPlanStepEditParameterPage)

CPlanStepEditAssignCameraToMonitorPage::CPlanStepEditAssignCameraToMonitorPage(AssignCameraToMonitorStep* step)
  : CPlanStepEditParameterPage(CPlanStepEditAssignCameraToMonitorPage::IDD, step),
    m_cameraParameter(*step->getCameraParameter()),
    m_monitorParameter(*step->getMonitorParameter())
{
    FUNCTION_ENTRY( "CPlanStepEditAssignCameraToMonitorPage" );

    // {{AFX_DATA_INIT(CPlanStepEditAssignCameraToMonitorPage)
    m_camera = _T("");
    m_monitor = _T("");
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CPlanStepEditAssignCameraToMonitorPage::~CPlanStepEditAssignCameraToMonitorPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditAssignCameraToMonitorPage" );

    // Do nothing.

    FUNCTION_EXIT;
}


void CPlanStepEditAssignCameraToMonitorPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditAssignCameraToMonitorPage)
    DDX_Text(pDX, IDC_CAMERA, m_camera);
    DDX_Text(pDX, IDC_MONITOR, m_monitor);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditAssignCameraToMonitorPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditAssignCameraToMonitorPage)
    ON_BN_CLICKED(IDC_PICK_CAMERA_BUTTON, OnPickCameraButton)
    ON_BN_CLICKED(IDC_PICK_MONITOR_BUTTON, OnPickMonitorButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditAssignCameraToMonitorPage message handlers


void CPlanStepEditAssignCameraToMonitorPage::OnPickCameraButton()
{
    FUNCTION_ENTRY( "OnPickCameraButton" );

    CWaitCursor waitcursor; //TD17494
    // Select the required camera.

    // as per TD 12386
    // only display the camera selection dialog
    // if list of cameras is available
    // error message will be displayed to the user
    if (m_cameraParameter.isAnyCameraAvailable ())
    {
        m_cameraParameter.editParameter();
        DisplayParameters();
    }

    FUNCTION_EXIT;
}


void CPlanStepEditAssignCameraToMonitorPage::OnPickMonitorButton()
{
    FUNCTION_ENTRY( "OnPickMonitorButton" );

    CWaitCursor waitcursor; //TD17494
    // Select the required monitor.
    // as per TD 12386
    // only display the monitor selection dialog
    // if list of monitors is available
    // error message will be displayed to the user
    if (m_monitorParameter.isAnyMonitorAvailable ())
    {
    m_monitorParameter.editParameter();
    DisplayParameters();
    }

    FUNCTION_EXIT;
}


void CPlanStepEditAssignCameraToMonitorPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CWnd* cameraButton = GetDlgItem(IDC_PICK_CAMERA_BUTTON);
    if (cameraButton)
    {
        cameraButton->EnableWindow(enable);
    }

    CWnd* monitorButton = GetDlgItem(IDC_PICK_MONITOR_BUTTON);
    if (monitorButton)
    {
        monitorButton->EnableWindow(enable);
    }

    FUNCTION_EXIT;
}


void CPlanStepEditAssignCameraToMonitorPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    m_camera = m_cameraParameter.getCameraName().c_str();
    m_monitor = m_monitorParameter.getMonitorName().c_str();

    UpdateData(false);

    FUNCTION_EXIT;
}


void CPlanStepEditAssignCameraToMonitorPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    AssignCameraToMonitorStep* step = dynamic_cast<AssignCameraToMonitorStep*>(GetPlanStep());
    TA_ASSERT(step!=NULL,"Incorrect step type for the assign camera to monitor step page");    // KTTODO This should be an exception, but at the moment it won't be handled.

    CameraStepParameter* cameraStepParameter = step->getCameraParameter();
    if ( 0 == cameraStepParameter )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to update Assign Camera to Monitor parameters." );
        TA_Base_Bus::TransActiveMessage userMsg;
        CString itemName;
        itemName.LoadString(IDS_CAMERA);
        userMsg << itemName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210076);

        FUNCTION_EXIT;
        return;     // KTTODO This should throw an exception... but would it be handled?
    }
    MonitorStepParameter* monitorStepParameter = step->getMonitorParameter();
    if ( 0 == monitorStepParameter )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to update Assign Camera to Monitor parameters." );
        TA_Base_Bus::TransActiveMessage userMsg;
        CString itemName;
        itemName.LoadString(IDS_MONITOR);
        userMsg << itemName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_210076);

        FUNCTION_EXIT;
        return;     // KTTODO This should throw an exception... but would it be handled?
    }
    cameraStepParameter->setCameraKey(m_cameraParameter.getCameraKey());
    cameraStepParameter->setCameraName(m_cameraParameter.getCameraName());
    monitorStepParameter->setMonitorKey(m_monitorParameter.getMonitorKey());
    monitorStepParameter->setMonitorName(m_monitorParameter.getMonitorName());

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_ASSIGN_CAMERA_TO_MONITOR)
