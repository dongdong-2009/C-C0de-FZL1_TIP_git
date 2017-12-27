// AuthenPeerTask.cpp: implementation of the AuthenPeerTask class.
//
//////////////////////////////////////////////////////////////////////



#include "bus/security/authentication_library/src/AuthenPeerTask.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "bus/security/authentication_library/src/GetActiveSessionsMO.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Bus
{
	const ACE_Time_Value AuthenPeerTask::m_msgQueueTimeout(0,  1000); // short
	const ACE_Time_Value AuthenPeerTask::m_activationQueueTimeout(0,  100000); // long
	const unsigned long  AuthenPeerTask::THREAD_COUNT = 1;

	AuthenPeerTask::AuthenPeerTask(AuthenticationAgentNamedObj * pAuthAgent, 
		AuthenticationLibrary * pAuthLib, unsigned long locationId)
		:m_pAuthAgent(pAuthAgent),
		 m_pAuthLib( pAuthLib),
		 m_locationId(locationId)
	{
		TA_Assert( m_pAuthLib != NULL && m_pAuthAgent != NULL );
	}


	AuthenPeerTask::~AuthenPeerTask()
	{
		ACE_Message_Block* aceMsg = NULL;
		int nResult = getq(aceMsg);

		aceMsg->release();
	}

	int AuthenPeerTask::open(void*)
	{
		return activate(THR_JOINABLE, THREAD_COUNT);
	}
		
	int AuthenPeerTask::close(u_long flags )
	{
		return 0;
	}

	int AuthenPeerTask::svc(void)
	{
		int rval = 0;

		try
		{
			while( true )
			{
				// check the activation queue
				ACE_Time_Value aqAbsTime( ACE_OS::gettimeofday() + m_activationQueueTimeout );
				size_t nSize = m_activationQueue.method_count();
				if ( nSize >= 1)
				{
					LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "authentication agnet at location %d has method request", m_locationId);
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
						TA_THROW( TA_Base_Core::AuthenticationAgentException( what.str().c_str() ) );
					}
					
					// Check the Message Queue
					ACE_Message_Block* aceMsg = NULL;
				
					ACE_Time_Value mqAbsTime( ACE_OS::gettimeofday() + m_msgQueueTimeout );
					
					if( AuthenLibConst::ACE_ERROR == getq(aceMsg, &mqAbsTime) )
					{
						if ( EWOULDBLOCK != ACE_OS::last_error() )
						{
							std::ostringstream what;
							what << "ACE_Task::getq failed with errno " << ACE_OS::last_error() << " at location "<< m_locationId;
							TA_THROW( TA_Base_Core::AuthenticationAgentException( what.str().c_str() ) );
						}
					}
					else
					{
						if( ACE_Message_Block::MB_HANGUP == aceMsg->msg_type() )
						{
							int nResult = putq( aceMsg );
							LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Received MB_HANGUP message at location %d, putQ result = %d", m_locationId, nResult );
							break;
						}
					}

				}
				else // Method to execute
				{
					// If we get to here we've got an object on the activation queue
					if( AuthenLibConst::ACE_ERROR == mo->call() )
					{
						LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Method call error when exceute authen peer task at location %d", m_locationId );
						break;
					}
				}
			}
		}
		catch( const std::exception& e )
		{
			std::string errorStr = e.what();
			LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "std::exception: %s when run task at locationId %d", errorStr.c_str(), m_locationId );
			rval = -1;
		}
		catch( const CORBA::Exception& e )
		{
			std::string errorStr = TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
			LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "corba::exception: %s when run task at locationId %d", errorStr.c_str(), m_locationId );
			rval = -1;
		}
		catch( ... )
		{
			LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "unknown exception when run task at location %d", m_locationId );
			rval = -1;
		}

		LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "task runing at location %d will finish", m_locationId );
		return(rval);
	}

	ACE_Future<bool> AuthenPeerTask::getActiveSessions( const std::string & noUseSessionId )
	{
		ACE_Future<bool> execResult;
		
		//no need to delete, because it is owned by auto_ptr in AuthenPeerTask::svc
		GetActiveSessionsMO* newMO = new GetActiveSessionsMO( this, noUseSessionId, execResult );

		m_activationQueue.enqueue( newMO );

		return execResult;

	}

	//the exception will dealed in GetActiveSessionsMO::Execute()
	void AuthenPeerTask::getActiveSessionsImpl( const std::string & noUseSessionId)
	{
		TA_Assert( m_pAuthAgent != NULL && m_pAuthLib != NULL );
		m_vtActiveSessionInfo.clear();
		
		m_pAuthLib->getActiveSessionsForOneLocation( noUseSessionId, *m_pAuthAgent, m_vtActiveSessionInfo );
		LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "get active sessions, its session size = %d at location %d", m_vtActiveSessionInfo.size(), m_locationId);
	}

}