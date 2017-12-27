/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyContextBuilderFactory.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Returns an IDutyContextBuilder depending on TransactiveAgentInfo::m_policyType
  */

#ifndef DUTYCONTEXTBUILDERFACTORY_H_INCLUDED
#define DUTYCONTEXTBUILDERFACTORY_H_INCLUDED

namespace TA_Base_Bus
{
	class DutyContextBuilderFactoryImpl
	{
	public:

		friend class ACE_Singleton<DutyContextBuilderFactoryImpl, ACE_Recursive_Thread_Mutex>;

		const IDutyContextBuilder& getDutyContextBuilder() const;

		class RailDutyContextBuilder : public IDutyContextBuilder
		{
		public:
			virtual void buildAllocRegionContext(DutyContext& context, const DutyPrimitive& session,
				const DASType::DbKey& regionKey ) const;

			virtual void buildAllocSubContext(DutyContext& context, const DutyPrimitive& session,
				const DASType::DbKey& subsystemKey, const SubsystemState::State& preState,
				const SubsystemState::State& postState) const;

			virtual void buildXferRegionContext(DutyContext& context, const DutyPrimitive& session ) const;

			virtual void buildXferSubViaRegionContext(DutyContext& context, const DutyPrimitive& session,
				const DASType::DbKey& subsystemKey, const SubsystemState::State& preState,
				const SubsystemState::State& postState) const;

			virtual void buildXferSubContext(DutyContext& context, const DutyPrimitive& session,
				const DASType::DbKey& subsystemKey, const SubsystemState::State& preState,
				const SubsystemState::State& postState) const;

			virtual void setSrcSession( DutyContext& context, const DutyPrimitive& session ) const;
			virtual void setNullSrcSession( DutyContext& context ) const;
		};
		
		// The road policy effectively removes profile groups from the equation
		class RoadDutyContextBuilder : public IDutyContextBuilder
		{
		public:
			virtual void buildAllocRegionContext(DutyContext& context, const DutyPrimitive& session,
				const DASType::DbKey& regionKey ) const;

			virtual void buildAllocSubContext(DutyContext& context, const DutyPrimitive& session,
				const DASType::DbKey& subsystemKey, const SubsystemState::State& preState,
				const SubsystemState::State& postState) const;

			virtual void buildXferRegionContext(DutyContext& context, const DutyPrimitive& session ) const;

			virtual void buildXferSubViaRegionContext(DutyContext& context, const DutyPrimitive& session,
				const DASType::DbKey& subsystemKey, const SubsystemState::State& preState,
				const SubsystemState::State& postState) const;

			virtual void buildXferSubContext(DutyContext& context, const DutyPrimitive& session,
				const DASType::DbKey& subsystemKey, const SubsystemState::State& preState,
				const SubsystemState::State& postState) const;

			virtual void setSrcSession( DutyContext& context, const DutyPrimitive& session ) const;
			virtual void setNullSrcSession( DutyContext& context ) const;
		};

	private:
		// Only we can construct and destruct ourselves
		DutyContextBuilderFactoryImpl();
		~DutyContextBuilderFactoryImpl();

		// Don't want to copy a Singleton
		DutyContextBuilderFactoryImpl( const DutyContextBuilderFactoryImpl& );
		DutyContextBuilderFactoryImpl& operator=( const DutyContextBuilderFactoryImpl& );

		std::vector<IDutyContextBuilder*> m_dutyContextBuilder;

	};

	typedef ACE_Singleton<DutyContextBuilderFactoryImpl, ACE_Recursive_Thread_Mutex> DutyContextBuilderFactory;
}

#endif //  DUTYCONTEXTBUILDERFACTORY_H_INCLUDED
