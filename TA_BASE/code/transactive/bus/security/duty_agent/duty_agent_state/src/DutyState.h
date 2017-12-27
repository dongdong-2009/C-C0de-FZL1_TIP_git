/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyState.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Keeps a record of state changes. DutyState is passed into functions that must
  *  change or query state. DutyState is then published and picked up by interested
  *  observers.
  *
  */

#if !defined(EA_8F2417C9_9F11_4a85_8054_3BF2BDE3130A__INCLUDED_)
#define EA_8F2417C9_9F11_4a85_8054_3BF2BDE3130A__INCLUDED_

#include <iosfwd>
namespace TA_Base_Bus
{
    // DutyState objects seem to be used to record changes in region and subsystem 
    // duty as Duty Manager algorithms process requests for change of duty.
    // DutyState::convert() allows IDutyStateConverter implementations to gain access 
    // to the information in the DutyState object.  convert() will call back 
    // converter methods as it traverses the internal structure of the DutyState
    // object, allowing the converter to capture the state it needs.
    // TODO LPP: Does this approach actually make it easier for clients?
    //
    // An overview of the internal structure of DutyState is helpful in order to 
    // understand the information that is available to an IDutyStateConverter
    // (keeping in mind that the internal state of DutyState is a private implementation 
    // detail and may change):
    //
    // DutyMap map {Region DbKey -> Duty}
    //
    // Duty struct 
    //      RegDuty 
    //      SubDutyMap
    //
    // RegDuty struct 
    //      DASType::DutySet m_add              - profiles that have gained duty for a region (location)
    //      DASType::DutySet m_remove           - profiles that have lost duty for a region (location)
    //
    // SubDutyMap map {Subsystem DbKey -> SubDuty}
    // 
    // SubDuty struct 
    //      SubsystemState::State m_addState     
    //      SubsystemState::State m_remState    
    //      DASType::DutySet m_add              - profiles that have gained duty for a subsystem
    //      DASType::DutySet m_remove           - profiles that have lost duty for a subysystem
    //      DutyPrimitive m_deniedBy            - TODO LPP: Why can a request for subsystem duty only be denied by one operator?
    //      DutyPrimitive m_timedOut 
    //
    // DASType::DutySet set {DutyPrimitive}
    //
    // DutyPrimitive struct 
    //      std::string m_session
    //      DbKey m_profile                     TODO LPP: Is this profile key be expected to match the session's profile key? 
    //
    // SubsystemState::State enum {SS_NORMAL, SS_DELEGATED, SS_DEGRADED};

	class DutyState
	{
	public:
		
		DutyState();
		DutyState( const DutyState& rhs );
		DutyState( const std::string& initiatingSession, const std::string& initiatingAction );
		DutyState( const std::string& accessControlSession, const std::string& initiatingSession,
			const std::string& initiatingAction );

		// TODO: Note that making the initiating session publicly avaialable obviates the need
		// to pass the session down through the Duty Agent separately. The duplicate session
		// information is contained in a reasonably large number of Duty Agent functions
		// which are currently known to work. This is a large disincentive.
		std::string getInitiatingSession() const { return(m_initiatingSession); }
		std::string getAccessControlSession() const { return(m_accessControlSession); }
		std::string getInitiatingAction() const { return(m_initiatingAction); }

		void set( const DutyAgentTypeCorbaDef::DaDutySequence& state );

		bool isEmpty() const { return(m_dutyMap.size() == 0); }

		void insert( DutyState& state );

		void convert( IDutyStateConverter& converter ) const;

		void setRegion( const DASType::DbKey& region );

        /**
         * setSubsystem
         *
         * Sets ensures that the specified subsystem is included in the state information, making use
         * of the provided subsystem states.
         *
         * @param subsystemKey The key of the subsystem.
         * @param currState The current state of the subsystem.
         * @param nextState The next state of the subsystem.
         */
        void setSubsystem( const DASType::DbKey& subsystemKey, const SubsystemState::State& currState,
            const SubsystemState::State& nextState );

