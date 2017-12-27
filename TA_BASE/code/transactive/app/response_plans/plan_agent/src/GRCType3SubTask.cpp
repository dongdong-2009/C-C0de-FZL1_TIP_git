// GRCType3SubTask.cpp: implementation of the GRCType3SubTask class.
//
//////////////////////////////////////////////////////////////////////

#include "ace/Time_Value.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/PlanAgentException.h"
#include "app/response_plans/common/src/ProjectDefinition.h"
#include "app/response_plans/plan_agent/src/ActivePlanMessageException.h"
#include "app/response_plans/plan_agent/src/ActiveDataPointSetStep.h"
#include "app/response_plans/plan_agent/src/GRCType3SubTaskMO.h"
#include "app/response_plans/plan_agent/src/GRCType3SubTask.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_App
{
    GRCType3SubTask::GRCType3SubTask(unsigned long stepNumber, ActiveDataPointSetStep* pStep) :
    m_activationQueue(),
    m_bInSvc( false ),
    m_stepNumber( stepNumber ),
    m_pStep( pStep )
	{
	}

	GRCType3SubTask::~GRCType3SubTask()
	{
		ACE_Message_Block* aceMsg = NULL;
		int nResult = getq(aceMsg);

		aceMsg->release();
	}

	int GRCType3SubTask::open(void*)
	{
		return activate(THR_JOINABLE, 1);
	}
		
	int GRCType3SubTask::close(u_long flags )
	{
        m_bInSvc = false;
		return 0;
	}

	int GRCType3SubTask::svc(void)
	{
		int rval = 0;
        m_bInSvc = true;

		try
		{
			while( m_bInSvc )
			{
                // check the activation queue
                static const ACE_Time_Value activationQueueTimeout(0,  100000); // long
				ACE_Time_Value aqAbsTime( ACE_OS::gettimeofday() + activationQueueTimeout );
				size_t nSize = m_activationQueue.method_count();
				if ( nSize >= 1)
				{
					LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "sub task for step number %d has method request", m_stepNumber);
				}

				std::auto_ptr<ACE_Method_Request> mo(m_activationQueue.dequeue(&aqAbsTime));
				
				// No method to execute.
				if( 0 == mo.get() )
				{
					// Check the activation queue is OK.
					if ( EWOULDBLOCK != ACE_OS::last_error() )
					{
						std::ostringstream what;
						what << "ACE_Activation_Queue::dequeue failed with errno " << ACE_OS::last_error();
						TA_THROW( ActivePlanMessageException( what.str().c_str() ) );
					}
					
					// Check the Message Queue
					ACE_Message_Block* aceMsg = NULL;

                    static const ACE_Time_Value msgQueueTimeout(0,  1000); // short
					ACE_Time_Value mqAbsTime( ACE_OS::gettimeofday() + msgQueueTimeout );
					
					if( -1 == getq(aceMsg, &mqAbsTime) )
					{
						if ( EWOULDBLOCK != ACE_OS::last_error() )
						{
							std::ostringstream what;
							what << "ACE_Task::getq failed with errno " << ACE_OS::last_error() << " for step "<< m_stepNumber;
							TA_THROW( ActivePlanMessageException( what.str().c_str() ) );
						}
					}
					else
					{
						if( ACE_Message_Block::MB_HANGUP == aceMsg->msg_type() )
						{
							int nResult = putq( aceMsg );
							LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Received MB_HANGUP message when execute sub task for step number %d, putQ result = %d", m_stepNumber, nResult );
							break;
						}
					}
				}
				else // Method to execute
				{
					// If we get to here we've got an object on the activation queue
					if( -1 == mo->call() )
					{
						LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Method call error when execute sub task for step number %d", m_stepNumber );
						break;
					}
				}
			}
		}
		catch( const std::exception& e )
		{
			std::string errorStr = e.what();
			LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "std::exception: %s when execute sub task for step number %d", errorStr.c_str(), m_stepNumber );
			rval = -1;
		}
		catch( ... )
		{
			LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "unknown exception when execute task for step number %d", m_stepNumber );
			rval = -1;
		}

		LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "task for runing step number %d finished", m_stepNumber );
		return(rval);
	}

	ACE_Future<bool> GRCType3SubTask::doSetDataPoint( )
	{
		ACE_Future<bool> execResult;
		
		GRCType3SubTaskMO* newMO = new GRCType3SubTaskMO( this, execResult );

		m_activationQueue.enqueue( newMO );

		return execResult;
	}
	
	void GRCType3SubTask::doSetDataPointImpl( )
	{
		m_pStep->execute();
	}

	ActiveDataPointSetStep * GRCType3SubTask::getDataPointSetStep()
	{
		return m_pStep;
	}

}