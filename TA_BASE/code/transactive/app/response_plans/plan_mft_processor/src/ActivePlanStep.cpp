/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/ActivePlanStep.cpp $
  * @author:  Huang Jian
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  *
  *
  **/

#include "ace/Guard_T.h"

#include "app/response_plans/plan_mft_processor/src/ActivePlanStep.h"
#include "app/response_plans/plan_mft_processor/src/PlanAgentUpdateProcessor.h"


namespace TA_Base_App
{

ActivePlanStep::ActivePlanStep( const TA_Base_Core::CurrentPlanStepDetail &stepDetail ):
m_eStepState( TA_Base_Core::Thread::THREAD_STATE_NEW ),
m_mtxState(),
m_stepDetail( stepDetail )
{
}

ActivePlanStep::~ActivePlanStep()
{
}

void ActivePlanStep::run()
{
    {
        ACE_Guard<ACE_Mutex> gudLock(m_mtxState);

        // Can not run when state is terminated
        if ( TA_Base_Core::Thread::THREAD_STATE_TERMINATED == m_eStepState )
        {
            // Reset to new state
            m_eStepState = TA_Base_Core::Thread::THREAD_STATE_NEW;
            return;
        }

        m_eStepState = TA_Base_Core::Thread::THREAD_STATE_RUNNING;
    }

    try
    {
        execute();
    }
    catch ( const std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Caught unknown exception when execute" );
    }

    {
        ACE_Guard<ACE_Mutex> gudLock(m_mtxState);
        m_eStepState = TA_Base_Core::Thread::THREAD_STATE_NEW;
    }
}

void ActivePlanStep::terminate()
{
    {
        ACE_Guard<ACE_Mutex> gudLock(m_mtxState);

        if ( TA_Base_Core::Thread::THREAD_STATE_RUNNING == m_eStepState )
        {
            m_eStepState = TA_Base_Core::Thread::THREAD_STATE_TERMINATED;
        }
        else if ( TA_Base_Core::Thread::THREAD_STATE_NEW == m_eStepState )
        {
            m_eStepState = TA_Base_Core::Thread::THREAD_STATE_TERMINATED;
            return;
        }
        else if ( TA_Base_Core::Thread::THREAD_STATE_TERMINATED == m_eStepState )
        {
            // Reset to new state
            m_eStepState = TA_Base_Core::Thread::THREAD_STATE_NEW;
            return;
        }
        else
        {
            // Default
            m_eStepState = TA_Base_Core::Thread::THREAD_STATE_NEW;
            return;
        }
    }

    try
    {
        stop();
    }
    catch ( const std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Caught unknown exception when stop" );
    }
}

bool ActivePlanStep::isRunning()
{
    ACE_Guard<ACE_Mutex> gudLock(m_mtxState);

    return ( m_eStepState == TA_Base_Core::Thread::THREAD_STATE_RUNNING );
}

void ActivePlanStep::reset()
{
    ACE_Guard<ACE_Mutex> gudLock(m_mtxState);
    m_eStepState = TA_Base_Core::Thread::THREAD_STATE_NEW;
}

}