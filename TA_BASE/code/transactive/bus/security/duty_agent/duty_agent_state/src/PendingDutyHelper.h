/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/PendingDutyHelper.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  * Regions and Subsystems both support the concept of two-stage duty change where a duty change is
  * validated by the Duty Agent's policies, verified externally then committed. This class encapsulates
  * the logic required.
  */

#if !defined(PENDINGDUTYHELPER_H_INCLUDED)
#define PENDINGDUTYHELPER_H_INCLUDED

namespace TA_Base_Bus
{
	class PendingDutyHelper
	{

	public:

		PendingDutyHelper();
		PendingDutyHelper( const DASType::DutySet& duty );

		const DASType::DutySet& getDuty() const;

		void processPolicyResult( const DutyPrimitive& dstSession, const DutyPrimitive& srcSession,
			const IDutyPolicy::POLICY_RESULT& policyResult );

		// For the case where ther is no source session
		void processPolicyResult( const DutyPrimitive& dstSession, const IDutyPolicy::POLICY_RESULT& policyResult );

		bool hasDuty(const DutyPrimitive& session) const;

		void add( const DutyPrimitive& duty );
		void rem( const DutyPrimitive& duty );

		bool apply();
		void cancel();

	private:

		DASType::DutySet m_pendingAdd;
		DASType::DutySet m_pendingRem;
		DASType::DutySet m_duty;
	};
}







#endif // !defined(PENDINGDUTYHELPER_H_INCLUDED)
