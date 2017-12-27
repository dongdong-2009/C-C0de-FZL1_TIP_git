/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/SnapshotQuery.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Gets a state snapshot.
  */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	SnapshotQuery::SnapshotQuery()
	{
		m_subsystems.insert(DASConst::ALL_SUBSYSTEMS);
		m_regions.insert   (DASConst::ALL_REGIONS   );
	}

	std::auto_ptr<IDutyQuery> SnapshotQuery::clone() const
	{
		std::auto_ptr<IDutyQuery> rval(new SnapshotQuery(*this));
		return(rval);
	}

	void SnapshotQuery::execute(DutyState& state, DASType::DutyAgentPeer* peer )
	{
		DutyAgentTypeCorbaDef::DaDutySequence_var corbaState(NULL);
		CORBA_CALL( (*peer),
		            peerGetState,
		            ( corbaState.out() ) );
		state.set(corbaState.in());
	}


	void SnapshotQuery::execute(DutyState& state, const DASType::DbKey& region, const DASType::DutySet& dutySet)
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
	bool SnapshotQuery::isValidToCheckSubsystems()
	{
		return true;
	}
	void SnapshotQuery::execute(DutyState& state, const DASType::DbKey& subsystem,
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

	const DASType::SubsystemSet& SnapshotQuery::getSubsystems() const
	{
		return( m_subsystems );
	}

	const DASType::RegionSet& SnapshotQuery::getRegions() const
	{
		return( m_regions );
	}
}




