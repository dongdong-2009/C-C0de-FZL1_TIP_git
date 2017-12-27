/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/Subsystem.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  * It maintains a session and profile list that is updated according to the duty
  * policy. In monitor mode, it also processes incoming udpates from the
  * appropriate control mode Duty Agent.
  */

#if !defined(EA_FA854F50_5FA5_4194_80CA_87F4DE674C60__INCLUDED_)
#define EA_FA854F50_5FA5_4194_80CA_87F4DE674C60__INCLUDED_

#include <iosfwd>
namespace TA_Base_Bus
{
	class Subsystem
	{

	public:
		Subsystem( const DASType::DbKey& key );

		/**
		 * An operator has logged on.
		 * @param state
		 * 
		 */
		void sessionBegin(DutyState& state, const DutyPrimitive& session);

		/**
		 * @param subject
		 * 
		 */
		void sessionEnd(DutyState& state, const DutyPrimitive& session);

		/**
		 * An operator has (directly) requested duty for the subsystem.
		 */
		void directTransfer(DutyState& state, const DutyPrimitive& session,bool apply);

		/**
		 * An operator has taken duty for the region associated with the subsystem.
		 */
		void indirectTransfer(DutyState& state, const DutyPrimitive& session);

		void degrade(DutyState& state, const DASType::DutySet& eligibleSessions);

		/**
		 * @param query
		 * 
		 */
		void executeQuery(DutyState& state, IDutyQuery& query) const;
	private:
		void cancel( DutyState& state );
		void apply ( DutyState& state );
	public:		
		void stateUpdate( const DASType::DutySet& add, const DASType::DutySet& rem,
			const SubsystemState::State& subState );

// CL19422 ++
//#ifdef DEBUG_DUTY_DETAIL
		/**
		 * print current session profile
		 */
		void print(std::ostream& out) const;
//#endif //DEBUG_DUTY_DETAIL
// ++ CL19422
	private:
		
		Subsystem( const Subsystem& );
		Subsystem& operator=( const Subsystem&);

		void processPolicyResult( DutyState& state, const DutyPrimitive& dstSession,
			const IDutyPolicy::POLICY_RESULT& result );

		// This is used for going into degraded mode and for direct transfer of subsystems
		void privateDirectTransfer(DutyState& state, const DutyPrimitive& session);

		bool isDuplicate(const DutyPrimitive& session) const;

		PendingDutyHelper m_dutyHelper;
		SubsystemState m_subState;
		DASType::DbKey m_subsystemKey;

		DutyPrimitive    m_dutyToReplace;
		DASType::DutySet m_dutyNotToAdd;

		mutable ACE_RW_Mutex  m_mutex;
	};
}







#endif // !defined(EA_FA854F50_5FA5_4194_80CA_87F4DE674C60__INCLUDED_)
