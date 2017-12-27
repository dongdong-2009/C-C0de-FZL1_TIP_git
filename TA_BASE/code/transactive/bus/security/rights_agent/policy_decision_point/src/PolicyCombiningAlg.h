#ifndef POLICYCOMBININGALG_H_INCLUDED
#define POLICYCOMBININGALG_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/policy_decision_point/src/PolicyCombiningAlg.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Implements the "permit-overrides" policy combining algorithm.
  *
  *  See http://www.oasis-open.org/committees/xacml/repository/oasis-xacml-1.0.pdf
  */

namespace TA_Base_Bus
{
	//Class declaration
	class PolicyCombiningAlg
	{
	public:
		
		PolicyCombiningAlg();

		void setResult( PdpConst::EPolicyResult result );

		bool resultKnown() const;

		PdpConst::EPolicyResult combinedResult() const;

	private:

		PolicyCombiningAlg& operator=( const PolicyCombiningAlg& );
		PolicyCombiningAlg( const PolicyCombiningAlg& );

		bool m_permit;
		bool m_atLeastOneError;
		bool m_atLeastOneDeny;

	}; // PolicyCombiningAlg
}; // TA_Base_Bus

#endif // POLICYCOMBININGALG_H_INCLUDED
