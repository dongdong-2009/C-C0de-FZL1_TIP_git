/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/TransferableRegionStateConverter.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Converts DutyState to a list of transferable regions suitable for sending to
 * the duty manager.
 */

#if !defined(EA_F398A8E5_AC50_4660_9CDC_C2591ADCD1BB__INCLUDED_)
#define EA_F398A8E5_AC50_4660_9CDC_C2591ADCD1BB__INCLUDED_

namespace TA_Base_App
{
	class TransferableRegionStateConverter : public TA_Base_Bus::IDutyStateConverter
	{

	public:

		TransferableRegionStateConverter();

		TA_Base_Bus::DutyAgentTypeCorbaDef::TransferableRegionSequence* getRegions();
		TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemTreeSequence* getSubsystems();

		virtual void setSession(const std::string& session);
		virtual void setAction(const std::string& action);
	
		virtual void setRegionCount( unsigned long count );

		virtual void setRegion(const TA_Base_Bus::DASType::DbKey& region);
		
		virtual void setSubsystemCount( unsigned long count );

		virtual void setSubsystem(const TA_Base_Bus::DASType::DbKey& subsystem,
			const TA_Base_Bus::SubsystemState::State& addState, const TA_Base_Bus::SubsystemState::State& remState );
		
		virtual void setRegionAddCount   ( unsigned long count );
		virtual void setRegionRemoveCount( unsigned long count );

		virtual void regionAdd(const TA_Base_Bus::DutyPrimitive& session);
		
		virtual void regionRemove(const TA_Base_Bus::DutyPrimitive& session);
		
		virtual void setSubsystemAddCount    ( unsigned long count );
		virtual void setSubsystemRemoveCount ( unsigned long count );

		virtual void subsystemAdd(const TA_Base_Bus::DutyPrimitive& session);
	
		virtual void subsystemRemove(const TA_Base_Bus::DutyPrimitive& session);
	
		virtual void subsystemDeny(const TA_Base_Bus::DutyPrimitive& session);
	
		virtual void subsystemTimeout(const TA_Base_Bus::DutyPrimitive& session);

		virtual void subsystemComplete();
		virtual void regionComplete();

	private:
		TA_Base_Bus::DutyAgentTypeCorbaDef::TransferableRegionSequence_var m_regionSequence;
		TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemTreeSequence_var m_subsystemSequence;

		CORBA::ULong   m_regIndex;
		CORBA::ULong   m_subIndex;
		CORBA::ULong   m_subRegIndex;

		TA_Base_Bus::DASType::DbKey m_region;

		unsigned long m_addCount;
		unsigned long m_remCount;

		bool m_alreadyAdd; // gongzong++ CL-21236
	};
}







#endif // !defined(EA_F398A8E5_AC50_4660_9CDC_C2591ADCD1BB__INCLUDED_)
