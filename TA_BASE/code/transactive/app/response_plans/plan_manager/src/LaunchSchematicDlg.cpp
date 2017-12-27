/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Bart Golab
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Facilitates the launching of a display when executing a Launch Schematic step.
  *
  */

#include "Stdafx.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "PlanManager.h"
#include "LaunchSchematicDlg.h"
#include "LabelMaker.h"
#include "MessageBox.h"
#include "PlanAgentAccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::StringUtilities;
using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CLaunchSchematicDlg dialog


CLaunchSchematicDlg::CLaunchSchematicDlg(const TA_Base_Core::CurrentPlanStepDetail& stepDetail) :
    CInteractiveStepDlg(stepDetail, TA_Base_Bus::IPlanManagerCorbaDef::LAUNCHSCHEMATIC_RESPONSE_TIMEOUT)
{
    FUNCTION_ENTRY( "CLaunchSchematicDlg" );

    // {{AFX_DATA_INIT(CLaunchSchematicDlg)
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CLaunchSchematicDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CInteractiveStepDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CLaunchSchematicDlg)
    DDX_Control(pDX, IDC_SCREEN_NUMBER_COMBO, m_screenNumberCombo);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CLaunchSchematicDlg, CInteractiveStepDlg)
    // {{AFX_MSG_MAP(CLaunchSchematicDlg)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLaunchSchematicDlg message handlers

BOOL CLaunchSchematicDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CInteractiveStepDlg::OnInitDialog();

    // Enable the operator to change the pre-configured target screen to display the schematic on.
    RECT lastScreenBounds = { 0, 0, 100, 100 };
    try
    {
        lastScreenBounds = TA_Base_Bus::AppLauncher::getInstance().getRect(
            TA_Base_Bus::TA_GenericGui::SCREEN_LAST, TA_Base_Bus::TA_GenericGui::AREA_SCREEN);
    }
    catch (...)
    {
    }

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
        "Last screen:  left = %d / right = %d", lastScreenBounds.left, lastScreenBounds.right);

    int lastScreen(lastScreenBounds.right / (lastScreenBounds.right - lastScreenBounds.left));
    int targetScreen(getStepDetail().data.launchSchematic().screen);

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
        "Configured target screen:  %d", lastScreen);
    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
        "Last screen:  %d", lastScreen);

    // Reset the target screen if it's greater than the last available screen.
    if (targetScreen > lastScreen)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
            "Configured target screen > last screen, resetting to %d", lastScreen);

        targetScreen = lastScreen;
    }

    for (int screen = 1; screen <= lastScreen; screen++)
    {
        m_screenNumberCombo.AddString(StringUtilities::convertToString(screen));
    }

    m_screenNumberCombo.SelectString(CB_ERR, StringUtilities::convertToString(targetScreen));

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


CString CLaunchSchematicDlg::getTitlePrefix() const
{
    FUNCTION_ENTRY( "getTitlePrefix" );
    FUNCTION_EXIT;
    return LabelMaker::getLaunchSchematicStepDialogTitlePrefix();
}


void CLaunchSchematicDlg::reportCompletionStatus() const
{
    FUNCTION_ENTRY( "reportCompletionStatus" );

    TA_Base_Core::EPlanStepCompletionStatus completionStatus(TA_Base_Core::PSCS_UNDEFINED);
    std::string completionError;

    try
    {
        // Get the target screen number as selected in the screen list.
        CString selectedScreen;
        m_screenNumberCombo.GetWindowText(selectedScreen);
        int targetScreen(::atoi(selectedScreen));

        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
            "Target screen for schematic: %d", targetScreen);

        // Determine the coordinate of the left side of the target screen.
        RECT screenBounds(TA_Base_Bus::AppLauncher::getInstance().getRect(TA_Base_Bus::TA_GenericGui::SCREEN_SPECIFIC,
                                                                          TA_Base_Bus::TA_GenericGui::AREA_SCREEN,
                                                                          targetScreen));

        // If additional schematic commands have been configured for this step, append them
        // to the schematic's path. The protocol tag to be used here is PLANS.
        TA_Base_Core::LaunchSchematicParameters parameters(getStepDetail().data.launchSchematic());
        std::string schematic(parameters.schematic.in());
        std::string commands(parameters.commands.in());

        if (!commands.empty())
        {
            if (commands.find("#PLANS,") != 0)
            {
                commands = "#PLANS," + commands;
            }

            schematic += commands;
        }

        // Now launch the schematic.
        TA_Base_Bus::AppLauncher::getInstance().launchGraphWorxDisplay(schematic, "", screenBounds.left);

        completionStatus = TA_Base_Core::PSCS_SUCCESS;
        completionError = "";
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo,    "TransactiveException", ex.what());

        completionStatus = TA_Base_Core::PSCS_FAILURE;
        completionError = ex.what();
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo,    "UnknownException", "");

        completionStatus = TA_Base_Core::PSCS_FAILURE;
        completionError = "Failed to launch schematic display";
    }

    try
    {
        PlanAgentAccess::getInstance().reportLaunchSchematicStepResult(getStepDetail().activePlan, completionStatus, completionError);
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_REPORT_COMPLETION_STATUS, ex);
    }

    FUNCTION_EXIT;
}


