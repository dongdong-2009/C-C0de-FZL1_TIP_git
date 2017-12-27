/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyPeerTaskResultHandler.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2015/01/21 10:20:13 $
  *  Last modified by:  $Author: liwei.gong $
  *
  *  Knows how to collate results from a command performed on multiple DutyPeerTask objects.
  */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	DutyPeerTaskResultHandler::Result::Result( const DutyState& state ) :
		m_state(state)
	{
	}

	DutyPeerTaskResultHandler::Result::Result( const Result& copyThis ) :
		m_state (copyThis.m_state ),
		m_future(copyThis.m_future)
	{
	}


	DutyPeerTaskResultHandler::DutyPeerTaskResultHandler( DutyState& state ) :
		m_state(state)
	{
	}

	void DutyPeerTaskResultHandler::set( const DASType::DbKey& location, DutyPeerTask& dpt,
		std::auto_ptr<IDutyPeerTaskCommand> command ,bool directCall )
	{
		set( location, dpt, *command,directCall );
		m_commandSet.insert( command.release() ); 
	}

	void DutyPeerTaskResultHandler::set( const DASType::DbKey& location, DutyPeerTask& dpt,
		IDutyPeerTaskCommand& command, bool directCall )
	{
		Result* result = new Result(m_state);

		try
		{
			result->m_future = command.dispatchExecute(dpt, result->m_state,directCall);
		}
		catch(...)
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Unhandled." );
			TA_ASSERT( false, "All exceptions MUST be caught before this point" );
		}

		m_resultMap.insert( ResultMap::value_type(location,result) );
	}
	
	DutyPeerTaskResultHandler::~DutyPeerTaskResultHandler()
	{
		{
			ResultMap::iterator it;

			for( it = m_resultMap.begin(); it != m_resultMap.end(); it++ )
			{
				delete it->second;
				it->second = NULL;
			}
			m_resultMap.clear();
		}

		{
			CommandSet::iterator it;

			for( it = m_commandSet.begin(); it != m_commandSet.end(); it++ )
			{
				delete (*it);
			}
			m_commandSet.clear();
		}
	}

	void DutyPeerTaskResultHandler::waitOnAll( DASType::LocationSet& failedPeers )
	{
		ResultMap::iterator it;

		for( it = m_resultMap.begin(); it != m_resultMap.end(); it++ )
		{
			bool peerOk = false;

			// Should wait "forever" here unless we use reference counted
			// pointers for duty state. If we don't wait forever then we will
			// be deleting stuff (possibly) before the peer thread has
			// finished with it.
			if( DASConst::ACE_ERROR == it->second->m_future.get(peerOk) )
			{
				failedPeers.insert(it->first);
				continue;
			}

			if( !peerOk )
			{
				failedPeers.insert(it->first);
				continue;
			}

			m_state.insert( it->second->m_state );
		}
	}

} // TA_Base_Bus
