/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/DutyManagerUpdateConverter.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Converts DutyState to a sequence of Duty Manager updates..
 */

#if !defined(DUTYMANAGERUPDATECONVERTER_H_INCLUDED)
#define DUTYMANAGERUPDATECONVERTER_H_INCLUDED

namespace TA_Base_App
{
	class DutyManagerUpdateConverter : public TA_Base_Bus::IDutyStateConverter
	{

	public:

		DutyManagerUpdateConverter();

		const TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDutySequence& getRegionsToAdd() const;
		const TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDutySequence& getRegionsToRemove() const;

		const TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence& getSubsystemsToAdd() const;
		const TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence& getSubsystemsToRemove() const;
		
		virtual void setSession(const std::string& session);
		virtual void setAction(const std::string& action);
	
		virtual void setRegionCount( unsigned long count );

		virtual void setRegion(const TA_Base_Bus::DASType::DbKey& region);
		
		virtual void setSubsystemCount( unsigned long count );

		virtual void setSubsystem(const TA_Base_Bus::DASType::DbKey& subsystem,
			const TA_Base_Bus::SubsystemState::State& addState, const TA_Base_Bus::SubsystemState::State& remState );
		
		virtual void setRegionAddCount   ( unsigned long count );
		virtual void setRegionRemoveCount( unsigned long count );

		virtual void regionAdd   ( const TA_Base_Bus::DutyPrimitive& session );
		virtual void regionRemove( const TA_Base_Bus::DutyPrimitive& session );
		
		virtual void setSubsystemAddCount    ( unsigned long count );
		virtual void setSubsystemRemoveCount ( unsigned long count );

		virtual void subsystemAdd    ( const TA_Base_Bus::DutyPrimitive& session );
		virtual void subsystemRemove ( const TA_Base_Bus::DutyPrimitive& session );
		virtual void subsystemDeny   ( const TA_Base_Bus::DutyPrimitive& session );
		virtual void subsystemTimeout( const TA_Base_Bus::DutyPrimitive& session );

		virtual void subsystemComplete();
		virtual void regionComplete();

	private:

		void updateRegionSeq( const TA_Base_Bus::DutyPrimitive& session,
			TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDutySequence& regionSeq, CORBA::ULong& index );

		void updateSubsystemSeq( const TA_Base_Bus::DutyPrimitive& session, const TA_Base_Bus::SubsystemState::State& subState,
			TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence& subsystemSeq, CORBA::ULong& index );

		TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDutySequence m_regionsToAdd;
		TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDutySequence m_regionsToRemove;

		TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence m_subsystemsToAdd;
		TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence m_subsystemsToRemove;

		CORBA::ULong m_rtaIndex;
		CORBA::ULong m_rtrIndex;
		CORBA::ULong m_staIndex;
		CORBA::ULong m_strIndex;

		TA_Base_Bus::DASType::DbKey m_region;
		TA_Base_Bus::DASType::DbKey m_subsystem;
		TA_Base_Bus::SubsystemState::State m_addSubState;
		TA_Base_Bus::SubsystemState::State m_remSubState;
	};
}

#endif // !defined(DUTYMANAGERUPDATECONVERTER_H_INCLUDED)
