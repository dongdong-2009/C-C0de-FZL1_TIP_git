/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/ActivePlanAgency.cpp $
  * @author:  Huang Jian
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  *
  */

#include "ace/Guard_T.h"

#include "core/utilities/src/DataConversion.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/response_plans/plan_agent/src/PlanAgentLibrary.h"
#include "bus/response_plans/plan_agent/src/SortableActiveStepId.h"
#include "app/response_plans/plan_mft_processor/src/ActivePlanStep.h"
#include "app/response_plans/plan_mft_processor/src/ActivePlanAgency.h"
#include "app/response_plans/plan_mft_processor/src/LaunchSchematicActiveStep.h"
#include "app/response_plans/plan_mft_processor/src/LaunchGuiAppActiveStep.h"
#include "app/response_plans/plan_mft_processor/src/PlanAgentUpdateProcessor.h"
#include "app/response_plans/plan_mft_processor/src/RightsManager.h"
#include "app/response_plans/plan_mft_processor/src/LogActiveStep.h"
#include "app/response_plans/plan_mft_processor/src/DecisionActiveStep.h"
#include "app/response_plans/plan_mft_processor/src/PromptActiveStep.h"

namespace TA_Base_App
{
    ActivePlanAgencyImpl::ActivePlanAgencyImpl() :
    m_mtxStepsLock(),
    m_activePlanStepMap(),
    m_planAgent(NULL)
    {
    }

    ActivePlanAgencyImpl::~ActivePlanAgencyImpl()
    {
        uninit();
    }

    void ActivePlanAgencyImpl::init()
    {
        const unsigned long ulLocationKey = TA_Base_Core::DataConversion::toUnsignedInteger( TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY) );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "LOCATIONKEY:%d", ulLocationKey );

        if( m_planAgent == NULL )
        {
            m_planAgent = new TA_Base_Bus::PlanAgentLibrary(ulLocationKey);
        }

        RightsManager::getInstance().init();
        PlanAgentUpdateProcessor::getInstance();

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ActivePlanAgencyImpl::doInitialization() ok");
    }

    void ActivePlanAgencyImpl::uninit()
    {
        cleanSteps();

        PlanAgentUpdateProcessor::destroyInstance();
        RightsManager::destroyInstance();

        if(NULL != m_planAgent)
        {
            delete m_planAgent;
            m_planAgent = NULL;
        }
    }

    void ActivePlanAgencyImpl::processActiveStepDetailUpdate( const TA_Base_Core::CurrentPlanStepDetail &currentPlanStepDetail )
    {
        TA_Base_Bus::SortableActiveStepId srtStep = currentPlanStepDetail;

        ACE_Guard<ACE_Mutex> gudLock( m_mtxStepsLock );

        ActivePlanStepMapIt itExist = m_activePlanStepMap.find(srtStep);
        if (m_activePlanStepMap.end() != itExist )
        {
            if ( TA_Base_Core::Thread::THREAD_STATE_RUNNING == itExist->second->getCurrentState() )
            {
                itExist->second->terminateAndWait();
            }

             delete itExist->second;
             itExist->second = NULL;
             m_activePlanStepMap.erase( itExist );
        }

        TA_Base_App::ActivePlanStep* pNewStep = NULL;
        // Create a new thread to run the step
        switch (currentPlanStepDetail.data._d())
        {
        case TA_Base_Core::BACKGROUND_LAUNCH_SCHEMATIC_STEP:
            pNewStep = new TA_Base_App::LaunchSchematicActiveStep(currentPlanStepDetail);
            break;
        case  TA_Base_Core::BACKGROUND_LAUNCH_GUI_APP_STEP:
            pNewStep = new TA_Base_App::LaunchGuiAppActiveStep(currentPlanStepDetail);
            break;
        case TA_Base_Core::BACKGROUND_LOG_STEP:
            pNewStep = new TA_Base_App::LogActiveStep(currentPlanStepDetail);
            break;
        case TA_Base_Core::BACKGROUND_DECISION_STEP:
            pNewStep = new TA_Base_App::DecisionActiveStep(currentPlanStepDetail);
            break;
        case TA_Base_Core::BACKGROUND_PROMPT_STEP:
            pNewStep = new TA_Base_App::PromptActiveStep(currentPlanStepDetail);
            break;
        default:
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unrecognized step type %d", currentPlanStepDetail.data._d());
            break;
        }

        if ( NULL != pNewStep )
        {
            m_activePlanStepMap.insert( ActivePlanStepMap::value_type( srtStep, pNewStep ));
            pNewStep->start();
            pNewStep = NULL;
        }
    }

    TA_Base_Bus::PlanAgentLibrary* ActivePlanAgencyImpl::getPlanAgent() const
    {
        return m_planAgent;
    }

    void ActivePlanAgencyImpl::cleanSteps()
    {
        ACE_Guard<ACE_Mutex> gudLock( m_mtxStepsLock );

        for ( ActivePlanStepMapIt stepIt = m_activePlanStepMap.begin(); stepIt != m_activePlanStepMap.end(); ++stepIt )
        {
            if ( TA_Base_Core::Thread::THREAD_STATE_RUNNING == stepIt->second->getCurrentState() )
            {
                stepIt->second->terminateAndWait();
            }

            delete stepIt->second;
            stepIt->second = NULL;
        }

        m_activePlanStepMap.clear();
    }

    void ActivePlanAgencyImpl::processActivePlanUpdate( const TA_Base_Core::ActivePlanDetail& refPlanDetail )
    {
        if (( TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE == refPlanDetail.activePlanState ) ||
            ( TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE == refPlanDetail.activePlanState ) ||
            ( TA_Base_Core::ABORTED_ACTIVE_PLAN_STATE == refPlanDetail.activePlanState ) ||
            ( TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE == refPlanDetail.activePlanState ))
        {
            ACE_Guard<ACE_Mutex> gudLock( m_mtxStepsLock );

            for ( ActivePlanStepMapIt itStep = m_activePlanStepMap.begin(); itStep != m_activePlanStepMap.end(); )
            {
                if (( itStep->first.getNodeId() == refPlanDetail.plan.plan ) && ( itStep->first.getPlanInstance() == refPlanDetail.plan.instance ))
                {
                    if ( TA_Base_Core::Thread::THREAD_STATE_RUNNING == itStep->second->getCurrentState() )
                    {
                        itStep->second->terminateAndWait();
                    }

                    delete itStep->second;
                    itStep->second = NULL;
                    m_activePlanStepMap.erase( itStep++ );
                }
                else
                {
                    ++itStep;
                }
            }
        }
    }
}