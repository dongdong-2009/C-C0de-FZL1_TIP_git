/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyPeerTaskManager.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 * Maintains a list of Duty Agent peers and acts as an intemdiary between the loca
 * Duty Agent front end and the peers.
 * 
 * Also maintains a list of outstanding requests. This is used to prevent multiple
 * requests being processed for the same subsystem. If this situation should occur
 * it will deny the request giving the reason as "outstanding requests".
 * 
 * This should probably know about profiles too so that it doesn't go off talking
 * to remote duty agents when it doesn't have to.
 */

#if !defined(EA_049A6FA0_EB7D_411e_A5FF_17BD344C6125__INCLUDED_)
#define EA_049A6FA0_EB7D_411e_A5FF_17BD344C6125__INCLUDED_

#define RPARAM_DUTY_THREAD_COUNT  "ThreadCount"

namespace TA_Base_Bus
{
	class DutyPeerTaskManager
	{

	public:
		
		DutyPeerTaskManager();
		~DutyPeerTaskManager();

		/**
		 * Allocate duty to the owner of the specified session.
		 * 
		 */
		void sessionBegin( DutyState& state, const SessionInfo& session,
			const DASType::LocationSet& locations );
		
		/**
		 * Free all duty associated with the specified session.
		 * 
		 */
		void sessionEnd  ( DutyState& state, const SessionInfo& session,
			const DASType::LocationSet& locations, const DASType::RegionSet& regions );

		/**
		 * Reallocate duty to the owner of the specified session.
		 * 
		 */
		void sessionOverrideBegin( DutyState& state, const SessionInfo& session,
			const DASType::LocationSet& locations );

		/**
		 * Remove any duty gained by the overriding profile for the specified session.
		 * 
		 */
		void sessionOverrideEnd( DutyState& state, const SessionInfo& session,
			const DASType::LocationSet& locations );

		/**
		 * Attempt to transfer the regions in the set to the owner of the specified session.
		 * 
		 */
		void transfer( DutyState& state, const TA_Base_Bus::SessionInfo& session, const DASType::RegionSet& regions );

		/**
		 * Attempt to transfer the subsystems in the map to the owner of the specified session.
		 * 
		 */
		void transfer( DutyState& state, const TA_Base_Bus::SessionInfo& session, const DASType::SubsystemMap& subsystems );

		/**
		 * Degrade the specified subsystems.
		 * 
		 */
		void degrade( DutyState& state, const DASType::SubsystemSet& subsystems,
			const DASType::LocationSet& locations );

		/**
		 * Get a list of regions that could be transfered to the owner of the specified session.
		 * @param state Holds the results of the query.
		 * 
		 */
		void getTransferableRegions( DutyState& state, const TA_Base_Bus::SessionInfo& session,
			const DASType::LocationSet& locations, bool checkSubs );
		/**
		 * Get a list of subsystems that could be transfered to the owner of the specified session.
		 * @param state Holds the results of the query.
		 * 
		 */
		void getTransferableSubsystems( DutyState& state, const TA_Base_Bus::SessionInfo& session,
			const DASType::RegionSet& regions );

		/**
		 * Create a snapshot of the the current duty state.
		 * @param state Holds the results of the query.
		 * 
		 */
		void getState( DutyState& state, const DASType::LocationSet& locations );

			/**
		 * Create a snapshot of the the current duty state.
		 * @param state Holds the results of the query.
		 * 
		 */
		void getSessionState( DutyState& state, const std::string& session, const DASType::LocationSet& locations );



		/**
		 * Create a snapshot of the the current duty state of subsystems.
		 * @param state Holds the results of the query.
		 */
		void getSubsystemState( DutyState& state,const DASType::SubsystemSet& subsystems, const DASType::LocationSet& locations );

		
		/**
		 * Get any exclusive duty an operator has for any subsystem anywhere on the system.
		 * @param state Holds the results of the query.
		 * @param session The Operator's session id.
		 * 
		 */
		void getExclusiveDuty( DutyState& state, const TA_Base_Bus::SessionInfo& session, 
			const DASType::LocationSet& locations );
		/**
		 * Get the duty (if any) an operator has for a particular subsystem within a
		 * particular region.
		 * @param state Holds the results of the query.
		 * @param session The Operator's session id.
		 * @param region The Region being queried.
		 * @param subsysm The Subsystem being queried.
		 * 
		 */
		void getDuty( DutyState& state, const TA_Base_Bus::SessionInfo& session, const DASType::DbKey& region,
			const DASType::DbKey& subsystem );

		/*
		* update the specific location's dutyPeerTask' dutyPeer.
		*
		*
		*/
		void updateDutyPeer(DASType::DbKey locationKey, std::auto_ptr<IDutyPeer> dutyPeer);

	private:
		DutyPeerTaskManager( const DutyPeerTaskManager& );
		DutyPeerTaskManager& operator=( const DutyPeerTaskManager& );
		typedef std::map<DASType::DbKey, IDutyPeerTaskCommand*> CommandMap;
		
