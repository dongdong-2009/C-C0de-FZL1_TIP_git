/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/DegradedPopupConverter.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Implements DegradedPopupConverter.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/DegradedPopupConverter.h"

namespace TA_Base_App
{
	DegradedPopupConverter::DegradedPopupConverter() :
        m_action(""),
		m_region(TA_Base_Bus::DASConst::NO_REGION),
		m_subsystem(TA_Base_Bus::DASConst::NO_SUBSYSTEM)
	{
	}

	void DegradedPopupConverter::buildPopupData(PopupData& popupData) const
	{
		InternalPopupMap::const_iterator gainedIt;
		TA_Base_Bus::DASType::SubsystemMap::const_iterator regIt;
		TA_Base_Bus::DASType::SubsystemSet::const_iterator subIt;

		unsigned long regIdx = 0;
		unsigned long subIdx = 0;

		for( gainedIt=m_gainedMap.begin(); gainedIt!=m_gainedMap.end(); gainedIt++ )
		{
			TA_Base_Bus::DutyAgentTypeCorbaDef::DutyPopup newPopupData;
			
			newPopupData.targetSession = CORBA::string_dup( gainedIt->first.m_session.c_str() );

			newPopupData.lost.length(0);
			newPopupData.denied.length(0);

            // TD11301: Degraded pop ups do not mention who the duty was taken from, there should therefore
            //          only be one empty session in the duty tree sequence.
			newPopupData.gained.length( 1 );

			newPopupData.gained[0].session = CORBA::string_dup("");
			newPopupData.gained[0].profile = ULONG_MAX;

			newPopupData.gained[0].subsystems.length(gainedIt->second.size());

			for( regIt=gainedIt->second.begin(), regIdx=0; regIt!=gainedIt->second.end(); regIt++, regIdx++ )
			{
				newPopupData.gained[0].subsystems[regIdx].region = regIt->first;

				newPopupData.gained[0].subsystems[regIdx].subsystems.length(regIt->second.size());

				for( subIt=regIt->second.begin(),subIdx=0; subIt!=regIt->second.end(); subIt++,subIdx++ )
				{
					newPopupData.gained[0].subsystems[regIdx].subsystems[subIdx] = *subIt;
				}
			}

			popupData.push_back( newPopupData );
		}

        // TD10432: Send pop up messages for any lost duty that require notification.

        InternalPopupMap::const_iterator lostIt;
        for ( lostIt=m_lostMap.begin(); lostIt!=m_lostMap.end(); lostIt++ )
        {
            TA_Base_Bus::DutyAgentTypeCorbaDef::DutyPopup newPopupData;

            newPopupData.targetSession = CORBA::string_dup( lostIt->first.m_session.c_str() );

            // Degraded popups for lost duty do not contain gained or denied data.
            newPopupData.gained.length(0);
            newPopupData.denied.length(0);

            // Degraded popups for lost duty do not mention who the duty was lost to.
            newPopupData.lost.length(1);

            // Use an empty session and profile.
            newPopupData.lost[0].session = CORBA::string_dup("");
            newPopupData.lost[0].profile = ULONG_MAX;

            // Iterate through all regions and subsystems to populate the required popup data.
            newPopupData.lost[0].subsystems.length( lostIt->second.size() );
            for ( regIt=lostIt->second.begin(), regIdx=0; regIt!=lostIt->second.end(); regIt++, regIdx++ )
            {
                newPopupData.lost[0].subsystems[regIdx].region = regIt->first;

                newPopupData.lost[0].subsystems[regIdx].subsystems.length( regIt->second.size() );
                for ( subIt=regIt->second.begin(), subIdx=0; subIt!=regIt->second.end(); subIt++, subIdx++ )
                {
                    newPopupData.lost[0].subsystems[regIdx].subsystems[subIdx] = *subIt;
                }
            }

            popupData.push_back( newPopupData );
        }
	}
	
	void DegradedPopupConverter::setSession(const std::string& session)
	{
	}

	void DegradedPopupConverter::setAction(const std::string& action)
	{
		TA_ASSERT( (DutyAction::AUTOMATIC_DELEGATION == action) ||
			       (DutyAction::SYSTEMWIDE_DELEGATION  == action) ||
                   (DutyAction::SUBSYSTEM_DEGRADED == action), "Can only be used for degrading subsystems" );
        m_action = action;
	}

	void DegradedPopupConverter::setRegionCount(unsigned long count)
	{
	}

	void DegradedPopupConverter::setRegion(const TA_Base_Bus::DASType::DbKey& region)
	{
		m_region = region;
	}

	void DegradedPopupConverter::setSubsystemCount(unsigned long count)
	{
	}

	void DegradedPopupConverter::setSubsystem( const TA_Base_Bus::DASType::DbKey& subsystem,
		const TA_Base_Bus::SubsystemState::State& addState, const TA_Base_Bus::SubsystemState::State& remState )
	{
		m_subsystem  = subsystem;
	}


	void DegradedPopupConverter::setRegionAddCount( unsigned long count )
	{
	}

	void DegradedPopupConverter::setRegionRemoveCount( unsigned long count )
	{
	}

	void DegradedPopupConverter::regionAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void DegradedPopupConverter::regionRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void DegradedPopupConverter::setSubsystemAddCount( unsigned long count )
	{
	}

	void DegradedPopupConverter::setSubsystemRemoveCount( unsigned long count )
	{
	}

	void DegradedPopupConverter::subsystemAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
		std::pair<InternalPopupMap::iterator,bool>              sesInsertResult;
		std::pair<TA_Base_Bus::DASType::SubsystemMap::iterator,bool> regInsertResult;

		// insert or locate the session
		sesInsertResult = m_gainedMap.insert( InternalPopupMap::value_type(session, m_emptySubMap) );

		// insert or locate the region
		regInsertResult = sesInsertResult.first->second.insert(
			TA_Base_Bus::DASType::SubsystemMap::value_type(m_region, m_emptySubSet) );

		// insert the subsystem
		regInsertResult.first->second.insert( m_subsystem );
	}

	void DegradedPopupConverter::subsystemRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
        // TD10432: Send pop up messages for duty lost when the degradation was triggered by an operator (I.e.
        //          System Wide Delegation), or by an agent (I.e. Traincomms transfer).

        if ( ( DutyAction::SYSTEMWIDE_DELEGATION == m_action ) ||
			 ( DutyAction::SUBSYSTEM_DEGRADED == m_action )    ||
			 ( DutyAction::AUTOMATIC_DELEGATION == m_action ) )
        {
            std::pair<InternalPopupMap::iterator,bool> sesInsertResult;
            std::pair<TA_Base_Bus::DASType::SubsystemMap::iterator,bool> regInsertResult;

            // insert or locate the session
            sesInsertResult = m_lostMap.insert( InternalPopupMap::value_type(session,m_emptySubMap) );

            // insert or locate the region
            regInsertResult = sesInsertResult.first->second.insert(
                TA_Base_Bus::DASType::SubsystemMap::value_type(m_region,m_emptySubSet) );

            // insert the subsystem
            regInsertResult.first->second.insert( m_subsystem );
        }
	}

	void DegradedPopupConverter::subsystemDeny( const TA_Base_Bus::DutyPrimitive& session )
	{
	}

	void DegradedPopupConverter::subsystemTimeout(const TA_Base_Bus::DutyPrimitive& session)
	{
	}

	void DegradedPopupConverter::subsystemComplete()
	{
	}

	void DegradedPopupConverter::regionComplete()
	{
	}

}



