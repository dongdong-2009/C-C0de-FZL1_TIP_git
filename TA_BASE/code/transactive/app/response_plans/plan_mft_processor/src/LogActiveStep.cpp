/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/LogActiveStep.cpp $
  * @author:  Huang Jian
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  *
  */

#include <afxwin.h>
#include "bus/response_plans/plan_agent/src/PlanAgentLibrary.h"
#include "app/response_plans/plan_mft_processor/src/RightsManager.h"
#include "app/response_plans/plan_mft_processor/src/LogActiveStep.h"
#include "app/response_plans/plan_mft_processor/src/LogEntryDlg.h"
#include "app/response_plans/plan_mft_processor/src/ActivePlanAgency.h"

namespace TA_Base_App
{


LogActiveStep::LogActiveStep( const TA_Base_Core::CurrentPlanStepDetail& stepDetail ):
ActivePlanStep(stepDetail),
m_pLogEntryDlg(NULL)
{
    FUNCTION_ENTRY( "LogActiveStep" );

    FUNCTION_EXIT;
}

void LogActiveStep::execute()
{
    launch();
}

void LogActiveStep::stop()
{
    FUNCTION_ENTRY( "stop" );
    
    if (NULL != m_pLogEntryDlg)
    {
        m_pLogEntryDlg->EndDialog(IDABORT);
    }

    FUNCTION_EXIT;
}

LogActiveStep::~LogActiveStep()
{
    FUNCTION_ENTRY( "~LogActiveStep" );

    if (NULL != m_pLogEntryDlg)
    {
        delete m_pLogEntryDlg;
        m_pLogEntryDlg = NULL;
    }

    FUNCTION_EXIT;
}

void LogActiveStep::launch()
{
    FUNCTION_ENTRY( "launch" );

    if ( !isRunning() )
    {
        FUNCTION_EXIT;
        return;
    }

    if ( !RightsManager::getInstance().isProfileMatched( m_stepDetail.data.bgLog().profile ))
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Can not lunch the dialog, request profile = %d", m_stepDetail.data.bgLog().profile);
        FUNCTION_EXIT;
        return;
    }

    if ( !isRunning() )
    {
        FUNCTION_EXIT;
        return;
    }

    try
    {
        ActivePlanAgency::instance()->getPlanAgent()->processBackgroundLogStepResponse( m_stepDetail, TA_Base_Core::PSCS_SUCCESS, "");
    }
    catch(...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Execute MftMessageActiveStep fail");
    }

    if ( !isRunning() )
    {
        FUNCTION_EXIT;
        return;
    }

    if ( NULL != m_pLogEntryDlg )
    {
        m_pLogEntryDlg->CloseWindow();
    }

    if ( !isRunning() )
    {
        FUNCTION_EXIT;
        return;
    }
    
    m_pLogEntryDlg = new CLogEntryDlg(m_stepDetail);
    m_pLogEntryDlg->DoModal();

    FUNCTION_EXIT;
}

}