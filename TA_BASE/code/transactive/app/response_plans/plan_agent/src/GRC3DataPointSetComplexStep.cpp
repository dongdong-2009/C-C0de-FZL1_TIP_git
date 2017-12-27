/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/GRC3DataPointSetComplexStep.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2013/01/31 20:24:40 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "app/response_plans/common/src/ProjectDefinition.h"

#if defined ( STEPTYPE_GRC )

#include <map>

#include "core/utilities/src/DebugUtil.h"
#include "core/threads/src/Thread.h"
#include "app/response_plans/common/src/StringConstants.h"
#include "app/response_plans/plan_agent/src/GRCType3SubTask.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActiveDataPointSetStep.h"
#include "app/response_plans/plan_agent/src/GRC3DataPointSetComplexStep.h"


namespace TA_Base_App
{

    GRC3DataPointSetComplexStep::GRC3DataPointSetComplexStep(const TA_Base_Core::StepDetail& stepDetail, IActivePlanFlow& flowControl) : 
    ActiveStep(stepDetail, flowControl),
    m_vtDataPointSetSubTask(),
    m_vtActiveDataPointSetStep(),
    m_pChildStep( NULL )
	{
		m_detail.ignoreFailure = false;
		m_detail.skip = false;
        m_detail.skippable = false;
        m_detail.nowait = false;
        m_detail.delay = 0u;
        m_detail.type = TA_Base_Core::UNDEFINED_STEP; // Should be added in future
	}

	GRC3DataPointSetComplexStep::~GRC3DataPointSetComplexStep()
    {
        if ( NULL != m_pChildStep )
        {
            delete m_pChildStep;
            m_pChildStep = NULL;
        }

		m_vtActiveDataPointSetStep.clear();
    }

    void GRC3DataPointSetComplexStep::customise( const TA_Base_Core::StepCustomisableDetail& newDetail )
    {
        if ( NULL != m_pChildStep )
        {
            m_pChildStep->customise( newDetail );
        }
    }

	void GRC3DataPointSetComplexStep::startAllSubTaskThread()
	{
		LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "begin to start sub task %d for GRC Type 3 Step %d", m_vtActiveDataPointSetStep.size(), m_detail.position);

        unsigned long currentStep = m_detail.position;
        for ( std::vector<ActiveDataPointSetStep*>::iterator itLoop = m_vtActiveDataPointSetStep.begin(); m_vtActiveDataPointSetStep.end() != itLoop; ++itLoop )
        {
			std::auto_ptr<GRCType3SubTask> newTask( new GRCType3SubTask(currentStep++, *itLoop) );
			newTask->open(0);
			m_vtDataPointSetSubTask.push_back(newTask.release());
		}
	}

	void GRC3DataPointSetComplexStep::stopAllSubTaskThread()
	{
		LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "begin to end sub task %d for GRC Typ3 3 Step %d", m_vtActiveDataPointSetStep.size(), m_detail.position);

		std::vector<GRCType3SubTask*>::iterator it;

		for( size_t i=0; i< m_vtDataPointSetSubTask.size(); i++ )
		{
			ACE_Message_Block* msg = new ACE_Message_Block;

			msg->msg_type( ACE_Message_Block::MB_HANGUP );

			if( -1 != m_vtDataPointSetSubTask[i]->putq(msg) )
			{
				m_vtDataPointSetSubTask[i]->wait();
				LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "finish sub task for step %d end", m_detail.position+i);
			}
		}
		
		//no need to wait again
		//ACE_Thread_Manager::instance()->wait();
		LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "all running thread for task should finish after manager wait");

		for( it = m_vtDataPointSetSubTask.begin(); it != m_vtDataPointSetSubTask.end(); it++ )
		{
			delete (*it);
		}
		m_vtDataPointSetSubTask.clear();
    }

    void GRC3DataPointSetComplexStep::execute( void )
    {
        m_remark = RemarkExecutingNoWaitSteps;

        try
        {
            executeImpl();
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "unknown", dumpContext().c_str() );
        }
    }

	TA_Base_Core::EActiveStepState GRC3DataPointSetComplexStep::executeImpl( void )
	{
		startAllSubTaskThread();

		bool fFinalResult = doSetDataPoint();

		m_flowControl.setCurrentStepPosition(m_detail.position + m_vtActiveDataPointSetStep.size());

		stopAllSubTaskThread();

		//always return EXECUTED_ACTIVE_STEP_STATE, because ignore failure
		return TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;
    }

    void GRC3DataPointSetComplexStep::setChildStep( ActiveDataPointSetStep* pDataPointSetStep )
    {
        m_pChildStep = pDataPointSetStep;
    }

	void  GRC3DataPointSetComplexStep::addChildStep(ActiveDataPointSetStep *pDataPointSetStep)
	{
        m_vtActiveDataPointSetStep.push_back(pDataPointSetStep);
	}
	
	bool GRC3DataPointSetComplexStep::doSetDataPoint()
	{
		std::map< unsigned long, ACE_Future<bool> > vtSubTaskResult;
		
		size_t i=0;
		for( i=0; i<m_vtDataPointSetSubTask.size(); i++ )
		{
			GRCType3SubTask * pSubTask = m_vtDataPointSetSubTask[i];
            if ( NULL == pSubTask )
            {
                continue;
            }
			ACE_Future<bool> future = pSubTask->doSetDataPoint();
			vtSubTaskResult[i] = future;
		}

		TA_Base_Core::Thread::sleep(2000);

		bool fFinalResult = true;
		bool fHasError = false;

		for( i=0; i<m_vtDataPointSetSubTask.size(); i++ )
		{
			bool subTaskResult = false;
            GRCType3SubTask * pSubTask = m_vtDataPointSetSubTask[i];
            if ( NULL == pSubTask )
            {
                continue;
            }

			ACE_Future<bool> resultFuture = vtSubTaskResult[i];

			if( -1 == resultFuture.get(subTaskResult) )
			{
				LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "set Datapoint value Failed in GRC step %d", m_detail.position+i);
				fHasError = true;
				continue;
			}

			if( !subTaskResult )
			{
				LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "set Datapoint value Failed in GRC step index %d, future return false", m_detail.position+i);
			}
			else
			{
				LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "set Datapoint value successfully in GRC step index %d, future return true", m_detail.position+i);
			}

			fFinalResult &= subTaskResult;

		}

		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"set GRC Type 3 data points finished, including %d sub steps", 
			m_vtDataPointSetSubTask.size());

		if ( fHasError )
		{
			return false;
		}

		return fFinalResult;
	}
}

#endif