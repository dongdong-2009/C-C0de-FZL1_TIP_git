/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/SubsystemStateSnapshotConverter.cpp $
 * @author:   Bradley Cleaver
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Converts DutyState to a sequence of subsystem states.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/SubsystemStateSnapshotConverter.h"

namespace TA_Base_App
{
    SubsystemStateSnapshotConverter::SubsystemStateSnapshotConverter() :
        m_regionKey(0)
    {
        m_subsystemsOfInterest.length(0);
        m_subsystemStates.clear();
    }


    SubsystemStateSnapshotConverter::SubsystemStateSnapshotConverter( const TA_Base_Bus::DutyAgentTypeCorbaDef::KeySequence& subsystems ) :
        m_regionKey(0),
        m_subsystemsOfInterest(subsystems)
    {
        m_subsystemStates.clear();
    }


    SubsystemStateSnapshotConverter::~SubsystemStateSnapshotConverter()
    {
    }


    void SubsystemStateSnapshotConverter::setSession( const std::string& session )
    {
    }


    void SubsystemStateSnapshotConverter::setAction( const std::string& action )
    {
    }


    void SubsystemStateSnapshotConverter::setRegionCount( unsigned long count )
    {
    }


    void SubsystemStateSnapshotConverter::setRegion( const TA_Base_Bus::DASType::DbKey& region )
    {
        m_regionKey = region;
    }


    void SubsystemStateSnapshotConverter::regionComplete()
    {
    }


    void SubsystemStateSnapshotConverter::setSubsystemCount(unsigned long count)
    {
    }


    void SubsystemStateSnapshotConverter::setSubsystem( const TA_Base_Bus::DASType::DbKey& subsystem,
        const TA_Base_Bus::SubsystemState::State& addState, const TA_Base_Bus::SubsystemState::State& remState )
    {
        // Only add the subsystem if its one that we're interested in.
        if ( isSubsystemOfInterest( subsystem ) )
        {
            // Store the subsystem state information.
            TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemState subsystemState;
            subsystemState.regionKey = m_regionKey;
            subsystemState.subsystemKey = subsystem;
            subsystemState.currState = addState;
            subsystemState.prevState = remState;

            // Add it to the list.
            m_subsystemStates.push_back( subsystemState );
        }
    }


    void SubsystemStateSnapshotConverter::subsystemComplete()
    {
    }


    void SubsystemStateSnapshotConverter::setRegionAddCount( unsigned long count )
    {
    }


    void SubsystemStateSnapshotConverter::setRegionRemoveCount( unsigned long count )
    {
    }


    void SubsystemStateSnapshotConverter::regionAdd( const TA_Base_Bus::DutyPrimitive& session )
    {
    }


    void SubsystemStateSnapshotConverter::regionRemove( const TA_Base_Bus::DutyPrimitive& session )
    {
    }


    void SubsystemStateSnapshotConverter::setSubsystemAddCount( unsigned long count )
    {
    }


    void SubsystemStateSnapshotConverter::setSubsystemRemoveCount( unsigned long count )
    {
    }


    void SubsystemStateSnapshotConverter::subsystemAdd( const TA_Base_Bus::DutyPrimitive& session )
    {
    }


    void SubsystemStateSnapshotConverter::subsystemRemove(const TA_Base_Bus::DutyPrimitive& session)
    {
    }


    void SubsystemStateSnapshotConverter::subsystemDeny(const TA_Base_Bus::DutyPrimitive& session)
    {
    }


    void SubsystemStateSnapshotConverter::subsystemTimeout(const TA_Base_Bus::DutyPrimitive& session)
    {
    }


    TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence* SubsystemStateSnapshotConverter::getSubsystemStates()
    {
        TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence_var subsystemStates = new TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence();

        // Convert our list into a sequence.
        subsystemStates->length( m_subsystemStates.size() );

        unsigned long i;
        std::list<TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemState>::iterator it;
        for ( it=m_subsystemStates.begin(), i=0; it!=m_subsystemStates.end(); it++, i++ )
        {
            subsystemStates[i] = *it;
        }

        return subsystemStates._retn();
    }


    bool SubsystemStateSnapshotConverter::isSubsystemOfInterest( const TA_Base_Bus::DASType::DbKey& subsystem )
    {
        // If the list is empty then we're interested in all.
        if ( 0 == m_subsystemsOfInterest.length() )
        {
            return true;
        }

        // If the specified subsystem is in the list then it is of interest.
        for ( unsigned long i=0; i<m_subsystemsOfInterest.length(); i++ )
        {
            if ( subsystem == m_subsystemsOfInterest[i] )
            {
                return true;
            }
        }

        // Couldn't find it so not interested.
        return false;
    }
}
