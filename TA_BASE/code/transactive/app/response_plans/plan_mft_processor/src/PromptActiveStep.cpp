/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/PromptActiveStep.cpp $
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
#include "app/response_plans/plan_mft_processor/src/PromptDlg.h"
#include "app/response_plans/plan_mft_processor/src/ActivePlanAgency.h"
#include "app/response_plans/plan_mft_processor/src/PromptActiveStep.h"

namespace TA_Base_App
{

PromptActiveStep::PromptActiveStep( const TA_Base_Core::CurrentPlanStepDetail& stepDetail ):
ActivePlanStep(stepDetail),
m_pPromptDlg(NULL)
{
    FUNCTION_ENTRY( "PromptActiveStep" );

    FUNCTION_EXIT;
}

void PromptActiveStep::execute()
{
    launch();
}

void PromptActiveStep::stop()
{
    FUNCTION_ENTRY( "stop" );

    if (NULL != m_pPromptDlg)
    {
        m_pPromptDlg->EndDialog(IDABORT);
    }

    FUNCTION_EXIT;
}

PromptActiveStep::~PromptActiveStep()
{
    FUNCTION_ENTRY( "~PromptActiveStep" );

    if (NULL != m_pPromptDlg)
    {
        delete m_pPromptDlg;
        m_pPromptDlg = NULL;
    }

    FUNCTION_EXIT;
}

void PromptActiveStep::launch()
{
    FUNCTION_ENTRY( "launch" );

    if ( !isRunning() )
    {
        FUNCTION_EXIT;
        return;
    }

    if ( !RightsManager::getInstance().isProfileMatched( m_stepDetail.data.bgPrompt().profile ))
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Can not lunch the dialog, request profile = %d", m_stepDetail.data.bgPrompt().profile);
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
        ActivePlanAgency::instance()->getPlanAgent()->processBackgroundPromptStepResponse( m_stepDetail, TA_Base_Core::PSCS_SUCCESS);
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

    if ( NULL != m_pPromptDlg )
    {
        m_pPromptDlg->CloseWindow();
    }

    if ( !isRunning() )
    {
        FUNCTION_EXIT;
        return;
    }
    
    m_pPromptDlg = new CPromptDlg(m_stepDetail);
    m_pPromptDlg->DoModal();

    FUNCTION_EXIT;
}

}