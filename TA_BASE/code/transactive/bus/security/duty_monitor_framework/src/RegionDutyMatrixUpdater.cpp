// RegionDutyMatrixUpdater.cpp: implementation of the RegionDutyMatrixUpdater class.
//
//////////////////////////////////////////////////////////////////////

#include "DutyMonitorFramework.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus
{
	const ACE_Time_Value RegionDutyMatrixUpdater::m_mqTimeout(0,   1000); // short
	const ACE_Time_Value RegionDutyMatrixUpdater::m_aqTimeout(0, 100000); // long
	const unsigned long RegionDutyMatrixUpdater::THREAD_COUNT = 1;
	
	void RegionDutyMatrixUpdater::updateSession(const std::string& sessionId, bool deleteFlag)
	{		
		RegionMatrixSessionUpdateMO* newMO = new RegionMatrixSessionUpdateMO(m_matrix, sessionId, deleteFlag );
		
		m_activationQueue.enqueue( newMO );
	}
	void RegionDutyMatrixUpdater::updateDuty( DutyAgentTypeCorbaDef::RegionDutySequence& regionDutySeq, EDutyUpdateTupe udpateType)
	{
		RegionMatrixDutyUpdateMO* newMO= new RegionMatrixDutyUpdateMO(m_matrix,regionDutySeq,udpateType );
		m_activationQueue.enqueue( newMO );
	}


	void RegionDutyMatrixUpdater::removeLocationDuty(unsigned long locationKey)
	{
		RemoveLocationDutyMO* newMO = new RemoveLocationDutyMO(m_matrix, locationKey);
		m_activationQueue.enqueue( newMO );
	}


	RegionDutyMatrixUpdater::RegionDutyMatrixUpdater(RegionDutyMatrix& matrix):
	m_matrix(matrix)
	{
		
	}
	
	RegionDutyMatrixUpdater::~RegionDutyMatrixUpdater()
	{
		ACE_Message_Block* aceMsg = NULL;
		getq(aceMsg);
		
		aceMsg->release();
	}
	int RegionDutyMatrixUpdater::open(void*)
	{
		return activate(THR_JOINABLE, THREAD_COUNT);
	}
	int RegionDutyMatrixUpdater::close(u_long flags /* = 0 */ )
	{
		return 0;
	}
	int RegionDutyMatrixUpdater::svc(void)
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
						TA_THROW( TA_Base_Bus::DutyMonitorFrameworkException( what.str() ) );
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
							TA_THROW( TA_Base_Bus::DutyMonitorFrameworkException( what.str() ) );
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
}
