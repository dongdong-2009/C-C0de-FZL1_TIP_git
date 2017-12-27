/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/IDutyContextBuilder.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  DutyContextBuilder Interface
  */

#ifndef IDUTYCONTEXTBUILDER_H_INCLUDED
#define IDUTYCONTEXTBUILDER_H_INCLUDED

namespace TA_Base_Bus
{
	class IDutyContextBuilder
	{
	public:
		virtual ~IDutyContextBuilder(){}
		// Build the initial context
		virtual void buildAllocRegionContext(DutyContext& context, const DutyPrimitive& session,
			const DASType::DbKey& regionKey ) const = 0;

		virtual void buildAllocSubContext(DutyContext& context, const DutyPrimitive& session,
			const DASType::DbKey& subsystemKey, const SubsystemState::State& preState,
			const SubsystemState::State& postState) const = 0;

		virtual void buildXferRegionContext(DutyContext& context, const DutyPrimitive& session ) const = 0;

		virtual void buildXferSubViaRegionContext(DutyContext& context, const DutyPrimitive& session,
			const DASType::DbKey& subsystemKey, const SubsystemState::State& preState,
			const SubsystemState::State& postState) const = 0;

		virtual void buildXferSubContext(DutyContext& context, const DutyPrimitive& session,
			const DASType::DbKey& subsystemKey, const SubsystemState::State& preState,
			const SubsystemState::State& postState) const = 0;

		// Becuase there may be multiple source sessions, the attributes associated with these
		// are set separately.
		virtual void setSrcSession( DutyContext& context, const DutyPrimitive& session ) const = 0;

		virtual void setNullSrcSession( DutyContext& context ) const = 0;

	protected:

		void setRegionEquality(DutyContext& context, const DutyPrimitive& session,
			const DASType::DbKey& regionKey ) const;

		void setSubContext(DutyContext& context, const DutyPrimitive& session,
			const DASType::DbKey& subsystemKey, const SubsystemState::State& preState,
			const SubsystemState::State& postState ) const;

		void setProfileGroupEquality( DutyContext& context, const DutyPrimitive& session ) const;

		void setSrc( DutyContext& context, const DutyPrimitive& session ) const;

		void setNullSrc( DutyContext& context ) const;

	};
}

#endif //  IDUTYCONTEXTBUILDER_H_INCLUDED
