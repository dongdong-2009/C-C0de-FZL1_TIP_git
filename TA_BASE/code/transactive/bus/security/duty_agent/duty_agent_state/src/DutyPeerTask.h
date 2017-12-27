/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyPeerTask.h $
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

#if !defined(EA_84297973_BD7A_49ba_9943_0DDC8FF61B6C__INCLUDED_)
#define EA_84297973_BD7A_49ba_9943_0DDC8FF61B6C__INCLUDED_

namespace TA_Base_Bus
{
	class DutyPeerTask : public ACE_Task<ACE_MT_SYNCH>
	{

	public:
		/**
		 * @param dutyPeer
		 * 
		 */
		DutyPeerTask(std::auto_ptr<IDutyPeer> dutyPeer,bool local, int threadCount );

		virtual ~DutyPeerTask();
		
		virtual int open(void*);
		/**
		 * call back when thread end
		 * @param flags
		 * 
		 */
		virtual int close(u_long flags = 0 );
		/**
		 * svc  This is the entry point for the callback from ACE_Task.
		 * 
		 */
		virtual int svc(void);

		/*
		*	shut down the ace task.
		*/
		virtual void 	terminate();
		void			terminateAndWait();
		void			setDutyPeer(std::auto_ptr<IDutyPeer> dutyPeer);

		/**
		 * @param state
		 * 
		 */
		ACE_Future<bool> sessionBegin( DutyState& state, const SessionInfo& session );
		/**
		 * @param state
		 * 
		 */
		ACE_Future<bool> sessionEnd( DutyState& state, const SessionInfo& session, const DASType::RegionSet& regions );
		/**
		 * @param state
		 * 
		 */
		ACE_Future<bool> sessionOverrideBegin( DutyState& state, const SessionInfo& session );
		/**
		 * @param state
		 * 
		 */
		ACE_Future<bool> sessionOverrideEnd( DutyState& state, const SessionInfo& session );
		/**
		 * @param state
		 * 
		 */
		ACE_Future<bool> transfer( DutyState& state, const TA_Base_Bus::SessionInfo& session, const DASType::RegionSet& regions );
		/**
		 * @param state
		 * 
		 */
		ACE_Future<bool> transfer( DutyState& state, const TA_Base_Bus::SessionInfo& session, const DASType::SubsystemMap& subsystems );
		/**
		 * @param state
		 * 
		 */
		ACE_Future<bool> degrade( DutyState& state, const DASType::SubsystemSet& subsystems );
		/**
		 * @param state
		 * 
		 */
		ACE_Future<bool> executeQuery(DutyState& state, IDutyQuery& query);
		
		void sessionBeginImpl        ( DutyState& state, const SessionInfo& session );
		void sessionEndImpl          ( DutyState& state, const SessionInfo& session, const DASType::RegionSet& regions );
		void sessionOverrideBeginImpl( DutyState& state, const SessionInfo& session );
		void sessionOverrideEndImpl  ( DutyState& state, const SessionInfo& session );
		void transferImpl            ( DutyState& state, const TA_Base_Bus::SessionInfo& session, const DASType::RegionSet& regions );
		void transferImpl            ( DutyState& state, const TA_Base_Bus::SessionInfo& session, const  DASType::SubsystemMap& subsystems );
		void degradeImpl             ( DutyState& state, const DASType::SubsystemSet& subsystems );
		void executeQueryImpl        ( DutyState& state, IDutyQuery& query );
	private:
		
		bool 			m_bLocalPeer; 
		int  			m_threadCount;
		volatile bool	m_isRunning;
		mutable ACE_RW_Mutex 		m_peerMutex;
		std::auto_ptr<IDutyPeer> 	m_dutyPeer;
		ACE_Activation_Queue 		m_activationQueue;

		static const unsigned long THREAD_COUNT;
	};
} // TA_Base_Bus







#endif // !defined(EA_84297973_BD7A_49ba_9943_0DDC8FF61B6C__INCLUDED_)
