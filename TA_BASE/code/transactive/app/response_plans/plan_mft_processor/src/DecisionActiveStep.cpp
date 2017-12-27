/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/DecisionActiveStep.cpp $
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
#include "app/response_plans/plan_mft_processor/src/DecisionDlg.h"
#include "app/response_plans/plan_mft_processor/src/ActivePlanAgency.h"
#include "app/response_plans/plan_mft_processor/src/DecisionActiveStep.h"

namespace TA_Base_App
{


DecisionActiveStep::DecisionActiveStep( const TA_Base_Core::CurrentPlanStepDetail& stepDetail ):
ActivePlanStep(stepDetail),
m_pDecisionDlg(NULL)
{
    FUNCTION_ENTRY( "DecisionActiveStep" );

    FUNCTION_EXIT;
}

void DecisionActiveStep::execute()
{
    launch();
}

void DecisionActiveStep::stop()
{
    FUNCTION_ENTRY( "stop" );
    
    if (NULL != m_pDecisionDlg)
    {
        m_pDecisionDlg->EndDialog(IDABORT);
    }

    FUNCTION_EXIT;
}

DecisionActiveStep::~DecisionActiveStep()
{
    FUNCTION_ENTRY( "~DecisionActiveStep" );

    if (NULL != m_pDecisionDlg)
    {
        delete m_pDecisionDlg;
        m_pDecisionDlg = NULL;
    }

    FUNCTION_EXIT;
}

void DecisionActiveStep::launch()
{
    FUNCTION_ENTRY( "launch" );

    if ( !isRunning() )
    {
        FUNCTION_EXIT;
        return;
    }

    if ( !RightsManager::getInstance().isProfileMatched( m_stepDetail.data.bgDecision().profile ))
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Can not lunch the dialog, request profile = %d", m_stepDetail.data.bgDecision().profile);
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
        ActivePlanAgency::instance()->getPlanAgent()->processBackgroundDecisionStepResponse( m_stepDetail, TA_Base_Core::PSCS_SUCCESS);
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

    if ( NULL != m_pDecisionDlg )
    {
        m_pDecisionDlg->CloseWindow();
    }

    if ( !isRunning() )
    {
        FUNCTION_EXIT;
        return;
    }
    
    m_pDecisionDlg = new CDecisionDlg(m_stepDetail);
    m_pDecisionDlg->DoModal();

    FUNCTION_EXIT;
}

}