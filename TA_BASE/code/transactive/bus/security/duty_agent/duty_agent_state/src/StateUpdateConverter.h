/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/StateUpdateConverter.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 * Converts DutyState to the format used for duty requests.
 */

#if !defined(STATEUPDATECONVERTER_H_INCLUDED)
#define STATEUPDATECONVERTER_H_INCLUDED

namespace TA_Base_Bus
{
	class StateUpdateConverter : public IDutyStateConverter
	{
	public:

		StateUpdateConverter( std::map<DASType::DbKey, Region*>& regionMap );

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

		std::map<DASType::DbKey, Region*>& m_regionMap;
		std::map<DASType::DbKey, Region*>::iterator m_currentRegion;

		DASType::DbKey m_region;
		DASType::DbKey m_subsystem;

		DASType::DutySet m_regAdd;
		DASType::DutySet m_regRem;

		DASType::DutySet m_subAdd;
		DASType::DutySet m_subRem;

		SubsystemState::State m_subState;
	};
}

#endif // !defined(STATEUPDATECONVERTER_H_INCLUDED)
