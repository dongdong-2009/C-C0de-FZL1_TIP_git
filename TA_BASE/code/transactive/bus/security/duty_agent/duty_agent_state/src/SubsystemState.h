/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/SubsystemState.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  * The rights an operator has for a subsystem depend on its state.This
  * encapsulates the subsystem state machine so the appropriate rights are used.
  */

#if !defined(EA_44DDD2F1_E524_49e9_A68A_F0E89C8BFCC6__INCLUDED_)
#define EA_44DDD2F1_E524_49e9_A68A_F0E89C8BFCC6__INCLUDED_

namespace TA_Base_Bus
{
	class SubsystemState
	{
	public:
		enum State
		{
			SS_NORMAL,
			SS_DELEGATED,
			SS_DEGRADED
		};

		enum Transition
		{
			TR_ALLOC,
			TR_INDIRECT_XFER,
			TR_DIRECT_XFER,
			TR_DEGRADE
		};

		SubsystemState();
		SubsystemState( const SubsystemState& copyThis );

		State getCurrentState() const;
		State getNextState() const;
		State getNextState( Transition trans, const DutyPrimitive& session ) const;

		void  setNextState(Transition trans, const DutyPrimitive& session);
		void  setNextState(Transition trans);
		State transition();
		void  degrade();

		void set( const SubsystemState::State& state );
		
		/**
		 * add at: 2008-9-23, when Subsystem cancel() called, m_nextState should turn to m_currentState
		 *	
		 */
		void cancel();
	private:

		enum ProfileType
		{
			PT_CENTRAL,
			PT_LOCAL
		};
			
		State m_currentState;
		State m_nextState;

		// State, Transition, Profile Type
		const static State m_stateMachine[3][4][2];
	};
}








#endif // !defined(EA_44DDD2F1_E524_49e9_A68A_F0E89C8BFCC6__INCLUDED_)
