/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_TIP/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/SubSnapshotQuery.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/10/31 12:12:40 $
  *  Last modified by:  $Author: grace.koh $
  *
  *  Gets state snapshot of subsystems.
  */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	SubSnapshotQuery::SubSnapshotQuery(const DASType::SubsystemSet& subsystems):
		m_subsystems(subsystems)
	{
		//m_subsystems.insert(DASConst::ALL_SUBSYSTEMS);
		m_regions.insert   (DASConst::ALL_REGIONS   );


		
		m_subsystemSeq.length( m_subsystems.size() );
		
		DASType::SubsystemSet::const_iterator it;
		CORBA::ULong i = 0;
		for( it = m_subsystems.begin(); it != m_subsystems.end(); it++, i++ )
		{
			m_subsystemSeq[i] = *it;	
		}
	}

	std::auto_ptr<IDutyQuery> SubSnapshotQuery::clone() const
	{
		std::auto_ptr<IDutyQuery> rval(new SubSnapshotQuery(*this));
		return(rval);
	}

	bool SubSnapshotQuery::isValidToCheckSubsystems()
	{
		return true;
	}
	void SubSnapshotQuery::execute(DutyState& state, DASType::DutyAgentPeer* peer )
	{
		DutyAgentTypeCorbaDef::DaDutySequence_var corbaState(NULL);
		CORBA_CALL( (*peer),
		            peerGetSubsystemState,
		            ( corbaState.out() ,m_subsystemSeq ) );
		state.set(corbaState.in());
	}


	void SubSnapshotQuery::execute(DutyState& state, const DASType::DbKey& region, const DASType::DutySet& dutySet)
	{
		DASType::DutySet::const_iterator dsIt;

        // TD12082: Make sure generic region information is included for regions that don't have
        //          any allocated duty.
        state.setRegion( region );

		for( dsIt = dutySet.begin(); dsIt != dutySet.end(); dsIt++ )
		{
			state.processRegionPolicyResult(region, *dsIt, IDutyPolicy::PR_ADD );
		}
	}

	void SubSnapshotQuery::execute(DutyState& state, const DASType::DbKey& subsystem,
		const DASType::DutySet& dutySet, const SubsystemState& subState)
	{
		DASType::DutySet::const_iterator dsIt;

        // TD12082: Make sure generic subsystem state information is included for subsystems
        //          that don't have any allocated duty.
        state.setSubsystem( subsystem, subState.getCurrentState(), subState.getCurrentState() );

		for( dsIt = dutySet.begin(); dsIt != dutySet.end(); dsIt++ )
		{
			state.processSubsystemPolicyResult(subsystem, subState.getCurrentState(),
				subState.getCurrentState(), *dsIt, IDutyPolicy::PR_ADD );
		}
	}

	const DASType::SubsystemSet& SubSnapshotQuery::getSubsystems() const
	{
		return( m_subsystems );
	}

	const DASType::RegionSet& SubSnapshotQuery::getRegions() const
	{
		return( m_regions );
	}
}