		void	showLocationString(const DASType::LocationSet& locations);
		void	showLocationString(CommandMap& commandMap);
		// DutyPeerTask Commands
		class SesInfoCmd : public IDutyPeerTaskCommand
		{
		public:
			SesInfoCmd( ACE_Future<bool> (DutyPeerTask::*cmd)(DutyState&, const SessionInfo&),
				void (DutyPeerTask::*cmdImpl)(DutyState&, const SessionInfo&),
				const SessionInfo& session );

			virtual std::auto_ptr<IDutyPeerTaskCommand> clone() const;

			virtual ACE_Future<bool> execute(DutyPeerTask& dpt, DutyState& state);
			virtual void directExecute(DutyPeerTask& dpt, DutyState& state);

		private:
			ACE_Future<bool> (DutyPeerTask::*m_cmd)(DutyState&, const SessionInfo&);
			void (DutyPeerTask::*m_cmdImpl)(DutyState&, const SessionInfo&);
			const SessionInfo& m_session;
		};

		class SesEndCmd : public IDutyPeerTaskCommand
		{
		public:
			SesEndCmd( const SessionInfo& session, const DASType::RegionSet& regions );

			virtual std::auto_ptr<IDutyPeerTaskCommand> clone() const;

			virtual ACE_Future<bool> execute(DutyPeerTask& dpt, DutyState& state);
			virtual void directExecute(DutyPeerTask& peerTask, DutyState& state);

		private:
			const SessionInfo& m_session;
			const DASType::RegionSet& m_regions;
		};

		class DegradeCmd : public IDutyPeerTaskCommand
		{
		public:
			DegradeCmd( const DASType::SubsystemSet& subsystems );

			virtual std::auto_ptr<IDutyPeerTaskCommand> clone() const;

			virtual ACE_Future<bool> execute(DutyPeerTask& dpt, DutyState& state);
			virtual void directExecute(DutyPeerTask& peerTask, DutyState& state);

		private:
			const DASType::SubsystemSet& m_subsystems;
		};

		class XferSubCmd : public IDutyPeerTaskCommand
		{
		public:
			XferSubCmd( const TA_Base_Bus::SessionInfo& session, const DASType::SubsystemMap& subsystems );

			virtual std::auto_ptr<IDutyPeerTaskCommand> clone() const;

			virtual ACE_Future<bool> execute(DutyPeerTask& dpt, DutyState& state);
			virtual void directExecute(DutyPeerTask& peerTask, DutyState& state);

		private:
			const TA_Base_Bus::SessionInfo& m_session;
			const DASType::SubsystemMap& m_subsystems;
		};

		class XferRegCmd : public IDutyPeerTaskCommand
		{
		public:
			XferRegCmd( const TA_Base_Bus::SessionInfo& session, const DASType::RegionSet& regions );

			virtual std::auto_ptr<IDutyPeerTaskCommand> clone() const;

			virtual ACE_Future<bool> execute(DutyPeerTask& dpt, DutyState& state);
			virtual void directExecute(DutyPeerTask& dpt, DutyState& state);
		private:
			const TA_Base_Bus::SessionInfo& m_session;
			const DASType::RegionSet& m_regions;
		};

		class QueryCmd : public IDutyPeerTaskCommand
		{
		public:
			QueryCmd( std::auto_ptr<IDutyQuery> query );

			virtual std::auto_ptr<IDutyPeerTaskCommand> clone() const;

			virtual ACE_Future<bool> execute(DutyPeerTask& dpt, DutyState& state);
			virtual void directExecute(DutyPeerTask& peerTask, DutyState& state);

		private:

			std::auto_ptr<IDutyQuery> m_query;
		};

		/**
		 * Execute a different command for each specified location.
		 */
		void DutyPeerTaskManager::peerExecute( DutyState& state, CommandMap& commandMap );

		/**
		 * Execute the same command each specified location.
		 */
		void peerExecute( DutyState& state, IDutyPeerTaskCommand& command, const DASType::LocationSet& locations );

		void restrictLocations( const SessionInfo& session,
			const DASType::LocationSet& locationConstraints, DASType::LocationSet& restrictedLocations );

		void restrictLocations( const std::string& session,
			const DASType::LocationSet& locationConstraints, DASType::LocationSet& restrictedLocations );

		void restrictLocations( const std::vector<DASType::DbKey>& profiles,
			const DASType::DbKey& sessionLocation, const DASType::LocationSet& locationConstraints,
			DASType::LocationSet& restrictedLocations );

		typedef std::map<DASType::DbKey, DutyPeerTask*> TaskMap;
		TaskMap m_taskMap;
		DASType::ReverseRegionMap m_regionMap;
	};
}







#endif // !defined(EA_049A6FA0_EB7D_411e_A5FF_17BD344C6125__INCLUDED_)
