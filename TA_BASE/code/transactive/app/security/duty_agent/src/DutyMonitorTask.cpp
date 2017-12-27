/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/DutyMonitorTask.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/06/18 16:11:24 $
 * Last modified by:  $Author: haijun.li $
 *
 * Manages the messaging and threading for the peers.
 * 
 * For C830 there will most likely be a single thread per peer. However for other
 * projects that have multiple regions per location, this could be increased.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/DutyMonitorTask.h"

namespace TA_Base_App
{
	const unsigned long DutyMonitorTask::POLL_PERIOD_IN_MS = 5000;

	const ACE_Time_Value DutyMonitorTask::m_mqTimeout(0,    1000); 
	const ACE_Time_Value DutyMonitorTask::m_aqTimeout(0,  100000); 
	const unsigned long DutyMonitorTask::THREAD_COUNT = 3;

	DutyMonitorTask::StartMO::StartMO( DutyMonitorTask& task ) :
		m_task(task)
	{
	}

	int DutyMonitorTask::StartMO::call()
	{
		m_task.startImpl();
		return(0);
	}

	DutyMonitorTask::AddRegMO::AddRegMO( DutyMonitorTask& task, const TA_Base_Bus::DASType::DbKey& region ) :
		m_task(task),
		m_region(region)
	{
	}

	int DutyMonitorTask::AddRegMO::call()
	{
		m_task.addImpl(m_region);
		return(0);
	}

	DutyMonitorTask::RemRegMO::RemRegMO( DutyMonitorTask& task, const TA_Base_Bus::DASType::DbKey& region ) :
		m_task(task),
		m_region(region)
	{
	}

	int DutyMonitorTask::RemRegMO::call()
	{
		m_task.remImpl(m_region);
		return(0);
	}

	DutyMonitorTask::AddSubMO::AddSubMO( DutyMonitorTask& task, const TA_Base_Bus::DASType::DbKey& region,
		const TA_Base_Bus::DASType::DbKey& subsystem ) :
		m_task(task),
		m_region(region),
		m_subsystem(subsystem)
	{
	}

	int DutyMonitorTask::AddSubMO::call()
	{
		m_task.addImpl(m_region, m_subsystem);
		return(0);
	}

	DutyMonitorTask::RemSubMO::RemSubMO( DutyMonitorTask& task, const TA_Base_Bus::DASType::DbKey& region,
		const TA_Base_Bus::DASType::DbKey& subsystem ) :
		m_task(task),
		m_region(region),
		m_subsystem(subsystem)
	{
	}

	int DutyMonitorTask::RemSubMO::call()
	{
		m_task.remImpl(m_region, m_subsystem);
		return(0);
	}

	DutyMonitorTask::DutyMonitorTask() :
		m_stopSem (0, 0, 0, 1),
		m_startSem(1, 0, 0, 1)
	{
	}


	DutyMonitorTask::~DutyMonitorTask()
	{
		ACE_Message_Block* aceMsg = NULL;
		getq(aceMsg);

		aceMsg->release();
	}


	int DutyMonitorTask::open(void*)
	{
		return activate(THR_JOINABLE, THREAD_COUNT);
	}


	int DutyMonitorTask::close(u_long flags /*=0*/)
	{
		return(0);
	}


