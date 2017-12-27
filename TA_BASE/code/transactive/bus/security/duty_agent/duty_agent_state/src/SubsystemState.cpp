/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/SubsystemState.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
///////////////////////////////////////////////////////////
//  SubsystemState.cpp
//  Implementation of the Class SubsystemState
//  Created on:      08-Jul-2004 01:35:03 PM
//  Original author: Gregg Kirkpatrick
///////////////////////////////////////////////////////////

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	const SubsystemState::State SubsystemState::m_stateMachine[3][4][2] = {
		/*Current State,    Transition,              Profile Type, Result
		-----------------------------------------------------------------*/
		/*SS_NORMAL   */ {/*TR_ALLOC        */ {/*PT_CENTRAL*/ SS_NORMAL   ,
		/*SS_NORMAL   */  /*TR_ALLOC        */  /*PT_LOCAL  */ SS_NORMAL   },
		/*SS_NORMAL   */  /*TR_INDIRECT_XFER*/ {/*PT_CENTRAL*/ SS_NORMAL   ,
		/*SS_NORMAL   */  /*TR_INDIRECT_XFER*/  /*PT_LOCAL  */ SS_NORMAL   },
		/*SS_NORMAL   */  /*TR_DIRECT_XFER  */ {/*PT_CENTRAL*/ SS_NORMAL   ,
		/*SS_NORMAL   */  /*TR_DIRECT_XFER  */  /*PT_LOCAL  */ SS_DELEGATED},
		/*SS_NORMAL   */  /*TR_DEGRADE      */ {/*PT_CENTRAL*/ SS_DEGRADED ,
		/*SS_NORMAL   */  /*TR_DEGRADE      */  /*PT_LOCAL  */ SS_DEGRADED }},
		/*SS_DELEGATED*/ {/*TR_ALLOC        */ {/*PT_CENTRAL*/ SS_DELEGATED,
		/*SS_DELEGATED*/  /*TR_ALLOC        */  /*PT_LOCAL  */ SS_DELEGATED},
		/*SS_DELEGATED*/  /*TR_INDIRECT_XFER*/ {/*PT_CENTRAL*/ SS_DELEGATED,
		/*SS_DELEGATED*/  /*TR_INDIRECT_XFER*/  /*PT_LOCAL  */ SS_DELEGATED},
		/*SS_DELEGATED*/  /*TR_DIRECT_XFER  */ {/*PT_CENTRAL*/ SS_NORMAL   ,
		/*SS_DELEGATED*/  /*TR_DIRECT_XFER  */  /*PT_LOCAL  */ SS_DELEGATED},
		/*SS_DELEGATED*/  /*TR_DEGRADE      */ {/*PT_CENTRAL*/ SS_DEGRADED ,
		/*SS_DELEGATED*/  /*TR_DEGRADE      */  /*PT_LOCAL  */ SS_DEGRADED }},
		/*SS_DEGRADED */ {/*TR_ALLOC        */ {/*PT_CENTRAL*/ SS_DEGRADED ,
		/*SS_DEGRADED */  /*TR_ALLOC        */  /*PT_LOCAL  */ SS_DEGRADED },
		/*SS_DEGRADED */  /*TR_INDIRECT_XFER*/ {/*PT_CENTRAL*/ SS_DEGRADED ,
		/*SS_DEGRADED */  /*TR_INDIRECT_XFER*/  /*PT_LOCAL  */ SS_DEGRADED },
		/*SS_DEGRADED */  /*TR_DIRECT_XFER  */ {/*PT_CENTRAL*/ SS_NORMAL   ,
		/*SS_DEGRADED */  /*TR_DIRECT_XFER  */  /*PT_LOCAL  */ SS_DEGRADED },
		/*SS_DEGRADED */  /*TR_DEGRADE      */ {/*PT_CENTRAL*/ SS_DEGRADED ,
		/*SS_DEGRADED */  /*TR_DEGRADE      */  /*PT_LOCAL  */ SS_DEGRADED }}
	};
		
	SubsystemState::SubsystemState() :
		m_currentState(SS_NORMAL),
		m_nextState   (SS_NORMAL)
	{
	}

	SubsystemState::SubsystemState( const SubsystemState& copyThis ) :
		m_currentState(copyThis.m_currentState),
		m_nextState   (copyThis.m_nextState)
	{
	}

	SubsystemState::State SubsystemState::getCurrentState() const
	{
		return( m_currentState );
	}

	SubsystemState::State SubsystemState::getNextState() const
	{
		return( m_nextState );
	}

	SubsystemState::State SubsystemState::getNextState( SubsystemState::Transition trans, const DutyPrimitive& session ) const
	{
		ProfileType pt;
		if( IDutyPolicy::PT_LOCAL == DutyDbCache::instance()->getProfileType( session.m_profile ) )
		{
			pt = PT_LOCAL;
		}
		else
		{
			pt = PT_CENTRAL;
		}

		return( m_stateMachine[m_currentState][trans][pt] );
	}

	void SubsystemState::setNextState( SubsystemState::Transition trans, const DutyPrimitive& session )
	{
		m_nextState = getNextState( trans, session );
	}

	void SubsystemState::setNextState( SubsystemState::Transition trans )
	{
		switch(trans)
		{
		case TR_DEGRADE:
			m_nextState = m_stateMachine[m_currentState][trans][PT_CENTRAL];
			break;

		default:
			std::ostringstream msg;
			msg << "Invalid transition: " << trans << ", expecting TR_DEGRADE"; 
			TA_ASSERT( TR_DEGRADE != trans, msg.str().c_str() );
			break;
		};
	}

	SubsystemState::State SubsystemState::transition()
	{
		m_currentState = m_nextState;
		return( m_currentState );
	}

	void SubsystemState::set( const SubsystemState::State& state )
	{
		m_currentState = state;
		m_nextState    = state;
	}
	void SubsystemState::cancel()
	{
		m_nextState    = m_currentState;
	}
}
