#ifndef RULECOMBININGALG_H_INCLUDED
#define RULECOMBININGALG_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/policy_decision_point/src/RuleCombiningAlg.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Implements the "deny-overrides" rule combining algorithm.
  *
  *  See http://www.oasis-open.org/committees/xacml/repository/oasis-xacml-1.0.pdf
  */

namespace TA_Base_Bus
{
	//Class declaration
	class RuleCombiningAlg
	{
	public:
		
		RuleCombiningAlg();

		void setResult( PdpConst::EPolicyResult effect, PdpConst::EPolicyResult result );

		bool resultKnown() const;

		PdpConst::EPolicyResult combinedResult() const;

	private:

		RuleCombiningAlg& operator=( const RuleCombiningAlg& );
		RuleCombiningAlg( const RuleCombiningAlg& );

		bool m_deny;
		bool m_atLeastOneError;
		bool m_potentialDeny;
		bool m_atLeastOnePermit;

	}; // RuleCombiningAlg
}; // TA_Base_Bus

#endif // RULECOMBININGALG_H_INCLUDED