	int DutyMonitorTask::svc(void )
	{
		int rval = 0;

		try
		{
			while( true )
			{
				// check the activation queue
				ACE_Time_Value aqAbsTime( ACE_OS::gettimeofday() + m_aqTimeout );

				std::auto_ptr<ACE_Method_Request> mo(m_activationQueue.dequeue(&aqAbsTime));
				
				// No method to execute.
				if( 0 == mo.get() )
				{
					// Check the activation queue is OK.
					if ( EWOULDBLOCK != ACE_OS::last_error() )
					{
						std::ostringstream what;
						what << "ACE_Activation_Queue::dequeue failed with errno " << ACE_OS::last_error();
						TA_THROW( TA_Base_Core::DutyAgentException( what.str() ) );
					}
					
					// Check the Message Queue
					ACE_Message_Block* aceMsg = NULL;
				
					ACE_Time_Value mqAbsTime( ACE_OS::gettimeofday() + m_mqTimeout );
					
					if( TA_Base_Bus::DASConst::ACE_ERROR == getq(aceMsg, &mqAbsTime) )
					{
						if ( EWOULDBLOCK != ACE_OS::last_error() )
						{
							std::ostringstream what;
							what << "ACE_Task::getq failed with errno " << ACE_OS::last_error();
							TA_THROW( TA_Base_Core::DutyAgentException( what.str() ) );
						}
					}
					else
					{
						if( ACE_Message_Block::MB_HANGUP == aceMsg->msg_type() )
						{
							LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Received MB_HANGUP message." );
							putq( aceMsg );
							break;
						}
					}

				}
				else // Method to execute
				{
					try
					{
						mo->call();
					}
					catch( const std::exception& e )
					{
						LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, typeid(e).name(), e.what(), "Continuing to monitor." );
					}
					catch( const CORBA::Exception& e )
					{
						LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Continuing to monitor." );
					}
					catch( ... )
					{
						LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Continuing to monitor." );
					}
				}
			}
		}
		catch( const std::exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, typeid(e).name(), e.what(), "Unhandled." );
			rval = TA_Base_Bus::DASConst::ACE_ERROR;
		}
		catch( const CORBA::Exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Unhandled." );
			rval = TA_Base_Bus::DASConst::ACE_ERROR;
		}
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Unhandled." );
			rval = TA_Base_Bus::DASConst::ACE_ERROR;
		}

		return(rval);
	}


	void DutyMonitorTask::start()
	{
		StartMO* mo = new StartMO(*this);

		m_activationQueue.enqueue(mo);
	}
	
	void DutyMonitorTask::stop()
	{
		// if we would be unable to acquire the lock (i.e. start has
		// been called) then execute the stop. otherwise don't bother.
		if( TA_Base_Bus::DASConst::ACE_ERROR == m_startSem.tryacquire() )
		{
			m_stopSem.release();
			m_stopSig.signal();
		}
		else
		{
			// otherwise, we have decremented the semaphore and
			// need to put things back
			m_startSem.release();
		}
	}
	
	void DutyMonitorTask::add( TA_Base_Bus::DASType::DbKey region )
	{
		FUNCTION_ENTRY("add");

		AddRegMO* mo = new AddRegMO(*this, region);

		m_activationQueue.enqueue(mo);

		FUNCTION_EXIT;
	}

	void DutyMonitorTask::rem( TA_Base_Bus::DASType::DbKey region )
	{
		FUNCTION_ENTRY("rem");

		RemRegMO* mo = new RemRegMO(*this, region);

		m_activationQueue.enqueue(mo);

		FUNCTION_EXIT;
	}

	void DutyMonitorTask::add( TA_Base_Bus::DASType::DbKey region, TA_Base_Bus::DASType::DbKey subsystem  )
	{
		FUNCTION_ENTRY("add");

		AddSubMO* mo = new AddSubMO(*this, region, subsystem);

		m_activationQueue.enqueue(mo);

		FUNCTION_EXIT;
	}
	
	void DutyMonitorTask::rem( TA_Base_Bus::DASType::DbKey region, TA_Base_Bus::DASType::DbKey subsystem  )
	{
		FUNCTION_ENTRY("add");

		RemSubMO* mo = new RemSubMO(*this, region, subsystem);

		m_activationQueue.enqueue(mo);

		FUNCTION_EXIT;
	}

	void DutyMonitorTask::startImpl()
	{
		FUNCTION_ENTRY("startImpl");

		m_startSem.acquire();

		if( 0 == TA_Base_Bus::TransactiveAgentInfo::m_uncontrolledTimeout )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Not starting duty monitor with 0 sec timeout" );
			m_startSem.release();
			FUNCTION_EXIT;
			return;
		}

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Starting duty monitor with %lu sec timeout", TA_Base_Bus::TransactiveAgentInfo::m_uncontrolledTimeout );

		bool keepChecking = true;

		while( keepChecking )
		{
			int waitResult = 0;
		

			// pick up stops that are sent while we are not waiting
			if( TA_Base_Bus::DASConst::ACE_ERROR != m_stopSem.tryacquire() )
			{
				keepChecking = false;
			}
			else
			{
				if( EBUSY != ACE_OS::last_error() )
				{
					LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "errno %d attempting to acquire m_stopSem", ACE_OS::last_error() );
					keepChecking = false;
					continue;
				}

				waitResult = m_stopSig.timedWait(POLL_PERIOD_IN_MS);

				if( waitResult == 0 )
				{
					m_dutyMonitor.check();
				}
				else
				{
					m_stopSem.tryacquire(); // decrement the semaphore for next time
					keepChecking = false;
				}
			}
		}
		
		m_startSem.release();
		FUNCTION_EXIT;
	}

	void DutyMonitorTask::addImpl( TA_Base_Bus::DASType::DbKey region )
	{
		m_dutyMonitor.add(region);
	}

	void DutyMonitorTask::remImpl( TA_Base_Bus::DASType::DbKey region )
	{
		m_dutyMonitor.rem(region);
	}

	void DutyMonitorTask::addImpl( TA_Base_Bus::DASType::DbKey region, TA_Base_Bus::DASType::DbKey subsystem )
	{
		m_dutyMonitor.add(region, subsystem);
	}

	void DutyMonitorTask::remImpl( TA_Base_Bus::DASType::DbKey region, TA_Base_Bus::DASType::DbKey subsystem )
	{
		m_dutyMonitor.rem(region, subsystem);
	}
}


