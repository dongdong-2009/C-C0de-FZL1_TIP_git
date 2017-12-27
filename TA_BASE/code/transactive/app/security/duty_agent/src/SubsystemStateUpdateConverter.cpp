/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/SubsystemStateUpdateConverter.cpp $
 * @author:   Derrick Liew
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Implements SubsystemStateUpdateConverter.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/SubsystemStateUpdateConverter.h"

namespace TA_Base_App
{
    SubsystemStateUpdateConverter::SubsystemStateUpdateConverter() :
        m_regionKey(0),
		m_bSubSystemSet(false)
    {
        m_subsystemStates.clear();
    }


    SubsystemStateUpdateConverter::~SubsystemStateUpdateConverter()
    {
        m_subsystemStates.clear();
    }


    void SubsystemStateUpdateConverter::setSession( const std::string& session )
    {
    }


    void SubsystemStateUpdateConverter::setAction( const std::string& action )
    {
    }


    void SubsystemStateUpdateConverter::setRegionCount( unsigned long count )
    {
    }


    void SubsystemStateUpdateConverter::setRegion( const TA_Base_Bus::DASType::DbKey& region )
    {
        // Store the region key and reset the list of subsystems with changes in state.
        m_regionKey = region;
        m_subsystemStates.clear();
    }


    void SubsystemStateUpdateConverter::regionComplete()
    {
    }


    void SubsystemStateUpdateConverter::setSubsystemCount(unsigned long count)
    {
    }


    void SubsystemStateUpdateConverter::setSubsystem( const TA_Base_Bus::DASType::DbKey& subsystem,
        const TA_Base_Bus::SubsystemState::State& addState, const TA_Base_Bus::SubsystemState::State& remState )
    {
        // TD12082: We're only interested in subsystems whose state has changed.
        if ( addState != remState )
        {
            // Add the state information to the list.
            TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemState subsystemState;
            subsystemState.regionKey = m_regionKey;
            subsystemState.subsystemKey = subsystem;
            subsystemState.currState = addState;
            subsystemState.prevState = remState;

			//TD15417++
			m_setSubsystemState = subsystemState;
			m_bSubSystemSet = true;
			//++TD15417
        }
    }


    void SubsystemStateUpdateConverter::subsystemComplete()
    {
    }


    void SubsystemStateUpdateConverter::setRegionAddCount( unsigned long count )
    {
    }


    void SubsystemStateUpdateConverter::setRegionRemoveCount( unsigned long count )
    {
    }


    void SubsystemStateUpdateConverter::regionAdd( const TA_Base_Bus::DutyPrimitive& session )
    {
    }


    void SubsystemStateUpdateConverter::regionRemove( const TA_Base_Bus::DutyPrimitive& session )
    {
    }


    void SubsystemStateUpdateConverter::setSubsystemAddCount( unsigned long count )
    {
    }


    void SubsystemStateUpdateConverter::setSubsystemRemoveCount( unsigned long count )
    {
    }


    void SubsystemStateUpdateConverter::subsystemAdd( const TA_Base_Bus::DutyPrimitive& session )
    {

		//TD15417++
		if(m_bSubSystemSet == true)
		{
			m_subsystemStates.push_back( m_setSubsystemState );
			m_bSubSystemSet = false;
		}
		//++TD15417

    }


    void SubsystemStateUpdateConverter::subsystemRemove(const TA_Base_Bus::DutyPrimitive& session)
    {

		//TD15417++
		if(m_bSubSystemSet == true)
		{
			m_subsystemStates.push_back( m_setSubsystemState );
			m_bSubSystemSet = false;
		}
		//++TD15417

    }


    void SubsystemStateUpdateConverter::subsystemDeny(const TA_Base_Bus::DutyPrimitive& session)
    {
		//TD15417++
		if(m_bSubSystemSet == true)
		{
			m_setSubsystemState.currState = m_setSubsystemState.prevState;
			m_subsystemStates.push_back( m_setSubsystemState );
			m_bSubSystemSet = false;
		}
		//++TD15417

    }


    void SubsystemStateUpdateConverter::subsystemTimeout(const TA_Base_Bus::DutyPrimitive& session)
    {

		//TD15417++
		if(m_bSubSystemSet == true)
		{
			m_subsystemStates.push_back( m_setSubsystemState );
			m_bSubSystemSet = false;
		}
		//++TD15417

    }


    TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence* SubsystemStateUpdateConverter::getSubsystemStates()
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
}
