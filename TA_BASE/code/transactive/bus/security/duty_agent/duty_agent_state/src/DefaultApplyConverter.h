/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DefaultApplyConverter.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 * Converts DutyState to the format used for duty requests.
 */

#if !defined(DEFAULTAPPLYCONVERTER_H_INCLUDED)
#define DEFAULTAPPLYCONVERTER_H_INCLUDED

namespace TA_Base_Bus
{
	class DefaultApplyConverter : public IDutyStateConverter
	{
	public:

		DefaultApplyConverter( DASType::SubsystemMap& subsToApply );

		virtual void setSession(const std::string& session);
		virtual void setAction(const std::string& action);
	
		virtual void setRegionCount( unsigned long count );

		virtual void setRegion(const DASType::DbKey& region);
		
		virtual void setSubsystemCount( unsigned long count );

		virtual void setSubsystem(const DASType::DbKey& subsystem,
			const SubsystemState::State& addState, const SubsystemState::State& remState );
		
		virtual void setRegionAddCount   ( unsigned long count );
		virtual void setRegionRemoveCount( unsigned long count );

		virtual void regionAdd   ( const DutyPrimitive& session );
		virtual void regionRemove( const DutyPrimitive& session );
		
		virtual void setSubsystemAddCount    ( unsigned long count );
		virtual void setSubsystemRemoveCount ( unsigned long count );

		virtual void subsystemAdd    ( const DutyPrimitive& session );
		virtual void subsystemRemove ( const DutyPrimitive& session );
		virtual void subsystemDeny   ( const DutyPrimitive& session );
		virtual void subsystemTimeout( const DutyPrimitive& session );

		virtual void subsystemComplete();
		virtual void regionComplete();

	private:

		DASType::DbKey m_region;
		DASType::DbKey m_subsystem;

		unsigned long m_addCount;
		unsigned long m_remCount;

		DASType::SubsystemMap& m_subsToApply;
		DASType::SubsystemSet  m_emptySubSet;
		// add 2008-9-23, for Default Subsystem Apply, 
		// using in subsystemComplete(), subsystemAdd(), subsystemRemove()
		DutyPrimitive m_subAddSession;
		DutyPrimitive m_subRemoveSession;
	};
}

#endif // !defined(DEFAULTAPPLYCONVERTER_H_INCLUDED)
