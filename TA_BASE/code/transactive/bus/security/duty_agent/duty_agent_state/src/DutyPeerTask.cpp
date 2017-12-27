/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyPeerTask.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 * Manages the messaging and threading for the peers.
 * 
 * For C830 there will most likely be a single thread per peer. However for other
 * projects that have multiple regions per location, this could be increased.
 */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	const unsigned long DutyPeerTask::THREAD_COUNT = 5;

	DutyPeerTask::DutyPeerTask(std::auto_ptr<IDutyPeer> dutyPeer,bool local, int threadCount) :
		m_bLocalPeer(local),

		m_threadCount(threadCount),
		m_isRunning(false),
		m_dutyPeer(dutyPeer),
		m_activationQueue(msg_queue_)
	{
		TA_ASSERT( NULL != m_dutyPeer.get(), "NULL IDutyPeer!" );
		if(m_threadCount <=0)
		{
			m_threadCount = THREAD_COUNT;
		}
	}


	DutyPeerTask::~DutyPeerTask()
	{

	}


	int DutyPeerTask::open(void*)
	{
		m_isRunning = true;
		if(msg_queue_->state() == ACE_Message_Queue_Base::DEACTIVATED || msg_queue_->state()==ACE_Message_Queue_Base::PULSED)
		{
			msg_queue_->activate();
		}
		return activate(THR_JOINABLE, m_threadCount);
	}


	int DutyPeerTask::close(u_long flags /*=0*/)
	{
		return 0;
	}

	
	void	DutyPeerTask::terminate()
	{
		m_isRunning = false;
		msg_queue_->pulse();
	}

	
	void DutyPeerTask::terminateAndWait()
	{
		this->terminate();
		this->wait();
	}

	void DutyPeerTask::setDutyPeer(std::auto_ptr<IDutyPeer> dutyPeer)
	{
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_peerMutex);
		m_dutyPeer = dutyPeer;
	}


	int DutyPeerTask::svc(void )
	{
		int rval = 0;

		try
		{
			while( m_isRunning )
			{
				std::auto_ptr<ACE_Method_Request> mo(m_activationQueue.dequeue());
				if(mo.get() != NULL)
				{
					LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "queue size is %lu",m_activationQueue.method_count());
					if(m_bLocalPeer)
					{
						LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugWarn, "DutyPeerTask for LocalDutyPeer should not go here");
					}
					// If we get to here we've got an object on the activation queue
					if( DASConst::ACE_ERROR == mo->call() )
					{
						LOG( SOURCE_INFO, GENERIC_LOG_TYPE, WARN_LOG_LEVEL, "Method call error." );
						break;
					}
				}
			}
		}
		catch( const std::exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, typeid(e).name(), e.what(), "Unhandled." );
			rval = -1;
		}
		catch( const CORBA::Exception& e )
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Unhandled." );
			rval = -1;
		}
		catch( ... )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Unhandled." );
			rval = -1;
		}
		return(rval);
	}


	ACE_Future<bool> DutyPeerTask::sessionBegin( DutyState& state, const SessionInfo& session )
	{
		ACE_Future<bool> future;

		if(m_bLocalPeer)
		{
			try{

				this->sessionBeginImpl(state,session);
				future.set(true);
			}
			catch(...)
			{
				future.set(false);
			}
		}
		else
		{
			DutyPeerTaskMO* newMO = new SessionBeginMO( *this, future, state, session );			
			m_activationQueue.enqueue( newMO );
		}
		return( future );
	}


	ACE_Future<bool> DutyPeerTask::sessionEnd( DutyState& state, const SessionInfo& session, const DASType::RegionSet& regions )
	{
		ACE_Future<bool> future;
		
		if(m_bLocalPeer)
		{
			try
			{
				this->sessionEndImpl(state,session,regions);
				future.set(true);
			}
			catch(...)
			{
				future.set(false);
			}
			
		}
		else
		{
			DutyPeerTaskMO* newMO = new SessionEndMO( *this, future, state, session, regions );
			
			m_activationQueue.enqueue( newMO );
		}
		

		return( future );
	}


	ACE_Future<bool> DutyPeerTask::sessionOverrideBegin( DutyState& state, const SessionInfo& session )
	{
		ACE_Future<bool> future;
		
		if(m_bLocalPeer)
		{
			try
			{
				this->sessionOverrideBeginImpl(state,session);
				future.set(true);
			}
			catch (...)
			{				
			    future.set(false);
			}
		}
		else
		{
			DutyPeerTaskMO* newMO = new SessionOvrBeginMO( *this, future, state, session );
			
			m_activationQueue.enqueue( newMO );
		}
		

		return( future );
	}


	ACE_Future<bool> DutyPeerTask::sessionOverrideEnd( DutyState& state, const SessionInfo& session )
	{
		ACE_Future<bool> future;

		if(m_bLocalPeer)
		{
			try
			{
				sessionOverrideEndImpl(state,session);
				future.set(true);
			}
			catch (...)
			{				
			    future.set(false);
			}
		}
		else
		{
			DutyPeerTaskMO* newMO = new SessionOvrEndMO( *this, future, state, session );
			
			m_activationQueue.enqueue( newMO );
		}
		
		return( future );
	}


	ACE_Future<bool> DutyPeerTask::transfer( DutyState& state, const TA_Base_Bus::SessionInfo& session,
			const DASType::RegionSet& regions )
	{
		ACE_Future<bool> future;

		if(m_bLocalPeer)
		{
			try
			{
				transferImpl(state,session,regions);
				future.set(true);
			}
			catch (...)
			{
			    future.set(false);
			}
		}
		else
		{
			DutyPeerTaskMO* newMO = new TransferRegionsMO( *this, future, state, session, regions );
			
			m_activationQueue.enqueue( newMO );
		}
		
		return( future );
	}


	ACE_Future<bool> DutyPeerTask::transfer( DutyState& state, const TA_Base_Bus::SessionInfo& session,
			const  DASType::SubsystemMap& subsystems )
	{
		ACE_Future<bool> future;

		if(m_bLocalPeer)
		{
			try
			{
				this->transferImpl(state,session, subsystems);
				future.set(true);
			}
			catch (...)
			{
			    future.set(false);
			}
		}
		else
		{		
			DutyPeerTaskMO* newMO = new TransferSubsystemsMO( *this, future, state, session, subsystems );

			m_activationQueue.enqueue( newMO );
		}

		return( future );
	}


	ACE_Future<bool> DutyPeerTask::degrade( DutyState& state, const DASType::SubsystemSet& subsystems )
	{
		ACE_Future<bool> future;

		if(m_bLocalPeer)
		{
			try{
				this->degradeImpl(state,subsystems);
				future.set(true);
			}
			catch(...)
			{
				future.set(false);
			}
			
		}
		else
		{
			DutyPeerTaskMO* newMO = new DegradeMO( *this, future, state, subsystems );

			m_activationQueue.enqueue( newMO );

		}
		return( future );
	}


	ACE_Future<bool> DutyPeerTask::executeQuery( DutyState& state, IDutyQuery& query )
	{
		ACE_Future<bool> future;

		if(m_bLocalPeer)
		{
			try
			{
				executeQueryImpl(state,query);
				future.set(true);
			}
			catch(...)
			{
				future.set(false);
			}
			
			
		}
		else
		{
			DutyPeerTaskMO* newMO = new ExecuteQueryMO( *this, future, state, query );			
			m_activationQueue.enqueue( newMO );
		}
		
		return( future );
	}

	void DutyPeerTask::sessionBeginImpl(DutyState& state, const SessionInfo& session)
	{
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_peerMutex);
		m_dutyPeer->sessionBegin( state, session );
	}

	void DutyPeerTask::sessionEndImpl(DutyState& state, const SessionInfo& session, const DASType::RegionSet& regions)
	{
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_peerMutex);
		m_dutyPeer->sessionEnd( state, session, regions );
	}

	void DutyPeerTask::sessionOverrideBeginImpl(DutyState& state, const SessionInfo& session)
	{
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_peerMutex);
		m_dutyPeer->sessionOverrideBegin( state, session );
	}

	void DutyPeerTask::sessionOverrideEndImpl(DutyState& state, const SessionInfo& session)
	{
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_peerMutex);
		m_dutyPeer->sessionOverrideEnd( state, session );
	}

	void DutyPeerTask::transferImpl( DutyState& state, const TA_Base_Bus::SessionInfo& session, 
		const DASType::RegionSet& regions )
	{
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_peerMutex);
		m_dutyPeer->transfer( state, session, regions );
	}

	void DutyPeerTask::transferImpl( DutyState& state, const TA_Base_Bus::SessionInfo& session,
			const  DASType::SubsystemMap& subsystems )
	{
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_peerMutex);
		m_dutyPeer->transfer( state, session, subsystems );
	}

	void DutyPeerTask::degradeImpl( DutyState& state, const DASType::SubsystemSet& subsystems )
	{
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_peerMutex);
		m_dutyPeer->degrade( state, subsystems );
	}

	void DutyPeerTask::executeQueryImpl( DutyState& state, IDutyQuery& query )
	{
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_peerMutex);
		m_dutyPeer->executeQuery( state, query );
	}

}


