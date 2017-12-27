/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/GetDutyConverter.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Converts DutyState to a sequence of Duty Manager updates..
 */

#if !defined(GETDUTYCONVERTER_H_INCLUDED)
#define GETDUTYCONVERTER_H_INCLUDED

namespace TA_Base_App
{
	class GetDutyConverter : public TA_Base_Bus::IDutyStateConverter
	{

	public:

		GetDutyConverter(const char* session, TA_Base_Bus::DutyAgentTypeCorbaDef::Key region,
			TA_Base_Bus::DutyAgentTypeCorbaDef::Key subsystem, TA_Base_Bus::DutyAgentTypeCorbaDef::Key profile,
			TA_Base_Bus::DutyAgentTypeCorbaDef::Key& actionGroup );

		bool getResult() const;
		
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

		const std::string m_session;
		const TA_Base_Bus::DutyAgentTypeCorbaDef::Key m_region;
		const TA_Base_Bus::DutyAgentTypeCorbaDef::Key m_subsystem;
		const TA_Base_Bus::DutyAgentTypeCorbaDef::Key m_profile;

		TA_Base_Bus::DutyAgentTypeCorbaDef::Key& m_actionGroup;

		TA_Base_Bus::SubsystemState::State m_subState;
		
		bool m_foundRegion;
		bool m_foundSubsystem;
		bool m_hasDuty;
	};
}

#endif // !defined(GETDUTYCONVERTER_H_INCLUDED)
