/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/Region.h $
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

#if !defined(EA_E15D9D76_DEFF_4e18_9AE2_C11C6F50B665__INCLUDED_)
#define EA_E15D9D76_DEFF_4e18_9AE2_C11C6F50B665__INCLUDED_

#include <iosfwd>
namespace TA_Base_Bus
{
	class Region
	{

	public:

		Region( const DASType::DbKey& key );
		~Region();

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
		 * An operator wants to transfer regions.
		 */
		void transfer(DutyState& state, const DutyPrimitive& session);

		/**
		 * An operator wants to transfer subsystems, prepare the data for prompt at controlstations.
		 * @param subsystems
		 * 
		 */
		void transfer(DutyState& state, const DutyPrimitive& session, const DASType::SubsystemSet& subsystems,bool apply);
		

		/**
		 * @param query
		 * 
		 */
		void executeQuery(DutyState& state, IDutyQuery& query);


	private:
 		void apply( DutyState& state );

		/**
		 * @param subsystems
		 * 
		 */
		void apply(DutyState& state, const DASType::SubsystemSet& subsystems);

		/**
		 * @param subsystems
		 * 
		 */
		void cancel(DutyState& state, const DASType::SubsystemSet& subsystems);
	public:
		void degradeSubsystems(DutyState& state, const DASType::SubsystemSet& subsystems );

		/**
		 * State updates
		 * @param subsystems
		 * 
		 */
		void stateUpdate( const DASType::DutySet& add, const DASType::DutySet& rem );
		void stateUpdate( const DASType::DbKey& subsystem, const DASType::DutySet& add,
			const DASType::DutySet& rem, const SubsystemState::State& subState );
// CL19422 ++
		/** CL19422 add
		 * regainLocalProfileDuty:
		 * NOTE: this function will call indirectTransfer for all local profile in this region
		 *			 after subsystem state change by transfer-direct. 
		 */
	private:
		void regainLocalProfileDuty(DutyState& state);
// ++ CL19422

// CL19422 ++
	private:
		/**
		 * print current session profile
		 */
		void print(std::ostream& out) const;
// ++ CL19422 		
	private:

		// Degrading subsystems is a special case for the IRegSubCommand pattern
		class SubDegradeCommand : public IRegSubCommand<Subsystem>
		{
		public:
			SubDegradeCommand( DutyState& state, const DASType::DutySet& eligibleSessions );
			virtual void execute(Subsystem& subsystem, bool doApply);

		private:
			DutyState& m_state;
			const DASType::DutySet& m_eligibleSessions;
		};

		Region( const Region& );
		Region& operator=( const Region&);

		// executes the command on all subsystems
		void subsystemExecute( IRegSubCommand<TA_Base_Bus::Subsystem>& command, bool doApply );

		// executes the command on the given subsystems
		void subsystemExecute( IRegSubCommand<TA_Base_Bus::Subsystem>& command, const DASType::SubsystemSet& subsystems, bool doApply );

		void processPolicyResult( DutyState& state, const DutyPrimitive& dstSession,
		const DutyPrimitive& dutyToReplace, const IDutyPolicy::POLICY_RESULT& result );

		PendingDutyHelper m_dutyHelper;

		typedef std::map<DASType::DbKey, Subsystem*> Subsystems;
		Subsystems m_subsystems;

		DASType::DbKey m_regionKey;

		mutable ACE_RW_Mutex m_mutex;
	};
}







#endif // !defined(EA_E15D9D76_DEFF_4e18_9AE2_C11C6F50B665__INCLUDED_)