		void processRegionPolicyResult( const DASType::DbKey& region, const DutyPrimitive& dstSession,
			const DutyPrimitive& srcSession, const IDutyPolicy::POLICY_RESULT& result );

		void processRegionPolicyResult( const DASType::DbKey& region, const DutyPrimitive& dstSession,
			const IDutyPolicy::POLICY_RESULT& result );

		void processSubsystemPolicyResult( const DASType::DbKey& subsystem, const SubsystemState::State& currState,
			const SubsystemState::State& nextState, const DutyPrimitive& dstSession, const DutyPrimitive& srcSession,
			const IDutyPolicy::POLICY_RESULT& result );

		void processSubsystemPolicyResult( const DASType::DbKey& subsystem, const SubsystemState::State& currState,
			const SubsystemState::State& nextState, const DutyPrimitive& dstSession,
			const IDutyPolicy::POLICY_RESULT& result );
	
		void processDeny   ( const DASType::DbKey& region, const DASType::DbKey& subsystem );
		void processTimeout( const DASType::DbKey& region, const DASType::DbKey& subsystem );

		// CL19422++
		/**
		 * addition method for check DutyState info
		 *
		 */
		// print m_dutyMap
		void print(std::ostream& strOut) const;
//		TA_Base_Bus::DASType::SubsystemSet getStateChangedSubsystem(DASType::DbKey regionKey = 0);
		// ++CL19422

		// yanrong++: CL-19418 -- print duty map with Subsystem name and profile name
		void printEx(std::ostream& strOut) const;
		// ++yanrong

	private:

		// regions
		void add( const DASType::DbKey& region, const DutyPrimitive& dstSession );
		void replace( const DASType::DbKey& region, const DutyPrimitive& dstSession,
			const DutyPrimitive& srcSession );
		void remove( const DASType::DbKey& region   , const DutyPrimitive& session );

		// subsystems
		void add( const DASType::DbKey& subsystem, const SubsystemState::State& currState,
			const SubsystemState::State& nextState, const DutyPrimitive& dstSession );
		void replace( const DASType::DbKey& subsystem, const SubsystemState::State& currState,
			const SubsystemState::State& nextState, const DutyPrimitive& dstSession, const DutyPrimitive& srcSession );
		void remove( const DASType::DbKey& subsystem, const SubsystemState::State& currState,
			const SubsystemState::State& nextState, const DutyPrimitive& session );

		void insertCurrentRegion();
		void convertSubState( unsigned long srcState, SubsystemState::State& dstState );
		
		const std::string m_initiatingAction;
		const std::string m_initiatingSession;
		const std::string m_accessControlSession;

    public:
		struct SubDuty
		{
			SubsystemState::State m_addState;
			SubsystemState::State m_remState;
			DASType::DutySet      m_add;
			DASType::DutySet      m_remove;
			DutyPrimitive         m_deniedBy;
			DutyPrimitive         m_timedOut;
		};

		struct RegDuty
		{
			DASType::DutySet m_add;
			DASType::DutySet m_remove;
		};

		typedef std::map<DASType::DbKey, SubDuty> SubDutyMap;

		struct Duty
		{
			RegDuty     m_region;
			SubDutyMap  m_subsystem;
		};
		typedef std::map<DASType::DbKey, Duty> DutyMap;

    private:
		// THE map
		DutyMap m_dutyMap;

		const Duty          m_copyMeDuty;
		SubDuty             m_copyMeSubDuty;
		const DutyPrimitive m_emptySession;

		std::pair<DutyMap::iterator,bool> m_currentRegion;
		DASType::DbKey                    m_region;
		bool                              m_regionInserted;

    public:
        DutyMap getDutyMap() const {return(m_dutyMap); }

	}; // DutyState

} // TA_Base_Bus

#endif // !defined(EA_8F2417C9_9F11_4a85_8054_3BF2BDE3130A__INCLUDED_)
