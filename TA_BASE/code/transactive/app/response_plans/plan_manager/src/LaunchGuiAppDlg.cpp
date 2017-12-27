//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File$
// @author:  Bart Golab
// @version: $Revision$
//
// Last modification: $DateTime$
// Last modified by:  $Author$
//
// <description>

#include "app/response_plans/plan_manager/src/Stdafx.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "app/response_plans/plan_manager/src/PlanManager.h"
#include "app/response_plans/plan_manager/src/LaunchGuiAppDlg.h"
#include "app/response_plans/plan_manager/src/LabelMaker.h"
#include "app/response_plans/plan_manager/src/MessageBox.h"
#include "app/response_plans/plan_manager/src/PlanAgentAccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CLaunchGuiAppDlg dialog


CLaunchGuiAppDlg::CLaunchGuiAppDlg(const TA_Base_Core::CurrentPlanStepDetail& stepDetail) :
    CInteractiveStepDlg(stepDetail, TA_Base_Bus::IPlanManagerCorbaDef::LAUNCHGUIAPP_RESPONSE_TIMEOUT)
{
    FUNCTION_ENTRY( "CLaunchGuiAppDlg" );

    // {{AFX_DATA_INIT(CLaunchGuiAppDlg)
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CLaunchGuiAppDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CInteractiveStepDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CLaunchGuiAppDlg)
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CLaunchGuiAppDlg, CInteractiveStepDlg)
    // {{AFX_MSG_MAP(CLaunchGuiAppDlg)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLaunchGuiAppDlg message handlers

CString CLaunchGuiAppDlg::getTitlePrefix() const
{
    FUNCTION_ENTRY( "getTitlePrefix" );
    FUNCTION_EXIT;
    return LabelMaker::getLaunchGuiAppStepDialogTitlePrefix();
}


void CLaunchGuiAppDlg::reportCompletionStatus() const
{
    FUNCTION_ENTRY( "reportCompletionStatus" );

    TA_Base_Core::EPlanStepCompletionStatus completionStatus(TA_Base_Core::PSCS_UNDEFINED);
    std::string completionError;

    try
    {
        // Determine app position and size. Want the app on the same screen as the main Plan Manager window.
        RECT appBounds(TA_Base_Bus::AppLauncher::getInstance().getRect(TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT,
                                                                       TA_Base_Bus::TA_GenericGui::AREA_SCREEN, 0));

        unsigned long appPosType(TA_Base_Bus::TA_GenericGui::POS_BOUNDED);
        TA_Base_Core::LaunchGuiAppParameters parameters(getStepDetail().data.launchGuiApp());

        appBounds.left += parameters.pos.left;
        appBounds.top = parameters.pos.top;

        // Width and height are optional parameters of the Launch GUI App step.
        // If not specified, the default values for the app will be used.
        if (parameters.pos.width != static_cast<unsigned long>(-1) &&
            parameters.pos.height != static_cast<unsigned long>(-1))
        {
            appPosType |= TA_Base_Bus::TA_GenericGui::POS_RESIZABLE;

            appBounds.right = appBounds.left + parameters.pos.width;
            appBounds.bottom = appBounds.top + parameters.pos.height;
        }
        else
        {
            appBounds.right = appBounds.bottom = 0;
        }

        // Now launch the app
        TA_Base_Bus::AppLauncher::getInstance().launchApplication(parameters.app,
                                                                  parameters.args.in(),
                                                                  appPosType,
                                                                  0,
                                                                  NULL,
                                                                  &appBounds);

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
        completionError = "Failed to launch GUI application";
    }

    try
    {
        PlanAgentAccess::getInstance().reportLaunchGuiAppStepResult(getStepDetail().activePlan, completionStatus, completionError);
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_REPORT_COMPLETION_STATUS, ex);
    }

    FUNCTION_EXIT;
}


