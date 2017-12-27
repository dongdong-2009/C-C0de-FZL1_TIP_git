// PeerStateChangeTask.cpp: implementation of the PeerStateChangeTask class.
//
//////////////////////////////////////////////////////////////////////
#include "DutyAgentState.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus
{
	
	const ACE_Time_Value PeerStateChangeTaskImpl::m_mqTimeout(0,   1000); // short
	const ACE_Time_Value PeerStateChangeTaskImpl::m_aqTimeout(0, 100000); // long
	const unsigned long PeerStateChangeTaskImpl::THREAD_COUNT = 1;
	
	PeerStateChangeTaskImpl::PeerStateChangeTaskImpl()
	{
		this->open(0);
	}
	
	PeerStateChangeTaskImpl::~PeerStateChangeTaskImpl()
	{
		ACE_Message_Block* aceMsg = NULL;
		getq(aceMsg);
		
		aceMsg->release();
	}

	int PeerStateChangeTaskImpl::open(void*)
	{
		return activate(THR_JOINABLE, THREAD_COUNT);
	}

	int PeerStateChangeTaskImpl::close(u_long flags /* = 0 */ )
	{
		return 0;
	}
	int PeerStateChangeTaskImpl::svc(void)
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
					
					if( DASConst::ACE_ERROR == getq(aceMsg, &mqAbsTime) )
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
	void PeerStateChangeTaskImpl::updateSubject(DutyState& state)
	{
		PeerStateChangeMO* newMO = new PeerStateChangeMO( state );
		
		m_activationQueue.enqueue( newMO );
	}

}
