/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/CorbaDutyStateConverter.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Converts DutyState to a CORBA version of DutyState.
 */

#if !defined(CORBADUTYSTATECONVERTER_H_INCLUDED)
#define CORBADUTYSTATECONVERTER_H_INCLUDED

namespace TA_Base_App
{
	class CorbaDutyStateConverter : public TA_Base_Bus::IDutyStateConverter
	{

	public:

		CorbaDutyStateConverter();
		
		TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence* getCorbaDutyState();

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
		
		TA_Base_Bus::DutyAgentTypeCorbaDef::DaDutySequence_var m_corbaDutyState;

		CORBA::ULong m_regIdx;
		CORBA::ULong m_subIdx;

		CORBA::ULong m_regAddIdx;
		CORBA::ULong m_regRemIdx;

		CORBA::ULong m_subAddIdx;
		CORBA::ULong m_subRemIdx;

		bool m_firstRegion;
		bool m_firstSubsystem;
	};
}

#endif // !defined(CORBADUTYSTATECONVERTER_H_INCLUDED)
